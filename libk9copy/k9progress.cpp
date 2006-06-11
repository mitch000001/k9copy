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

k9Progress::k9Progress(QWidget* parent, const char* name,const QStringList &args)
: Progress(parent,name,true,0)
{
  m_process=new QProcess();
  connect(m_process, SIGNAL(processExited()), this, SLOT(accept()));
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
	if (!m_process->start())
		return -1;
	}
   if (exec()==QDialog::Rejected)
   	return 0;
   else
	return 1;
}


void k9Progress::bCancelClick() {
   m_process->tryTerminate();
   reject();
}

QProcess* k9Progress::getProcess() const {
    return m_process;
}

/*$SPECIALIZATION$*/


#include "k9progress.moc"
