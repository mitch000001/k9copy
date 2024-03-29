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
#include "k9burndvd.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9dvdtitleset.h"
#include "k9dvdbackup.h"
#include "k9mp4enc.h"
#include "k9settings.h"
#include "k9langselect.h"
#include "k9config.h"
#include "k9updatefactor.h"
#include "k9haldevice.h"
#include "k9titleencopt.h"
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
#include <qcheckbox.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <kdirselectdialog.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <kdiskfreesp.h>
#include <qvaluelist.h>
#include <kdeversion.h>
#include <qbitmap.h>


k9DVDListItem::k9DVDListItem(QObject *DVD,ckLvItem *List,eStreamType type)
{
  switch(type)
  {
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
  default :
    break;
  }
  listItem= List;
  streamType=type;
}

k9Main::k9Main(QWidget* parent, const char* name, k9CdDrives *_drives)
    : MainDlg(parent,name),pxVideo((const char **) img_video), pxSound((const char **) img_sound),
    pxText((const char **) img_text)
{
  drives=_drives;
  m_parent=(k9Copy*)parent;
  QImage img;
  img.loadFromData( img_chapter, sizeof( img_chapter ), "PNG" );
  pxChapter.convertFromImage( img);
  /*    m_factory = KLibLoader::self()->factory("libk9copy");
  if (m_factory)      {
  dvd=static_cast<k9DVD  *>(m_factory->create(this,"dvd", "k9DVD"));
  }
  */

  dvd=new k9DVD(this);

  updating=false;

  items.setAutoDelete(true);
  listView1->setDefaultRenameAction(QListView::Accept);
  //    KStandardDirs kd;
  m_prefOutput=locateLocal("tmp","k9copy/",true); //kd.findResource("tmp","");
  connect(drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
  connect(drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

  readSettings();
  bInputOpen->setPixmap(SmallIcon("fileopen"));
  bInputOpenDir->setPixmap(SmallIcon("folder_open"));
  emit SelectionChanged(NULL,true);
  m_update=new k9UpdateFactor(this,"");
  connect(m_update,SIGNAL(updateFactor_internal()),this,SLOT(updateFactor_internal()));  
}


k9DVDListItem *k9Main::addListItem(QObject *DVD,ckLvItem *List,eStreamType type)
{
  k9DVDListItem *litem;
  litem=new k9DVDListItem(DVD,List,type);
  items.append(litem);
  return litem;
}

int LvItem::rtti() const
{
  return(1000);
}


int ckLvItem::rtti() const
{
  return(1001);
}

int ckLvItem::compare ( QListViewItem * i, int col, bool ascending ) const
{
  if (i->rtti() !=1001)
    return 1;
  ckLvItem *litem = (ckLvItem*)i;
  k9DVDTitleset *titleset1,*titleset2;
  ckLvItem *l;
  uint id,id2;
  if (col==0)
  {
    switch(this->objectType)
    {
    case TITLESET:
      titleset1=(k9DVDTitleset*)obj;
      titleset2=(k9DVDTitleset*)litem->obj;
      return titleset1->getnum() -titleset2->getnum();
    case STREAM:
      l=(ckLvItem*)this;
      id=streamType*100;
      id2=litem->streamType*100;

      if (streamType==AUD)
      {
        id+=((k9DVDAudioStream*)l->stream)->getID();
      }
      if (streamType==SUB)
      {
        id+=((k9DVDSubtitle*)l->stream)->getID().first();
      }

      if (litem->streamType==AUD)
      {
        id2+=((k9DVDAudioStream*)litem->stream)->getID();
      }
      if (litem->streamType==SUB)
      {
        id2+=((k9DVDSubtitle*)litem->stream)->getID().first();
      }


      return k9Main::compare(id,id2);
      break;
    case CHAPTER:
      l=(ckLvItem*)this;
      k9DVDChapter *ch1=(k9DVDChapter*)l->obj;
      k9DVDChapter *ch2=(k9DVDChapter*)litem->obj;
      return k9Main::compare(ch1->getnum()+ch1->getTitle()->getId()*1000 ,ch2->getnum()+ch2->getTitle()->getId()*1000);
      break;
    }
  }

  if (col ==1)
  {
    switch (this->objectType)
    {
    case TITLESET:
      titleset1=(k9DVDTitleset*)obj;
      titleset2=(k9DVDTitleset*)litem->obj;
      return k9Main::compare(titleset1->getsize(),titleset2->getsize());
      break;
    case STREAM:
      double size1,size2;
      l =(ckLvItem*)this;
      size1=l->getstreamSize();
      size2=litem->getstreamSize();
      return k9Main::compare(size1,size2);
      break;
    case CHAPTER:
      l=(ckLvItem*)this;
      k9DVDChapter *ch1=(k9DVDChapter*)l->obj;
      k9DVDChapter *ch2=(k9DVDChapter*)litem->obj;
      return k9Main::compare(ch1->getsectors(),ch2->getsectors());
    }
  }
  return QCheckListItem::compare(i,col,ascending);
}

double ckLvItem::getstreamSize()
{
  double size;
  k9DVDTitle *title;
  switch (streamType)
  {
  case SUB:
    {
      k9DVDSubtitle *sub=(k9DVDSubtitle*)stream;
      size=sub->getsize_mb();
      break;
    }
  case AUD:
    {
      k9DVDAudioStream *aud=(k9DVDAudioStream*)stream;
      size=aud->getsize_mb();
      break;
    }
  case VID:
    {
      title=(k9DVDTitle*)obj;
      size=title->gettotalvideosize_mb();
      break;
    }
  default:
    size=0;
    break;
  }
  return size;
}


int LvItem::compare ( QListViewItem * i, int col, bool ascending ) const
{
  if(i->rtti()==1001)
    return -1;
  LvItem *litem = (LvItem*)i;
  k9DVDTitle *title1,*title2;
  if (col ==1 && objectType==TITLE)
  {
    title1=(k9DVDTitle*)obj;
    title2=(k9DVDTitle*)litem->obj;
    return k9Main::compare(title1->gettotalsize_mb(),title2->gettotalsize_mb());
  }
  if (col==0 && objectType==TITLE)
  {
    title1=(k9DVDTitle*)obj;
    title2=(k9DVDTitle*)litem->obj;
    return k9Main::compare(title1->getpgc(),title2->getpgc());
  }

  return QListViewItem::compare(i,col,ascending);
}

void ckLvItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
  if (column==1)
  {
    p->eraseRect(0,0,width,height());
    QFont f(p->font());

    switch (objectType)
    {
    case TITLESET:
      {
        k9DVDTitleset *titleset=(k9DVDTitleset*)obj;
        f.setBold(true);
        p->setFont(f);
        p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,titleset->getsize_mb()+" " +i18n("MB"));
        break;
      }
    case STREAM:
      {
        if ( ! mainDlg->getquickScan())
        {
          double size=getstreamSize();
          QString c;
          c.sprintf("%.2f ",size);
          p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,c+i18n("MB"));
        }
        break;
      }
    case CHAPTER :
      {
        k9DVDChapter *chapter=(k9DVDChapter*) obj;
        double size=(double)chapter->getsectors()/512;
        QString c;
        c.sprintf("%.2f ",size);
        p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,c+i18n("MB"));
      }
      break;
    default:
      QCheckListItem::paintCell(p,cg,column,width,align);
    }
  }
  else
    QCheckListItem::paintCell(p,cg,column,width,align);

}

void LvItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
  if (column==1 && objectType==TITLE)
  {
    p->eraseRect(0,0,width,height());
    k9DVDTitle * title=(k9DVDTitle*)obj;
    QString c;
    c.sprintf("%.2f ",title->gettotalsize_mb());
    p->drawText(0,0,width,height(),Qt::AlignRight | Qt::AlignVCenter ,c+i18n("MB"));
  }
  else
    QListViewItem::paintCell(p,cg,column,width,align);
}

void ckLvItem::stateChange(bool state)
{
  if (mainDlg !=NULL)
  {
    if (mainDlg->getupdating())
      return;
    switch (objectType)
    {
    case ROOT :
      mainDlg->checkAll(state);
      break;
    case TITLESET:
      mainDlg->checkTS(state,this);
      break;
    default :
      mainDlg->checkTitle(state,this);
      break;
    }
    //        k9DVD *d = mainDlg->dvd;
    mainDlg->setDVDSize();
    mainDlg->updateFactor();
    //(mainDlg->factor)->setValue((int) (d->getfactor(mainDlg->withMenus(),true )*100)-100 );
  }
}


bool k9Main::getupdating()
{
  return updating;
}


void k9Main::Copy()
{
  QString c,filename;
  if (!dvd->getopened())
  {
    KMessageBox::error( this, i18n("DVD is not opened"), i18n("DVD Copy"));
    return;
  }
  setDVDSize();
  updateSelection();
  
  QString outputDir=m_prefOutput;
  bool burn=false;

  if (cbOutputDev->currentItem() ==0)
  {
    filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
    if (filename =="")
      return;
  } else if (cbOutputDev->currentItem()==1) {
    outputDir= KFileDialog::getExistingDirectory(QDir::homeDirPath());
    if (outputDir=="")
       return;
  }

  if (getFreeSpace( m_prefOutput) <  m_prefSize)
  {
    if (KMessageBox::warningContinueCancel (this, i18n("Insufficient disk space on %1\n%2 mb expected.").arg(m_prefOutput).arg(m_prefSize),i18n("DVD Copy"))==KMessageBox::Cancel)
      return;
  }
  stopPreview();
  changeStatusbar( i18n("Backup in progress"),sbMessage);
  if (!m_useDvdAuthor || withMenus())
  {
    //copy with k9DVDBackup
    //k9DVDBackup *backup = static_cast<k9DVDBackup  *>(m_factory->create(dvd,"backup", "k9DVDBackup"));
    m_playbackOptions->setSequence(); // JMP : temporaire

    k9DVDBackup *backup=new k9DVDBackup(dvd,"backup");
    
    setProgressWindow(backup->getDialog());
    backup->setOutput(outputDir);
    backup->setDevice(dvd->getDevice());
    backup->setWithMenus( withMenus());
    backup->execute();
    burn=backup->getErrMsg()=="";
    delete backup;
    removeProgressWindow();  
  }
  else
  {
    //k9DVDAuthor *b=static_cast<k9DVDAuthor  *>(m_factory->create(dvd,"dvdauthor", "k9DVDAuthor"));
    
    k9DVDAuthor *b=new k9DVDAuthor(dvd,"dvdauthor");
    setProgressWindow( b->getDialog());
    m_playbackOptions->setSequence();
    b->setworkDir(outputDir);
    b->author();
    if (!b->getError())
      burn=true;
    removeProgressWindow();
    
    delete b;
  }

  if (burn && cbOutputDev->currentItem()!=1)
  {
    changeStatusbar(i18n("Burning DVD"),sbMessage);

    k9BurnDVD b;
    
    setProgressWindow( b.getDialog()); 
    b.setworkDir(m_prefOutput);
    b.setUseK3b(m_prefK3b);
    b.setAutoBurn(m_prefAutoBurn);
    b.setvolId(dvd->getDVDTitle());
    b.setSpeed( cbBurnSpeed->currentText());
    if (cbOutputDev->currentItem() >1)
    {
      k9CdDrive * drive=(k9CdDrive*)recorderList.at(cbOutputDev->currentItem()-2);
      b.setburnDevice(drive->device);
      closeDVD();
      if (dvd->getDevice()== drive->device)
        b.setAutoBurn(false);
    }
    else
      b.makeIso(filename);
    b.burn();
    removeProgressWindow();
  }
  if(dvd->getopened())
    changeStatusbar(i18n("Ready"),sbMessage);

}

