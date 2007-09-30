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
#include "k9config.h"
#include <qcstring.h>
#include <qapplication.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qdir.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <qstringlist.h>
#include <ktempfile.h>
#include <kstandarddirs.h>
#include "k9tools.h"
#include "k9audiocodecs.h"
#include "k9videocodecs.h"
#include <qcstring.h>

k9MP4Enc::k9MP4Enc(QObject *parent, const char *name,const QStringList& )
        : QObject(parent, name) {
    m_fourcc=m_height=m_width=m_audioBitrate=m_videoBitrate=m_filename="";
    m_codec=0; //lavc_mp4;
    m_audioCodec=0;
    m_cpt=-1;
    m_parts=1;

    QStringList laudio;
    QStringList llabels;
    QStringList lvideo;

    k9Config config;
    m_lstAudio=config.getCodecAudio();
    m_lstCodecs=config.getCodecLabels();
    m_lstVideo=config.getCodecVideo();

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
    m_progress=new k9MP4Dlg(qApp->mainWidget(),0);

}

QString k9MP4Enc::round16(QString _wh) {
    if (_wh !="") {
        int value=_wh.toInt()/16;
        return QString::number(value*16);

    } else
        return _wh;


}

QString k9MP4Enc::getChapterList(k9DVDTitle *_title) {
    QString res="";
    QPtrList <k9DVDChapter> chapters=_title->getChapters();
    for (k9DVDChapter *chapter=chapters.first();chapter;chapter=chapters.next()) {
        if (chapter->getSelected()) {
            res+=res=="" ? QString::number( chapter->getnum()) : ","+QString::number( chapter->getnum());
        }
    }
    QPtrList <k9DVDTitle> titles=_title->getTitles();

    for (k9DVDTitle *title=titles.first();title;title=titles.next()) {
        chapters=title->getChapters();
        for (k9DVDChapter *chapter=chapters.first();chapter;chapter=chapters.next()) {
            if (chapter->getSelected()) {
                res+=res=="" ? QString::number( chapter->getnum()) : ","+QString::number( chapter->getnum());
            }
        }
    }
    return res;

}


int k9MP4Enc::getselectedSubp(k9DVDTitle *_title) {
    for (int i=0;i< _title->getsubPictureCount();i++) {
        if (_title->getsubtitle( i)->getselected()) {
            return _title->getsubtitle( i)->getID()-1;
        }
    }
    //nos subtitle selected
    return -1;
}



