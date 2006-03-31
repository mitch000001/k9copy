/**************************************************************************
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

#include "kburndvd.h"
#include "kburndvd.moc"
#include "k9progress.h"

#include <qdir.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kfiledialog.h>

kBurnDVD::kBurnDVD()
        : QObject(NULL,"") {
    cancelled=false;
    useK3b=false;
    autoBurn=false;
    volId="K9Copy";
    iso=false;
    m_filename="";
}


kBurnDVD::~kBurnDVD() {}

/** Read property of QString burnDevice. */
const QString& kBurnDVD::getburnDevice() {
    return burnDevice;
}
/** Write property of QString burnDevice. */
void kBurnDVD::setburnDevice( const QString& _newVal) {
    burnDevice = _newVal;
}
/** Write property of QString workDir. */
void kBurnDVD::setworkDir( const QString& _newVal) {
    workDir = _newVal;
    if (workDir.right(1)!='/')
        workDir +='/';
}

void kBurnDVD::makeIso(QString _filename) {
    iso=true;
    m_filename=_filename;
}

void kBurnDVD::setvolId(QString _newVal) {
    volId=_newVal.replace(" ","_");
}

void kBurnDVD::setUseK3b(bool _newVal) {
    useK3b=_newVal;
}

void kBurnDVD::setAutoBurn(bool _newVal) {
    autoBurn=_newVal;
}
void kBurnDVD::burn() {
    if (!useK3b)
        burnWithGrowisofs();
    else
        burnWithK3b();
}

void kBurnDVD::burnWithK3b() {
    KProcess *k3b = new KProcess();

    *k3b << "k3b";
    *k3b << QDir::cleanDirPath(workDir +"/dvd/VIDEO_TS");
    *k3b << "-videodvd";
    k3b->start();
    k3b->detach();
    delete k3b;
}

void kBurnDVD::mkisoSizeStderr() {
    QString c(proc2->readStderr());
    imageSize=c.replace("\n","");
    ;
}

void kBurnDVD::mkisoSizeStdout() {
    QString c(proc2->readStdout());
    imageSize=c.replace("\n","");
    ;
}

const QString &kBurnDVD::getImageSize() {
    QString c;
    c="mkisofs";
    proc2=new QProcess(c,0);
    proc2->addArgument("-quiet");
    proc2->addArgument("-print-size");
    proc2->addArgument("-dvd-video");
    proc2->addArgument("-udf");
    proc2->addArgument("-r");
    proc2->addArgument("-V "+volId);
    c=QDir::cleanDirPath(workDir +"/dvd");
    proc2->addArgument(c);

    connect( proc2, SIGNAL(readyReadStderr()),this, SLOT(mkisoSizeStderr()) );
    connect( proc2, SIGNAL(readyReadStdout()),this, SLOT(mkisoSizeStdout()) );
    if (proc2->start()) {
        while (proc2->isRunning()) {
            qApp->processEvents();
	}
        if (proc2->exitStatus()==0) {
            delete proc2;
            return imageSize;
        }
    }
    delete proc2;
    return "";
}


void kBurnDVD::burnWithGrowisofs() {

//    progress= new KProgressDialog (qApp->mainWidget(),"progress",i18n("k9Copy - Burning DVD"),"",true);
    progress=new k9Progress(qApp->mainWidget(),"progress",true,0);
    progress->setCaption(i18n("k9Copy - Burning DVD"));
    progress->setLabelText(i18n("Burning DVD"));
    bool bok=false;

    int nbTry=0;
    while (!cancelled && !bok) {
        burnSpeed=0;
        QString c;
        if (iso)
            c="mkisofs";
        else
            c="growisofs";
        proc=progress->getProcess();
	proc->addArgument(c);

        if (!iso) {
            proc->addArgument("-overburn");
            proc->addArgument("-Z");
            proc->addArgument(burnDevice  );
            proc->addArgument("-use-the-force-luke=tty");
	    proc->addArgument("-use-the-force-luke=tracksize:"+getImageSize());
            proc->addArgument("-use-the-force-luke=dao:" + imageSize);
            proc->addArgument("-dvd-compat");
	    if (m_speed !=i18n("default")) 
	            proc->addArgument("-speed=" + m_speed);
			
        } else {
	    QString fileName=m_filename;
	    if (fileName =="") 
                 fileName=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
            if (fileName !="") {
                proc->addArgument("-o");
                proc->addArgument(fileName);
            } else
                cancelled=true;

        }

        proc->addArgument("-dvd-video");
        proc->addArgument("-udf");
        proc->addArgument("-r");
        proc->addArgument("-V "+volId);
        c=QDir::cleanDirPath(workDir +"dvd");
        proc->addArgument(c);

        connect( proc, SIGNAL(readyReadStderr()),this, SLOT(growisoStderr()) );
        connect( proc, SIGNAL(readyReadStdout()),this, SLOT(growisoStdout()) );
        if (!autoBurn && !iso) {
            c=i18n("Insert a recordable DVD");
            if ( KMessageBox::warningContinueCancel ( 0,c, i18n("authoring"))!=KMessageBox::Continue)
                cancelled=true;
        }
        if (!cancelled) {
	    int res=progress->execute();
            if ( res==-1 ) {
                KMessageBox::error( 0, i18n("Error burning DVD :\n", i18n("DVD burning")) +lastMsg);
            } else {
                if (proc->exitStatus()==0) {
                    bok=true;
                    KMessageBox::information( 0, i18n("DVD Burning finished"), i18n("DVD burning") );
                    //delete temporary files
                    //        clearOutput(workDir+"dvd");
                } else {
                    QString c;

                    c=i18n("An error occured while Burning DVD: %1").arg(lastMsg) +"\n" +i18n("Insert an other DVD");
                    if ( KMessageBox::warningContinueCancel ( 0,c, i18n("authoring"))!=KMessageBox::Continue) {
                        cancelled=true;
                    }
                }
            }
        }
    }
    delete progress;
}

/** No descriptions */
void kBurnDVD::growisoStderr() {
    QString c(proc->readStderr()),d;
    char s[255];
    int a,b;
    int pos;
    lastMsg=c;
    if (c.contains("Current Write Speed")) {
        sscanf(c.latin1(),"%s \"Current Write Speed\" is %d.%d",s,&a,&b);
        burnSpeed=a+b/10;
    }
    d=i18n("Burning DVD") +"\n" + i18n("Current write speed :%1 x").arg(burnSpeed);
    progress->setLabelText(d);
    if (c.contains("% done")) {
        pos=c.find("%");
        if (pos!=-1) {
            c=c.mid(1,pos-4);
            //progress->setLabelText(c);
            progress->setProgress(c.toInt(),100);
            qApp->processEvents();
        }
    }
}
void kBurnDVD::growisoStdout() {
    QString c(proc->readStdout());
    int pos;
    pos=c.find("STAT");
    if (pos!=-1) {
        c=c.mid(pos);
        progress->setLabelText(c);
        qApp->processEvents();
    }
}