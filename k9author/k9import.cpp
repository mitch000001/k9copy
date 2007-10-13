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
#include <kiconloader.h>
#include <kselect.h>
#include "k9processlist.h"

k9Import::k9Import(QWidget* parent, const char* name,k9CdDrives *_drives)
        : import(parent,name) {
    m_parent=(k9Copy*) parent;
    drives=_drives;

    connect(&m_newDVD,SIGNAL(sigAddTitle()),this,SLOT(titleAdded()));
}

k9Import::~k9Import() {}



/*$SPECIALIZATION$*/

void k9Import::init() {
    m_root=new QListViewItem(lvDVD,i18n("Root Menu"));
    m_root->setOpen(true);
    m_root->setPixmap(0,SmallIcon("dvd_unmount"));
    emit aviFileSelected(NULL);

    connect(drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
    connect(drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

    readDrives();
    m_root->setSelected(true);
    emit rootSelected(&m_newDVD);


}

void k9Import::readDrives() {
    cbOutputDev->clear();
    recorderList.clear();
    cbOutputDev->insertItem(i18n("ISO Image"));
    cbOutputDev->insertItem(i18n("Folder"));

    drives->scanDrives();

}

void k9Import::deviceAdded(k9CdDrive *_drive) {
    addDrive(_drive);
}

void k9Import::deviceRemoved(k9CdDrive *_drive) {

    int i=recorderList.find(_drive);
    recorderList.remove( i);
    cbOutputDev->removeItem(i+2);

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
                itemChapter->setText(1,_aviFile->getStart().toString("hh:mm:ss") +" - "+_aviFile->getEnd().toString("hh:mm:ss"));
                itemChapter->setPixmap(0,QPixmap(_aviFile->getImage().smoothScale(50,40)));
            }
            itemChapter=(k9LvItemImport*)itemChapter->nextSibling();
        }
        itemTitle=(k9LvItemImport*)itemTitle->nextSibling();
    }
    updateTotalTime();
}

void k9Import::buttonUpdated(k9MenuButton *_button, const QImage &_image) {
    k9LvItemImport *itemTitle = (k9LvItemImport*)m_root->firstChild();
    while (itemTitle) {
        if (itemTitle->getTitle()->getButton()==_button) {
            QImage img=_image.smoothScale(50,40,QImage::ScaleFree);
            QPixmap pix(img);
            itemTitle->setPixmap(0,pix);
        }
        itemTitle=(k9LvItemImport*)itemTitle->nextSibling();
    }
}



void k9Import::lvDVDItemSelected (QListViewItem *_item) {
    k9LvItemImport *item=(k9LvItemImport*)_item;

    switch (_item->rtti()) {
    case 1002:
        emit aviFileSelected(item->getAviFile());
        emit titleSelected(item->getTitle());
        break;
    case 1001:
        emit titleSelected(item->getTitle());
        break;
    default:
        emit rootSelected(&m_newDVD);
        break;
    }
}

void k9Import::setProgressWindow(QWidget *_widget) {
    m_toolView=m_parent->setToolWindow(_widget,KDockWidget::DockRight,i18n("processing"),i18n("processing"));
    m_dockWidget = m_parent->getVisibleDock();
    m_parent->setActions( false);
    m_toolView->show();
//    this->setEnabled(false);

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
    k9Config config;
    QString outputDir=config.getPrefOutput();
    if (cbOutputDev->currentItem() ==0) {
        filename=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
        if (filename =="")
            return;
    } else if (cbOutputDev->currentItem()==1) {
       outputDir= KFileDialog::getExistingDirectory(QDir::homeDirPath());
       if (outputDir=="")
       return;
    }

//    k9Progress *progress=new k9Progress(m_parent,0);
//    m_newDVD.setProgress(progress);
//    setProgressWindow(progress);
    k9ProcessList *p=new k9ProcessList(this,0,0);
    setProgressWindow(p);
    m_newDVD.setProcessList(p);
    m_newDVD.setWorkDir(outputDir);
    m_newDVD.execute();
    removeProgressWindow();
    bool burn=(m_newDVD.getError()=="") ;
    if (burn && cbOutputDev->currentItem()!=1) {
        //changeStatusbar(i18n("Burning DVD"),sbMessage);

        k9BurnDVD b;

        setProgressWindow( b.getDialog());
        b.setworkDir(config.getPrefOutput() );
        b.setUseK3b(config.getPrefK3b() );
        b.setAutoBurn(config.getPrefAutoBurn());
        b.setvolId("DVD");
        b.setSpeed( i18n("default"));
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
        m_menuEdit->setFormat(k9MenuEdit::PAL);
    } else {
        m_newDVD.setFormat(k9NewDVD::NTSC);
        m_menuEdit->setFormat(k9MenuEdit::NTSC);
    }

}

void k9Import::setMenuEdit(k9MenuEdit* _value) {
    m_menuEdit = _value;
}

void k9Import::setEnableCreate(bool _state) {
    m_parent->setEnabledCreateDVD(true);
}

void k9Import::titleAdded() {
    emit rootSelected(&m_newDVD);
}

void k9Import::updateTotalTime() {
    int total=m_newDVD.getTotalTime();
    gsTotal->setValue(total/60);
    QTime t(0,0,0);
    t=t.addSecs(total);
    lTotal->setText(t.toString("hh:mm:ss"));
}
