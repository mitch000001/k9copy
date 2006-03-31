/***************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   jm_petit@laposte.net                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "k9main.h"
#include "k9common.h"
#include "k9dvdauthor.h"
#include "kburndvd.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9dvdbackup.h"

#include <kselect.h>
#include <kcombobox.h>
#include <qtextbrowser.h>
#include <qapplication.h>
#include <qlistview.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <qdir.h>
#include <kstandarddirs.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <qaction.h>
#include <ktoolbar.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qfont.h>
#include <klibloader.h>
#include <kdirselectdialog.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kdiskfreesp.h>
#include <qvaluelist.h>
#include <kdeversion.h>

k9Main::k9Main(QWidget* parent, const char* name, const QStringList &sl)
        : MainDlg(parent,name),pxVideo((const char **) img_video ),
        pxSound((const char **) img_sound),
pxText((const char **) img_text) {

    m_factory = KLibLoader::self()->factory("libk9copy");
    if (m_factory)      {
//        dvd=static_cast<k9DVD  *>(m_factory->create(this,"dvd", "k9DVD"));
    }
   dvd=new k9DVD(this);
    //    dvd=new k9DVD;

    updating=false;

    items.setAutoDelete(true);
    listView1->setDefaultRenameAction(QListView::Accept);
    KStandardDirs kd;
    urOutput->setURL(kd.findResource("tmp",""));
    urOutput->setMode(2);
    readDrives();
    readSettings();
    frPlayback->setEnabled(false);
    lbTitle->setEnabled(false);
    cbTitle->setEnabled(false);
    ckMenuClick();
    bInputOpen->setPixmap(SmallIcon("fileopen"));
    bInputOpenDir->setPixmap(SmallIcon("folder_open"));
}

k9DVDListItem::k9DVDListItem(QObject *DVD,ckLvItem *List,eStreamType type) {
    switch(type) {
    case AUD :
        audioStream=(k9DVDAudioStream*)DVD;
        subtitle=NULL;
        title=NULL;
        break;
    case SUB :
        subtitle = (k9DVDSubtitle*)DVD;
        audioStream=NULL;
        title=NULL;
        break;
    case VID :
        subtitle=NULL;
        audioStream=NULL;
        title =(k9DVDTitle*)DVD;
        break;
    }
    listItem= List;
    streamType=type;
}

k9DVDListItem *k9Main::addListItem(QObject *DVD,ckLvItem *List,eStreamType type) {
    k9DVDListItem *litem;
    litem=new k9DVDListItem(DVD,List,type);
    items.append(litem);
    return litem;
}

int LvItem::rtti() const {
    return(1000);
}


int ckLvItem::rtti() const {
    return(1001);
}

int ckLvItem::compare ( QListViewItem * i, int col, bool ascending ) const {
    ckLvItem *litem = (ckLvItem*)i;
    k9DVDTitleset *titleset1,*titleset2;
    ckLvItem *l;
    uint id,id2;
    if (col==0) {
        switch(depth()) {
        case 1:
            titleset1=(k9DVDTitleset*)obj;
            titleset2=(k9DVDTitleset*)litem->obj;
            return titleset1->getnum() -titleset2->getnum();
        case 3:
	    l=(ckLvItem*)this;
	    id=streamType*100;
	    id2=litem->streamType*100;

	    if (streamType==AUD) {
		id+=((k9DVDAudioStream*)l->stream)->getID();
	   }
	    if (streamType==SUB) {
		id+=((k9DVDSubtitle*)l->stream)->getID();
	   }

	    if (litem->streamType==AUD) {
		id2+=((k9DVDAudioStream*)litem->stream)->getID();
	   }
	    if (litem->streamType==SUB) {
		id2+=((k9DVDSubtitle*)litem->stream)->getID();
	   }


            k9Main::compare(id,id2);
        }
    }

    if (col ==1) {
        switch (depth()) {
        case 1:
            titleset1=(k9DVDTitleset*)obj;
            titleset2=(k9DVDTitleset*)litem->obj;
            return k9Main::compare(titleset1->getsize(),titleset2->getsize());
            break;
        case 3:
            double size1,size2;
            l =(ckLvItem*)this;
            size1=l->getstreamSize();
            size2=litem->getstreamSize();
            return k9Main::compare(size1,size2);
            break;
        }
    }
    return QCheckListItem::compare(i,col,ascending);
}

double ckLvItem::getstreamSize() {
    double size;
    k9DVDTitle *title;
    switch (streamType) {
    case SUB: {
        k9DVDSubtitle *sub=(k9DVDSubtitle*)stream;
        size=sub->getsize_mb();
        break;
	}
    case AUD:{
        k9DVDAudioStream *aud=(k9DVDAudioStream*)stream;
        size=aud->getsize_mb();
        break;
	}
    case VID: {
        title=(k9DVDTitle*)obj;
        size=title->getvideosize_mb();
        break;
	}
    default:
        size=0;
        break;
    }
    return size;
}


int LvItem::compare ( QListViewItem * i, int col, bool ascending ) const {
    LvItem *litem = (LvItem*)i;
    k9DVDTitle *title1,*title2;
    if (col ==1 && depth()==2) {
        title1=(k9DVDTitle*)obj;
        title2=(k9DVDTitle*)litem->obj;
        return k9Main::compare(title1->getsize_mb(),title2->getsize_mb());
    }
    if (col==0 && depth()==2) {
        title1=(k9DVDTitle*)obj;
        title2=(k9DVDTitle*)litem->obj;
        return k9Main::compare(title1->getpgc(),title2->getpgc());
    }

    return QListViewItem::compare(i,col,ascending);
}

void ckLvItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) {

    if (column==1) {
        p->eraseRect(0,0,width,height());
        QFont f(p->font());

        switch (depth()) {
        case 1: {
                k9DVDTitleset *titleset=(k9DVDTitleset*)obj;
                f.setBold(true);
                p->setFont(f);
                p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,titleset->getsize_mb()+" mb");
                break;
            }
        case 3: {
		if ( ! mainDlg->ckQuickScan->isChecked()) {
			double size=getstreamSize();
			QString c;
			c.sprintf("%.2f mb",size);
			p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,c);
		}
                break;
            }
        default:
            QCheckListItem::paintCell(p,cg,column,width,align);
        }
    } else
        QCheckListItem::paintCell(p,cg,column,width,align);

}

void LvItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) {

    if (column==1 && depth()==2) {
        p->eraseRect(0,0,width,height());
        k9DVDTitle * title=(k9DVDTitle*)obj;
        QString c;
        c.sprintf("%.2f mb",title->getsize_mb());
        p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,c);
    } else
        QListViewItem::paintCell(p,cg,column,width,align);
}

void ckLvItem::stateChange(bool state) {
    if (mainDlg !=NULL) {
        if (mainDlg->getupdating())
            return;
        switch (depth()) {
        case 0 :
            mainDlg->checkAll(state);
            break;
        case 1:
            mainDlg->checkTS(state,this);
            break;
        default :
            mainDlg->checkTitle(state,this);
            break;
        }
        k9DVD *d = mainDlg->dvd;
        mainDlg->setDVDSize();
        (mainDlg->factor)->setValue( (d->getfactor(mainDlg->ckMenu->isChecked(),true )*100)-100 );
    }
}

void ckLvLangItem::stateChange(bool state) {
    if (mainDlg->getupdating())
        return;
    mainDlg->checkLang(language,streamType,state);
    mainDlg->updateSelection();
    k9DVD *d = mainDlg->dvd;
    mainDlg->setDVDSize();
    (mainDlg->factor)->setValue( (d->getfactor(mainDlg->ckMenu->isChecked(),true )*100)-100 );

}

bool k9Main::getupdating() {
    return updating;
}


void k9Main::Copy() {
    QString c,filename;
    if (!dvd->getopened()) {
        KMessageBox::error( this, i18n("DVD is not opened"), i18n("DVD Copy"));
        return;
    }
    setDVDSize();
    updateSelection();

    if (cbOutputDev->currentItem() ==0)  {
        filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
	if (filename =="")
		return;
    }

   if (getFreeSpace( urOutput->url()) <  sbSize->value()) {
	if (KMessageBox::warningContinueCancel (this, i18n("Insuffisant disk space on %1\n%2 mb expected.").arg(urOutput->url()).arg(sbSize->value()),i18n("DVD Copy"))==KMessageBox::Cancel)
		return;
   }

    bool burn=false;
    if (ckMenu->isChecked()) {
        //copy with k9DVDBackup
        //k9DVDBackup *backup = static_cast<k9DVDBackup  *>(m_factory->create(dvd,"backup", "k9DVDBackup"));
        k9DVDBackup *backup=new k9DVDBackup(dvd,"backup");
        backup->setOutput(urOutput->url());
        backup->setDevice(dvd->getDevice());
        backup->execute();
        burn=backup->getErrMsg()=="";
        delete backup;
    } else {
        //k9DVDAuthor *b=static_cast<k9DVDAuthor  *>(m_factory->create(dvd,"dvdauthor", "k9DVDAuthor"));
        k9DVDAuthor *b=new k9DVDAuthor(dvd,"dvdauthor");
        b->setworkDir(urOutput->url());
        b->author();
        if (!b->getError())
            burn=true;
        delete b;
    }

    if (burn) {
        kBurnDVD b;
        b.setworkDir(urOutput->url());
        b.setUseK3b(ckK3b->isChecked());
        b.setAutoBurn(ckAutoBurn->isChecked());
        b.setvolId(dvd->getDVDTitle());
	b.setSpeed( cbBurnSpeed->currentText());
        if (cbOutputDev->currentItem() !=0) {
            kCDDrive * drive=(kCDDrive*)recorderList.at(cbOutputDev->currentItem()-1);
            b.setburnDevice(drive->device);
            closeDVD();
            if (dvd->getDevice()== drive->device)
                drives.eject(dvd->getDevice());
        } else
            b.makeIso(filename);
        b.burn();
    }


}

void k9Main::bDevicesClick() {
    kConfigDlg wconfigDlg;
    wconfigDlg.exec();
    readDrives();
}

QString  k9Main::getDevice(QComboBox *_combo) {
    int index=-1;
    for (uint i=0; i<_combo->count();i++) {
	   QString t =_combo->text(i);
	   if (_combo->text(i) == _combo->currentText())
		index=i;
    }
    QString res="";
    if ((index==-1) || (_combo->currentText() ==i18n("ISO Image"))) {
	res=_combo->currentText();
#if KDE_VERSION >= KDE_MAKE_VERSION(3,4,0)
        KURL url=KURL::fromPathOrURL(res);
	//KIO::mount(true,0,res,"",true);
	KIO::UDSEntry entry;
	//first call to mount the device
	KIO::NetAccess::stat(KURL::fromPathOrURL(res+"/."),entry,0);
	if (KIO::NetAccess::stat(url,entry,0) ){
 		KIO::UDSEntry::iterator it;
    		for ( it = entry.begin(); it != entry.end(); ++it ) {
			if ( (*it).m_uds== KIO::UDS_LOCAL_PATH)
 				res=(*it).m_str;
		}
	}
#endif
    }
    else {
	kCDDrive * drive=(kCDDrive*)driveList.at(index);
	res=drive->device;
   }
   return res;
}

void k9Main::Open() {
    int i;
    k9DVDTitle * l_track;
    QString t;
    QTime h;
    connect(this, SIGNAL(sig_progress(QString)), this, SLOT(slot_progress(QString)));
    connect(listView1,SIGNAL(itemRenamed(QListViewItem*,int)),this,SLOT(itemRenamed(QListViewItem *,int)));


    closeDVD();
    dvd->scandvd(getDevice(cbInputDev),ckQuickScan->isChecked());
    if (dvd->geterror()) {
        KMessageBox::error( this, dvd->geterrMsg(), i18n("Open DVD"));
        return;
    }

    listView1->clear();
    items.clear();
    tsItems.clear();

    root = new ckLvItem (listView1,this );
    root->setOpen( TRUE );
    root->setText(0, dvd->getDVDTitle());
    root->setRenameEnabled(0,true);
    root->obj=NULL;
    root->streamType=NONE;

    for (i=0;i<dvd->gettitlesetCount();i++) {
        ckLvItem *tsItem = new ckLvItem(root,this);
        tsItem->setOpen(TRUE);
        QString c;
        c=i18n("Titleset %1").arg(i+1);
        tsItem->setText(0,c);
        tsItem->setText(1,"   "+dvd->gettitleset(i)->getsize_mb() +" mb");
        tsItem->obj=dvd->gettitleset(i) ;
        tsItem->streamType=NONE;
        tsItem->setRenameEnabled(0,false);
        tsItems.append(tsItem);
    }

    for (i=0;i<dvd->gettitleCount();i++) {
        l_track=dvd->gettitle(i);
        if ((l_track->getIndexed()))
            addTitle(l_track);

        h=l_track->getlength();
    }

    fillTitleList();
    fillLvLanguages();
    frPlayback->setEnabled(!ckMenu->isChecked() );
    /*  if(index==0)
       	dvd->setstart(NULL);
       else
       	dvd->setstart(dvd->gettitle(index-1));
    */
    k9DVDTitle *track=dvd->getlongestTitle();
    for (i =0 ; i < cbStart->count();i++) {
        if (dvd->gettitleByNum(i)->getnumTitle()== track->getnumTitle()) {
            cbStart->setCurrentItem(i);
            break;
        }
    }
    dvd->setstart(track);
    listView1->setSorting(0,true);
}

