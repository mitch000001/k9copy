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
#include "k9dvdauthor.h"
#include "k9dvdtitle.h"
#include "k9dvdbackup.h"
#include "kmessagebox.h"
#include "k9tools.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <qdom.h>
#include <qfile.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qlabel.h>
#include <qprogressdialog.h>
#include <signal.h>
#include <klocale.h>
#include <qvaluelist.h>
#include <kstandarddirs.h>
#include <qapplication.h>


class k9Progress;

k9DVDAuthor::k9DVDAuthor(QObject *DVDStruct,const char* name,const QStringList& args)  {
    DVD=(k9DVD*)DVDStruct;
    xml=NULL;
    cancelled=false;
    error=false;
    progress = new k9Progress(qApp->mainWidget(),"progress",NULL);
    files.setAutoDelete(true);

}
k9DVDAuthor::~k9DVDAuthor() {
    QPtrListIterator <KTempFile> it (files);
    KTempFile *file;
    while ( (file = it.current()) != 0 ) {
        ++it;
        file->file()->remove();
    }
    if (xml!=NULL)
        delete xml;

}
/** No descriptions */
void k9DVDAuthor::createXML() {
    int i;
    if (xml!=NULL)
        delete xml;
    xml=new QDomDocument();

    QDomElement root = xml->createElement( "dvdauthor" );
    root.setAttribute ("dest",workDir+"dvd");
    xml->appendChild( root );

    // Create vmgm menu
    QDomElement vmgm = xml->createElement("vmgm");
    root.appendChild(vmgm);
    addMenus(vmgm);

    m_totalPartSize=0;

    m_forced=0;
    m_forcedsh=0;
    uint64_t chapterssize=0;
    //computes the size of related titles
    for (int iTitle=0; iTitle < DVD->gettitleCount();iTitle++) {
        k9DVDTitle *title=DVD->gettitle(iTitle);
        if (title->isSelected() && title->getIndexed()) {
	    chapterssize+= title->getChaptersSize(false);
            for (int iTitle2=0;iTitle2<title->getTitles().count() ;iTitle2++) {
                k9DVDTitle *title2=title->getTitles().at(iTitle2);
                m_totalPartSize+= title2->getsize_mb() *1024*1024;
		if (title->getforceFactor()) {
			m_forced+=title2->getsectors()*2048;
			m_forcedsh+=(title2->getsectors()/title->getfactor())*2048;
		}		
		chapterssize+= title2->getChaptersSize(false);
            }
	}
        //total size of forced titles    
	if (title->isSelected() && title->getforceFactor()) {
	    m_forced+=title->getsectors()*2048;
	    m_forcedsh+=(title->getsectors()/title->getfactor())*2048;
	}

    }
     
    //total size of titles to copy
    m_totalSize=((uint64_t)DVD->getsizeSelected(false))*DVD_VIDEO_LB_LEN + m_totalPartSize -chapterssize*DVD_VIDEO_LB_LEN;
    
    m_firsttitle=true;

    for (i=0;i< DVD->gettitleCount();i++) {
        k9DVDTitle *tmp = DVD->gettitle(i);
        addTitle(root,tmp);
    }

    QString x = xml->toString();
    
    //QFile file(  locateLocal("tmp", "k9copy/k9author.xml" ));
    
   // if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( m_xml->file() );
        xml->save(stream,1);
        m_xml->file()->close();
   // }
}

void k9DVDAuthor::addMenus(QDomElement &titleSet) {
    int i;
    QDomElement menu,video,pgc,pre,vob,button;
    QDomText precmd;
    QDomText buttonText;
    QTime max(0,0);
    QString c,start;

    k9DVDTitle * l_track= DVD->getstart();
    if (l_track==NULL) {
        start="call vmgm menu;";
    } else {
        if( DVD->getnewTitleNum(l_track)==-1) {
            error=true;
            QString sMsg;
            //sMsg.QString::sprintf(tr2i18n("'%s' not selected"),l_track->getname().latin1());
            sMsg=i18n("'%1' not selected").arg(l_track->getname());
            KMessageBox::error( 0,sMsg, tr2i18n("authoring"));
        }
        start.sprintf("jump titleset %d menu;",DVD->getnewTitleNum(l_track));
    }
    factor=DVD->getfactor(false,false,true);
    menu=xml->createElement("menus");
    titleSet.appendChild(menu);
    pgc=xml->createElement("pgc");
    pgc.setAttribute("entry","title");
    menu.appendChild(pgc);
    pre=xml->createElement("pre");
    pgc.appendChild(pre);
    precmd=xml->createTextNode("");
    QString cmd="if (g0==0) \n { \n subtitle=0; \n g0=1;\n "+start+"\n}\n";
    pre.appendChild(precmd);
    int numt=0;

    for (i=0;i < DVD->gettitleCount();i++) {
        if (DVD->gettitle(i)->isSelected() && DVD->gettitle(i)->getIndexed()) {
            ++numt;
            cmd+=c.sprintf("if (g1==%d) {jump titleset %d menu;}\n", numt,numt);
        }
    }

    precmd.setNodeValue(cmd);
}