void k9Main::setProgressWindow(QWidget *_widget) {
    m_toolView=m_parent->setToolWindow(_widget,KDockWidget::DockRight,i18n("processing"),i18n("processing"));
    m_dockWidget = m_parent->getVisibleDock();
    m_parent->setActions( false);
    m_toolView->show();
    this->setEnabled(false);
 
}

void k9Main::removeProgressWindow() {
    m_parent->setActions(true);

    m_parent->removeToolWindow( m_toolView);
    this->setEnabled(true);
    if (m_dockWidget!=NULL)
    	m_dockWidget->changeHideShowState();
}

void k9Main::eject()
{
  closeDVD();
  drives->eject( getDevice(cbInputDev));
}



QString  k9Main::getDevice(QComboBox *_combo)
{
  int index=-1;
  for (int i=0; i<_combo->count();i++)
  {
    QString t =_combo->text(i);
    if (_combo->text(i) == _combo->currentText())
      index=i;
  }
  QString res="";
  if ((index==-1) || (_combo->currentText() ==i18n("ISO Image")))
  {
    res=_combo->currentText();
#if KDE_VERSION >= KDE_MAKE_VERSION(3,4,0)

    KURL url=KURL::fromPathOrURL(res);
    //KIO::mount(true,0,res,"",true);
    KIO::UDSEntry entry;
    //first call to mount the device
    KIO::NetAccess::stat(KURL::fromPathOrURL(res+"/."),entry,0);
    if (KIO::NetAccess::stat(url,entry,0) )
    {
      KIO::UDSEntry::iterator it;
      for ( it = entry.begin(); it != entry.end(); ++it )
      {
        if ( (*it).m_uds== KIO::UDS_LOCAL_PATH) {
          res=(*it).m_str; 
        }
      }
    }
#endif

  }
  else
  {
    k9CdDrive * drive=(k9CdDrive*)driveList.at(index);
    res=drive->device;
  }
  return res;
}

void k9Main::Open()
{
  int i;
  k9DVDTitle * l_track;
  QString t;
  QTime h;
  connect(listView1,SIGNAL(itemRenamed(QListViewItem*,int)),this,SLOT(itemRenamed(QListViewItem *,int)));
  connect(listView1,SIGNAL(expanded( QListViewItem*)),this,SLOT(expanded( QListViewItem* )));
  connect(listView1,SIGNAL(collapsed( QListViewItem*)),this,SLOT(collapsed( QListViewItem* )));

  closeDVD();
  QString sDevice=getDevice(cbInputDev);
  
  QString sVolName="";  
  //if no dvd label, try to get it from hal
  for (k9CdDrive *drive=driveList.first();drive;drive=driveList.next()) {
  #ifdef HAVE_HAL
	if (drive->getDevice() != NULL) {
		if (drive->getDevice()->mountPoint()==sDevice) {
			sVolName=drive->getDevice()->getVolumeName();
			sDevice=drive->device;
			break;
		}
	}
#endif
  }     	
 
  dvd->scandvd(sDevice,m_quickScan);
  if (dvd->geterror())
  {
    KMessageBox::error( this, dvd->geterrMsg(), i18n("Open DVD"));
    return;
  }
  if (dvd->getDVDTitle()==i18n("unknown") && sVolName!="") {
     dvd->setDVDTitle( sVolName);
  }
 
  listView1->clear();
  items.clear();
  tsItems.clear();
  chItems.clear();

  root = new ckLvItem (listView1,this,ROOT );
  root->setOpen( TRUE );
  
  root->setText(0, dvd->getDVDTitle());
  root->setRenameEnabled(0,true);
  root->obj=NULL;
  root->streamType=NONE;
  root->setPixmap(0,SmallIcon("dvd_unmount"));

  for (i=0;i<dvd->gettitlesetCount();i++)
  {
    ckLvItem *tsItem = new ckLvItem(root,this,TITLESET);
    tsItem->setOpen(TRUE);
    QString c;
    c=i18n("Titleset %1").arg(i+1);
    tsItem->setText(0,c);
    tsItem->setText(1,"   "+dvd->gettitleset(i)->getsize_mb() +" " +i18n("MB"));
    tsItem->obj=dvd->gettitleset(i) ;
    tsItem->streamType=NONE;
    tsItem->setRenameEnabled(0,false);
    tsItems.append(tsItem);
    tsItem->setPixmap( 0,SmallIcon("folder"));
  }

  for (i=0;i<dvd->gettitleCount();i++)
  {
    l_track=dvd->gettitle(i);
    if ((l_track->getIndexed()))
      addTitle(l_track);

    h=l_track->gettotallength();
  }

  m_playbackOptions->fillTitleList();
  m_langSelect->fillLvLanguages();
  listView1->setSorting(0,true);
  changeStatusbar( i18n("Ready"),sbMessage);
  emit SelectionChanged(dvd,withMenus());
}

