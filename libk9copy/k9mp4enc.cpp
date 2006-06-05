//
// C++ Implementation: k9mp4enc
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9mp4enc.h"
#include "k9mp4dlg.h"

#include <qcstring.h>
#include <qapplication.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qdir.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <qstringlist.h>
#include <ksimpleconfig.h>

k9MP4Enc::k9MP4Enc(QObject *parent, const char *name,const QStringList& args)
        : QObject(parent, name) {
    m_fourcc=m_height=m_width=m_audioBitrate=m_filename="";
    m_codec=xvid; //lavc_mp4;
    m_cpt=-1;
    m_parts=1;

    QStringList laudio;
    QStringList llabels;
    QStringList lvideo;

    KSimpleConfig settings("K9Copy");
    m_lstAudio=settings.readListEntry("mencoder/audio");
    m_lstCodecs=settings.readListEntry("mencoder/labels");
    m_lstVideo=settings.readListEntry("mencoder/video");
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );

}

void k9MP4Enc::execute(k9DVDTitle *_title) {
    time = new QTime(0,0);
    time->start();
    bool error=false;
    m_percent=0;
    m_remain="--:--:--";

    for (uint m_part =1 ; (m_part <=m_parts) && !error ;m_part++) {
        uint32_t sec1=_title->getChapter(0)->getstartSector();
        uint32_t sec2=_title->getChapter(_title->getchapterCount()-1)->getendSector();

        uint32_t nbSectors= (_title->getChapter(_title->getchapterCount()-1)->getendSector() - _title->getChapter(0)->getstartSector()) / m_parts   ;

        uint32_t startSector= nbSectors*(m_part-1);
        uint32_t endSector= startSector+nbSectors;

        //calculer le bitrate en faisant la somme des cells compris entre startSector et endSector

        m_stderr="";
        m_title=_title;
        if (m_height=="")
            m_height="-2";
        if (m_width=="")
            m_width="640";
        if (m_audioBitrate=="")
            m_audioBitrate="128";
        if (m_size=="")
            m_size="700";
        if (m_filename=="")
            m_filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.avi", 0,i18n("Save file to disk"));
        if (m_filename =="")
            return;

        QDir d=QDir::root();
        if (d.exists(m_filename))
            d.remove(m_filename);

        m_progress=new k9MP4Dlg(qApp->mainWidget(),0);
        m_progress->setbitrate(QString::number(getBitRate(_title)));
        m_progress->setsize(m_size +i18n("mb") +" X " +QString::number(m_parts));
        m_process=new KProcess();
        m_process->setUseShell(true);
        *m_process << "k9copy" << "--play" << "--startsector" << QString::number(startSector) << "--endsector" << QString::number(endSector) ;
        *m_process << "--input" << "'"+m_device+"'";
        *m_process << "--dvdtitle" << QString::number(_title->getnumTitle());

        for (uint i=0;i<_title->getaudioStreamCount();i++) {
            if (_title->getaudioStream(i)->getselected()) {
                *m_process << "--audiofilter" << QString::number(_title->getaudioStream(i)->getID());
		break;
            }
        }

        *m_process << "| mencoder" << "/dev/stdin";
        *m_process << "-ovc";

        bool audio=false;

        switch (m_codec) {
        case xvid:
            *m_process << "xvid";
            *m_process <<"-xvidencopts";
            *m_process <<"bitrate=" + QString::number(getBitRate(_title));
            m_progress->setTitleLabel(i18n("Encoding %1").arg("XviD"));

            break;
        case lavc_mp4 :
            *m_process << "lavc";
            *m_process << "-lavcopts";
            *m_process << QString("vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=%1").arg(getBitRate(_title));
            m_progress->setTitleLabel(i18n("Encoding %1").arg("lavc MPEG-4"));
	    m_fourcc="DIVX";  //best compatibility
            break;
        default:
            QStringList::Iterator it = m_lstVideo.at((int)m_codec - 2 );
            *m_process << replaceParams((*it));
            for (uint i=0;i<_title->getaudioStreamCount();i++) {
                if (_title->getaudioStream(i)->getselected()) {
                    *m_process << "-oac";
                    it = m_lstAudio.at((int)m_codec - 2 );
                    *m_process << replaceParams((*it));
                    //*m_process <<"-aid";
                    //*m_process << QString::number(_title->getaudioStream(i)->getStreamId());
                    audio=true;
                    break;
                }
            }
            it = m_lstCodecs.at((int)m_codec - 2 );
            m_progress->setTitleLabel(i18n("Encoding %1").arg((*it)));
            break;

        }

	if (m_fourcc !="") 
	    *m_process << "-ffourcc" << m_fourcc;

        if (m_codec == xvid || m_codec == lavc_mp4) {
            *m_process <<"-vf" << QString("pp=de,crop=0:0:0:0,scale=%1:%2").arg(m_width).arg(m_height);
            //looking for first audio selected
            for (uint i=0;i<_title->getaudioStreamCount();i++) {
                if (_title->getaudioStream(i)->getselected()) {
                    *m_process << "-oac";
                    *m_process << "mp3lame";
                    *m_process <<"-lameopts" << QString("abr:br=%1").arg(m_audioBitrate);

                    //*m_process << QString::number(_title->getaudioStream(i)->getStreamId());
                    audio=true;
                    break;
                }
            }
        }
        if (!audio)
            *m_process << "-nosound";

        QString path=m_filename;

        if (m_parts>1) {
            QString ext=m_filename.section(".",-1);
            if (ext!="")
                ext="."+ext;
            path=m_filename.left(m_filename.length()-ext.length());
            path=path+QString::number(m_part)+ext;
        }

        *m_process <<"-o" << "'"+path+"'";

        connect(m_process, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(getStdout(KProcess *, char *, int) ));
        connect(m_process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));
        connect(m_process, SIGNAL(processExited(KProcess*)),this,SLOT(exited(KProcess*)));
        m_process->start(KProcess::OwnGroup, KProcess::All);
        timer->start(500, 0 );

        if(m_progress->exec() == QDialog::Rejected) {
            m_process->kill();
            KMessageBox::information (qApp->mainWidget(),i18n("MPEG-4 Encoding cancelled"), i18n("MPEG-4 Encoding"));
            error=true;
        } else if (!m_process->normalExit()) {
            KMessageBox::error (qApp->mainWidget(),"<b>"+i18n("Error while running mencoder :") +"<b><br>"+m_stderr, i18n("Encoding error"));
            error=true;
        }

        delete m_progress;
    }
}

