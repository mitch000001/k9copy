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

#include "k9prefauthor.h"
#include "k9config.h"
#include <qcombobox.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>

k9prefAuthor::k9prefAuthor(QWidget* parent, const char* name, WFlags fl)
: prefAuthor(parent,name,fl)
{
    load();
}

void k9prefAuthor::load() {
    k9Config config;
    cbAudioFormat->setCurrentText(config.getPrefAudioFormat());
    cbAudioBitrate->setCurrentText(QString::number(config.getPrefAudioBitrate()));
    inButtonWidth->setValue(config.getPrefButtonWidth());
    inButtonHeight->setValue(config.getPrefButtonHeight());
    cbHiliteColor->setColor(config.getPrefButtonHiliteColor());
    cbTextColor->setColor(config.getPrefButtonTextColor());
    m_font=config.getPrefButtonFont();
}

k9prefAuthor::~k9prefAuthor()
{
}

/*$SPECIALIZATION$*/
void k9prefAuthor::bButtonFontClicked()
{
    KFontDialog::getFont( m_font );
}

void k9prefAuthor::save() {
    k9Config config;
    config.setPrefAudioFormat(cbAudioFormat->currentText());
    config.setPrefAudioBitrate(cbAudioBitrate->currentText().toInt());
    config.setPrefButtonWidth(inButtonWidth->value());
    config.setPrefButtonHeight(inButtonHeight->value());
    config.setPrefButtonHiliteColor(cbHiliteColor->color());
    config.setPrefButtonFont(m_font);
    config.setPrefButtonTextColor(cbTextColor->color());
    config.save();
}

#include "k9prefauthor.moc"

