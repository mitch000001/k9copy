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
#include "k9common.h"
#include "k9cddrive.h"
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
#include <qmutex.h>

enum  eStreamType {SUB,AUD,VID,NONE,CHAP} ;
enum  eObjectType {TITLE,CHAPTER,TITLESET,STREAM,ROOT};

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
class k9UpdateFactor;

class ckLvItem : public QCheckListItem {
public:
    ckLvItem( QListViewItem *parent,k9Main *dlg,eObjectType _objectType)
            : QCheckListItem( parent,"",QCheckListItem::CheckBox) {
        mainDlg=dlg;
        obj=NULL;
        stream=NULL;
        streamType=NONE;
        language="";
        objectType=_objectType;
    }
    ckLvItem( QListView *parent,k9Main *dlg,eObjectType _objectType)
            : QCheckListItem( parent,"",QCheckListItem::CheckBox) {
        mainDlg=dlg;
        obj=NULL;
        stream=NULL;
        streamType=NONE;
        language="";
        objectType=_objectType;
    }
    eStreamType streamType;
    eObjectType objectType;

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

    k9Main(QWidget* parent = 0, const char* name = 0,  k9CdDrives *_drives=0 );
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
    bool getquickScan() {
        return m_quickScan;
    };
    void setPlaybackOptions(k9PlaybackOptions *_value) {
        m_playbackOptions=_value;
    };
    void setLangSelect(k9LangSelect *_value) {
        m_langSelect=_value;
    };
    QObjectList *getItems() {
        return &items;
    };
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
    virtual void updateFactor_internal();
    virtual void	   deviceAdded(k9CdDrive *_drive);
    virtual void	   deviceRemoved(k9CdDrive *_drive);
    virtual void	   expanded(QListViewItem*);
    virtual void	   collapsed (QListViewItem*);
public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void          PreviewTitle();
    virtual void          CreateMP4();
    virtual void	  extractMPEG2();
    virtual void          Copy();
    virtual void          Open();
    virtual void	   Clone(QString _input,QString _output);
    virtual void	   setInput(QString _input);
    virtual void 	   setOutput(QString _output);
    virtual void 	  volumeChanged(const QString &device,const QString &volumeName);

    void setDrives(k9CdDrives* _value);

protected:
    /*$PROTECTED_FUNCTIONS$*/
    QObjectList items;
    k9DVDListItem *addListItem(QObject *DVD,ckLvItem *List,eStreamType type);

    void readDrives();
    void addDrive (k9CdDrive *_drive);
    k9Copy *m_parent;
    QPtrList<ckLvItem> tsItems;
    QPtrList<ckLvItem> chItems;
    ckLvItem * root;
    QPtrList <k9CdDrive> driveList;
    QPtrList <k9CdDrive> recorderList;
    k9CdDrives *drives;
    QPixmap pxVideo;
    QPixmap pxSound;
    QPixmap pxText;
    QPixmap pxChapter;
    QMutex m_mutex;
    bool updating;
    bool fspFinish;
    long fspAvail;
    void closeEvent( QCloseEvent* ce );
    void closeDVD();
    KLibFactory *m_factory;
    QString  getDevice(QComboBox *_combo);
    k9UpdateFactor *m_update;
    //PREFERENCES
    QString m_prefOutput;
    bool m_useDvdAuthor;
    bool m_quickScan;
    int  m_prefSize;
    bool m_prefK3b;
    bool m_prefMenu;
    bool m_prefAutoBurn;
    k9PlaybackOptions *m_playbackOptions;
    k9LangSelect *m_langSelect;

    KMdiToolViewAccessor *m_toolView;
    KDockWidget *m_dockWidget;
protected slots:
    /*$PROTECTED_SLOTS$*/
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
    void setProgressWindow(QWidget *_widget);
    void removeProgressWindow();


};

#endif

