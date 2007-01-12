/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   jm_petit@laposte.net                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef KCONFIGDLG_H
#define KCONFIGDLG_H

#include "k9common.h"
#include <configDlg.h>
#include <qtable.h>
#include <qstringlist.h>
#include <qfile.h>

/**
@author Jean-Michel PETIT
*/
class kConfigDlg : public configDlg
{
public:
    kConfigDlg(QWidget *parent);

    ~kConfigDlg();
    void bAddClick();
    void bRemoveClick();
    void save();

};

#endif
