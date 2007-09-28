//
// C++ Implementation: k9process
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9process.h"
#include <qobject.h>
#include <qapplication.h>
#include <qeventloop.h>

k9Process::k9Process(QObject *parent, const char *name)
 : KProcess(parent, name),m_waitSync(false)
{
   connect(this,SIGNAL(processExited( KProcess* )),this,SLOT(slotProcessExited( KProcess* )));
 
}


k9Process::~k9Process()
{
}

void k9Process::sync() {
  m_waitSync=true;
  QApplication::eventLoop()->enterLoop();
}


void k9Process::slotProcessExited( KProcess * proc) {
   if (m_waitSync) {
   	QApplication::eventLoop()->exitLoop();
   	m_waitSync=false;	
   }
}

const QString & k9Process::debug() {
    m_debug="";
    for (int i=0;i<arguments.count();i++ ){
        m_debug +=" "+ *(arguments.at(i));
    }
    return m_debug;
}

bool  k9Process::start (RunMode runmode, Communication comm) {
  m_elapsed.start();
  return KProcess::start(runmode,comm);
}
#include "k9process.moc"


int k9Process::getElapsed() const {
    return m_elapsed.elapsed();
}
