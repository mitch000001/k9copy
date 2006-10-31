//
// C++ Implementation: factory
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
#include "k9mplayer.h"


#include <kgenericfactory.h>
typedef K_TYPELIST_1( K9Mplayer) k9x;
K_EXPORT_COMPONENT_FACTORY( libk9mplayer, KGenericFactory<k9x> )

 