QString k9MP4Enc::replaceParams(QString _value) {
    QString str=_value;
    str.replace("$WIDTH",m_width);
    str.replace("$HEIGHT",m_height);
    str.replace("$VIDBR",QString::number(getBitRate(m_title)));
    str.replace("$AUDBR",m_audioBitrate);
    return str;
}


void k9MP4Enc::exited(KProcess * process) {
    m_progress->accept();
}

int k9MP4Enc::getBitRate(k9DVDTitle *_title) {
    // bitrate video = (MB *8388.608) /SEC    - bitrate audio
    int size=m_size.toInt();
    if (_title->getsize_mb() < size)
	size=_title->getsize_mb();
    QTime t1(0,0);
    int sec=t1.secsTo(_title->getlength());
    int bitrate=((size*m_parts) * 8388.608)/sec  - m_audioBitrate.toInt();
    return bitrate;

}


void k9MP4Enc::getStdout(KProcess *proc, char *buffer, int buflen) {
    QString tmp(buffer);
    m_cpt++;
    if (m_cpt==100)
        m_cpt=0;

    if (m_cpt!=0)
        return;



    int pos=tmp.find("Pos:");
    if (pos!=-1) {
        QString tmp2=tmp.mid(pos);
        float t;
        int frame;
        int percent;
        int fps;
        int trem;
        int fsize;
        sscanf(tmp2.latin1(),"Pos: %f%*s%d",&t,&frame);
        tmp2=tmp2.mid(tmp2.find("(")+1);
        //sscanf(tmp2.latin1(),"%d",&percent);
        tmp2=tmp2.mid(tmp2.find(")")+1);
        sscanf(tmp2.latin1(),"%d",&fps);
        tmp2=tmp2.mid(tmp2.find("Trem:")+5);
        //sscanf(tmp2.latin1(),"%d",&trem);
        tmp2=tmp2.mid(tmp2.find("min")+3);
        //sscanf(tmp2.latin1(),"%d",&fsize);

        m_progress->setfps(QString::number(fps));
        //m_progress->setremain(QString::number(trem) + " "+i18n("min"));
        //m_progress->setsize(QString::number(fsize) + " "+i18n("mb"));
    }

}

void k9MP4Enc::getStderr(KProcess *proc, char *buffer, int buflen) {
    m_stderr=QString(buffer);

    int pos=m_stderr.find("INFOPOS:");
    if (pos!=-1) {
        QString tmp=m_stderr.mid(pos);
        uint32_t totalBytes,totalSize;
        sscanf(tmp.latin1(),"INFOPOS: %d %d",&totalBytes,&totalSize);
        m_percent=(float)totalBytes / (float)totalSize;


        QTime time2(0,0);
        time2=time2.addMSecs(time->elapsed());
        if (m_percent>0) {
            QTime time3(0,0);
            time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
            m_remain=time3.toString("hh:mm:ss");
        }

        m_percent*=100;
        m_progress->setProgress(m_percent);
        m_progress->setremain(time2.toString("hh:mm:ss") +" / " +m_remain);

        m_progress->setProgress(m_percent);
    } else
        qDebug(m_stderr);

}

void k9MP4Enc::timerDone() {
        QTime time2(0,0);
        time2=time2.addMSecs(time->elapsed());
        m_progress->setremain(time2.toString("hh:mm:ss") +" / " +m_remain);

}


k9MP4Enc::~k9MP4Enc() {}


#include "k9mp4enc.moc"
