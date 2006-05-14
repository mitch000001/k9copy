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


#include "k9prefmpeg4.h"
#include <ksimpleconfig.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>

k9prefMPEG4::k9prefMPEG4(QWidget* parent, const char* name, WFlags fl)
: prefMPEG4(parent,name,fl)
{
    load();
}

void k9prefMPEG4::load() {
    KSimpleConfig settings("K9Copy");

    sbMp4Size->setValue(settings.readEntry("/mp4/size",QString("700")).toInt());
    sbMp4NumberCD->setValue(settings.readEntry("/mp4/numberCD",QString("1")).toInt());
    
    leMp4Width->setText(settings.readEntry("/mp4/width","640"));
    leMp4Height->setText(settings.readEntry("/mp4/height",""));

    ckMp4AspectRatio->setChecked(settings.readEntry("/mp4/aspectratio",0).toInt());
    leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());


    leMp4AudioBitrate->setText(settings.readEntry("/mp4/audiobitrate","128"));

    QStringList m_codecLabels=settings.readListEntry("mencoder/labels");

    cbMp4Codec->clear();
    cbMp4Codec->insertItem("Xvid");
    cbMp4Codec->insertItem("lavc MPEG-4");

    cbMp4Codec->insertStringList(m_codecLabels);
    cbMp4Codec->setCurrentItem(settings.readEntry("/mp4/codec",0).toInt());

}


k9prefMPEG4::~k9prefMPEG4()
{
}

void k9prefMPEG4::save() {
    KSimpleConfig settings("K9Copy");
    settings.writeEntry("/mp4/codec",cbMp4Codec->currentItem());
    settings.writeEntry("/mp4/size",(int)sbMp4Size->value());
    settings.writeEntry("/mp4/numberCD",(int)sbMp4NumberCD->value());
    settings.writeEntry("/mp4/width",leMp4Width->text());
    settings.writeEntry("/mp4/height",leMp4Height->text());
    settings.writeEntry("/mp4/aspectratio",(int)ckMp4AspectRatio->isChecked());
    settings.writeEntry("/mp4/audiobitrate",leMp4AudioBitrate->text());

}

void k9prefMPEG4::ckMp4AspectRatioClick() {
    leMp4Height->setEnabled(!ckMp4AspectRatio->isChecked());
    if (ckMp4AspectRatio->isChecked()) leMp4Height->setText("");
}



/*$SPECIALIZATION$*/


#include "k9prefmpeg4.moc"