/** No descriptions */
void k9DVDAuthor::addTitle(QDomElement &root, k9DVDTitle *title) {
    int i,j;

    k9DVDSubtitle *l_sub;
    k9DVDAudioStream *l_auds;
    k9DVDTitle *l_track= title;
    QDomElement e,t,pgc;
    QString caud="",csub="",c,palette;

    if (l_track->isSelected() && l_track->getIndexed()) {
	double titleFactor;
	if (l_track->getforceFactor())
		titleFactor=l_track->getfactor();
	else
		titleFactor=factor;
        QDomElement titleSet = xml->createElement("titleset");
        root.appendChild(titleSet);
        QDomElement titleMenu = xml->createElement("menus");
        titleSet.appendChild(titleMenu);
        QDomElement pgc = xml->createElement("pgc");
        titleMenu.appendChild(pgc);
        QDomElement pre = xml->createElement("pre");
        pgc.appendChild(pre);
        QDomText precmd=xml->createTextNode("");

        int subtitle=0,audio=-1;
        if (l_track->getDefSubtitle() != NULL) {
            for (int isub=0;isub<l_track->getsubPictureCount();isub++) {
                if (l_track->getsubtitle(isub)->getselected()) {
                    subtitle++;
                }
                if (l_track->getsubtitle(isub)==l_track->getDefSubtitle())
                    break;
            }
        }
        if (l_track->getDefAudio() != NULL) {
            for (int iaud=0;iaud < l_track->getaudioStreamCount();iaud++) {
                if (l_track->getaudioStream(iaud)->getselected()) {
                    audio++;
                }
                if(l_track->getaudioStream(iaud)==l_track->getDefAudio())
                    break;
            }
        }

        QString txtcmd;
        if (audio !=-1) {
            txtcmd="subtitle=%1;\n audio=%2;\n g1=0;jump title 1;";
            txtcmd=txtcmd.arg(subtitle+63).arg(audio);
        } else {
            txtcmd="subtitle=%1;\n g1=0;jump title 1;";
            txtcmd=txtcmd.arg(subtitle+63);
        }

        precmd.setNodeValue(txtcmd);
        pre.appendChild(precmd);

        //create palette for subpictures
        KTempFile *paletteFile=new KTempFile(locateLocal("tmp", "k9copy/k9p"), ".yuv");
        files.append(paletteFile);
        paletteFile->setAutoDelete(false);
        palette=paletteFile->name();

        //palette.sprintf("palette%d.yuv",l_track->getnumTitle());
        //palette= locateLocal("tmp", "k9copy/k9" +palette);
        QTextStream stream( paletteFile->file() );
        for (j=0;j<16;j++) {
            l_track->getpalette(j,c);
            stream << c+"\n";
        }
        paletteFile->file()->close();

        t=xml->createElement("titles");
        titleSet.appendChild(t);
        e=xml->createElement("video");
        e.setAttribute("aspect",l_track->getaspectRatio());
        e.setAttribute("format",l_track->getformat().lower());
        if (l_track->getaspectRatio()!="4:3") {
            e.setAttribute("widescreen","nopanscan");
        }
        t.appendChild(e);

        for (i=0;i<l_track->getaudioStreamCount();i++) {
            l_auds=l_track->getaudioStream(i);
            if (l_auds->getselected()) {
                e=xml->createElement("audio");
                e.setAttribute("format",l_auds->getformat());
                e.setAttribute("channels",l_auds->getchannels());
                e.setAttribute("quant",l_auds->getquantization());
                e.setAttribute("lang",l_auds->getlangCod());
                t.appendChild(e);
                if (caud != "")
                    caud+=',';
                caud+=c.sprintf("%d",l_auds->getID());
            }
        }

        for (i=0;i<l_track->getsubPictureCount();i++) {
            l_sub=l_track->getsubtitle(i);
            if (l_sub->getselected()) {
                e=xml->createElement("subpicture");
                e.setAttribute("lang",l_sub->getlangCod());
                t.appendChild(e);
                if (csub !="")
                    csub+=',';
                csub+=c.sprintf("%d",l_sub->getID());
            }
        }

        pgc=xml->createElement("pgc");
        pgc.setAttribute("palette",palette);
        t.appendChild(pgc);

        if (caud !="")
            caud="--audiofilter "+caud;
        if (csub !="")
            csub="--subpicturefilter "+csub;

        int numPart=0;

        for (int iTitle=0;iTitle<=l_track->getTitles().count();iTitle++) {
            k9DVDTitle *title;
            if (iTitle==0)
                title=l_track;
            else
                title=l_track->getTitles().at(iTitle-1);

            for (i=0;i<title->getchapterCount();i++) {
                numPart++;
                uint icell=0;
                k9DVDChapter *l_chap=title->getChapter(i);
		if (!l_chap->getSelected())
		    continue;
		
                bool first=true;
                uint32_t chapterSize= (l_chap->getendSector()-l_chap->getstartSector())*DVD_VIDEO_LB_LEN;
                QString sChapter,sCell;

                for (k9ChapterCell *cell =l_chap->cells.first();cell ;cell =l_chap->cells.next() ) {
                    icell++;

                    sCell = QString("--cell %1").arg(icell);
                    sChapter=QString("--chapter %1").arg(numPart);

                    //test
                    uint32_t itotSize = (cell->getlastSector()-cell->getstartSector())* DVD_VIDEO_LB_LEN;
                    QString file;
                    e=xml->createElement("vob");
                    file=QString("k9copy --play --input %1 --dvdtitle %2 %3 %4  %5  %6 --vampsfactor %7 --inputsize %8 --chaptersize %9 ")
                         .arg(DVD->getDevice())
                         .arg(title->getnumTitle())
                         .arg(sChapter)
                         .arg(sCell)
                         .arg(caud)
                         .arg(csub)
                         .arg(titleFactor,0,'f',2)
                         .arg(itotSize,0,'f',0)
                         .arg(chapterSize,0,'f',0);
                    if (m_firsttitle) {
                        file +=" --initstatus ";
                        m_firsttitle=false;
                    }
		    if (l_track->getforceFactor()) {
			file +=" --ffactor ";    
		    }
                    file +=QString(" --inject %1 --totalsize %2 --dvdsize %3 |")
                           .arg(inject)
                           .arg(m_totalSize -m_forced  ,0,'f',0)
                           .arg(((uint64_t)k9DVDSize::getMaxSize() *1024 *1024) - m_forcedsh,0,'f',0);

                    e.setAttribute("file",file);
                    if (first)
                        e.setAttribute("chapters",l_chap->gettime().toString("0"));
                    pgc.appendChild(e);
                    first=false;
                }
                //          }
            }

        }
        QDomElement post = xml->createElement("post");
        pgc.appendChild(post);
        QDomText postcmd=xml->createTextNode("");

        k9DVDTitle * l_next=l_track->getnextTitle();
        if (l_next!=NULL) {
            c.sprintf("g1=%d;\ncall vmgm menu;",DVD->getnewTitleNum(l_next));
            if( DVD->getnewTitleNum(l_next)==-1) {
                error=true;
                c=i18n("'%1' not selected").arg(l_next->getname());
                KMessageBox::error( 0, c, tr2i18n("authoring"));
            }

            postcmd.setNodeValue(c);
        }
        post.appendChild(postcmd);
    }

}


