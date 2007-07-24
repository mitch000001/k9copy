//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9common.h"
#include "k9mp4title.h"
#include "k9config.h"
#include "k9titleencopt.h"
#include "k9dvdtitle.h"
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <klocale.h>
#include <qlabel.h>
#include <qframe.h>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include "k9videocodecs.h"
#include "k9audiocodecs.h"

k9MP4Title::k9MP4Title(QWidget* parent, const char* name, WFlags fl)
: prefMPEG4(parent,name,fl)
{
    m_titleEncOpt=NULL;
    load();
    tabWidget->setEnabled(false);
    lTitle->setText(i18n("Select a Title in the treeview..."));

}

k9MP4Title::~k9MP4Title()
{
}


void k9MP4Title::load() {
    //KSimpleConfig settings("K9Copy");
    k9VideoCodecs *v=new k9VideoCodecs(0,0);
    v->save();
    delete v;
    k9AudioCodecs *a=new k9AudioCodecs(0,0);
    a->save();
    delete a;


    k9Config config;
    
    sbMp4Size->setValue(config.getPrefMp4Size());
    sbMp4Size->setSuffix(" "+ i18n("MB"));
    sbMp4NumberCD->setValue(config.getPrefMp4NumberCD());
    sbAudioGain->setValue(config.getPrefMp4AudioGain());

    
    leMp4Width->setText(config.getPrefMp4Width());
    leMp4Height->setText(config.getPrefMp4Height());

    ckMp4AspectRatio->setChecked(config.getPrefMp4AspectRatio());
    leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());

    ck2passes->setChecked(config.getPrefMp42Passes());

    leMp4AudioBitrate->setText(config.getPrefMp4AudioBitrate());
    leMp4VideoBitrate->setText(config.getPrefMp4VideoBitrate());

    QStringList m_codecLabels=config.getCodecLabels();
    QStringList m_codecLabelsAudio=config.getCodecLabelsAudio();

    cbMp4Codec->clear();

    cbMp4Codec->insertStringList(m_codecLabels);
    cbMp4Codec->setCurrentItem(config.getPrefMp4Codec());

    cbMp4AudioCodec->clear();
    cbMp4AudioCodec->insertStringList(m_codecLabelsAudio);
    cbMp4AudioCodec->setCurrentItem(config.getPrefMp4AudioCodec());

    cbMp4AudioCodec->setCurrentItem(config.getPrefMp4AudioCodec());
    if(config.getPrefMp4VideoBitrate() =="") 
	rbSize->setChecked(true);
    else
	rbBitrate->setChecked(true);
}


/*$SPECIALIZATION$*/
void k9MP4Title::ckMp4AspectRatioClick()
{
  leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());
   if (m_titleEncOpt)
   	m_titleEncOpt->setKeepAspectRatio(ckMp4AspectRatio->isChecked());

}

void k9MP4Title::selectionChanged(k9DVD *_dvd,bool) {
   if (_dvd==NULL)
	m_titleEncOpt=NULL;
   if (m_titleEncOpt)
   	tabWidget->setEnabled(m_titleEncOpt->getTitle()->isSelected());;


}


void k9MP4Title::titleChanged( k9DVDTitle *_title) {
    if (_title==NULL) {
    	m_titleEncOpt=NULL;
    	tabWidget->setEnabled(false);
    	lTitle->setText(i18n("Select a Title in the treeview..."));
    }
    else {
        lTitle->setText(_title->getname());
    	tabWidget->setEnabled( _title->isSelected());
	m_titleEncOpt=_title->getEncOpt();
	k9TitleEncOpt *opt=m_titleEncOpt;
	sbMp4Size->setValue(opt->getMaxSize());
	sbMp4NumberCD->setValue(opt->getNumParts() );
	sbAudioGain->setValue(opt->getAudioGain() );
	
	leMp4Width->setText(opt->getWidth() );
	leMp4Height->setText(opt->getHeight());
	
	ckMp4AspectRatio->setChecked(opt->getKeepAspectRatio());
	leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());
	
	ck2passes->setChecked(opt->get2Passes());
	
	leMp4AudioBitrate->setText(opt->getAudioBr());
	leMp4VideoBitrate->setText(opt->getVideoBr());
	cbMp4Codec->setCurrentItem(opt->getCodec() );   
        cbMp4AudioCodec->setCurrentItem(opt->getAudioCodec());

	if(opt->getVideoBr() =="") 
		rbSize->setChecked(true);
	else
		rbBitrate->setChecked(true);
    }
   leMp4VideoBitrate->setEnabled(rbBitrate->isChecked());
   sbMp4Size->setEnabled(rbSize->isChecked());

}


void k9MP4Title::cbMp4CodecActivated( int _value) {
   if (m_titleEncOpt)
   	m_titleEncOpt->setCodec(_value);
}

void k9MP4Title::cbMp4AudioCodecActivated( int _value) {
   if (m_titleEncOpt)
   	m_titleEncOpt->setAudioCodec(_value);
}

void k9MP4Title::ck2passesClicked() {
   if (m_titleEncOpt)
   	m_titleEncOpt->set2Passes(ck2passes->isChecked());
}

void k9MP4Title::sbMp4SizeValueChanged( int _value) {
   if (m_titleEncOpt)   
   	m_titleEncOpt->setMaxSize(_value);
}

void k9MP4Title::sbMp4NumberCDValueChanged( int _value ) {
   if (m_titleEncOpt)   	
   	m_titleEncOpt->setNumParts(_value);
}

void k9MP4Title::sbAudioGainValueChanged( int _value ) {
   if (m_titleEncOpt)   	
   	m_titleEncOpt->setAudioGain(_value);
}

void k9MP4Title::leMp4WidthTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setWidth(_value);
}

void k9MP4Title::leMp4HeightTextChanged( const QString & _value) {
   if (m_titleEncOpt)
        m_titleEncOpt->setHeight(_value);
}

void k9MP4Title::leMp4AudioBitrateTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setAudioBr(_value);
}


void k9MP4Title::leMp4VideoBitrateTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setVideoBr(_value);
}


void k9MP4Title::rgVideoSizeChanged(){
   if (rbSize->isChecked()) 
	leMp4VideoBitrate->setText("");
   leMp4VideoBitrate->setEnabled(rbBitrate->isChecked());
   sbMp4Size->setEnabled(rbSize->isChecked());
}

#include "k9mp4title.moc"

