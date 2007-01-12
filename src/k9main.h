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
#include "k9mainw.h"
#include "kconfigdlg.h"
#include "k9playbackoptions.h"
#include <qstring.h>
#include <kmessagebox.h>
#include <qlistview.h>
#include <kmainwindow.h>
#include <kaboutdata.h>
#include <qevent.h>
#include <kfiledialog.h>
#include <qlistbox.h>
#include "k9copy.h"

enum  eStreamType {SUB,AUD,VID,NONE,CHAP} ;
enum  eObjectType {TITLES,CHAPTERS};

class LvItem : public QListViewItem {
public:
    LvItem( QListViewItem *parent,eObjectType _objectType)
            : QListViewItem( parent), obj( NULL ) {
        objectType=_objectType;
    }
    LvItem( QListView *parent,eObjectType _objectType)
            : QListViewItem( parent), obj( NULL ) {
	    objectType=_objectType;
    }
    eObjectType objectType;
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
class k9PlaybackOptions;
class k9LangSelect;


class ckLvItem : public QCheckListItem {
public:
    ckLvItem( QListViewItem *parent,k9Main *dlg)
            : QCheckListItem( parent,"",QCheckListItem::CheckBox) {
        mainDlg=dlg;
        obj=NULL;
        stream=NULL;
        streamType=NONE;
        language="";
    }
    ckLvItem( QListView *parent,k9Main *dlg)
            : QCheckListItem( parent,"",QCheckListItem::CheckBox) {
        mainDlg=dlg;
        obj=NULL;
        stream=NULL;
        streamType=NONE;
        language="";
    }
    eStreamType streamType;

    k9Main *mainDlg;
    k9DVDTitle *mainTitle;
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


class k9DVDListItem : public QObject {
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


class k9Main : public MainDlg {
    Q_OBJECT

public:
   
    k9Main(QWidget* parent = 0, const char* name = 0,  const QStringList &sl=0 );
    ~k9Main();
    /*$PUBLIC_FUNCTIONS$*/
    void addTitle(k9DVDTitle *track);
    void addChapters(QListViewItem *_parent,k9DVDTitle *_title);
    void updateSelection();
    void checkAll(bool state);
    void checkTS( bool _state,ckLvItem *_item );
    void checkTitle(bool state, ckLvItem *_item);
    void checkLang(QString lang, eStreamType streamType,bool state);
    bool getupdating();
    void saveSettings();
    void setDVDSize();
    static int compare(double v1,double v2);
    void readSettings();
    bool getquickScan() {return m_quickScan;};
    void setPlaybackOptions(k9PlaybackOptions *_value) { m_playbackOptions=_value;};
    void setLangSelect(k9LangSelect *_value) {m_langSelect=_value;};
    QObjectList *getItems() {return &items;};
    void updateFactor();
    bool withMenus();
    k9DVD *dvd;
    void eject();
private slots:
    virtual void          listView1CurrentChanged( QListViewItem * );
    virtual void          bSaveClick();
    virtual void	  cbOutputDevActivated(int);
    virtual void	  bInputOpenClick();
    virtual void	  bInputOpenDirClick();
    virtual void	  foundMountPoint (const QString &mountPoint, unsigned long kBSize, unsigned long kBUsed, unsigned long kBAvail);
    virtual void 	  fspDone();
public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void          PreviewTitle();
    virtual void          CreateMP4();
    virtual void          Copy();
    virtual void          Open();
    virtual void	   Clone(QString _input,QString _output);
    virtual void	   setInput(QString _input);
    virtual void 	   setOutput(QString _output);
protected:
    /*$PROTECTED_FUNCTIONS$*/
    QObjectList items;
    k9DVDListItem *addListItem(QObject *DVD,ckLvItem *List,eStreamType type);

    void readDrives();
    QPtrList<ckLvItem> tsItems;
    QPtrList<ckLvItem> chItems;
    ckLvItem * root;
    QObjectList driveList;
    QObjectList recorderList;
    kCDDrives drives;
    QPixmap pxVideo;
    QPixmap pxSound;
    QPixmap pxText;
    QPixmap pxChapter;
    bool updating;
    bool fspFinish;
    long fspAvail;
    void closeEvent( QCloseEvent* ce );
    void closeDVD();
    KLibFactory *m_factory;
    QString  getDevice(QComboBox *_combo);

    //PREFERENCES
    QString m_prefOutput;
    bool m_useDvdAuthor;
    bool m_quickScan;
    int  m_prefSize;
    bool m_prefK3b;
    bool m_prefMenu;
    bool m_prefAutoBurn;
    int  m_prefMp4Codec;
    int  m_prefMp4Size;
    int  m_prefMp4NumberCD;
    QString  m_prefMp4Width;
    QString  m_prefMp4Height;
    QString  m_prefMp4AudioBitrate;
    QStringList m_codecAudio;
    QStringList m_codecLabels;
    QStringList m_codecVideo;
    QString m_prefCodecAudio,m_prefCodecVideo,m_prefCodecLabel;
    k9PlaybackOptions *m_playbackOptions;
    k9LangSelect *m_langSelect;
protected slots:
    /*$PROTECTED_SLOTS$*/
    void slot_progress(QString str);
    void itemRenamed ( QListViewItem *item, int col );
signals: // Signals
    /** No descriptions */
    void sig_progress(QString str);
    void changeStatusbar(const QString& str,int id);
    void changeCaption(const QString& str);
    void showPreview(k9DVD *_dvd,k9DVDTitle * title,int chapter);
    void stopPreview();
    void SelectionChanged(k9DVD *_dvd,bool _withMenus);
    void changedTitle(k9DVDTitle *_title);
private:
    void fillLvLanguages();
    void updateLvLang(const eStreamType streamType,const QString & lang) ;
    long getFreeSpace(const QString & _path);
	
};

#endif

