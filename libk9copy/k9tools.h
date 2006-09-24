//
// C++ Interface: k9tools
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9TOOLS_H
#define K9TOOLS_H

#include <qstring.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Tools{
private:
    k9Tools() {};
    ~k9Tools(){};
public: 
   static bool checkProgram(QString _progName);

};

#endif
