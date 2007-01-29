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


#include "k9playbackoptions.h"
#include "k9config.h"
#include <kpushbutton.h>
#include <kiconloader.h>

k9PlaybackOptions::k9PlaybackOptions(k9Main *mainWindow, QWidget* parent, const char* name, WFlags fl)
: PlaybackOptionsw(parent,name,fl)
{
    
    setMain( mainWindow);
    m_main->setPlaybackOptions(this);
    enable(false);
    bSeqUp->setPixmap(SmallIcon("1uparrow"));
    bSeqDown->setPixmap(SmallIcon("1downarrow"));
    k9Config config;
    ckMenu->setChecked(config.getKeepMenus());
    ckMenuClick();

}

k9PlaybackOptions::~k9PlaybackOptions()
{
}

/*$SPECIALIZATION$*/
 void k9PlaybackOptions::setMain(k9Main* _value) { 
     m_main = _value; m_dvd=m_main->dvd; 
}


void k9PlaybackOptions::fillTitleList() {
    QString txt=lbSequence->currentText();

    lbSequence->clear();
    k9DVDTitle *title=m_dvd->getstart();
  
    bool found=title !=NULL;
    //Reconstitution de l'ordre de lecture à partir des titres du DVD
    while (found) {
        if (title->isSelected()) {
	        lbItem *item=new lbItem(lbSequence,title->getname());
		item->setTitle(title);
		if (title->getname()==txt) 
		lbSequence->setCurrentItem(item);
	}
	title=title->getnextTitle();
	found=title!=NULL;	
    }

    
    for (int i=0;i<m_dvd->gettitleCount();i++) {
        k9DVDTitle* l_track=m_dvd->gettitle(i);
        if (l_track->getIndexed()) {
            QString sTitle=l_track->getname();
            if(l_track->isSelected()) {
   		bool foundtitle=false;
		for (uint j=0; j<lbSequence->count();j++) {
		   lbItem *tmp=(lbItem*)lbSequence->item(j);
		   if (tmp->getTitle() == l_track)
			foundtitle=true;
		}	
		if (!foundtitle) {
			lbItem *item=new lbItem(lbSequence,sTitle);
			item->setTitle(l_track);
			if (sTitle==txt) 
			lbSequence->setCurrentItem(item);
		}
            }
        }
    }
    if (lbSequence->currentItem()==-1)
	lbSequence->setCurrentItem(0);
    setSequence();
    enable(!ckMenu->isChecked() );

}

void k9PlaybackOptions::ckMenuClick() {
     
    if (m_main!=NULL) {
        enable((!ckMenu->isChecked()) && m_dvd->getopened());
 	m_main->updateFactor();
    } else
	enable(false);
}

void k9PlaybackOptions::bSeqUpClick() {
    int cur=lbSequence->currentItem();
    if (cur >0) {
        lbItem *lbi=(lbItem*)lbSequence->item(cur);
        lbSequence->takeItem(lbi);
        lbSequence->insertItem(lbi,cur-1);
        lbSequence->setCurrentItem(lbi);
    }
    setSequence();
}

void k9PlaybackOptions::bSeqDownClick() {
    uint cur=lbSequence->currentItem();
    if (cur <lbSequence->count()) {
        lbItem *lbi=(lbItem*)lbSequence->item(cur);
        lbSequence->takeItem(lbi);
        lbSequence->insertItem(lbi,cur+1);
        lbSequence->setCurrentItem(lbi);
    }
    setSequence();
}

void k9PlaybackOptions::setSequence() {
    for (int i=0;i <m_dvd->gettitleCount();i++)
	m_dvd->gettitle(i)->setnextTitle( NULL);

    lbItem *lbi = (lbItem*)lbSequence->item(lbSequence->topItem());
    if (lbi !=NULL) {
	lbItem *lbi2;
	m_dvd->setstart(lbi->getTitle());
	for (uint i=0 ; i < lbSequence->count()-1;i++) {
		lbi=(lbItem*)lbSequence->item(i);
		lbi2=(lbItem*)lbSequence->item(i+1);
		lbi->getTitle()->setnextTitle(lbi2->getTitle());
	}
    }
}

void k9PlaybackOptions::lbSequenceChanged(QListBoxItem *_item) {
    if (_item == NULL)
	return;
    lbItem *lbi=(lbItem*) _item;
    k9DVDTitle *title=lbi->getTitle();
    cbDefAudio->clear();
    cbDefSub->clear();
    lstAudioDef.clear();
    lstSubDef.clear();
    cbDefAudio->insertItem(i18n("default"));
    lstAudioDef.append(NULL);
    cbDefSub->insertItem(i18n("none"));
    lstSubDef.append(NULL);

    for (int i=0;i < title->getaudioStreamCount();i++) {
	if (title->getaudioStream(i)->getselected()) {
//		if ( !title->getDefAudioSet())
//		   title->setDefAudio(title->getaudioStream(i));
		cbDefAudio->insertItem(QString("%1 - %2").arg(title->getaudioStream(i)->getID()).arg(title->getaudioStream(i)->getlanguage()));
		if (title->getaudioStream(i)==title->getDefAudio()) {
		   cbDefAudio->setCurrentItem(cbDefAudio->count()-1);
		}
		lstAudioDef.append(title->getaudioStream(i));
	}
    }

    for (int i=0;i < title->getsubPictureCount();i++) {
	if (title->getsubtitle(i)->getselected()) {
		cbDefSub->insertItem(QString("%1 - %2").arg(title->getsubtitle(i)->getID()).arg(title->getsubtitle(i)->getlanguage()));
		if (title->getsubtitle(i)==title->getDefSubtitle()) {
		   cbDefSub->setCurrentItem(cbDefSub->count()-1);
		}
		lstSubDef.append(title->getsubtitle(i));
	}
    }

}

void k9PlaybackOptions::cbDefAudioActivated(int _index) {
    int cur=lbSequence->currentItem();
    lbItem *lbi=(lbItem*)lbSequence->item(cur);
    lbi->getTitle()->setDefAudio(lstAudioDef.at(_index));
}
 
void k9PlaybackOptions::cbDefSubActivated(int _index) {
    int cur=lbSequence->currentItem();
    lbItem *lbi=(lbItem*)lbSequence->item(cur);
    lbi->getTitle()->setDefSubtitle(lstSubDef.at(_index));
}

void k9PlaybackOptions::titleRenamed (k9DVDTitle *_title, QString _newName) {
    for (uint j=0; j<lbSequence->count();j++) {
	    lbItem *tmp=(lbItem*)lbSequence->item(j);
	     if (tmp->getTitle() == _title) {
		lbItem *item=new lbItem(NULL,_newName);
		item->setTitle(_title);
		lbSequence->changeItem(item,j);
		break;
	     }
	}	

}

bool k9PlaybackOptions::withMenus() {
    return ckMenu->isChecked();
}

void k9PlaybackOptions::setwithMenus(bool _value) {
    ckMenu->setChecked(_value);

}


void k9PlaybackOptions::enable(bool _value) {
    frPlayback->setEnabled(_value);
}

void k9PlaybackOptions::clear() {
    lbSequence->clear();
    enable(false);
}

#include "k9playbackoptions.moc"