void k9Main::setDVDSize() {
    //k9DVDSize *dvdsize=static_cast<k9DVDSize  *>(m_factory->create(this,"dvd", "k9DVDSize"));
    k9DVDSize *dvdsize=new k9DVDSize(this);
    dvdsize->setMaxSizeDyn(sbSize->value());
    delete dvdsize;
}


/** No descriptions */
void k9Main::slot_progress(QString str) {
    //  log->setText(log->text() + "\n" +str);
    //  qApp->processEvents();

}



k9Main::~k9Main() {
    delete dvd;

}

void k9Main::closeEvent( QCloseEvent* ce ) {
    saveSettings();
    ce->accept();
}

/** No descriptions */
void k9Main::addTitle(k9DVDTitle *track) {
    const int col1 =0;
    const int col2 =1;
    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    int i;
    QString c,ch;
    listView1->setRootIsDecorated(true);

    LvItem * itemTrack = new LvItem( tsItems.at(track->getVTS()-1));
    itemTrack->setOpen( false );
    itemTrack->setText(col1,track->getname());
    itemTrack->setRenameEnabled(0,true);
    c.sprintf("%.2f mb", track->getsize_mb());
    itemTrack->setText(col2,c);
    itemTrack->obj=track;

    ckLvItem *video;
    video=new ckLvItem( itemTrack,this);
    video->streamType=VID;
    addListItem(track,video,VID);
    video->setOpen( false );
    c=i18n("video %1 ").arg(track->getformat());
    c.append (" - " + track->getlength().toString("h:mm:ss"));
    video->setText(col1, c);
    c.sprintf("%.2f mb",  track->getvideosize_mb());
    video->setText(col2,c);
    video->setPixmap(col1,pxVideo);
    video->obj=track;
    lvItems.append(video);


    for (i=0;i< track->getaudioStreamCount();i++) {
        l_auds=track->getaudioStream(i);
        c=i18n("audio %1 ").arg(i+1);
        c.append( l_auds->getlanguage() + " " +l_auds->getformat()+" ");
        ch.sprintf("%dch ",l_auds->getchannels());
        c.append(ch+l_auds->getfrequency()+" "+l_auds->getcontent()+" "+l_auds->getquantization());
        ckLvItem * item = new ckLvItem( itemTrack,this);
        item->streamType=AUD;
        item->language=l_auds->getlanguage();
        addListItem(l_auds,item,AUD);
        item->setText( col1,  c );
        c.sprintf("%.2f mb",  l_auds->getsize_mb());
        item->setText( col2,c);
        item->setPixmap(col1,pxSound);
        item->obj=track;
        item->stream=l_auds;
    }
    for (i=0;i< track->getsubPictureCount();i++) {
        l_sub=track->getsubtitle(i);
        c=i18n("subpicture %1 ").arg(i+1);
        c.append( l_sub->getlanguage());
        ckLvItem * item = new ckLvItem( itemTrack,this);
        item->streamType=SUB;
        item->language=l_sub->getlanguage();
        addListItem(l_sub,item,SUB);
        item->setText( col1,  c );
        c.sprintf("%.2f mb",  l_sub->getsize_mb());
        item->setText( col2,c);
        item->setPixmap(col1,pxText);
        item->obj=track;
        item->stream=l_sub;
    }
}
/** No descriptions */
void k9Main::updateSelection() {
    int i;
    k9DVDListItem *litem;
    for (i=0;i<items.count();i++) {
        litem=(k9DVDListItem*)items.at(i);
        switch (litem->streamType) {
        case SUB:
            k9DVDSubtitle *l_sub;
            l_sub=litem->subtitle;
            l_sub->setselected(litem->listItem->isOn());
            break;
        case AUD:
            k9DVDAudioStream *l_auds;
            l_auds=litem->audioStream;
            l_auds->setselected(litem->listItem->isOn());
            break;
        case VID :
            k9DVDTitle *l_title;
            l_title=litem->title;
            l_title->setforceSelection(litem->listItem->isOn());
            break;
        }

    }
}