const QString& k9DVDAuthor::getworkDir() {
    return workDir;
}
void k9DVDAuthor::setworkDir( const QString& _newVal) {
    workDir = _newVal;
    if (workDir.right(1)!='/')
        workDir +='/';
}

void k9DVDAuthor::author() {

    if ( ! k9Tools::checkProgram("dvdauthor")) {
	KMessageBox::error (qApp->mainWidget(),i18n("Unable to run %1").arg("dvdauthor") , i18n("authoring"));	
	error = TRUE;
	return;
    }


    bool burnOk=false;
    //nettoyage du répertoire de sortie
    k9Tools::clearOutput(workDir+"dvd");

    time = new QTime(0,0);
    time->start();

    progress->setTitle(i18n("Authoring"));
    progress->setCaption(i18n("k9Copy - Backup progression"));
    progress->setProgress(0,100);
    //progress->show();

    m_xml=new KTempFile(locateLocal("tmp", "k9copy/k9a"),".xml");

    m_inject=new KTempFile(locateLocal("tmp", "k9copy/k9v"),".inj");
    inject=m_inject->name();

    if (!cancelled && !error)
        createXML();
//    if (error || cancelled)
//        delete progress;

    //run dvdauthor
    if (!cancelled  && !error) {
        QString c("dvdauthor");
        proc=progress->getProcess();//  new QProcess(c,0);
        *proc << c << "-x" <<  m_xml->name();  //locateLocal("tmp", "k9copy/k9author.xml");
        connect( proc, SIGNAL(receivedStderr(KProcess *, char *, int)),
                 this, SLOT(DVDAuthorStderr(KProcess *, char *, int )) );
        connect( proc, SIGNAL(receivedStdout(KProcess *, char *, int )),
                 this, SLOT(DVDAuthorStdout(KProcess *, char *, int)) );
        //    connect(progress, SIGNAL(cancelled()), this, SLOT(stopProcess()));

        m_copied=0;
        m_lastPos=0;
        //if (m_totalSize >k9DVDSize::getMaxSize())
        //    m_totalSize=k9DVDSize::getMaxSize();
        proc-> setWorkingDirectory(workDir);
        int result=progress->execute();
        if ( result==-1 ) {
            KMessageBox::error( 0,  tr2i18n("Dvdauthor error :\n") + lastMsg,tr2i18n("authoring"));

        } else {
            cancelled=(result==0);
            if ((proc->exitStatus()==0) && (proc->normalExit()) && !cancelled && !error) {
                burnOk=true;
            } else {
                if (cancelled) {
                    //QMessageBox::critical( 0, tr2i18n("authoring"), tr2i18n("Authoring canceled"));
                    KMessageBox::error( 0,  tr2i18n("Authoring cancelled"),tr2i18n("authoring"));

                } else
                    KMessageBox::error( 0,  tr2i18n("An error occured while running DVDAuthor:\n")+lastMsg ,tr2i18n("authoring"));

            }
        }
        //        delete proc;
 //       delete progress;
        if (!burnOk)
            error=true;
    }

    delete time;
    m_xml->file()->remove();
    m_inject->file()->remove();
    delete m_inject;
    delete m_xml;
}


