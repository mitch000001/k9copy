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
#include "k9tools.h"
#include <qdir.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klibloader.h>
#include <kprocess.h>
#include <qapplication.h>
#include <kstandarddirs.h>

k9BurnDVD::k9BurnDVD()
        : QObject(NULL,"") {
    cancelled=false;
    useK3b=false;
    autoBurn=false;
    volId="K9Copy";
    iso=false;
    m_filename="";
    progress=new k9Progress(NULL,"progress",NULL);
    QString s= KGlobal::dirs()->findResource( "data", "k9copy/anim.gif");
    progress->setMovie(s);
    m_cdrkit=k9Tools::checkProgram("genisoimage") && k9Tools::checkProgram("wodim");
}



k9BurnDVD::~k9BurnDVD() {
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
    if (!useK3b) {
        if (! m_cdrkit)
            burnWithGrowisofs();
        else
            burnWithGenisoimage();
    } else
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
    if (proc2->start(KProcess::NotifyOnExit,KProcess::All)) {
//        while (proc2->isRunning()) {
//            qApp->processEvents();
//        }
        proc2->sync();
        if (proc2->exitStatus()==0) {
            delete proc2;
            return imageSize;
        }
    }
    delete proc2;
    return "";
}

void k9BurnDVD::getGenisoimageCmd(k9Process *proc,QString _fileName,bool _printSize) {
    QString progname="genisoimage";
    *proc << progname;
    *proc << "-gui";
    *proc << "-graft-points";
    *proc << "-volid" << volId;
    *proc  <<"-appid" << "k9copy";
    *proc << "-volset-size" << "1";
    *proc << "-volset-seqno" << "1";
    *proc << "-no-cache-inodes" << "-udf";
    *proc << "-iso-level" << "1";
    *proc << "-dvd-video";
    if (!_printSize) {
        if (_fileName !="") {
            *proc <<"-o";
            *proc <<_fileName;
        }
        connect( proc, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(growisoStderr(KProcess *, char *, int)) );
        connect( proc, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(growisoStdout(KProcess *, char *, int)) );
    } else {
        *proc << "-print-size" << "-quiet";
        connect( proc, SIGNAL(receivedStderr(KProcess *, char *, int )),this, SLOT(mkisoSizeStderr(KProcess *, char *, int)) );
        connect( proc, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(mkisoSizeStdout(KProcess *, char *, int)) );
    }
    *proc <<QDir::cleanDirPath(workDir +"/dvd");

}

void k9BurnDVD::getWodimCmd(k9Process *proc) {
    *proc <<"|wodim";
    *proc <<"-dao";
    *proc <<"-overburn";
    *proc <<"-data";

    if (m_speed !=i18n("default"))
        *proc << QString("speed=%1").arg(m_speed);
    *proc <<QString("dev=%1").arg(burnDevice);
    *proc <<QString("tsize=%1s").arg(imageSize);
    *proc << "-";
}

void k9BurnDVD::burnWithGenisoimage() {
    time = new QTime(0,0);
    time->start();
    progress->setCaption(i18n("k9Copy - Burning DVD"));
    progress->setTitle(i18n("Burning DVD"));
    proc=progress->getProcess();

    bool bok=false;

    while (!cancelled && !bok) {
        burnSpeed=0;
        QString fileName="";
        if (iso) {
            fileName=m_filename;
            if (fileName =="")
                fileName=KFileDialog::getSaveFileName (QDir::homeDirPath(),"*.iso", 0,i18n("Save image to disk"));
            if (fileName =="") {
                cancelled=true;
            } 
        }
        proc->clearArguments();
        getGenisoimageCmd(proc,fileName,false);
        if (!iso) {
            k9Process *proc2=new k9Process(NULL,NULL);
            getGenisoimageCmd(proc2,"",true);
            proc2->start(KProcess::NotifyOnExit,KProcess::All);
            proc2->sync();
            getWodimCmd(proc);
            proc->setUseShell(true);
            if (!autoBurn) {
                if ( KMessageBox::warningContinueCancel ( 0,i18n("Insert a recordable DVD"), i18n("DVD burning"))!=KMessageBox::Continue)
                    cancelled=true;
            }
            qDebug(proc->debug());
        }        


        if (!cancelled) {
            int res=progress->execute();
            if ( res==-1 ) {
                KMessageBox::error( 0, i18n("Error burning DVD :\n")+i18n("Unable to run %1").arg("genisoimage"), i18n("DVD burning") );
                cancelled=true;
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
        QString c,progname;
        if (iso)
            progname="mkisofs";
        else
            progname="growisofs";
        proc=progress->getProcess();
        *proc << progname;

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
            if ( KMessageBox::warningContinueCancel ( 0,c, i18n("DVD burning"))!=KMessageBox::Continue)
                cancelled=true;
        }
        if (!cancelled) {
            int res=progress->execute();
            if ( res==-1 ) {
                KMessageBox::error( 0, i18n("Error burning DVD :\n")+i18n("Unable to run %1").arg(progname), i18n("DVD burning") );
                cancelled=true;
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
    if (c.contains("Speed set to")) {
        sscanf(c.latin1(),"Speed set to %d",&a);
        burnSpeed=a/1385;
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
