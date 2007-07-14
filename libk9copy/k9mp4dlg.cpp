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


void k9MP4Dlg::setImage(QString _fileName) {
    QPixmap pixmap;
    pixmap.load(_fileName);

    
    int top,left;
    QPainter p(image);
        
   double wratio=(double)image->width()/(double)pixmap.width();
   double hratio=(double)image->height()/(double)pixmap.height();
   double ratio= wratio < hratio ? wratio:hratio;

   top =(int) (image->height() -pixmap.height()*ratio)/2+1;
   left =(int) (image->width() -pixmap.width()*ratio)/2 +1;

   p.scale(ratio,ratio);
   p.drawPixmap((int)(left/ratio),(int)(top/ratio),pixmap);
}



#include "k9mp4dlg.moc"

