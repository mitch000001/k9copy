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

k9MP4Dlg::k9MP4Dlg(QWidget* parent, const char* name)
: MP4Dlg(parent,name, true)
{
}

k9MP4Dlg::~k9MP4Dlg()
{
}

/*$SPECIALIZATION$*/

void k9MP4Dlg::accept() {
   QDialog::accept();
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

#include "k9mp4dlg.moc"

