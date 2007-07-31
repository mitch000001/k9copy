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

#ifndef K9PREFMPEG4_H
#define K9PREFMPEG4_H

#include "prefMPEG4.h"
#include "k9common.h"
class k9prefMPEG4 : public prefMPEG4
{
  Q_OBJECT

public:
  k9prefMPEG4(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9prefMPEG4();
  /*$PUBLIC_FUNCTIONS$*/
  void save();
  void load();
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void ckMp4AspectRatioClick();
  virtual void rgVideoSizeChanged();

  virtual void cbMp4CodecActivated( int ){};
  virtual void cbMp4AudioCodecActivated( int ){};

  virtual void ck2passesClicked(){};
  virtual void sbMp4SizeValueChanged( int ){};
  virtual void sbAudioGainValueChanged( int ){};

  virtual void sbMp4NumberCDValueChanged( int ) {};
  virtual void leMp4WidthTextChanged( const QString & ) {};
  virtual void leMp4HeightTextChanged( const QString & ) {};
  virtual void leMp4AudioBitrateTextChanged( const QString & ) {};
  virtual void leMp4VideoBitrateTextChanged( const QString & ) {};

  virtual void ckUseCacheClick() {};


};

#endif

