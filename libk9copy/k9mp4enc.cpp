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


k9MP4Enc::k9MP4Enc(QObject *parent, const char *name,const QStringList& args)
        : QObject(parent, name) {
    m_fourcc=m_height=m_width=m_audioBitrate=m_filename="";
    m_codec=xvid; //lavc_mp4;
    m_cpt=-1;
    m_parts=1;

    QStringList laudio;
    QStringList llabels;
    QStringList lvideo;

    k9Config config;
    m_lstAudio=config.getCodecAudio();
    m_lstCodecs=config.getCodecLabels();
    m_lstVideo=config.getCodecVideo();
    
    m_2pass=config.getPrefMp42Passes();
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );

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

void k9MP4Enc::execute(k9DVDTitle *_title) {
    bool error=false;

    if ( ! k9Tools::checkProgram("mencoder")) {
        KMessageBox::error (qApp->mainWidget(),i18n("Unable to run %1").arg("mencoder") , i18n("Encoding error"));
        error = TRUE;
        return;
    }

    time = new QTime(0,0);
    time->start();
    m_percent=0;
    m_remain="--:--:--";


    m_totalSize=_title->getChaptersSize(true);//  getsectors();
/*    for (int ititle=0 ; ititle < _title->getTitles().count();ititle++) {
        k9DVDTitle *tmp=_title->getTitles().at(ititle);
        m_totalSize+= tmp->getChaptersSize( true);//   getsectors();
    }
*/

    QString injectName;
    KTempFile injectFile(locateLocal("tmp", "k9copy/k9v"), "");
    injectFile.setAutoDelete(true);
    injectFile.close();
    injectName=injectFile.name();


    int maxPass=0;
    int pass=0;

    for (uint m_part =1 ; (m_part <=m_parts) && !error ;m_part++) {
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

            for (uint i=0;i<_title->getaudioStreamCount();i++) {
                if (_title->getaudioStream(i)->getselected()) {
                    *m_process << "--audiofilter" << QString::number(_title->getaudioStream(i)->getID());
                    break;
                }
            }

            *m_process << "| mencoder" << "/dev/stdin";
            *m_process << "-passlogfile" << passLogFile.name();
            *m_process << "-ovc";

            bool audio=false;
	    QString sPass="";
	    QString sCodec="";
            switch (m_codec) {
            case xvid:
                if (pass>0) {
                    qDebug( QString("encoding pass %1").arg(pass));
                    sPass=QString(":turbo:pass=%1").arg(pass);
                }

                *m_process << "xvid";
                *m_process <<"-xvidencopts";
                *m_process <<"bitrate=" + QString::number(getBitRate(_title)) +sPass;
                sCodec="XviD";
                break;
            case lavc_mp4 :
                if (pass>0) {
                    qDebug( QString("encoding pass %1").arg(pass));
                    sPass=QString(":turbo:vpass=%1").arg(pass);
                }

                *m_process << "lavc";
                *m_process << "-lavcopts";
                *m_process << QString("vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=%1"+sPass).arg(getBitRate(_title));
                m_fourcc="DIVX";  //best compatibility
		sCodec="lavc MPEG-4";
                break;
            case x264:
                if (pass>0) {
                    qDebug( QString("encoding pass %1").arg(pass));
                    sPass=QString(":turbo=1:pass=%1").arg(pass);
                }

                *m_process << "x264";
                *m_process <<"-x264encopts";
                *m_process <<"bitrate=" + QString::number(getBitRate(_title)) +sPass;
                sCodec="x264";
                m_width=round16(m_width);
                m_height=round16(m_height);

                break;
            default:
                QStringList::Iterator it = m_lstVideo.at((int)m_codec - 3 );
                *m_process << replaceParams((*it));
                for (uint i=0;i<_title->getaudioStreamCount();i++) {
                    if (_title->getaudioStream(i)->getselected()) {
                        *m_process << "-oac";
                        it = m_lstAudio.at((int)m_codec - 3 );
                        *m_process << replaceParams((*it));
                        audio=true;
                        break;
                    }
                }
                it = m_lstCodecs.at((int)m_codec - 3 );
                sCodec=*it;
                break;

            }
	    if (pass >0)
		m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec)+" - "+i18n("pass %1").arg(pass));
	    else
		m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec));

            if (m_fourcc !="")
                *m_process << "-ffourcc" << m_fourcc;

            if (m_codec == xvid || m_codec == lavc_mp4 || m_codec== x264) {
                *m_process <<"-vf" << QString("pp=de,crop=0:0:0:0,scale=%1:%2").arg(m_width).arg(m_height);
                //looking for first audio selected
                for (uint i=0;i<_title->getaudioStreamCount();i++) {
                    if (_title->getaudioStream(i)->getselected()) {
                        *m_process << "-oac";
                        *m_process << "mp3lame";
                        *m_process <<"-lameopts" << QString("abr:br=%1").arg(m_audioBitrate);
			*m_process <<"-af volume=10";
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
	    if (pass==1)
		*m_process << "-o" << "/dev/null";
	    else
            	*m_process <<"-o" << "'"+path+"'";
	     
	   
	/*    QString s="";
	    for ( int i=0; i< m_process->args().count();i++) {
	    	QCString str=*(m_process->args().at(i));
	    	s +=QString(str)+" ";
	    }
	    qDebug (s);
	  */ 
	   
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
            if (maxPass >0)
            	pass++;
        } while (pass<=maxPass && !error && m_2pass);

        delete m_progress;
    }
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


void k9MP4Enc::exited(KProcess * process) {
    m_progress->accept();
}

int k9MP4Enc::getBitRate(k9DVDTitle *_title) {
    // bitrate video = (MB *8388.608) /SEC    - bitrate audio
    int size=m_size.toInt();
    float titleSize=_title->getChaptersSize_mb( true);
    if ( titleSize< (float)size)
        size=(int)(titleSize/m_parts) ;
    m_progress->setsize(QString::number(size) +i18n("mb") +" X " +QString::number(m_parts));
    QTime t1(0,0);
    int sec=t1.secsTo(_title->getSelectedLength());
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

        m_progress->setProgress((int)m_percent);
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
