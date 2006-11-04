//
// C++ Implementation: k9settings
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9settings.h"

#include <qframe.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qlayout.h>

k9settings::k9settings(QWidget *parent,const QString &caption):  KDialogBase (IconList,caption,
        Ok|Apply|Cancel, Ok) {

    setInitialSize(QSize(750,350), false);

    QFrame *frDevices;
    frDevices=addPage (i18n("Devices"),i18n("Devices not detected by k9copy"),KGlobal::iconLoader()->loadIcon("blockdevice", KIcon::Panel, KIcon::SizeMedium));
    QGridLayout *grid = new QGridLayout(frDevices, 0, 0, 10 );
    m_configDlg=new kConfigDlg(frDevices);
    grid->addWidget(m_configDlg,0,0);


    QFrame *frDVD;
    frDVD=addPage (i18n("DVD"),i18n("DVD Backup"),KGlobal::iconLoader()->loadIcon("dvdcopy", KIcon::Panel, KIcon::SizeMedium));
    grid = new QGridLayout(frDVD, 0, 0, 10 );
    m_prefDVD =new k9prefDVD(frDVD);
    grid->addWidget(m_prefDVD,0,0);

    //TODO afficher uniquement si mencoder et mplayer installés

    QFrame *frMPEG4;
    frMPEG4=addPage (i18n("MPEG-4"),i18n("MPEG-4 Encoding"),KGlobal::iconLoader()->loadIcon("mp4", KIcon::Panel, KIcon::SizeMedium));
    grid = new QGridLayout(frMPEG4, 0, 0, 10 );
    m_prefMPEG4 =new k9prefMPEG4(frMPEG4);
    grid->addWidget(m_prefMPEG4,0,0);

    QFrame *frMencoder;
    frMencoder=addPage (i18n("MEncoder"),i18n("MPEG-4 Codecs"),KGlobal::iconLoader()->loadIcon("mencoder", KIcon::Panel, KIcon::SizeMedium));
    grid = new QGridLayout(frMencoder, 0, 0, 10 );
    m_prefMencoder =new k9prefMencoder(frMencoder);
    grid->addWidget(m_prefMencoder,0,0);

    QFrame *frPreview;
    frPreview=addPage (i18n("Preview"),i18n("Title preview"),KGlobal::iconLoader()->loadIcon("mplayer", KIcon::Panel, KIcon::SizeMedium));
    grid = new QGridLayout(frPreview, 0, 0, 10 );
    m_prefPreview =new k9prefPreview(frPreview);
    grid->addWidget(m_prefPreview,0,0);


    connect(this, SIGNAL(okClicked()), SLOT(slotOkClicked()));
    connect(this, SIGNAL(applyClicked()), SLOT(slotApplyClicked()));
}

void k9settings::slotOkClicked() {
    m_configDlg->save();
    m_prefDVD->save();
    m_prefMPEG4->save();
    m_prefMencoder->save();
    m_prefPreview->save();
}

void k9settings::slotApplyClicked() {
   slotOkClicked();
  
   m_prefMPEG4->load();
}

k9settings::~k9settings() {
    delete m_configDlg;
    delete m_prefDVD;
    delete m_prefMPEG4;
    delete m_prefMencoder;
    delete m_prefPreview;
}

#include "k9settings.moc"
