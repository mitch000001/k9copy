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
#include <qdir.h>

bool k9Tools::checkProgram(QString _progName) {
   return KStandardDirs::findExe( _progName,NULL,false) !=NULL ;
}

void k9Tools::clearOutput(QString name) {
    QDir dir(name);
    //delete files in directory
    QStringList lst = dir.entryList( "*",QDir::Files |QDir::Hidden );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QString c(( *it ).latin1() );
        dir.remove (c);
    }
    //scanning subdir
    QStringList lstdir = dir.entryList( "*",QDir::Dirs );
    for ( QStringList::Iterator it = lstdir.begin(); it != lstdir.end(); ++it ) {
        QString c(( *it ).latin1() );
        if ((c!=".") && c!="..") {
            clearOutput(dir.absFilePath(c));
            dir.rmdir(c);
        }
    }

}