void k9Main::setDVDSize()
{
  //k9DVDSize *dvdsize=static_cast<k9DVDSize  *>(m_factory->create(this,"dvd", "k9DVDSize"));
  k9DVDSize *dvdsize=new k9DVDSize(this);
  dvdsize->setMaxSizeDyn(m_prefSize);
  delete dvdsize;
}


k9Main::~k9Main()
{
  delete dvd;
}

void k9Main::closeEvent( QCloseEvent* ce )
{
  saveSettings();
  ce->accept();
}


void k9Main::addChapters(QListViewItem *_parent,k9DVDTitle *_title)
{
  LvItem *chapter = new LvItem(_parent,CHAPTER);
  chapter->setText(0, i18n("chapters"));
  chapter->setOpen( false);
  chapter->setPixmap(0,pxChapter);

  int ch=0;
  for (int i=0;i< _title->getchapterCount();i++)
  {
    ckLvItem *it =new ckLvItem(chapter,this,CHAPTER);
    it->setText(0,i18n("chapter %1").arg(++ch));
    it->setText(1,i18n("%1 MB").arg((double)(_title->getChapter(i)->getsectors()) /512));
    it->streamType=CHAP;
    it->obj=_title->getChapter(i);
    chItems.append( it);
    it->mainTitle=_title;
  }
  for (int j=0;j <_title->getTitles().count();j++)
  {
    k9DVDTitle *title2=_title->getTitles().at(j);
    for (int i=0;i< title2->getchapterCount();i++)
    {
      ckLvItem *it =new ckLvItem(chapter,this,CHAPTER);
      it->setText(0,i18n("chapter %1").arg(++ch));
      it->streamType=CHAP;
      it->obj=title2->getChapter(i);
      it->setText(1,i18n("%1 MB").arg((double)(title2->getChapter(i)->getsectors()) /512));
      chItems.append( it);
      it->mainTitle=_title;
    }

  }

}
/** No descriptions */
void k9Main::addTitle(k9DVDTitle *track)
{
  const int col1 =0;
  const int col2 =1;
  const int col3 =2;
  k9DVDAudioStream *l_auds;
  k9DVDSubtitle *l_sub;
  int i;
  QString c,ch;


  listView1->setRootIsDecorated(true);

  LvItem * itemTrack = new LvItem( tsItems.at(track->getVTS()-1),TITLE);
  itemTrack->setOpen( false );
  itemTrack->setText(col1,track->getname());
  itemTrack->setRenameEnabled(0,true);
  itemTrack->setPixmap(col1,SmallIcon("title"));
  c.sprintf("%.2f ", track->gettotalsize_mb());

  itemTrack->setText(col2,c+i18n("MB"));
  itemTrack->obj=track;

  addChapters( itemTrack,track);

  ckLvItem *video;
  video=new ckLvItem( itemTrack,this,STREAM);
  video->streamType=VID;
  addListItem(track,video,VID);
  video->setOpen( false );
  c=i18n("video %1 ").arg(track->getformat());
  c.append (" - " + track->gettotallength().toString("h:mm:ss"));
  video->setText(col1, c);
  c.sprintf("%.2f ",  track->gettotalvideosize_mb());
  video->setText(col2,c +i18n("MB"));
  video->setPixmap(col1,pxVideo);
  video->obj=track;

  for (i=0;i< track->getaudioStreamCount();i++)
  {
    l_auds=track->getaudioStream(i);
    c=i18n("audio %1 ").arg(i+1);
    c.append( l_auds->getlanguage() + " " +l_auds->getformat()+" " );
    ch.sprintf("%dch ",l_auds->getchannels());
    c.append(ch+l_auds->getfrequency()+" "+l_auds->getquantization());
    ckLvItem * item = new ckLvItem( itemTrack,this,STREAM);
    item->streamType=AUD;
    item->language=l_auds->getlanguage();
    addListItem(l_auds,item,AUD);
    item->setText( col1,  c );
    c=i18n("%1 MB").arg(l_auds->getsize_mb());
    item->setText( col2,c);
    item->setText( col3,l_auds->getcontent());
    item->setPixmap(col1,pxSound);
    
    item->obj=track;
    item->stream=l_auds;
  }
  for (i=0;i< track->getsubPictureCount();i++)
  {
    l_sub=track->getsubtitle(i);
    c=i18n("subpicture %1 ").arg(i+1);
    c.append( l_sub->getlanguage());
    ckLvItem * item = new ckLvItem( itemTrack,this,STREAM);
    item->streamType=SUB;
    item->language=l_sub->getlanguage();
    addListItem(l_sub,item,SUB);
    item->setText( col1,  c );
    c=i18n("%1 MB").arg(l_sub->getsize_mb());
    item->setText( col2,c);
    item->setText( col3, l_sub->getcontent());
    item->setPixmap(col1,pxText);

    item->obj=track;
    item->stream=l_sub;
  }
}