void k9Main::checkLang(QString lang,eStreamType streamType,bool state) {
    updating =true;
    for (uint i=0;i<items.count();i++) {
        bool checked=false;
        k9DVDListItem *litem=(k9DVDListItem*)items.at(i);
        if (litem->streamType == streamType ) {
            switch (litem->streamType) {
            case SUB:
                k9DVDSubtitle *l_sub;
                l_sub=litem->subtitle;
                if (l_sub->getlanguage() == lang) {
                    l_sub->setselected(state);
                    litem->listItem->setOn(state);
                    checked=state;

                    k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
                    ckLvItem * itemtitleset=(ckLvItem*)litem -> listItem->parent()->parent();
                    itemtitleset->setOn(title->gettitleset()->getselected());
                }
                break;
            case AUD:
                k9DVDAudioStream *l_auds;
                l_auds=litem->audioStream;
                if (l_auds->getlanguage() == lang) {
                    l_auds->setselected(state);
                    litem->listItem->setOn(state);
                    checked=state;

                    k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
                    ckLvItem * itemtitleset=(ckLvItem*)litem -> listItem->parent()->parent();
                    itemtitleset->setOn(title->gettitleset()->getselected());
                }
                break;
            }
            //we must check the video item
            if (checked) {
                for (uint j=0; j<items.count();j++) {
                    k9DVDListItem *litem2=(k9DVDListItem*)items.at(j);
                    if (litem2->listItem->parent()==litem->listItem->parent() && litem2->streamType==VID ) {
                        litem2->title->setforceSelection(state);
                        litem2->listItem->setOn(state);
                    }
                }
            }
        }
    }

    updating=false;
}

