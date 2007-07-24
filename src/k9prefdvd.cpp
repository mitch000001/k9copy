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


#include "k9prefdvd.h"
#include "k9config.h"
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <klocale.h>

k9prefDVD::k9prefDVD(QWidget* parent, const char* name, WFlags fl)
: prefDVD(parent,name,fl)
{
    k9Config config;
    urOutput->setMode(2);
    urOutput->setURL(config.getPrefOutput());
    ckK3b->setChecked(config.getPrefK3b());

    ckAutoBurn->setChecked(config.getPrefAutoBurn());
    ckQuickScan->setChecked(config.getQuickScan());
    ckDvdAuthor->setChecked(config.getUseDvdAuthor());
    ckDelTmpFiles->setChecked(config.getPrefDelTmpFiles());
    sbSize->setValue(config.getPrefSize());
    sbSize->setSuffix(" "+ i18n("MB"));
}

k9prefDVD::~k9prefDVD()
{
}

/*$SPECIALIZATION$*/

void k9prefDVD::save() {
    k9Config config;
    config.setPrefOutput( urOutput->url());
    config.setPrefK3b(ckK3b->isChecked());
    config.setPrefAutoBurn( ckAutoBurn->isChecked());
    config.setPrefSize(sbSize->value());
    config.setQuickScan( ckQuickScan->isChecked());
    config.setUseDvdAuthor( ckDvdAuthor->isChecked());    
    config.setPrefDelTmpFiles(ckDelTmpFiles->isChecked());
    config.save();
}

#include "k9prefdvd.moc"

