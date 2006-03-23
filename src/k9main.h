//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MAIN_H
#define K9MAIN_H

#include "kcddrive.h"
#include "images.h"
#include "kviewmpeg2.h"
#include "k9mainw.h"
#include "kconfigdlg.h"

#include <qstring.h>
#include <kmessagebox.h>
#include <qlistview.h>
#include <ksimpleconfig.h>
#include <kmainwindow.h>
#include <kaboutdata.h>
#include <qevent.h>
#include <kfiledialog.h>


enum eStreamType {SUB,AUD,VID,NONE};

class LvItem : public QListViewItem
{
public:
  LvItem( QListViewItem *parent)
      : QListViewItem( parent), obj( NULL ) { streamType=NONE;}
  LvItem( QListView *parent)
      : QListViewItem( parent), obj( NULL ) {streamType=NONE;}
 eStreamType streamType;
  QObject *obj;
  virtual int rtti () const;
  int compare ( QListViewItem * i, int col, bool ascending ) const;
  void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );
};

class k9Main;
class k9DVD;
class  k9DVDAudioStream;
class  k9DVDSubtitle;
class k9DVDTitle;
class KLibFactory;

class ckLvItem : public QCheckListItem
{
public:
  ckLvItem( QListViewItem *parent,k9Main *dlg)
      : QCheckListItem( parent,"",QCheckListItem::CheckBox){ mainDlg=dlg; obj=NULL;stream=NULL;streamType=NONE;language="";}
  ckLvItem( QListView *parent,k9Main *dlg)
      : QCheckListItem( parent,"",QCheckListItem::CheckBox){ mainDlg=dlg; obj=NULL;stream=NULL;streamType=NONE;language="";}
 eStreamType streamType;

  k9Main *mainDlg;
  QObject *obj;
  QObject *stream;
  QString language;
  virtual int rtti () const;
  void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );
  int compare ( QListViewItem * i, int col, bool ascending ) const;
  double getstreamSize();

protected:  
  void stateChange(bool state);

};

class ckLvLangItem : public QCheckListItem
{
public:
  ckLvLangItem( QListViewItem *parent,k9Main *dlg)
      : QCheckListItem( parent,"",QCheckListItem::CheckBox){ mainDlg=dlg; streamType=NONE;}
 eStreamType streamType;
  k9Main *mainDlg;
  QString language;
protected:  
  void stateChange(bool state);
};



class k9DVDListItem : public QObject
{
  Q_OBJECT
public:
  k9DVDAudioStream *audioStream;
  k9DVDSubtitle *subtitle;
  k9DVDTitle *title;
  ckLvItem *listItem;
  eStreamType streamType;
public:
  k9DVDListItem(QObject *DVD,ckLvItem *List,eStreamType type);
};


class k9Main : public MainDlg
{
  Q_OBJECT

public:
  k9Main(QWidget* parent = 0, const char* name = 0,  const QStringList &sl=0 );
  ~k9Main();
  /*$PUBLIC_FUNCTIONS$*/
  void addTitle(k9DVDTitle *track);
  void updateSelection();
  void checkAll(bool state);
  void checkTS( bool _state,ckLvItem *_item );
  void checkTitle(bool state, ckLvItem *_item);
  void checkLang(QString lang, eStreamType streamType,bool state);
  bool getupdating();
  void saveSettings();
  void setDVDSize();
  static int compare(double v1,double v2);

  //streamanalyze stream;
  k9DVD *dvd;
private slots:
  virtual void          ckMenuClick();
  virtual void          listView1CurrentChanged( QListViewItem * );
  virtual void          cbTitleActivated( int );
  virtual void          cbStartActivated( int );
  virtual void          bDevicesClick();
  virtual void          bSaveClick();
  virtual void	  cbOutputDevActivated(int);
  virtual void	  bInputOpenClick();
  virtual void	  bInputOpenDirClick();
  virtual void	  foundMountPoint (const QString &mountPoint, unsigned long kBSize, unsigned long kBUsed, unsigned long kBAvail);
  virtual void 	  fspDone();

public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          PreviewTitle();
  virtual void          Copy();
  virtual void          Open();
  virtual void	   Clone(QString _input,QString _output);
  virtual void	   setInput(QString _input);
  virtual void 	   setOutput(QString _output);
protected:
  /*$PROTECTED_FUNCTIONS$*/
  QObjectList items;
  k9DVDListItem *addListItem(QObject *DVD,ckLvItem *List,eStreamType type);

  void readSettings();
  void readDrives();
  QPtrList<QListViewItem> lvItems;
  QPtrList<ckLvItem> tsItems;
  QPtrList <ckLvLangItem> langAudItems;
  QPtrList <ckLvLangItem> langSubItems;
  ckLvItem * root;
  QObjectList driveList;
  QObjectList recorderList;
  kCDDrives drives;
  QPixmap pxVideo;
  QPixmap pxSound;
  QPixmap pxText;
  kViewMPEG2 viewer;
  bool updating;
  bool fspFinish;
  long fspAvail;
   void fillTitleList();
   void closeEvent( QCloseEvent* ce );
   void closeDVD();
   KLibFactory *m_factory;;
   QString  getDevice(QComboBox *_combo);
protected slots:
  /*$PROTECTED_SLOTS$*/
  void slot_progress(QString str);
  void itemRenamed ( QListViewItem *item, int col );

signals: // Signals
  /** No descriptions */
  void sig_progress(QString str);
private:
    void fillLvLanguages();
    void updateLvLang(const eStreamType streamType,const QString & lang) ;
   long getFreeSpace(const QString & _path);
};

#endif

