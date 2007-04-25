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


#include "k9mp4dlg.h"
#include <qlabel.h>
#include <kprogress.h>
#include <qpainter.h>
#include <kstandarddirs.h>

k9MP4Dlg::k9MP4Dlg(QWidget* parent, const char* name)
: MP4Dlg(parent,name, true)
{
  m_x1=0;m_x2=0;

  connect(&m_timer,SIGNAL(timeout()),this,SLOT(drawImage()));
  
  QString s= KGlobal::dirs()->findResource( "data", "k9copy/pellicule.png");
  image1.load(s);
  image2=image1;
   m_timer.start(200,FALSE);


}

k9MP4Dlg::~k9MP4Dlg()
{
}

/*$SPECIALIZATION$*/

void k9MP4Dlg::Cancel() {
   //QDialog::accept();
   emit sigCancel();
}

void k9MP4Dlg::setTitleLabel(QString _titleLabel) {
    lblTitle->setText(_titleLabel);
}


void k9MP4Dlg::setfps(QString _fps) {
    lblfps->setText(_fps);
}

void k9MP4Dlg::setremain(QString _remain) {
    lblRemain->setText(_remain);
}

void k9MP4Dlg::setProgress(int _progress) {
    pbProgress->setProgress(_progress);

}

void k9MP4Dlg::setbitrate(QString _bitrate) {
    lblbitrate->setText(_bitrate);
}

void k9MP4Dlg::setsize( QString _size) {
    lblsize->setText(_size);
}

void k9MP4Dlg::drawImage() {
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


#include "k9mp4dlg.moc"

