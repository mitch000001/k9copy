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

#include "k9dvdprogress.h"
#include <qprogressbar.h>
#include <qlabel.h>
 
k9DVDProgress::k9DVDProgress(QWidget* parent, const char* name, bool modal, WFlags fl)
    : DVDProgress( parent, name, modal, fl )
{
}

k9DVDProgress::~k9DVDProgress(){
}
/** No descriptions */
void k9DVDProgress::setpbTitleTotalSteps(unsigned int total){
pbTitle->setTotalSteps(total);

}
/** No descriptions */
void k9DVDProgress::setpbTitleStep(unsigned int position){
pbTitle->setProgress(position);
}
/** No descriptions */
void k9DVDProgress::setpbTotalTotalSteps(int total){
  pbTotal->setTotalSteps(total);
}
/** No descriptions */
void k9DVDProgress::setpbTotalStep(int position){
  pbTotal->setProgress(position);
}
/** No descriptions */
void k9DVDProgress::setlblTitle(QString & text){
lblTitle->setText(text);
}
/** No descriptions */
void k9DVDProgress::setlblTotal(QString& text){
lblTotal->setText(text);
}

void k9DVDProgress::closeEvent( QCloseEvent* ce )
{
  ce->ignore();
  return;
}