void k9Main::checkTitle(bool state, ckLvItem *_item) {

    k9DVDListItem *litem;
    updating=true;
    for (uint i=0;i<items.count();i++) {
        litem=(k9DVDListItem*)items.at(i);
        if (litem->listItem->parent() == _item->parent()) {
            //if current item is video and is unchecked, uncheck audio and subtitles
            if (!state && _item->streamType==VID) {
                switch (litem->streamType) {
                case SUB:
                    k9DVDSubtitle *l_sub;
                    l_sub=litem->subtitle;
                    l_sub->setselected(state);
                    litem->listItem->setOn(state);
                    break;
                case AUD:
                    k9DVDAudioStream *l_auds;
                    l_auds=litem->audioStream;
                    l_auds->setselected(state);
                    litem->listItem->setOn(state);
                    break;
                }
            } else if ( litem->streamType == VID && state &&  (_item->streamType==SUB || _item->streamType==AUD)) {
                litem->title->setforceSelection(state);
                litem->listItem->setOn(state);
            }
        }
    }
    updateSelection();

    for (uint i=0;i<langAudItems.count();i++)
        updateLvLang(AUD,langAudItems.at(i)->language);
    for (uint i=0;i<langSubItems.count();i++)
        updateLvLang(SUB,langSubItems.at(i)->language);

    k9DVDTitleset * titleset=NULL;
    k9DVDTitle *title=(k9DVDTitle*) _item->obj;
    titleset=title->gettitleset();

    if (titleset!=NULL) {
        ckLvItem * itemtitleset=(ckLvItem*)_item->parent()->parent();
        itemtitleset->setOn(titleset->getselected());
    }
    updating=false;
}

