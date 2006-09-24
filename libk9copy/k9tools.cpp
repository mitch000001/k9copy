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
#include <kprocess.h>


bool k9Tools::checkProgram(QString _progName) {
    KProcess proc;
    proc << _progName;
    return proc.start();
}

