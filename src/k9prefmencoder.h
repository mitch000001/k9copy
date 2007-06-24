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

#ifndef K9PREFMENCODER_H
#define K9PREFMENCODER_H

#include "k9common.h"
#include "prefMencoder.h"
#include "k9videocodecs.h"
#include "k9audiocodecs.h"
#include <qstringlist.h>
#include <qstring.h>
#include <qlineedit.h>

class k9prefMencoder : public prefMencoder
{
  Q_OBJECT

public:
  k9prefMencoder(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9prefMencoder();
  /*$PUBLIC_FUNCTIONS$*/
  void save(void);
public slots:
  /*$PUBLIC_SLOTS$*/
   void teOptATextChanged();

   void teOpt1TextChanged();
   void teOpt3TextChanged();
   void teOpt2TextChanged();
   void leVLabelTextChanged(const QString &);
   void leALabelTextChanged(const QString &);
   void leFourccTextChanged(const QString &);
private:
    k9VideoCodecs m_codecs;
    k9AudioCodecs m_audioCodecs;
    bool m_update;
protected slots:
  /*$PROTECTED_SLOTS$*/
  void lbvCodecsCurrentChanged(QListBoxItem *_item);
  void lbaCodecsCurrentChanged(QListBoxItem *_item);
  void btnOpt1Clicked();
  void btnOpt2Clicked();
  void btnOpt3Clicked();
  void btnOptAClicked();
  void btnAddVClicked();
  void btnDeleteVClicked();
  void btnAddAClicked();
  void btnDeleteAClicked();

};


#endif