void k9MP4Enc::execute(k9DVDTitle *_title) {
    bool error=false;

    if ( ! k9Tools::checkProgram("mencoder")) {
        KMessageBox::error (qApp->mainWidget(),i18n("Unable to run %1").arg("mencoder") , i18n("Encoding error"));
        error = TRUE;
        return;
    }

    time = new QTime(0,0);
    m_percent=0;
    m_remain="--:--:--";

    m_totalSize=_title->getChaptersSize(true);

    QString injectName;
    KTempFile injectFile(locateLocal("tmp", "k9copy/k9v"), "");
    injectFile.setAutoDelete(true);
    injectFile.close();
    injectName=injectFile.name();


    int maxPass=0;
    int pass=0;

    for (int m_part =1 ; (m_part <=m_parts) && !error ;m_part++) {
        if (m_2pass) {
            maxPass=2;
            pass=1;
        }
        KTempFile passLogFile(locateLocal("tmp", "k9copy/k9v"), "");
        passLogFile.setAutoDelete(true);
        passLogFile.close();

        do {
            uint32_t nbSectors= m_totalSize / m_parts   ;

            uint32_t startSector= nbSectors*(m_part-1);
            uint32_t endSector= startSector+nbSectors;

            //calculer le bitrate en faisant la somme des cells compris entre startSector et endSector
            //FIXME Mettre en place la sélection par chapitres
            m_stderr="";
            m_title=_title;
            if (m_height=="" || m_height=="0")
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

            m_progress->setbitrate(QString::number(getBitRate(_title)));
            m_progress->setsize(m_size +i18n("MB") +" X " +QString::number(m_parts));
            m_process=new k9Process(this,0 );
            m_process->setUseShell(true);
            *m_process << "k9copy" << "--play" << "--endsector" << QString::number(endSector) ;
            *m_process << "--inject" << injectName; //"/tmp/kde-jmp/inject";
            *m_process << "--input" << "'"+m_device+"'";
            *m_process << "--dvdtitle" << QString::number(_title->getnumTitle());
            *m_process << "--chapterlist" << getChapterList( _title);
            if (m_part==1)
                *m_process << "--initstatus";
            else
                *m_process << "--continue";
            if (pass==1)
                *m_process << "--firstpass";

            for (int i=0;i<_title->getaudioStreamCount();i++) {
                if (_title->getaudioStream(i)->getselected()) {
                    *m_process << "--audiofilter" << QString::number(_title->getaudioStream(i)->getID());
                    break;
                }
            }

            if (getselectedSubp( _title) !=-1) {
                *m_process << "--subpicturefilter" ;
                QString s="";
                for (int i=1; i<=_title->getsubPictureCount();i++)
                    s+= (i>1?",":"") + QString::number(i);
                *m_process << s;
            }

            if (m_usecache)
                *m_process << "--usecache";
            *m_process << "| mencoder" << "/dev/stdin";
            *m_process << "-passlogfile" << passLogFile.name();

            bool audio=false;
            QString sPass="";
            QString sCodec="";

            k9AudioCodecs *audioCodecs=new k9AudioCodecs(0,0);
            k9VideoCodecs *videoCodecs=new k9VideoCodecs(0,0);

            QString sVOption;
            m_pass=pass;
            switch (pass) {
            case 1:
                sVOption=replaceParams(videoCodecs->getOptions1(m_codec));
                break;
            case 2:
                sVOption=replaceParams(videoCodecs->getOptions2(m_codec));
                break;
            default:
                sVOption=replaceParams(videoCodecs->getOptions0(m_codec));
                break;
            }
            sCodec=videoCodecs->getCodecName(m_codec);
            sVOption=sVOption.simplifyWhiteSpace();
            int pos;
            //*m_process << "-ovc" << sVOption;
            /* int pos=sVOption.find("-vf");
             if (pos==-1)
                 *m_process <<"-vf" << QString("scale=%1:%2").arg(m_width).arg(m_height);
             else 
                 sVOption=sVOption.insert(pos+4,QString("scale=%1:%2,").arg(m_width).arg(m_height));
             */
            *m_process  << sVOption;

            QString sAOption=replaceParams(audioCodecs->getOptions(m_audioCodec)).simplifyWhiteSpace();



            if (pass >0)
                m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec)+" - "+i18n("pass %1").arg(pass));
            else
                m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec));

            if (m_fourcc !="")
                *m_process << "-ffourcc" << m_fourcc;
            else if (videoCodecs->getFourcc(m_codec)!="")
                *m_process << "-ffourcc" << videoCodecs->getFourcc(m_codec);

            delete audioCodecs;
            delete videoCodecs;

            //looking for first audio selected
            for (int i=0;i<_title->getaudioStreamCount();i++) {
                if (_title->getaudioStream(i)->getselected()) {
                    //*m_process << "-oac" << sAOption;
                    pos=sAOption.find("-af");
                    if (pos==-1)
                        *m_process << QString("-af volume=%1").arg(m_audioGain);
                    else
                        sAOption=sAOption.insert(pos+4,QString("volume=%1,").arg(m_audioGain));
                    *m_process << sAOption;

                    audio=true;
                    break;
                }
            }

            if (getselectedSubp( _title) !=-1) {
                *m_process << "-sid" << QString::number(getselectedSubp( _title));
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
            if (pass==1)
                *m_process << "-o" << "/dev/null";
            else
                *m_process <<"-o" << "'"+path+"'";
            if (path.upper().endsWith("MPEG") || path.upper().endsWith("MPG"))
                *m_process << "-of" << "mpeg";
            else
                *m_process << "-of" << "avi";

            QString s="";
            for ( uint i=0; i< m_process->args().count();i++) {
                QCString str=*(m_process->args().at(i));
                s +=QString(str)+" ";
            }
//	    qDebug (s);

            time->start();
            m_timer3.start();
            connect(m_process, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(getStdout(KProcess *, char *, int) ));
            connect(m_process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));
            //connect(m_process, SIGNAL(processExited(KProcess*)),this,SLOT(exited(KProcess*)));
            connect(m_progress,SIGNAL(sigCancel()),this,SLOT(slotCancel()));
            m_canceled=false;
            m_progress->show();
            m_process->start(KProcess::OwnGroup, KProcess::All);
            timer->start(500, 0 );
            m_process->sync();
            //if application is exiting, kill the encoding process
            if (m_process->isRunning()) {
                m_process->kill();
                return;
            }
            if (m_canceled) {
                KMessageBox::information (qApp->mainWidget(),i18n("MPEG-4 Encoding cancelled"), i18n("MPEG-4 Encoding"));
                error=true;
            } else if (!m_process->normalExit()) {
                KMessageBox::error (qApp->mainWidget(),"<b>"+i18n("Error while running mencoder :") +"</b><br>"+m_stderr, i18n("Encoding error"));
                error=true;
            }
            if (maxPass >0)
                pass++;
        } while (pass<=maxPass && !error && m_2pass);

    }
}


