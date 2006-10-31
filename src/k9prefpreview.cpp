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


#include "k9prefpreview.h"
#include <qwidgetstack.h>
#include <qradiobutton.h>
#include <ksimpleconfig.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <klocale.h>

k9prefPreview::k9prefPreview(QWidget* parent, const char* name, WFlags fl)
: prefPreview(parent,name,fl)
{ load();
}

k9prefPreview::~k9prefPreview()
{
}

/*$SPECIALIZATION$*/

void k9prefPreview::load() {
    KSimpleConfig settings("K9Copy");

    ckUseGL->setChecked(settings.readEntry("/options/useGL",0).toInt());
    cbVout->setCurrentItem(settings.readEntry("/mplayer/vout",0).toInt());
    cbAout->setCurrentItem(settings.readEntry("/mplayer/aout",0).toInt());
    rbMplayer->setChecked(settings.readEntry("/options/useMplayer",0).toInt());
    rbInternal->setChecked(!settings.readEntry("/options/useMplayer",0).toInt());
    if (rbMplayer->isChecked()) 
	rbMplayerClick();
    else
	rbInternalClick();

}



void k9prefPreview::save() {
    KSimpleConfig settings("K9Copy");

    if (rbMplayer->isChecked()) {
	settings.writeEntry("/mplayer/vout",cbVout->currentItem());
    	settings.writeEntry("/mplayer/aout",cbAout->currentItem());
	
    }else {
    	settings.writeEntry("/options/useGL",(int)ckUseGL->isChecked());

    }
    settings.writeEntry("/options/useMplayer",(int)rbMplayer->isChecked());
}


void k9prefPreview::rbInternalClick() {
  wsOptions->raiseWidget(0);
}

void k9prefPreview::rbMplayerClick() {
  wsOptions->raiseWidget(1);
}



#include "k9prefpreview.moc"

