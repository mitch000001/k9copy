//
// C++ Implementation: dummy
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvd.h"
#include "k9dvdbackup.h"
#include "k9dvdauthor.h"
#include "k9dvdsize.h"
#include "k9progress.h"

#include <kgenericfactory.h>
typedef K_TYPELIST_5( k9DVD,k9DVDBackup,k9DVDAuthor,k9DVDSize,k9Progress ) k9;
K_EXPORT_COMPONENT_FACTORY( libk9copy, KGenericFactory<k9> )


