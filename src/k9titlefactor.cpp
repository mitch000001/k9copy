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


#include "k9titlefactor.h"
#include "k9cellcopylist.h"
#include "k9dvdsize.h"
#include <qgroupbox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <klocale.h>



k9TitleFactor::k9TitleFactor(QWidget* parent, const char* name, WFlags fl)
        : titleFactor(parent,name,fl) {
    m_current=NULL;
    m_slFactorPressed=false;
    m_factor=-1;
    m_dvd=NULL;
    m_updating=false;
    SelectionChanged( NULL,m_withMenus);
    m_useDvdAuthor=false;
}

k9TitleFactor::~k9TitleFactor() {}

/*$SPECIALIZATION$*/
void k9TitleFactor::setUseDvdAuthor( bool _value) {
   m_useDvdAuthor=_value;
}

float k9TitleFactor::getFactor() {
    if (m_factor==-1)
	    m_factor=m_dvd->getfactor( m_withMenus,false,m_useDvdAuthor);
    return m_factor;	
}

void k9TitleFactor::ckAutoClicked(bool checked) {
    if (m_current!=NULL && !m_updating) {
        m_current->setforceFactor(checked);
        if ( checked) {
	    m_current->setfactor( getFactor());
            slFactor->setValue(m_current->getfactor()*100.0);
            setMinFactor();
        } else {
            slFactor->setValue(getFactor()*100.0);
            slFactor->setMinValue(100);
        }

    }
}

void k9TitleFactor::slFactorReleased() {
    m_slFactorPressed=false;
}

void k9TitleFactor::slFactorPressed() {
    if (m_current !=NULL)  {
        m_current->setforceFactor(true);
        setMinFactor();
    }
    m_slFactorPressed=true;

}

void k9TitleFactor::slFactor_valueChanged( int ) {
    if (m_current !=NULL ) {
	if (m_slFactorPressed) {
		m_current->setfactor( (double)slFactor->value() /100);
		for (int i=0;i <m_current->getTitles().count();i++) {
			k9DVDTitle * title=m_current->getTitles().at(i);
			title->setfactor( (double)slFactor->value() /100);
		}
		m_factor=-1;
	}
	if (m_current->getforceFactor())	
        	tFactor->setText(QString("%1").arg(m_current->getfactor(),4,'f',2));
	else
		tFactor->setText(QString("%1").arg(getFactor(),4,'f',2));
    }

}

void k9TitleFactor::changedTitle(k9DVDTitle *_title) {
	if (_title ==NULL) {
		gbTitle->hide();
		lbTitle->setText(i18n("Select a Title in the treeview..."));		
		return;
	}
	
	gbTitle->show();
	m_current=_title;
	
	m_updating=true;
	gbTitle->setChecked(m_current->getforceFactor());
	m_updating=false;
	
	lbTitle->setText(QString(i18n("Shrink Factor for %1")).arg(m_current->getname()));
	if (m_current->getforceFactor()) {
		slFactor->setValue(m_current->getfactor()*100.0);
		tFactor->setText(QString("%1").arg(m_current->getfactor(),4,'f',2));        
		setMinFactor();
	} else {
		slFactor->setValue(getFactor()*100.0);
		tFactor->setText(QString("%1").arg(getFactor(),4,'f',2));
		slFactor->setMinValue(100);
	}
		
}


void k9TitleFactor::setMinFactor() {
    k9CellCopyList *lst=new k9CellCopyList(m_dvd->getdvd(),m_dvd);
    double minFactor=lst->getMinFactor( true); //TODO : corriger pour copie sans menus
    slFactor->setMinValue(minFactor*100);

    delete lst;
}

void k9TitleFactor::SelectionChanged( k9DVD *_dvd,bool _withMenus) {
    m_dvd=_dvd;
    if (_dvd==NULL) 
	    m_current=NULL;
    m_factor=-1;
    m_withMenus=_withMenus;

    changedTitle( m_current);
    
}


#include "k9titlefactor.moc"

