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

#include "k9process.moc"
