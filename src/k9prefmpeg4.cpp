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

#include "k9common.h"
#include "k9prefmpeg4.h"
#include "k9config.h"
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <klocale.h>

k9prefMPEG4::k9prefMPEG4(QWidget* parent, const char* name, WFlags fl)
: prefMPEG4(parent,name,fl)
{
    load();
}

void k9prefMPEG4::load() {
    //KSimpleConfig settings("K9Copy");
    k9Config config;
    
    sbMp4Size->setValue(config.getPrefMp4Size());
    sbMp4Size->setSuffix(" "+ i18n("mb"));
    sbMp4NumberCD->setValue(config.getPrefMp4NumberCD());
    
    leMp4Width->setText(config.getPrefMp4Width());
    leMp4Height->setText(config.getPrefMp4Height());

    ckMp4AspectRatio->setChecked(config.getPrefMp4AspectRatio());
    leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());

    ck2passes->setChecked(config.getPrefMp42Passes());

    leMp4AudioBitrate->setText(config.getPrefMp4AudioBitrate());

    QStringList m_codecLabels=config.getCodecLabels();

    cbMp4Codec->clear();
    cbMp4Codec->insertItem("Xvid");
    cbMp4Codec->insertItem("lavc MPEG-4");
    cbMp4Codec->insertItem("x264");

    cbMp4Codec->insertStringList(m_codecLabels);
    cbMp4Codec->setCurrentItem(config.getPrefMp4Codec());

}


k9prefMPEG4::~k9prefMPEG4()
{
}

void k9prefMPEG4::save() {
    k9Config config;
    config.setPrefMp4Codec(cbMp4Codec->currentItem());
    config.setPrefMp4Size( (int)sbMp4Size->value());
    config.setPrefMp4NumberCD( (int)sbMp4NumberCD->value());
    config.setPrefMp4Width( leMp4Width->text());
    config.setPrefMp4Height(leMp4Height->text());
    config.setPrefMp4AspectRatio( ckMp4AspectRatio->isChecked());
    config.setPrefMp4AudioBitrate( leMp4AudioBitrate->text());
    config.setPrefMp42Passes(ck2passes->isChecked());
    config.save();
}

void k9prefMPEG4::ckMp4AspectRatioClick() {
    leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());
    if (ckMp4AspectRatio->isChecked()) leMp4Height->setText("");
}



/*$SPECIALIZATION$*/


#include "k9prefmpeg4.moc"

