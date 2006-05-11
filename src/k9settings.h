//
// C++ Interface: k9settings
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SETTINGS_H
#define K9SETTINGS_H

#include "k9common.h"
#include "kconfigdlg.h"
#include "k9prefdvd.h"
#include "k9prefmpeg4.h"
#include "k9prefmencoder.h"
#include <kdialogbase.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9settings:public KDialogBase{
Q_OBJECT
public:
    k9settings(QWidget *parent,const QString &caption);

    virtual ~k9settings();

private:
    kConfigDlg *m_configDlg;
    k9prefDVD *m_prefDVD;
    k9prefMPEG4 *m_prefMPEG4;
    k9prefMencoder *m_prefMencoder;

private slots:
    void slotOkClicked();
    void slotApplyClicked();
};

#endif
