//
// C++ Interface: k9importfiles
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9IMPORTFILES_H
#define K9IMPORTFILES_H

#include "k9common.h"

#include <qobject.h>
#include <kmdidefines.h>
#include <kmdichildarea.h>
#include <kmdimainfrm.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9Import;
class k9NewTitle;
class k9ImportFiles : public KMdiMainFrm
{
Q_OBJECT
public:
    k9ImportFiles(QWidget *parent = 0, const char *name = 0);

    ~k9ImportFiles();
private:
    k9Import *m_k9Import;
    KMdiChildView *m_k9ImportView;
    k9NewTitle *m_k9NewTitle;
    KMdiChildView *m_k9NewTitleView;
};

#endif
