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
#include <qmovie.h>
#include <qlayout.h>



k9Progress::k9Progress(QWidget* parent, const char* name,const QStringList &args)
        : Progress(parent,name,0) {
    m_process=new k9Process(this,0);
    m_wimage=new k9DrawImage(image,0);
    QGridLayout *l=new QGridLayout(image,1,1);
    l->addWidget(m_wimage,0,0);


}


k9Progress::~k9Progress() {
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
    if (! m_process->isRunning()) {
        if (!m_process->start(KProcess::NotifyOnExit,KProcess::All ))
            return -1;
    }

//   m_timer.start(200,FALSE);
    show();
    m_canceled=false;
    //the sync method allows to wait for the process end while receiving stdout.
    m_process->sync();

//   m_timer.stop();
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


void k9Progress::setImage(QString _fileName) {
    m_wimage->setImage(_fileName);
}

void k9Progress::setImage(const QImage &_image) {
    m_wimage->setImage(_image);
}


void k9Progress::setMovie(QString _fileName) {
    image->setPaletteBackgroundColor(this->paletteBackgroundColor());
    QMovie mv(_fileName,2048);
    mv.setBackgroundColor(this->paletteBackgroundColor());
    image->setMovie(mv);
    m_wimage->hide();
}


#include "k9progress.moc"


bool k9Progress::getCanceled() const {
    return m_canceled;
}
