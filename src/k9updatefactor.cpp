//
// C++ Implementation: k9updatefactor
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9updatefactor.h"

k9UpdateFactor::k9UpdateFactor(QObject *parent, const char *name)
 : QObject(parent, name), QThread()
{
}


k9UpdateFactor::~k9UpdateFactor()
{
   wait();
}

void k9UpdateFactor::run(){
    do  {
    	m_restart=false;
    	emit updateFactor_internal();
    } while (m_restart);
}

void k9UpdateFactor::updateFactor() {
   if (running()) {
   	m_restart=true;
   }
   else
   	start();
}


#include "k9updatefactor.moc"