void k9MP4Enc::slotCancel() {
    m_canceled=true;
    m_process->kill();
}


QString k9MP4Enc::replaceParams(QString _value) {
    QString str=_value;
    str.replace("$PASS",QString::number(m_pass));
    str.replace("$WIDTH",m_width);
    str.replace("$HEIGHT",m_height);
    str.replace("$VIDBR",QString::number(getBitRate(m_title)));
    str.replace("$AUDBR",m_audioBitrate);
    return str;
}


int k9MP4Enc::getBitRate(k9DVDTitle *_title) {
    // bitrate video = (MB *8388.608) /SEC    - bitrate audio

    if (m_videoBitrate!="") {
        return  m_videoBitrate.toInt();
    } else {
        int size=m_size.toInt();
        float titleSize=_title->getChaptersSize_mb( true);
        if ( titleSize< (float)size)
            size=(int)(titleSize/m_parts) ;
        m_progress->setsize(QString::number(size) +i18n("MB") +" X " +QString::number(m_parts));
        QTime t1(0,0);
        int sec=t1.secsTo(_title->getSelectedLength());
        int bitrate=(int)( ((size*m_parts) * 8388.608)/sec  - m_audioBitrate.toInt());
        return bitrate;
    }
}


void k9MP4Enc::getStdout(KProcess *, char *buffer, int buflen) {
    QCString tmp(buffer,buflen);
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
        int fps;
        sscanf(tmp2.latin1(),"Pos: %f%*s%d",&t,&frame);
        tmp2=tmp2.mid(tmp2.find("(")+1);
        tmp2=tmp2.mid(tmp2.find(")")+1);
        sscanf(tmp2.latin1(),"%d",&fps);

        m_progress->setfps(QString::number(fps));
    }


}

void k9MP4Enc::getStderr(KProcess *proc, char *buffer, int buflen) {
    //m_stderr=QString::fromLatin1(buffer,buflen);
    QCString cstderr(buffer,buflen+1);

    if (cstderr.find("FATAL:")!=-1) {
        proc->kill();
    }

    int pos=cstderr.find("INFOPOS:");
    if (pos!=-1 ) {
        if (m_timer3.elapsed()>500) {
            m_timer3.restart();
            QString tmp=cstderr.mid(pos);
            uint32_t totalBytes,totalSize;
            sscanf(tmp.latin1(),"INFOPOS: %d %d",&totalBytes,&totalSize);
            if (totalSize !=0)
                m_percent=(float)totalBytes / (float)m_totalSize;


            QTime time2(0,0);
            time2=time2.addMSecs(time->elapsed());
            if (m_percent>0) {
                QTime time3(0,0);
                time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
                m_remain=time3.toString("hh:mm:ss");
            }

            m_percent*=100;
            m_progress->setProgress((int)m_percent);
            m_progress->setremain(time2.toString("hh:mm:ss") +" / " +m_remain);
        }
    } else {
        pos=cstderr.find("INFOIMAGE:");
        if (pos!=-1) {
            m_progress->setImage(cstderr.mid(pos+10));
        } else
            qDebug("[%s]",buffer);
    }
    m_stderr=cstderr;
}

void k9MP4Enc::timerDone() {
    QTime time2(0,0);
    time2=time2.addMSecs(time->elapsed());
    m_progress->setremain(time2.toString("hh:mm:ss") +" / " +m_remain);

}

bool k9MP4Enc::isCanceled() {
    return m_canceled;
}


k9MP4Enc::~k9MP4Enc() {}


#include "k9mp4enc.moc"
