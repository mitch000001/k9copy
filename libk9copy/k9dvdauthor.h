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
#ifndef K9DVDAUTHOR_H
#define K9DVDAUTHOR_H

#include "k9common.h"
#include "k9dvd.h"
#include "k9progress.h"

#include <qdom.h>
#include <qprocess.h>
#include <qdatastream.h>
#include <qprogressdialog.h>
#include <klocale.h>
/**
  *@author
  */

class k9DVDAuthor : public QObject  {
Q_OBJECT
public:
	k9DVDAuthor(QObject *DVDStruct,const char* name=0,const QStringList& args=0) ;
	~k9DVDAuthor();
  virtual void setworkDir( const QString& _newVal);
  virtual const QString& getworkDir();
  virtual bool getError();
  virtual void author();
  virtual void setburnDVD( const bool& _newVal);
  virtual const bool& getburnDVD();
private: // Private attributes
  /**  */
  k9DVD* DVD;
  double factor;
  QDomDocument *xml;
  QString lastMsg,totSize,inject;
  QProcess *proc;
  QString workDir;
  k9Progress *progress;
  bool cancelled;
  bool error;
  bool burnDVD;
  int totalSize;

  void clearOutput(QString name);
  void createXML();
  void addTitle(QDomElement &root, int title);
  void createMenus(bool preview);
  void addMenus(QDomElement &titleSet);
  void processMenu();
  void spumux();
private slots: // Private slots
  /** No descriptions */
  void DVDAuthorStderr();
  void DVDAuthorStdout();

  /** No descriptions */
  void stopProcess();
};

#endif

