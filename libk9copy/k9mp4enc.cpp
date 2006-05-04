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

k9MP4Enc::k9MP4Enc(QObject *parent, const char *name,const QStringList& args)
 : QObject(parent, name){
   m_height=m_width=m_audioBitrate=m_filename="";
   m_codec=xvid; //lavc_mp4;
   m_cpt=-1;
}

void k9MP4Enc::execute(k9DVDTitle *_title) {
    time = new QTime(0,0);
    time->start();

   m_stderr="";
   m_title=_title;
   if (m_height=="") m_height="-2";
   if (m_width=="") m_width="640";
   if (m_audioBitrate=="") m_audioBitrate="128";
   if (m_size=="") m_size="700";
   if (m_filename=="") 
      m_filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.avi", 0,i18n("Save file to disk"));
   if (m_filename =="") return;

   QDir d=QDir::root();
   if (d.exists(m_filename)) d.remove(m_filename);

   m_progress=new k9MP4Dlg(qApp->mainWidget(),0);

   m_process=new KProcess();
   m_process->setUseShell(true);
   *m_process << "k9copy" << "--play";
   *m_process << "--input" << m_device;
   *m_process << "--dvdtitle" << QString::number(_title->getnumTitle());
   *m_process << "| mencoder" << "/dev/stdin";
   //QString pgc("dvd://%1 -dvd-device %2");
   //*m_process << pgc.arg(_title->getnumTitle()).arg(m_device);
   *m_process << "-ovc";
   switch (m_codec) {
   	case xvid:
   		*m_process << "xvid";
   		*m_process <<"-xvidencopts";
   		*m_process <<":bitrate=" + QString::number(getBitRate(_title));
		m_progress->setTitleLabel(i18n("Encoding %1").arg("XviD"));
		break;
	case lavc_mp4 :
		*m_process << "lavc";
		*m_process << "-lavcopts";
		*m_process << QString("vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=%1").arg(getBitRate(_title)); 
		m_progress->setTitleLabel(i18n("Encoding %1").arg("lavc MPEG-4"));
		break;

   }

   *m_process <<"-vf" << QString("pp=de,crop=0:0:0:0,scale=%1:%2").arg(m_width).arg(m_height);
   bool audio=false;
   //looking for first audio selected
   for (uint i=0;i<_title->getaudioStreamCount();i++) {
  	if (_title->getaudioStream(i)->getselected()) {
	   *m_process << "-oac";
   	   *m_process << "mp3lame";
	   *m_process <<"-lameopts" << QString("abr:br=%1").arg(m_audioBitrate);
   	   *m_process <<"-aid";

	   *m_process << QString::number(_title->getaudioStream(i)->getStreamId());
           audio=true;
	   break;
	}
   }
   if (!audio) *m_process << "-nosound";

   *m_process <<"-o" << m_filename;

   connect(m_process, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(getStdout(KProcess *, char *, int) ));
   connect(m_process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));
   connect(m_process, SIGNAL(processExited(KProcess*)),this,SLOT(exited(KProcess*)));
   m_process->start(KProcess::OwnGroup, KProcess::All);
   if(m_progress->exec() == QDialog::Rejected) {
        m_process->kill();
   } else if (!m_process->normalExit()) {
	KMessageBox::error (qApp->mainWidget(),"<b>"+i18n("Error while running mencoder :") +"<b><br>"+m_stderr, i18n("Encoding error"));
   }

   delete m_progress;
   
}

void k9MP4Enc::exited(KProcess * process) {
   m_progress->accept();
}

int k9MP4Enc::getBitRate(k9DVDTitle *_title) {
// bitrate video = (MB *8388.608) /SEC    - bitrate audio
QTime t1(0,0);
int sec=t1.secsTo(_title->getlength());
int bitrate=(m_size.toInt() * 8388.608)/sec  - m_audioBitrate.toInt();
return bitrate;

}


void k9MP4Enc::getStdout(KProcess *proc, char *buffer, int buflen) {
  QString tmp(buffer);
  m_cpt++;
  if (m_cpt==100) m_cpt=0; 

  if (m_cpt!=0) return;



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
    float percent;
    uint32_t totalBytes,totalSize;
    sscanf(tmp.latin1(),"INFOPOS: %d %d",&totalBytes,&totalSize);
    percent=(float)totalBytes / (float)totalSize;


    QTime time2(0,0);
    time2=time2.addMSecs(time->elapsed());
    QString remain("--:--:--");
    if (percent>0) {
	QTime time3(0,0);
	time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/percent)));
	remain=time3.toString("hh:mm:ss");
    }

    percent*=100;
    m_progress->setProgress(percent);
    m_progress->setremain(time2.toString("hh:mm:ss") +" / " +remain);

    m_progress->setProgress(percent);
} else 
   qDebug(m_stderr);

}


k9MP4Enc::~k9MP4Enc()
{
}


#include "k9mp4enc.moc"
