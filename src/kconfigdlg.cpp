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
#include "kconfigdlg.h"
#include "kmessagebox.h"

#include <kiconloader.h>
#include <kpushbutton.h>

#include <kstdguiitem.h>
#include <kguiitem.h>

kConfigDlg::kConfigDlg(QWidget *parent)
    : configDlg(parent)
{
  QStringList ldev;
  QStringList llabels;
  QStringList lIO;
  tblDevices->setColumnWidth(0,150);
  tblDevices->setColumnWidth(1,210);
  tblDevices->setColumnWidth(2,40);
  tblDevices->setColumnWidth(3,45);
  KSimpleConfig settings("K9Copy");
  ldev=settings.readListEntry("devices/dev");
  llabels=settings.readListEntry("devices/labels");
  lIO=settings.readListEntry("devices/IO");
  int row=0;
  for ( QStringList::Iterator it = ldev.begin(); it != ldev.end(); ++it )
  {
    tblDevices->setNumRows(row+1);
    tblDevices->setText(row,0,(*it).latin1());
    QStringList::Iterator it2=llabels.at(row);
    tblDevices->setText(row,1,(*it2).latin1());

    row++;

  }
  for ( int j = 0; j < tblDevices->numRows(); ++j )
  {
    QCheckTableItem *I = new QCheckTableItem( tblDevices, "" );
    QCheckTableItem *O = new  QCheckTableItem( tblDevices, "" ) ;
    QStringList::Iterator it=lIO.at(j);
    QString c=(*it).latin1();
    if((c =="I") || (c =="IO")) I->setChecked(true);
    if((c =="O") || (c =="IO")) O->setChecked(true);
    tblDevices->setItem( j, 2, I );
    tblDevices->setItem( j, 3, O);
  }


//    buttonCancel->setIconSet(KStdGuiItem::cancel().iconSet());
//    buttonOk->setIconSet(KStdGuiItem::ok().iconSet());
}


kConfigDlg::~kConfigDlg()
{}




/*!
    \fn kConfigDlg::bAddClick()
 */
void kConfigDlg::bAddClick()
{
  tblDevices->setNumRows(tblDevices->numRows()+1);
  QCheckTableItem *I = new QCheckTableItem( tblDevices, "" );
  QCheckTableItem *O = new  QCheckTableItem( tblDevices, "" ) ;
  tblDevices->setItem( tblDevices->numRows()-1, 2, I );
  tblDevices->setItem( tblDevices->numRows()-1, 3, O);
}


/*!
    \fn kConfigDlg::bRemoveClick()
 */
void kConfigDlg::bRemoveClick()
{
  tblDevices->removeRow(tblDevices->currentRow());
}


void kConfigDlg::save()
{
  QStringList ldev;
  QStringList llabels;
  QStringList lIO;
  KSimpleConfig settings("K9Copy");
  ldev.clear();
  llabels.clear();
  for (int i=0;i<tblDevices->numRows();i++)
  {
    QString c=tblDevices->text(i,0);
    if (!c.isEmpty())
    {
      if (QFile::exists(c))
      {
        ldev.append(tblDevices->text(i,0));
        c=tblDevices->text(i,1);
        if (c.isEmpty()) c=tblDevices->text(i,0);
        llabels.append(c);
        QCheckTableItem *I = (QCheckTableItem*)tblDevices->item(i,2);
        QCheckTableItem *O = (QCheckTableItem*)tblDevices->item(i,3);
        if (O->isChecked() && I->isChecked())
        {
          lIO.append("IO");
        }
        else
        {
          if (O->isChecked()) lIO.append("O");
          if (I->isChecked()) lIO.append("I");
          if ((!O->isChecked()) && (!I->isChecked()))
          {
            lIO.append("x");
          }
        }
      }
      else
      {
        KMessageBox::error( this, c + " is not a valid device", "Devices");
      }
    }
  }
  settings.writeEntry("devices/labels",llabels);
  settings.writeEntry("devices/dev",ldev);
  settings.writeEntry("devices/IO",lIO);
}