void k9Main::updateLvLang(const eStreamType streamType,const QString & lang ) {
    uint Total=0,Selected=0;
    QString lg;
    for (uint i=0;i<items.count();i++) {
        k9DVDListItem *litem=(k9DVDListItem*)items.at(i);
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


void k9Main::checkAll(bool state) {
    k9DVDListItem *litem;
    updating=true;
    for (uint i=0;i<items.count();i++) {
        litem=(k9DVDListItem*)items.at(i);
        switch (litem->streamType) {
        case SUB:
            litem->listItem->setOn(state);
            break;
        case AUD:
            litem->listItem->setOn(state);
            break;
        case VID:
            k9DVDTitle * l_title;
            l_title=litem->title;
//            l_title->setforceSelection(state);
            litem->listItem->setOn(state);

//            k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
            ckLvItem * itemtitleset=(ckLvItem*)litem -> listItem->parent()->parent();
	    l_title->gettitleset()->setselected(state);
            itemtitleset->setOn(l_title->gettitleset()->getselected());
        }
    }

    for (ckLvLangItem  *langItem = langAudItems.first(); langItem; langItem = langAudItems.next() ) {
        langItem->setOn(state);
    }
    for (ckLvLangItem  *langItem = langSubItems.first(); langItem; langItem = langSubItems.next() ) {
        langItem->setOn(state);
    }
    updateSelection();
    updating=false;

}

void k9Main::checkTS( bool _state,ckLvItem *_item ) {
    k9DVDTitleset *titleset = (k9DVDTitleset*)_item;
    titleset->setselected(_state);
    k9DVDListItem *litem;
    updating=true;
    for (uint i=0;i<items.count();i++) {
        litem=(k9DVDListItem*)items.at(i);
        if (litem->listItem->parent ()->parent() == _item) {
            litem->listItem->setOn(_state);
        }
    }
    for (uint i=0;i<langAudItems.count();i++)
        updateLvLang(AUD,langAudItems.at(i)->language);
    for (uint i=0;i<langSubItems.count();i++)
        updateLvLang(SUB,langSubItems.at(i)->language);

    updateSelection();

    updating=false;

}


/** No descriptions */
void k9Main::itemRenamed(QListViewItem * item,int col) {
    k9DVDTitle *t=NULL;
    QString newText;
    if (item->rtti()==1000) {
        LvItem *it = (LvItem*) item;
        t=(k9DVDTitle*)it->obj;
        newText=it->text(0);
    } else if (item->rtti()==1001) {
        ckLvItem *ckit = (ckLvItem*) item;
        t=(k9DVDTitle*)ckit->obj;
        newText=ckit->text(0);
    }

    QString c;
    if (t !=NULL) {
        //QMessageBox::critical( this, "test", c.sprintf("%d",it->tag));
        t->setname(newText);
        cbTitle->changeItem(t->getname(),t->getnumTitle());
        cbStart->changeItem(t->getname(),t->getnumTitle()-1);
    } else
        dvd->setDVDTitle(item->text(0));
}

/** No descriptions */
void k9Main::readSettings() {
    KSimpleConfig settings("K9Copy");
    KStandardDirs kd;
    urOutput->setURL(settings.readEntry("/dir/output",kd.findResource("tmp","")));
    cbInputDev->setCurrentItem(settings.readEntry("/dev/input",0).toInt());
    cbOutputDev->setCurrentItem(settings.readEntry("/dev/output",0).toInt());
    ckK3b->setChecked(settings.readEntry("/options/usek3b",0).toInt());
    ckMenu->setChecked(settings.readEntry("/options/keepMenus",0).toInt());

    ckAutoBurn->setChecked(settings.readEntry("/options/autoburn",0).toInt());
    ckQuickScan->setChecked(settings.readEntry("/options/quickscan",0).toInt());
    sbSize->setValue(settings.readEntry("/options/dvdsize",QString("4400")).toInt());
    
     //fill the burn speed combo
    cbOutputDevActivated( cbOutputDev->currentItem());
}
/** No descriptions */
void k9Main::saveSettings() {
    KSimpleConfig settings("K9Copy");
    settings.writeEntry("/dir/output",urOutput->url());
    settings.writeEntry("/dev/input",cbInputDev->currentItem());
    settings.writeEntry("/dev/output",cbOutputDev->currentItem());
    settings.writeEntry("/options/usek3b",(int)ckK3b->isChecked());
    settings.writeEntry("/options/keepMenus",(int)ckMenu->isChecked());
    settings.writeEntry("/options/autoburn",(int)ckAutoBurn->isChecked());
    settings.writeEntry("/options/dvdsize",(int)sbSize->value());
    settings.writeEntry("/options/quickscan",(int)ckQuickScan->isChecked());
}
/** No descriptions */
void k9Main::bSaveClick() {
    saveSettings();
}

/** No descriptions */
void k9Main::PreviewTitle() {
    if( listView1->selectedItem()==NULL)
        return;
    if (listView1->selectedItem()->depth() <2)
	return;
    int rtti=listView1->selectedItem()->rtti();
    QObject *obj;
    if (rtti==1000) {
        LvItem *it=(LvItem*)listView1->selectedItem();
        obj=it->obj;
    } else {
        ckLvItem *it=(ckLvItem*)listView1->selectedItem();
        obj=it->obj;
    }
    if (obj !=NULL) {
        k9DVDTitle *t=(k9DVDTitle*)obj;

        //JMPkCDDrive * drive=(kCDDrive*)driveList.at(cbInputDev->currentItem());
        viewer.show();
        viewer.open(getDevice(cbInputDev),t);
    }
}


/*!
    \fn k9Main::readDrives()
 */
void k9Main::readDrives() {
    cbInputDev->clear();
    cbOutputDev->clear();
    driveList.clear();
    recorderList.clear();
    drives.scanDrives();

    cbOutputDev->insertItem(i18n("ISO Image"));

    for (int i=0; i<drives.count();i++) {
        if (drives.getDrive(i)->canReadDVD) {
            QString c(drives.getDrive(i)->name + "  ("+drives.getDrive(i)->device+")");
            cbInputDev->insertItem(c,-1);
            driveList.append(drives.getDrive(i));
        }
        if (drives.getDrive(i)->canWriteDVD) {
            cbOutputDev->insertItem(drives.getDrive(i)->name+ "  ("+drives.getDrive(i)->device+")",-1);
            recorderList.append(drives.getDrive(i));
        }

    }

}


/*!
    \fn k9Main::fillTitleList(QComboBox* cbTitle,bool isStart)
 */
void k9Main::fillTitleList() {
    cbStart->clear();
    cbTitle->clear();
    // cbStart->insertItem(i18n("Main menu"));
    cbTitle->insertItem(i18n("Next title"));
    for (int i=0;i<dvd->gettitleCount();i++) {
        k9DVDTitle* l_track=dvd->gettitle(i);
        if (l_track->getIndexed()) {
            QString sTitle=l_track->getname();
            cbStart->insertItem(sTitle);
            cbTitle->insertItem(sTitle);
        }
    }
}


/*!
    \fn k9Main::cbStartActivated (int index)
 */
void k9Main::cbStartActivated (int index) {
    dvd->setstart(dvd->gettitleByNum(index));

}


/*!
    \fn k9Main::cbTitleActivated(int index)
 */
void k9Main::cbTitleActivated(int index) {
    QListViewItem* curItem=listView1->currentItem();
    if (curItem->depth()==3)
        curItem=curItem->parent();
    LvItem * lvi=(LvItem*)curItem;
    k9DVDTitle *t=(k9DVDTitle*)lvi->obj;
    if (index==0)
        t->setnextTitle(NULL);
    else
        t->setnextTitle(dvd->gettitleByNum(index-1));

}


/*!
    \fn k9Main::listView1CurrentChanged( QListViewItem *newItem )
 */
void k9Main::listView1CurrentChanged( QListViewItem *newItem ) {
    if (newItem == NULL) {
        frPlayback->setEnabled(false);
    } else {
        if (!ckMenu->isChecked()) {
            LvItem*lvi =NULL;
            if (newItem->rtti()==1000)
                lvi=(LvItem*)newItem;
            frPlayback->setEnabled(true);
            switch(newItem->depth()) {
            case 0 :
                //root of the listview
                lbTitle->setEnabled(false);
                cbTitle->setEnabled(false);
                break;
            case 3 :
                //audio or subpicture entry
                lvi=(LvItem*) newItem->parent();
            case 2 :
                //the title
                lbTitle->setEnabled(true);
                cbTitle->setEnabled(true);
                break;
            }
            //get the associated dvd title
            if (lvi!=NULL)  {
                k9DVDTitle *t=(k9DVDTitle*)lvi->obj;
                if (t !=NULL) {
                    if (t->getnextTitle() ==NULL) {
                        cbTitle->setCurrentItem(0);
                    } else {
                        for (int i=1;i<cbTitle->count();i++) {
                            if (t->getnextTitle()->getnumTitle() == dvd->gettitleByNum(i-1)->getnumTitle()) {
                                cbTitle->setCurrentItem(i);
                                break;
                            }
                        }

                    }
                }
            }
        }
    }

}

void k9Main::ckMenuClick() {
    frPlayback->setEnabled((!ckMenu->isChecked()) && dvd->getopened());
    if (dvd->getopened()) {
        updateSelection();
        setDVDSize();
        factor->setValue( ( dvd->getfactor(ckMenu->isChecked(),true )*100)-100 );
    }
}
void k9Main::closeDVD() {
    listView1->clear();
    lvLanguages->clear();
    langAudItems.clear();
    langSubItems.clear();
    items.clear();
    frPlayback->setEnabled(false);
    lbTitle->setEnabled(false);
    cbTitle->setEnabled(false);
    dvd->close();

}

int k9Main::compare(double v1,double v2) {
	if (v1 <v2)
		return -1;
	if (v1>v2)
		return 1;
	if (v1==v2)
		return 0;
}

void k9Main::fillLvLanguages() {
    lvLanguages->clear();
    langAudItems.clear();
    langSubItems.clear();
    QStringList slAudLang,slSubLang;
    if (dvd->getopened()) {
        k9DVDSubtitle *l_sub;
        k9DVDAudioStream *l_auds;
        for (uint i=0;i<items.count();i++) {
            k9DVDListItem *litem=(k9DVDListItem*)items.at(i);
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
            }
        }
    }
    QListViewItem *audioRoot = new QListViewItem(lvLanguages);
    audioRoot->setOpen( TRUE );
    audioRoot->setText(0, i18n("Audio"));
    for ( QStringList::Iterator it = slAudLang.begin(); it != slAudLang.end(); ++it ) {
        ckLvLangItem *lvitem=new ckLvLangItem(audioRoot,this);
        lvitem->setText(0,*it);
        lvitem->streamType=AUD;
        lvitem->language=*it;
        langAudItems.append(lvitem);
    }

    QListViewItem *subRoot = new QListViewItem(lvLanguages);
    subRoot->setOpen( TRUE );
    subRoot->setText(0, i18n("Subtitles"));
    for ( QStringList::Iterator it = slSubLang.begin(); it != slSubLang.end(); ++it ) {
        ckLvLangItem *lvitem=new ckLvLangItem(subRoot,this);
        lvitem->setText(0,*it);
        lvitem->streamType=SUB;
        lvitem->language=*it;
        langSubItems.append(lvitem);
    }

}

 void k9Main::cbOutputDevActivated(int _index) {

      if (_index==0) return;

      kCDDrive * drive=(kCDDrive*)recorderList.at(_index-1);
      QValueList <int>list=drive->getWriteSpeeds();
      QValueList<int>::iterator it=list.begin();

      cbBurnSpeed->clear();
      cbBurnSpeed->insertItem(i18n("default"));      
      while( it !=list.end()) {  
	cbBurnSpeed->insertItem(QString::number((double)(*it) /1385));
	it++;
      }
}

 void k9Main::bInputOpenClick() {
QString result=KFileDialog::getOpenFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Open ISO Image"));
if (result!="")
	cbInputDev->setCurrentText(result);
}


 void k9Main::bInputOpenDirClick() {
QString result= KDirSelectDialog::selectDirectory (QDir::homeDirPath(), false, this,i18n("Open DVD folder")).path();
if (result!="")
	cbInputDev->setCurrentText(result);

}

void k9Main::fspDone() {
	fspFinish=true;
}

 void k9Main::foundMountPoint( const QString &mountPoint, unsigned long kBSize, unsigned long kBUsed, unsigned long kBAvail) {
	fspAvail=kBAvail;
}

long k9Main::getFreeSpace(const QString & _path) {
    fspFinish=false;
    fspAvail=0;
   KDiskFreeSp *FreeSp=new KDiskFreeSp();;

    connect(FreeSp,SIGNAL(foundMountPoint (const QString &, unsigned long , unsigned long , unsigned long )),this,SLOT(foundMountPoint (const QString &, unsigned long , unsigned long , unsigned long )));
    connect(FreeSp,SIGNAL(done ( )),this,SLOT(fspDone ()));
    QString mountPoint = KIO::findPathMountPoint( _path );
    FreeSp->readDF(mountPoint);
    do  {
	qApp->processEvents();
     } while (!fspFinish);
     fspAvail/=1024;
    return fspAvail;

}


void k9Main::setInput(QString _input) {
	cbInputDev->setCurrentText(_input);
}

void k9Main::setOutput(QString _output) {
	cbOutputDev->setCurrentItem(0);
	for (uint i=0 ;i <recorderList.count();i++) {
	   kCDDrive * drive=(kCDDrive*)recorderList.at(i);  
          QString c(drive->device);
	    qDebug ("output=" +c);
	    if (c==_output) {
		cbOutputDev->setCurrentItem(i+1);
		break;
	    }
	}
}

 void k9Main::Clone(QString _input,QString _output) {
	setInput(_input);
	setOutput(_output);
	ckMenu->setChecked(true);
	ckQuickScan->setChecked(true);
	Open();
	checkAll( true);
	Copy();
}
