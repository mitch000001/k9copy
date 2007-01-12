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
#include "k9config.h"
#include <qwidgetstack.h>
#include <qradiobutton.h>
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
    k9Config config;

    ckUseGL->setChecked(config.getUseGL());
    cbVout->setCurrentItem(config.getMplayerVout());
    cbAout->setCurrentItem(config.getMplayerAout());
    rbMplayer->setChecked(config.getUseMplayer());
    rbInternal->setChecked(!config.getUseMplayer());
    
    if (rbMplayer->isChecked()) 
	rbMplayerClick();
    else
	rbInternalClick();

}



void k9prefPreview::save() {
    k9Config config;

    if (rbMplayer->isChecked()) {
        config.setMplayerVout( cbVout->currentItem());
        config.setMplayerAout( cbAout->currentItem());	
    }else {
    	config.setUseGL( ckUseGL->isChecked());
    }
    config.setUseMplayer( rbMplayer->isChecked());
    config.save();
}


void k9prefPreview::rbInternalClick() {
  wsOptions->raiseWidget(0);
}

void k9prefPreview::rbMplayerClick() {
  wsOptions->raiseWidget(1);
}



#include "k9prefpreview.moc"

