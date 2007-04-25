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

#include "k9burndvd.h"
#include "k9burndvd.moc"

#include <qdir.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <kprocess.h>
#include <qapplication.h>
k9BurnDVD::k9BurnDVD()
        : QObject(NULL,"") {
    cancelled=false;
    useK3b=false;
    autoBurn=false;
    volId="K9Copy";
    iso=false;
    m_filename="";
    progress=new k9Progress(NULL,"progress",NULL);

}


k9BurnDVD::~k9BurnDVD() {
   delete progress;
}

/** Read property of QString burnDevice. */
const QString& k9BurnDVD::getburnDevice() {
    return burnDevice;
}
/** Write property of QString burnDevice. */
void k9BurnDVD::setburnDevice( const QString& _newVal) {
    burnDevice = _newVal;
}
/** Write property of QString workDir. */
void k9BurnDVD::setworkDir( const QString& _newVal) {
    workDir = _newVal;
    if (workDir.right(1)!='/')
        workDir +='/';
}

void k9BurnDVD::makeIso(QString _filename) {
    iso=true;
    m_filename=_filename;
}

void k9BurnDVD::setvolId(QString _newVal) {
    volId=_newVal.replace(" ","_");
}

void k9BurnDVD::setUseK3b(bool _newVal) {
    useK3b=_newVal;
}

void k9BurnDVD::setAutoBurn(bool _newVal) {
    autoBurn=_newVal;
}
void k9BurnDVD::burn() {
    if (!useK3b)
        burnWithGrowisofs();
    else
        burnWithK3b();
}

void k9BurnDVD::burnWithK3b() {
    KProcess *k3b = new KProcess();

    *k3b << "k3b";
    *k3b << QDir::cleanDirPath(workDir +"/dvd/VIDEO_TS");
    *k3b << "-videodvd";
    k3b->start();
    k3b->detach();
    delete k3b;
}

void k9BurnDVD::mkisoSizeStderr(KProcess *proc, char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);//   (proc2->readStderr());
    imageSize=c.replace("\n","");
    ;
}

void k9BurnDVD::mkisoSizeStdout(KProcess *proc, char *buffer, int buflen) {
    QString c=QString::fromLatin1(buffer,buflen);// (proc2->readStdout());
    imageSize=c.replace("\n","");
    ;
}

const QString &k9BurnDVD::getImageSize() {
    QString c;
    c="mkisofs";
    proc2=new k9Process;
    *proc2 << c;
    *proc2 <<"-quiet";
    *proc2 <<"-print-size";
    *proc2 <<"-dvd-video";
    *proc2 <<"-udf";
    *proc2 <<"-r";
    *proc2 <<"-V "+volId;
    *proc2 << QDir::cleanDirPath(workDir +"/dvd");

    connect( proc2, SIGNAL(receivedStderr(KProcess *, char *, int )),this, SLOT(mkisoSizeStderr(KProcess *, char *, int)) );
    connect( proc2, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(mkisoSizeStdout(KProcess *, char *, int)) );
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


void k9BurnDVD::burnWithGrowisofs() {
    time = new QTime(0,0);
    time->start();

    //KLibFactory *factory;
    //factory = KLibLoader::self()->factory("libk9copy");

//    progress=static_cast<k9Progress  *>(factory->create(qApp->mainWidget(),"progress", "k9Progress"));

    progress->setCaption(i18n("k9Copy - Burning DVD"));
    progress->setTitle(i18n("Burning DVD"));
    bool bok=false;


    while (!cancelled && !bok) {
        burnSpeed=0;
        QString c;
        if (iso)
            c="mkisofs";
        else
            c="growisofs";
        proc=progress->getProcess();
        *proc << c;

        if (!iso) {
            *proc <<"-overburn";
            *proc <<"-Z";
            *proc <<burnDevice;
            *proc <<"-use-the-force-luke=tty";
            *proc <<"-use-the-force-luke=tracksize:"+getImageSize();
            *proc <<"-use-the-force-luke=dao:" + imageSize;
            *proc <<"-dvd-compat";
            if (m_speed !=i18n("default"))
                *proc <<"-speed=" + m_speed;

        } else {
            QString fileName=m_filename;
            if (fileName =="")
                fileName=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
            if (fileName !="") {
                *proc <<"-o";
                *proc <<fileName;
            } else
                cancelled=true;

        }

        *proc <<"-dvd-video";
        *proc <<"-udf";
        *proc << "-r";
        *proc <<"-V "+volId;
        *proc <<QDir::cleanDirPath(workDir +"/dvd");

        connect( proc, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(growisoStderr(KProcess *, char *, int)) );
        connect( proc, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(growisoStdout(KProcess *, char *, int)) );
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
    delete time;

}

/** No descriptions */
void k9BurnDVD::growisoStderr(KProcess *proc, char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);// (proc->readStderr());
    char s[255];
    int a,b;
    int pos;
    lastMsg=c;
    if (c.contains("Current Write Speed")) {
        sscanf(c.latin1(),"%s \"Current Write Speed\" is %d.%d",s,&a,&b);
        burnSpeed=a+b/10;
    }
    progress->setTitle(i18n("Burning DVD"));
    progress->setLabelText(i18n("Current write speed :%1 x").arg(burnSpeed));
    if (c.contains("% done")) {
        pos=c.find("%");
        if (pos!=-1) {
            c=c.mid(1,pos-4);
            //progress->setLabelText(c);
            progress->setProgress(c.toInt(),100);
            float m_percent=c.toFloat()/100;
            QTime time2(0,0);
            time2=time2.addMSecs(time->elapsed());
            if (m_percent>0) {
                QTime time3(0,0);
		QString m_remain;
                time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
                m_remain=time3.toString("hh:mm:ss");
                progress->setElapsed(time2.toString("hh:mm:ss") +" / " +m_remain);

            }



        }
    }
}
void k9BurnDVD::growisoStdout(KProcess *proc, char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);// (proc->readStdout());
    int pos;
    pos=c.find("STAT");
    if (pos!=-1) {
        c=c.mid(pos);
        progress->setLabelText(c);
    }
}
