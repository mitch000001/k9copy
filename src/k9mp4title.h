//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MP4TITLE_H
#define K9MP4TITLE_H

#include "prefMPEG4.h"
#include "k9titleencopt.h"


class k9DVDTitle;
class k9DVD;
class k9MP4Title : public prefMPEG4
{
  Q_OBJECT

public:
  k9MP4Title(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9MP4Title();
  /*$PUBLIC_FUNCTIONS$*/
private:
  k9TitleEncOpt *m_titleEncOpt;
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          ckMp4AspectRatioClick();
  virtual void load();
  virtual void titleChanged(k9DVDTitle *_title);
  virtual void selectionChanged(k9DVD *_dvd,bool);
protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void cbMp4CodecActivated( int );
  virtual void ck2passesClicked();
  virtual void sbMp4SizeValueChanged( int );
  virtual void sbMp4NumberCDValueChanged( int );
  virtual void leMp4WidthTextChanged( const QString & );
  virtual void leMp4HeightTextChanged( const QString & );
  virtual void leMp4AudioBitrateTextChanged( const QString & );

};

#endif

