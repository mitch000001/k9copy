//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9prefmencoder.h"
#include <qlistbox.h>
#include <qtextedit.h>
#include "k9mencodercmdgen.h"
#include <kpushbutton.h>
#include <kiconloader.h>
#include <klocale.h>

k9prefMencoder::k9prefMencoder(QWidget* parent, const char* name, WFlags fl)
: prefMencoder(parent,name,fl)
{
   m_update=false;
   lbvCodecs->clear();
   for (int i=0;i < m_codecs.count();i++) {
       lbvCodecs->insertItem(m_codecs.getCodecName(i));              
   }
   lbvCodecs->setCurrentItem(0);

   lbaCodecs->clear();
   for (int i=0;i < m_audioCodecs.count();i++) {
       lbaCodecs->insertItem(m_audioCodecs.getCodecName(i));              
   }
   lbaCodecs->setCurrentItem(0);
   btnOpt1->setPixmap(SmallIcon("configure"));
   btnOpt2->setPixmap(SmallIcon("configure"));
   btnOpt3->setPixmap(SmallIcon("configure"));
   btnOptA->setPixmap(SmallIcon("configure"));

}

void k9prefMencoder::lbvCodecsCurrentChanged(QListBoxItem *_item) {
   if (_item) {
        int i=lbvCodecs->currentItem();            
        m_update=true;
        leFourcc->setText(m_codecs.getFourcc(i));
        teOpt1->setText(m_codecs.getOptions0(i));
        teOpt2->setText(m_codecs.getOptions1(i));
        teOpt3->setText(m_codecs.getOptions2(i));
        leVLabel->setText(_item->text());
        m_update=false;
    }
}

void k9prefMencoder::lbaCodecsCurrentChanged(QListBoxItem *_item) {
   if (_item) {
        int i=lbaCodecs->currentItem();            
        m_update=true;
        teOptA->setText(m_audioCodecs.getOptions(i));
        leALabel->setText(_item->text());
        m_update=false;
    }
}

void k9prefMencoder::save()
{
   m_codecs.save();
   m_audioCodecs.save();

}

void k9prefMencoder::btnOpt1Clicked() {
   QString s=teOpt1->text();
   if( k9MencoderCmdGen::getMencoderOptions(s))
        teOpt1->setText(s);

}
void k9prefMencoder::btnOpt2Clicked() {
   QString s=teOpt2->text();
   if( k9MencoderCmdGen::getMencoderOptions(s))
        teOpt2->setText(s);

}
void k9prefMencoder::btnOpt3Clicked() {
   QString s=teOpt3->text();
   if( k9MencoderCmdGen::getMencoderOptions(s))
        teOpt3->setText(s);

}
void k9prefMencoder::btnOptAClicked() {
   QString s=teOptA->text();
   if( k9MencoderCmdGen::getMencoderOptions(s))
        teOptA->setText(s);

}

void k9prefMencoder::btnAddVClicked() {
   lbvCodecs->insertItem(i18n("new profile"));
   m_codecs.setOptions0(lbvCodecs->count()-1,"-ovc copy");
   m_codecs.setOptions1(lbvCodecs->count()-1,"-ovc copy");
   m_codecs.setOptions2(lbvCodecs->count()-1,"-ovc copy");
   m_codecs.setCodecName(lbvCodecs->count()-1,i18n("new profile"));
   lbvCodecs->setCurrentItem(lbvCodecs->count()-1);

}

void k9prefMencoder::btnDeleteVClicked() {
   m_codecs.remove(lbvCodecs->currentItem());
   lbvCodecs->removeItem(lbvCodecs->currentItem());
}

void k9prefMencoder::btnAddAClicked() {
   lbaCodecs->insertItem(i18n("new profile"));
   m_audioCodecs.setOptions (lbaCodecs->count()-1,"-oac copy");
   m_audioCodecs.setCodecName(lbaCodecs->count()-1,i18n("new profile"));
   lbaCodecs->setCurrentItem(lbaCodecs->count()-1);

}

void k9prefMencoder::btnDeleteAClicked() {
   m_audioCodecs.remove(lbaCodecs->currentItem());
   lbaCodecs->removeItem(lbaCodecs->currentItem());
}


void k9prefMencoder::teOptATextChanged() {
  if (!m_update)
      m_audioCodecs.setOptions(lbaCodecs->currentItem(),teOptA->text());
}

void k9prefMencoder::teOpt1TextChanged() {
  if (!m_update)
      m_codecs.setOptions0(lbvCodecs->currentItem(),teOpt1->text());
}

void k9prefMencoder::teOpt2TextChanged() {
   if (!m_update)
   m_codecs.setOptions1(lbvCodecs->currentItem(),teOpt2->text());

}

void k9prefMencoder::teOpt3TextChanged() {
   if (!m_update)
   m_codecs.setOptions2(lbvCodecs->currentItem(),teOpt3->text());

}


void k9prefMencoder::leVLabelTextChanged(const QString &_value) {
   if (!m_update) {
       lbvCodecs->changeItem(_value,lbvCodecs->currentItem());
       m_codecs.setCodecName(lbvCodecs->currentItem(),_value);
    }
}
void k9prefMencoder::leALabelTextChanged(const QString &_value) {
   if (!m_update) {
       lbaCodecs->changeItem(_value,lbaCodecs->currentItem());
       m_audioCodecs.setCodecName(lbaCodecs->currentItem(),_value);
    }
}

void k9prefMencoder::leFourccTextChanged(const QString &_value) {
    if (!m_update) {
        m_codecs.setFourcc(lbvCodecs->currentItem(),_value);
    }
}


k9prefMencoder::~k9prefMencoder()
{
}



/*$SPECIALIZATION$*/


#include "k9prefmencoder.moc"