/** No descriptions */
void k9DVDAuthor::DVDAuthorStderr(KProcess *proc, char *buffer, int buflen ) {

    //QString m_stderr(proc->readStderr());
    QString m_stderr=QString::fromLatin1(buffer, buflen);
    float m_percent;
    QString m_remain;

    int pos=m_stderr.find("INFOPOS:");
    if (pos!=-1) {
        progress->setTitle(i18n("Authoring"));
        QString tmp=m_stderr.mid(pos);
        uint32_t totalBytes,totalSize;
        sscanf(tmp.latin1(),"INFOPOS: %d %d",&totalBytes,&totalSize);
        //if (totalBytes>m_lastPos)
        //   m_copied+=totalBytes - m_lastPos;
        m_copied=totalBytes;
        m_lastPos=totalBytes;
        //qDebug(QString("copied : %1   totalSize : %2").arg(m_copied).arg(m_totalSize*512));
        m_percent=(float)m_copied / (float)(m_totalSize/DVD_VIDEO_LB_LEN );


        QTime time2(0,0);
        time2=time2.addMSecs(time->elapsed());
        if (m_percent>0) {
            QTime time3(0,0);
            time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
            m_remain=time3.toString("hh:mm:ss");
        }

        m_percent*=100;
        progress->setLabelText("");

        progress->setProgress(m_percent,100);
        progress->setElapsed(time2.toString("hh:mm:ss") +" / " +m_remain);

    }
    /*    else {
            if (!m_stderr.startsWith("libdvdread") && m_stderr.startsWith("libdvdnav"))
    	   qDebug(m_stderr);
        }
    */
    int end;
    lastMsg=m_stderr;

    if (m_stderr.contains("STAT:")) {
        pos=m_stderr.find("fixing VOBU");
        if (pos!=-1) {
            progress->setTitle(i18n("Authoring"));
            progress->setLabelText(i18n("Fixing VOBUS"));
            end=m_stderr.find("%");
            if (end!=-1) {
                pos =end -2;
                m_stderr=m_stderr.mid(pos,end-pos);
                m_stderr=m_stderr.stripWhiteSpace();
                //progress->setLabelText(c);
                progress->setProgress(m_stderr.toInt(),100);
            }
        }
    }

    pos=m_stderr.find("INFOIMAGE:");
    if (pos!=-1) {
       progress->setImage(m_stderr.mid(pos+10));
    } 



}

void k9DVDAuthor::DVDAuthorStdout(KProcess *proc, char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);
    //(proc->readStdout());
    int pos;
    pos=c.find("STAT");
    if (pos!=-1) {
        c=c.mid(pos);
        progress->setLabelText(c);
        qApp->processEvents();
    }
}

/** No descriptions */
void k9DVDAuthor::stopProcess() {
    proc->kill();
    cancelled=true;
}

/** Read property of bool burnDVD. */
const bool& k9DVDAuthor::getburnDVD() {
    return burnDVD;
}
/** Write property of bool burnDVD. */
void k9DVDAuthor::setburnDVD( const bool& _newVal) {
    burnDVD = _newVal;
}



bool k9DVDAuthor::getError() {
    return error;
}