/** No descriptions */
void k9Main::updateSelection()
{

  k9DVDListItem *litem;
  for (uint i=0;i<items.count();i++)
  {
    litem=(k9DVDListItem*)items.at(i);

    switch (litem->streamType)
    {
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
      //if title is not selected => shrink factor is not forced
      if (!l_title->isSelected())
        l_title->setforceFactor( false);

      break;
    default :
      break;
    }
  }
  bool supdating=updating;
  updating=true;
  for (uint i=0;i<chItems.count();i++)
  {
    ckLvItem *it=chItems.at(i);
    k9DVDChapter *c=(k9DVDChapter*)it->obj;
    it->setOn( c->getSelected());
  }
  updating=supdating;
}

void k9Main::checkLang(QString lang,eStreamType streamType,bool state)
{
  updating =true;
  for (uint i=0;i<items.count();i++)
  {
    bool checked=false;
    k9DVDListItem *litem=(k9DVDListItem*)items.at(i);
    if (litem->streamType == streamType )
    {
      switch (litem->streamType)
      {
      case SUB:
        k9DVDSubtitle *l_sub;
        l_sub=litem->subtitle;
        if (l_sub->getlanguage() == lang)
        {
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
        if (l_auds->getlanguage() == lang)
        {
          l_auds->setselected(state);
          litem->listItem->setOn(state);
          checked=state;

          k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
          ckLvItem * itemtitleset=(ckLvItem*)litem -> listItem->parent()->parent();
          itemtitleset->setOn(title->gettitleset()->getselected());
        }
        break;
      default:
        break;
      }
      //we must check the video item
      if (checked)
      {
        for (uint j=0; j<items.count();j++)
        {
          k9DVDListItem *litem2=(k9DVDListItem*)items.at(j);
          if (litem2->listItem->parent()==litem->listItem->parent() && litem2->streamType==VID )
          {
            litem2->title->setforceSelection(state);
            litem2->listItem->setOn(state);
          }
        }
      }
    }
  }
  m_playbackOptions->fillTitleList();
  updating=false;
  emit SelectionChanged(dvd,withMenus());

}

void k9Main::checkTitle(bool state, ckLvItem *_item)
{
   m_update->wait();

  k9DVDListItem *litem;
  updating=true;

  k9DVDTitle *title;
  if (_item->streamType==CHAP)
  {
    k9DVDChapter *c=(k9DVDChapter*)_item->obj;
    c->setSelected( state);
    //title=c->getTitle();
    title=_item->mainTitle;
    for (uint i=0;i<items.count() && state ;i++)
    {
      litem=(k9DVDListItem*)items.at(i);
      if (title==litem->title && litem->streamType==VID)
        litem->listItem->setOn(state);

    }
  }
  else
    title=(k9DVDTitle*) _item->obj;


  for (uint i=0;i<items.count();i++)
  {
    litem=(k9DVDListItem*)items.at(i);
    if (litem->listItem->parent() == _item->parent())
    {
      //if current item is video and is unchecked, uncheck audio and subtitles
      if (!state && _item->streamType==VID)
      {
        switch (litem->streamType)
        {
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
        default:
          break;
        }
      }
      else if ( litem->streamType == VID && state &&  (_item->streamType==SUB || _item->streamType==AUD))
      {
        litem->title->setforceSelection(state);
        litem->listItem->setOn(state);
      }
    }
  }


  updateSelection();

  k9DVDTitleset * titleset=NULL;

  titleset=title->gettitleset();

  m_langSelect->update();

  //check the titleset checkbox
  if (titleset!=NULL)
  {
    ckLvItem * itemtitleset;
    if (_item->streamType==CHAP)
      itemtitleset=(ckLvItem*)_item->parent()->parent()->parent();
    else
      itemtitleset=(ckLvItem*)_item->parent()->parent();
    itemtitleset->setOn(titleset->getselected());
  }

  m_playbackOptions->fillTitleList();
  updating=false;
  emit SelectionChanged(dvd,withMenus());
}



void k9Main::checkAll(bool state)
{
  k9DVDListItem *litem;
  updating=true;
  for (uint i=0;i<items.count();i++)
  {
    litem=(k9DVDListItem*)items.at(i);
    switch (litem->streamType)
    {
    case SUB:
      litem->listItem->setOn(state);
      break;
    case AUD:
      litem->listItem->setOn(state);
      break;
    case VID:
      {
        k9DVDTitle * l_title;
        l_title=litem->title;
        //            l_title->setforceSelection(state);
        litem->listItem->setOn(state);

        //            k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
        ckLvItem * itemtitleset=(ckLvItem*)litem -> listItem->parent()->parent();
        l_title->gettitleset()->setselected(state);
        itemtitleset->setOn(l_title->gettitleset()->getselected());
      }
      break;
    default:
      break;
    }
  }

  m_langSelect->selectAll(state);

  updateSelection();
  m_playbackOptions->fillTitleList();
  updating=false;
  emit SelectionChanged(dvd,withMenus());

}

void k9Main::checkTS( bool _state,ckLvItem *_item )
{
  k9DVDTitleset *titleset = (k9DVDTitleset*)_item->obj;
  titleset->setselected(_state);
  k9DVDListItem *litem;
  updating=true;
  for (uint i=0;i<items.count();i++)
  {
    litem=(k9DVDListItem*)items.at(i);
    if (litem->listItem->parent ()->parent() == _item)
    {
      litem->listItem->setOn(_state);
    }
  }

  m_langSelect->update();

  updateSelection();

  m_playbackOptions->fillTitleList();
  updating=false;
  emit SelectionChanged(dvd,withMenus());
}

void k9Main::expanded( QListViewItem *item) {
if (item->rtti()==1001)
  {
    ckLvItem *ckit = (ckLvItem*) item;
    if (ckit->objectType==TITLESET)
    	ckit->setPixmap( 0,SmallIcon("folder_open"));
  }
}

void k9Main::collapsed( QListViewItem *item) {
if (item->rtti()==1001)
  {
    ckLvItem *ckit = (ckLvItem*) item;
    if (ckit->objectType==TITLESET)    
       ckit->setPixmap( 0,SmallIcon("folder"));
  }
}


/** No descriptions */
void k9Main::itemRenamed(QListViewItem * item,int )
{
  k9DVDTitle *t=NULL;
  QString newText;
  if (item->rtti()==1000)
  {
    LvItem *it = (LvItem*) item;
    t=(k9DVDTitle*)it->obj;
    newText=it->text(0);
  }
  else if (item->rtti()==1001)
  {
    ckLvItem *ckit = (ckLvItem*) item;
    t=(k9DVDTitle*)ckit->obj;
    newText=ckit->text(0);
  }

  QString c;
  if (t !=NULL)
  {
    //QMessageBox::critical( this, "test", c.sprintf("%d",it->tag));
    t->setname(newText);
    m_playbackOptions->titleRenamed( t,newText);
  }
  else
    dvd->setDVDTitle(item->text(0));
}

/** No descriptions */
void k9Main::readSettings()
{
  readDrives();
  k9Config config;
  config.read();

  m_prefOutput=config.getPrefOutput();
  cbInputDev->setCurrentItem(config.getInputDev());
  cbOutputDev->setCurrentItem(config.getOutputDev());
  m_prefK3b=config.getPrefK3b();

  m_prefAutoBurn=config.getPrefAutoBurn();
  m_quickScan=config.getQuickScan();
  m_prefSize=config.getPrefSize();
  //fill the burn speed combo
  //cbOutputDevActivated( cbOutputDev->currentItem());
  cbOutputDevActivated( config.getOutputDev());
 

  //m_useDvdAuthor=settings.readBoolEntry("/options/useDvdAuthor",true);
   m_useDvdAuthor=config.getUseDvdAuthor();
}
/** No descriptions */
void k9Main::saveSettings()
{
  k9Config config;
  config.read();

  config.setInputDev(cbInputDev->currentItem());
  config.setOutputDev(cbOutputDev->currentItem());
  config.setKeepMenus(withMenus());
  config.save();
}
/** No descriptions */
void k9Main::bSaveClick()
{
  saveSettings();
}

/** No descriptions */
void k9Main::PreviewTitle()
{
  if( listView1->selectedItem()==NULL)
    return;
  if (listView1->selectedItem()->depth() <2)
    return;
  int rtti=listView1->selectedItem()->rtti();
  QObject *obj;
  if (rtti==1000)
  {
    LvItem *it=(LvItem*)listView1->selectedItem();
    obj=it->obj;
  }
  else
  {
    ckLvItem *it=(ckLvItem*)listView1->selectedItem();
    obj=it->obj;
  }
  
  if (obj !=NULL)
  {
    k9DVDTitle *t;  
    ckLvItem *it=(ckLvItem*)listView1->selectedItem();
    int chapter=1;
    if (it->depth()==4) {
    	k9DVDChapter *c=(k9DVDChapter*)obj;
    	t=c->getTitle();
    	chapter=c->getnum();
    } else
        t=(k9DVDTitle*)obj;
    //viewer->show();
    //viewer->open(dvd,t);
    
    
    emit showPreview( dvd,t,chapter);
  }
}

void k9Main::CreateMP4()
{
  if (!dvd->getopened())
  {
    KMessageBox::error( this, i18n("DVD is not opened"), i18n("MPEG-4 Encoding"));
    return;
  }
  QString filename="";
  int cpt=0;
  for (int i=0; i < dvd->gettitleCount();i++)
  {
    k9DVDTitle *t=dvd->gettitle(i);
    changeStatusbar( i18n("Transcoding title : %1").arg(t->getname()) ,sbMessage);

    if (t->isSelected() && t->getIndexed() )
    {
      QString name;
      if (filename=="")
         filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.avi|Audio Video Interleave (*.avi)\n*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)\n*.mp4|MPEG-4 format (*.mp4)\n*.asf|Advanced Streaming Format (*.asf)\n*.swf|Adobe Flash (*.swf)\n*.flv|Adobe Flash video files (*.flv)\n*.rm|RealVideo (*.rm)\n*.mov|QuickTime (*.mov)", 0,i18n("Save file to disk"));
      if (filename=="")
         break;
    
      k9MP4Enc *mp4=new k9MP4Enc();
      setProgressWindow( mp4->getDialog());
      if (cpt >0) {
	QString ext=filename.section(".",-1);
        if (ext!="")
           ext="."+ext;
        QString path=filename.left(filename.length()-ext.length());
        path=path+QString::number(cpt)+ext;      
        mp4->setFilename(path);
      }  
      else
      	mp4->setFilename(filename);

      cpt++;	
      k9TitleEncOpt * opt=t->getEncOpt();
      mp4->setDevice(dvd->getDevice());
      mp4->setAudioBitrate(opt->getAudioBr());
      mp4->setVideoBitrate(opt->getVideoBr());
      mp4->setCodec( opt->getCodec());
      mp4->setAudioCodec(opt->getAudioCodec());
      mp4->setSize(QString::number(opt->getMaxSize()));
      mp4->setAudioGain(QString::number(opt->getAudioGain()));
      mp4->setNumberCD(QString::number(opt->getNumParts()));
      mp4->setWidth(opt->getWidth() );
      mp4->setHeight(opt->getHeight());
      mp4->set2Passes( opt->get2Passes());
      mp4->setUseCache(opt->getUseCache());
      mp4->setMpeg2(false);
      mp4->execute(t);
      bool bStop=mp4->isCanceled();
      delete mp4;
      removeProgressWindow();
      if (bStop)
        break;
    }
  }
  changeStatusbar( i18n("Ready") ,sbMessage);


}

