//
// C++ Implementation:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9import.h"
#include "k9title.h"
#include "k9avifile.h"
#include "k9lvitemimport.h"
#include <klocale.h>
#include "k9menuedit.h"
#include "k9haldevice.h"
#include "k9config.h"
#include "k9burndvd.h"
#include <qdir.h>
#include <kfiledialog.h>
#include <kcombobox.h>

k9Import::k9Import(QWidget* parent, const char* name,k9CdDrives *_drives)
        : import(parent,name) {
    m_parent=(k9Copy*) parent;
    drives=_drives;
}

k9Import::~k9Import() {}



/*$SPECIALIZATION$*/

void k9Import::init() {
    m_root=new QListViewItem(lvDVD,i18n("root"));
    m_root->setOpen(true);
    emit aviFileSelected(NULL);

    connect(drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
    connect(drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

    readDrives();

}

void k9Import::readDrives() {
    cbOutputDev->clear();
    recorderList.clear();
    cbOutputDev->insertItem(i18n("ISO Image"));

    drives->scanDrives();

}

void k9Import::deviceAdded(k9CdDrive *_drive) {
    addDrive(_drive);
}

void k9Import::deviceRemoved(k9CdDrive *_drive) {

    int i=recorderList.find(_drive);
    recorderList.remove( i);
    cbOutputDev->removeItem(i+1);

}

void k9Import::addDrive(k9CdDrive *drive) {
#ifdef HAVE_HAL
    if (drive->getDevice()!=NULL) {
        k9HalDevice *dev=drive->getDevice();
        connect(dev,SIGNAL(volumeChanged( const QString &,const QString &)),this,SLOT(volumeChanged(const QString&,const QString&)));
    }
#endif
    if (drive->canWriteDVD) {
        cbOutputDev->insertItem(drive->name+ "  ("+drive->device+")",-1);
        recorderList.append(drive);
    }

}

void k9Import::volumeChanged(const QString &device,const QString &volumeName) {}


#include "k9import.moc"



QListViewItem* k9Import::getRoot() const {
    return m_root;
}


void k9Import::aviFileUpdated(k9AviFile *_aviFile) {
    k9LvItemImport *itemTitle = (k9LvItemImport*)m_root->firstChild();
    while (itemTitle) {
        k9LvItemImport *itemChapter = (k9LvItemImport*)itemTitle->firstChild();
        while (itemChapter) {
            if (itemChapter->getAviFile()==_aviFile) {
                itemChapter->setText(1,_aviFile->getFileName());
                itemChapter->setPixmap(0,QPixmap(_aviFile->getImage().smoothScale(50,50)));
            }
            itemChapter=(k9LvItemImport*)itemChapter->nextSibling();
        }
        itemTitle=(k9LvItemImport*)itemTitle->nextSibling();

    }
}

void k9Import::buttonUpdated(k9MenuButton *_button, const QImage &_image) {
    k9LvItemImport *itemTitle = (k9LvItemImport*)m_root->firstChild();
    while (itemTitle) {
        if (itemTitle->getTitle()->getButton()==_button) {
            QImage img=_image.smoothScale(50,50,QImage::ScaleFree);
            QPixmap pix(img);
            itemTitle->setPixmap(0,pix);
        }
        itemTitle=(k9LvItemImport*)itemTitle->nextSibling();
    }
}



void k9Import::lvDVDItemSelected (QListViewItem *_item) {
    if (_item->rtti()==1002) {
        k9LvItemImport *item=(k9LvItemImport*)_item;
        emit aviFileSelected(item->getAviFile());
    }
}

void k9Import::setProgressWindow(QWidget *_widget) {
    m_toolView=m_parent->setToolWindow(_widget,KDockWidget::DockRight,i18n("processing"),i18n("processing"));
    m_dockWidget = m_parent->getVisibleDock();
    m_parent->setActions( false);
    m_toolView->show();
    this->setEnabled(false);

}

void k9Import::removeProgressWindow() {
    m_parent->setActions(true);

    m_parent->removeToolWindow( m_toolView);
    this->setEnabled(true);
    if (m_dockWidget!=NULL)
        m_dockWidget->changeHideShowState();
}

void k9Import::execute() {
  QString filename;
  if (cbOutputDev->currentItem() ==0)
  {
    filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
    if (filename =="")
      return;
  }


    k9Config config;
    k9Progress *progress=new k9Progress(m_parent,0);
    m_newDVD.setProgress(progress);
    setProgressWindow(progress);
    m_newDVD.setWorkDir(config.getPrefOutput());
    m_newDVD.execute();
    removeProgressWindow();
    bool burn=true;
    if (burn) {
        //changeStatusbar(i18n("Burning DVD"),sbMessage);

        k9BurnDVD b;

        setProgressWindow( b.getDialog());
        b.setworkDir(config.getPrefOutput() );
        b.setUseK3b(config.getPrefK3b() );
        b.setAutoBurn(config.getPrefAutoBurn());
        b.setvolId("DVD");
        //b.setSpeed( cbBurnSpeed->currentText());
        if (cbOutputDev->currentItem() !=0) {
            k9CdDrive * drive=(k9CdDrive*)recorderList.at(cbOutputDev->currentItem()-1);
            b.setburnDevice(drive->device);
        } else
            b.makeIso(filename);
        b.burn();
        removeProgressWindow();
    }


}


void k9Import::cbFormatActivated(const QString &_format) {
    if (_format == "PAL") {
        m_newDVD.setFormat(k9NewDVD::PAL);
        m_newDVD.getMenuEdit()->setFormat(k9MenuEdit::PAL);
    } else {
        m_newDVD.setFormat(k9NewDVD::NTSC);
        m_newDVD.getMenuEdit()->setFormat(k9MenuEdit::NTSC);
    }



}

