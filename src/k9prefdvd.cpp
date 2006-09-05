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
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kurlrequester.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <klocale.h>

k9prefDVD::k9prefDVD(QWidget* parent, const char* name, WFlags fl)
: prefDVD(parent,name,fl)
{
    KSimpleConfig settings("K9Copy");
//    KStandardDirs kd;
    urOutput->setMode(2);
    urOutput->setURL(settings.readEntry("/dir/output",locateLocal("tmp","k9copy/",true)));//kd.findResource("tmp","")));
    ckK3b->setChecked(settings.readEntry("/options/usek3b",0).toInt());
    ckUseGL->setChecked(settings.readEntry("/options/useGL",0).toInt());

    ckAutoBurn->setChecked(settings.readEntry("/options/autoburn",0).toInt());
    ckQuickScan->setChecked(settings.readEntry("/options/quickscan","1").toInt());
    sbSize->setValue(settings.readEntry("/options/dvdsize",QString("4400")).toInt());
    sbSize->setSuffix(" "+ i18n("mb"));
}

k9prefDVD::~k9prefDVD()
{
}

/*$SPECIALIZATION$*/

void k9prefDVD::save() {
    KSimpleConfig settings("K9Copy");
    settings.writeEntry("/dir/output",urOutput->url());
    settings.writeEntry("/options/usek3b",(int)ckK3b->isChecked());
    settings.writeEntry("/options/useGL",(int)ckUseGL->isChecked());

    settings.writeEntry("/options/autoburn",(int)ckAutoBurn->isChecked());
    settings.writeEntry("/options/dvdsize",(int)sbSize->value());
    settings.writeEntry("/options/quickscan",(int)ckQuickScan->isChecked());
}

#include "k9prefdvd.moc"