void k9Main::extractMPEG2() {
    if (!dvd->getopened()) {
        KMessageBox::error( this, i18n("DVD is not opened"), i18n("Extract Mpeg2"));
	return;
    }
    QString filename="";
    int cpt=0;
				    
 
    for (int i=0; i < dvd->gettitleCount();i++) {
        k9DVDTitle *t=dvd->gettitle(i);
        changeStatusbar( i18n("Extracting title : %1",t->getname()) ,sbMessage);
								
        if (t->isSelected() && t->getIndexed() ) {
            QString name;
    	    if (filename=="")
    	    filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)", 0,i18n("Save file to disk"));
    	    if (filename=="") {
        	break;
    	    }

    	    k9MP4Enc *mp4=new k9MP4Enc();
	    setProgressWindow( mp4->getDialog());
    	    mp4->setMpeg2(true);
            mp4->setUseCache(false);

    	    if (cpt >0) {
        	QString ext=filename.section(".",-1);
        	if (ext!="")
        	ext="."+ext;
        	QString path=filename.left(filename.length()-ext.length());
        	path=path+QString::number(cpt)+ext;
        	mp4->setFilename(path);
    	    } else
        	mp4->setFilename(filename);
    	    cpt++;
    	    k9TitleEncOpt * opt=t->getEncOpt();
    	    mp4->setDevice(dvd->getDevice());
    	    mp4->execute(t);
	    bool bStop=mp4->isCanceled();
    	    delete mp4;
	    removeProgressWindow();
    	    if (bStop)
        	break;
	}
    }
    changeStatusbar( i18n("Ready") ,sbMessage);
}

