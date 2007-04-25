//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9progress.h"
#include <kprogress.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qimage.h>
#include <qpainter.h>
#include <kstandarddirs.h>

k9Progress::k9Progress(QWidget* parent, const char* name,const QStringList &args)
: Progress(parent,name,true,0)
{
  m_process=new k9Process(this,0);
  m_x1=0;m_x2=0;

  connect(&m_timer,SIGNAL(timeout()),this,SLOT(drawImage()));
  
  QString s= KGlobal::dirs()->findResource( "data", "k9copy/pellicule.png");
  image1.load(s);
  image2=image1;
}


k9Progress::~k9Progress()
{
   delete m_process;
}


void k9Progress::setElapsed(const QString _text) {
   lblElapsed->setText(_text);
}

void k9Progress::setTitle(const QString _text) {
   lblTitle->setText(_text);
}


void k9Progress::setLabelText(const QString _text) {
   LabelText->setText(_text);
}

void k9Progress::setProgress(long _position,long _total) {
   ProgressBar->setRange(0,_total);
   ProgressBar->setProgress(_position);
}

int k9Progress::execute() {
   if(! m_process->isRunning()) {
	if (!m_process->start(KProcess::NotifyOnExit,KProcess::All ))
		return -1;
	}
	
   m_timer.start(200,FALSE);
   show();
   m_canceled=false;	
   //the sync method allows to wait for the process end while receiving stdout.
   m_process->sync();
  		
   m_timer.stop();
   if (!m_canceled && m_process->normalExit())
   	return 1;
   else
	return 0;
}


void k9Progress::bCancelClick() {
   m_process->kill();
   m_canceled=true;
}

k9Process* k9Progress::getProcess() const {
    return m_process;
}

/*$SPECIALIZATION$*/

void k9Progress::drawImage() {
   QPainter p(image);
   if (m_x2==m_x1)
   	m_x2=m_x1+image1.width();
   p.scale((double)image->width()/(double)image1.width(),(double)image->height()/(double)image1.height());
   
   p.drawImage(m_x1--,0,image1);
   p.drawImage(m_x2--,0,image2);
   
   
   if (m_x1<(-1 * image1.width()))
   	m_x1=m_x2+image2.width();
   if (m_x2<(-1 * image2.width()))
   	m_x2=m_x1+image1.width();
 //  image->setPixmap(pix);
}

#include "k9progress.moc"
