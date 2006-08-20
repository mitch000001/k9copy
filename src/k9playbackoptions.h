//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9PLAYBACKOPTIONS_H
#define K9PLAYBACKOPTIONS_H
#include "k9common.h"
#include "playbackoptionsw.h"
#include "k9dvd.h"
#include "k9main.h"
#include "k9dvdtitle.h"
#include <qcheckbox.h>
#include <qlistbox.h>

class lbItem : public QListBoxText {
protected:
    k9DVDTitle *m_title;
public:

    lbItem(QListBox * listbox, const QString & text = QString::null ):
    QListBoxText(listbox,text) {}
    ;
    void setTitle(k9DVDTitle  *_value) {
        m_title = _value;
    }
    k9DVDTitle *getTitle() const {
        return m_title;
    }	
};

class k9Main;

class k9PlaybackOptions : public PlaybackOptionsw
{
  Q_OBJECT

public:
  k9PlaybackOptions(k9Main *mainWindow,QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9PlaybackOptions();
  /*$PUBLIC_FUNCTIONS$*/
  void fillTitleList();
  k9DVD  *m_dvd;
  k9Main *m_main;
  void setDvd(k9DVD* _value) {m_dvd = _value;}
  void setMain(k9Main* _value);
  void setSequence();
  void titleRenamed (k9DVDTitle *_title, QString _newName);
  bool withMenus() ;
  void setwithMenus(bool _value);
  void enable(bool _value);
  void clear();
public slots:
  /*$PUBLIC_SLOTS$*/
    virtual void          ckMenuClick();

protected:
  /*$PROTECTED_FUNCTIONS$*/
    QPtrList <k9DVDAudioStream> lstAudioDef;
    QPtrList <k9DVDSubtitle> lstSubDef;

protected slots:
  /*$PROTECTED_SLOTS$*/
    virtual void 	  bSeqUpClick();
    virtual void	  bSeqDownClick();
    virtual void	  cbDefAudioActivated(int _index);
    virtual void	  cbDefSubActivated(int _index);
    virtual void 	  lbSequenceChanged (QListBoxItem * _item);
};

#endif