void k9Main::deviceAdded(k9CdDrive *_drive) {
   addDrive(_drive);
}

void k9Main::deviceRemoved(k9CdDrive *_drive) {
   if (_drive->device == dvd->getDevice()) 
       closeDVD();

  int i=driveList.find(_drive);
  driveList.remove(i);
  cbInputDev->removeItem(i);
  
  i=recorderList.find(_drive);
  recorderList.remove( i);
  cbOutputDev->removeItem(i+2);  

}

void k9Main::addDrive(k9CdDrive *drive) {
    #ifdef HAVE_HAL
    if (drive->getDevice()!=NULL) {
        k9HalDevice *dev=drive->getDevice();
        connect(dev,SIGNAL(volumeChanged( const QString &,const QString &)),this,SLOT(volumeChanged(const QString&,const QString&)));
    }
    #endif
    if (drive->canReadDVD)
    {
      QString c(drive->name + "  ("+drive->device+")");
      cbInputDev->insertItem(c,-1);
      driveList.append(drive);
    }
    if (drive->canWriteDVD)
    {
      cbOutputDev->insertItem(drive->name+ "  ("+drive->device+")",-1);
      recorderList.append(drive);
    }

}

/*!
    \fn k9Main::readDrives()
 */
void k9Main::readDrives()
{
  cbInputDev->clear();
  cbOutputDev->clear();
  driveList.clear();
  recorderList.clear();
  cbOutputDev->insertItem(i18n("ISO Image"));
  cbOutputDev->insertItem(i18n("Folder"));
  
  drives->scanDrives();

}


void k9Main::volumeChanged(const QString &device,const QString &volumeName) {
    if (device==dvd->getDevice() && dvd->getopened() && volumeName=="") 
    	closeDVD();
    else if (!dvd->getopened() && volumeName !="" && getDevice(cbInputDev)==device) {
  	Open();
    }
}

/*!
    \fn k9Main::listView1CurrentChanged( QListViewItem *newItem )
 */
