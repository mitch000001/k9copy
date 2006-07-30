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


#include "k9langselect.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9main.h"

k9LangSelect::k9LangSelect(k9Main *mainWindow,QWidget* parent, const char* name, WFlags fl)
: langSelect(parent,name,fl)
{
    setMain( mainWindow);
    m_main->setLangSelect( this);
}

k9LangSelect::~k9LangSelect()
{
}

/*$SPECIALIZATION$*/

void ckLvLangItem::stateChange(bool state) {
    if (mainDlg->getupdating())
        return;
    mainDlg->checkLang(language,streamType,state);
    mainDlg->updateSelection();
//    k9DVD *d = mainDlg->dvd;
    mainDlg->setDVDSize();
    mainDlg->updateFactor();
    //(mainDlg->factor)->setValue((int) (d->getfactor(mainDlg->withMenus(),true )*100)-100 );
}

void k9LangSelect::setMain(k9Main* _value) {
    m_main = _value;
    m_dvd=m_main->dvd;
}

void k9LangSelect::fillLvLanguages() {
    lvLanguages->clear();
    langAudItems.clear();
    langSubItems.clear();
    QStringList slAudLang,slSubLang;
    if (m_dvd->getopened()) {
        k9DVDSubtitle *l_sub;
        k9DVDAudioStream *l_auds;
	QObjectList *items=m_main->getItems();
        for (uint i=0;i<items->count();i++) {
            k9DVDListItem *litem=(k9DVDListItem*)items->at(i);
            switch (litem->streamType) {
            case SUB: {
                    l_sub=litem->subtitle;
                    if (slSubLang.contains(l_sub->getlanguage())==0)
                        slSubLang.append(l_sub->getlanguage());
                    break;
                }
            case AUD: {
                    l_auds=litem->audioStream;
                    if (slAudLang.contains(l_auds->getlanguage())==0)
                        slAudLang.append(l_auds->getlanguage());
                    break;
                }
	    default:
		break;
            }
        }
    }
    QListViewItem *audioRoot = new QListViewItem(lvLanguages);
    audioRoot->setOpen( TRUE );
    audioRoot->setText(0, i18n("Audio"));
    for ( QStringList::Iterator it = slAudLang.begin(); it != slAudLang.end(); ++it ) {
        ckLvLangItem *lvitem=new ckLvLangItem(audioRoot,m_main);
        lvitem->setText(0,*it);
        lvitem->streamType=AUD;
        lvitem->language=*it;
        langAudItems.append(lvitem);
    }

    QListViewItem *subRoot = new QListViewItem(lvLanguages);
    subRoot->setOpen( TRUE );
    subRoot->setText(0, i18n("Subtitles"));
    for ( QStringList::Iterator it = slSubLang.begin(); it != slSubLang.end(); ++it ) {
        ckLvLangItem *lvitem=new ckLvLangItem(subRoot,m_main);
        lvitem->setText(0,*it);
        lvitem->streamType=SUB;
        lvitem->language=*it;
        langSubItems.append(lvitem);
    }

}

void k9LangSelect::update() {
    for (uint i=0;i<langAudItems.count();i++)
        updateLvLang(AUD,langAudItems.at(i)->language);
    for (uint i=0;i<langSubItems.count();i++)
        updateLvLang(SUB,langSubItems.at(i)->language);
}


void k9LangSelect::updateLvLang(const eStreamType streamType,const QString & lang ) {
    uint Total=0,Selected=0;
    QString lg;
    QObjectList *items = m_main->getItems();
    for (uint i=0;i<items->count();i++) {
        k9DVDListItem *litem=(k9DVDListItem*)items->at(i);
        if (litem->streamType==streamType ) {
            switch (streamType) {
            case SUB :
                lg= litem->subtitle->getlanguage();
                if (lg==lang) {
                    Total++;
                    if (litem->listItem->isOn())
                        Selected ++;
                }
                break;
            case AUD :
                lg=litem->audioStream->getlanguage();
                if (lg==lang) {
                    Total++;
                    if (litem->listItem->isOn())
                        Selected++;
                }
                break;
	    default:
		break;
            }
        }
    }
    if (streamType==AUD) {
        for (ckLvLangItem  *langItem = langAudItems.first(); langItem; langItem = langAudItems.next() ) {
            if (langItem->language ==lang) {
                if (Selected==Total) {
                    langItem->setOn(true);
                } else
                    langItem->setOn(false);
            }
        }
    }
    if (streamType==SUB) {
        for (ckLvLangItem  *langItem = langSubItems.first(); langItem; langItem = langSubItems.next() ) {
            if (langItem->language ==lang) {
                if (Selected==Total) {
                    langItem->setOn(true);
                } else
                    langItem->setOn(false);
            }
        }
    }

}

void k9LangSelect::selectAll( bool _state) {
    for (ckLvLangItem  *langItem = langAudItems.first(); langItem; langItem = langAudItems.next() ) {
        langItem->setOn(_state);
    }
    for (ckLvLangItem  *langItem = langSubItems.first(); langItem; langItem = langSubItems.next() ) {
        langItem->setOn(_state);
    }
}

void k9LangSelect::clear() {
    langAudItems.clear();
    langSubItems.clear(); 
    lvLanguages->clear();
}
#include "k9langselect.moc"

