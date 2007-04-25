//
// C++ Implementation: k9tools
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9tools.h"
#include <kstandarddirs.h>


bool k9Tools::checkProgram(QString _progName) {
   return KStandardDirs::findExe( _progName,NULL,false) !=NULL ;
}