void k9Main::listView1CurrentChanged( QListViewItem *newItem )
{
  if (newItem == NULL)
  {
    m_playbackOptions->enable(false);
  }
  else
  {
    if (!withMenus())
    {
      m_playbackOptions->enable(true);
    }
  }

  k9DVDTitle *title=NULL;

  if (newItem->rtti()==1000)
  {
    LvItem *it = (LvItem*) newItem;
    title=(k9DVDTitle*)it->obj;
  }
  else if (newItem->rtti()==1001 && newItem->depth()>2)
  {
    ckLvItem *ckit = (ckLvItem*) newItem;
    if (((ckLvItem*)newItem)->streamType==CHAP)
    {
      LvItem *parent=(LvItem*)(ckit->parent()->parent());
      title=(k9DVDTitle*)parent->obj;
    }
    else
      title=(k9DVDTitle*)ckit->obj;
  }
  if (title !=NULL)
  {
    emit changedTitle(title);
  }

}


void k9Main::closeDVD()
{
  //viewer->bStopClick();
  emit stopPreview();
  emit changedTitle(NULL);
  emit SelectionChanged(NULL,withMenus());
  changeStatusbar( "",sbFactor);
  changeStatusbar( "",sbMessage);
  listView1->clear();
  items.clear();
  m_langSelect->clear();
  dvd->close();

}


int k9Main::compare(double v1,double v2)
{
  if (v1 <v2)
    return -1;
  if (v1>v2)
    return 1;
  return 0;
}


void k9Main::cbOutputDevActivated(int _index)
{

  if (_index==0)
    return;

  k9CdDrive * drive=(k9CdDrive*)recorderList.at(_index-1);
  if (drive !=NULL) {
	QValueList <int>list=drive->getWriteSpeeds();
	QValueList<int>::iterator it=list.begin();
	
	cbBurnSpeed->clear();
	cbBurnSpeed->insertItem(i18n("default"));
	while( it !=list.end())
	{
	cbBurnSpeed->insertItem(QString::number((double)(*it)));
	it++;
	}
  }
}

void k9Main::bInputOpenClick()
{
  QString result=KFileDialog::getOpenFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Open ISO Image"));
  if (result!="")
  {
    cbInputDev->setCurrentText(result);
    Open();
  }
}


void k9Main::bInputOpenDirClick()
{
  QString result= KDirSelectDialog::selectDirectory (QDir::homeDirPath(), false, this,i18n("Open DVD folder")).path();
  if (result!="")
  {
    cbInputDev->setCurrentText(result);
    Open();
  }
}

void k9Main::fspDone()
{
  fspFinish=true;
}

void k9Main::foundMountPoint( const QString &, unsigned long , unsigned long , unsigned long kBAvail)
{
  fspAvail=kBAvail;
}

long k9Main::getFreeSpace(const QString & _path)
{
  fspFinish=false;
  fspAvail=0;
  KDiskFreeSp *FreeSp=new KDiskFreeSp();
  ;

  connect(FreeSp,SIGNAL(foundMountPoint (const QString &, unsigned long , unsigned long , unsigned long )),this,SLOT(foundMountPoint (const QString &, unsigned long , unsigned long , unsigned long )));
  connect(FreeSp,SIGNAL(done ( )),this,SLOT(fspDone ()));
  QString mountPoint = KIO::findPathMountPoint( _path );
  FreeSp->readDF(mountPoint);
  do
  {
    qApp->processEvents();
  }
  while (!fspFinish);
  fspAvail/=1024;
  return fspAvail;

}


void k9Main::setInput(QString _input)
{
  bool found=false;
  for (uint i=0;i< driveList.count();i++) {
     k9CdDrive *drive=(k9CdDrive*)driveList.at(i);
     if (drive->device == _input) {
	cbInputDev->setCurrentItem(i);
 	found=true;
     }
  }
  if (!found)
     cbInputDev->setCurrentText(_input);
}

void k9Main::setOutput(QString _output)
{
  cbOutputDev->setCurrentItem(0);
  for (uint i=0 ;i <recorderList.count();i++)
  {
    k9CdDrive * drive=(k9CdDrive*)recorderList.at(i);
    QString c(drive->device);
    //        qDebug ("output=" +c);
    if (c==_output)
    {
      cbOutputDev->setCurrentItem(i+2);
      break;
    }
  }
}

void k9Main::Clone(QString _input,QString _output)
{
  setInput(_input);
  setOutput(_output);
  m_playbackOptions->setwithMenus( true);
  m_quickScan=true;
  Open();
  checkAll( true);
  Copy();
}

void k9Main::updateFactor(){

 // m_update->updateFactor();
  updateFactor_internal();
}

void k9Main::updateFactor_internal()
{
  if (dvd->getopened() && m_mutex.tryLock())
  {
    updateSelection();
    setDVDSize();
    double dbfactor=dvd->getfactor(withMenus(),true,m_useDvdAuthor);

    factor->setValue((int) ( dbfactor*100)-100 );

    if (dvd->getsizeSelected(FALSE)==0)
      changeStatusbar("",sbFactor);
    else
      changeStatusbar( QString::number(dbfactor,'f',2),sbFactor);
    emit SelectionChanged( dvd,withMenus());
    m_mutex.unlock();
  }
}

bool k9Main::withMenus()
{
  return m_playbackOptions->withMenus();
}

#include "k9main.moc"


void k9Main::setDrives(k9CdDrives* _value) {
    drives = _value;
}
