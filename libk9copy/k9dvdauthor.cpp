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
}
k9DVDAuthor::~k9DVDAuthor() {
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

    inject = locateLocal("tmp", "k9v" + (QTime::currentTime()).toString("hhmmss"));
    totSize.sprintf("%.0f",(double)DVD->getsizeSelected()*1024*1024);


    for (i=0;i< DVD->gettitleCount();i++) {
        addTitle(root,i);
    }

    QString x = xml->toString();

    QFile file(  locateLocal("tmp", "k9author.xml" ));
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( &file );
        xml->save(stream,1);
        file.close();
    }


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
    factor=DVD->getfactor(false,false);
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
void k9DVDAuthor::addTitle(QDomElement &root, int title) {
    int i,j;

    k9DVDSubtitle *l_sub;
    k9DVDAudioStream *l_auds;
    k9DVDTitle *l_track= DVD->gettitle(title);
    QDomElement e,t,pgc;
    QString caud="",csub="",c,palette;

    if (l_track->isSelected() && l_track->getIndexed()) {
        QDomElement titleSet = xml->createElement("titleset");
        root.appendChild(titleSet);
        QDomElement titleMenu = xml->createElement("menus");
        titleSet.appendChild(titleMenu);
        QDomElement pgc = xml->createElement("pgc");
        titleMenu.appendChild(pgc);
        QDomElement pre = xml->createElement("pre");
        pgc.appendChild(pre);
        QDomText precmd=xml->createTextNode("");
        precmd.setNodeValue("g1=0;jump title 1;");
        pre.appendChild(precmd);

        //create palette for subpictures
        palette.sprintf("palette%d.yuv",l_track->getnumTitle());
        palette= locateLocal("tmp", "k9" +palette);
        QFile file(palette);
        if ( file.open( IO_WriteOnly ) ) {
            QTextStream stream( &file );
            for (j=0;j<16;j++) {
                l_track->getpalette(j,c);
                stream << c+"\n";
            }
            file.close();
        }

        t=xml->createElement("titles");
        titleSet.appendChild(t);
        e=xml->createElement("video");
        e.setAttribute("aspect",l_track->getaspectRatio());
        e.setAttribute("format",l_track->getformat().lower());
        //if (l_track->getaspectRatio()=="16:9") {
        e.setAttribute("widescreen","nopanscan");
        //}
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

        for (i=0;i<l_track->getchapterCount();i++) {
            k9DVDChapter *l_chap=l_track->getChapter(i);
            bool first=true;
           for (k9ChapterCell *cell =l_chap->cells.first();cell ;cell =l_chap->cells.next() ) {
		if (cell->getangle()==1) {
			QString file;
			e=xml->createElement("vob");
			file.sprintf("play_cell %s %d %d %d |vamps ",DVD->getDevice().latin1(), l_track->getVTS() ,l_track->getpgc() +1,cell->getid() +1);
			if (caud!="")
			file+=" -a " + caud;
			if (csub!="")
			file+=" -s " + csub;
			file +=c.sprintf(" -E %6.2f", factor);
			file +=" -i " +inject;
			file +=" -S " +totSize;
			file +=" |";
	
			e.setAttribute("file",file);
			//if (first) e.setAttribute("chapters",l_chap->gettime().toString("h:mm:ss"));
			if (first)
			e.setAttribute("chapters",l_chap->gettime().toString("0"));
			pgc.appendChild(e);
			first=false;
		}
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
    bool burnOk=false;
    //nettoyage du répertoire de sortie
    clearOutput(workDir+"dvd");

    //progress= new QProgressDialog ("DVDAuthor",i18n("Cancel"),100,qApp->mainWidget(),"progress",true,0);
    progress = new k9Progress(qApp->mainWidget(),"progress",true,0);
    progress->setLabelText(tr2i18n("Authoring"));
    progress->setCaption(i18n("k9Copy - Backup progression"));
    progress->setProgress(100,100);
    //progress->show();
    if (!cancelled && !error)
        createXML();
    if (error || cancelled)
        delete progress;

    //run dvdauthor
    if (!cancelled  && !error) {
        QString c("dvdauthor");
        proc=progress->getProcess();//  new QProcess(c,0);
	proc->addArgument(c);
        proc->addArgument("-x");
        proc->addArgument( locateLocal("tmp", "k9author.xml"));
        connect( proc, SIGNAL(readyReadStderr()),
                 this, SLOT(DVDAuthorStderr()) );
        connect( proc, SIGNAL(readyReadStdout()),
                 this, SLOT(DVDAuthorStdout()) );
    //    connect(progress, SIGNAL(cancelled()), this, SLOT(stopProcess()));


        totalSize=(int)DVD->getsizeSelected();
        if (totalSize >k9DVDSize::getMaxSize())
            totalSize=k9DVDSize::getMaxSize();
        QDir dir(workDir);
        proc->setWorkingDirectory(dir);
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
        delete progress;
        if (!burnOk)
            error=true;
    }


}


/** No descriptions */
void k9DVDAuthor::DVDAuthorStderr() {
    QString c(proc->readStderr());
    int pos,end;
    lastMsg=c;
    if (c.contains("STAT:")) {
        pos=c.find("at ");
        if (pos!=-1) {
            pos+=3;
            end=c.find("MB");
            if (end!=-1) {
                c=c.mid(pos,end-pos);
                //progress->setLabelText(c);
                progress->setProgress(c.toInt(),totalSize);
                qApp->processEvents();
            }
        }
    }


}
void k9DVDAuthor::DVDAuthorStdout() {
    QString c(proc->readStdout());
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
    proc->tryTerminate();
    cancelled=true;
}

void k9DVDAuthor::clearOutput(QString name) {
    QDir dir(name);
    //delete files in directory
    QStringList lst = dir.entryList( "*",QDir::Files |QDir::Hidden );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QString c(( *it ).latin1() );
        dir.remove (c);
    }
    //scanning subdir
    QStringList lstdir = dir.entryList( "*",QDir::Dirs );
    for ( QStringList::Iterator it = lstdir.begin(); it != lstdir.end(); ++it ) {
        QString c(( *it ).latin1() );
        if ((c!=".") && c!="..") {
            clearOutput(dir.absFilePath(c));
            dir.rmdir(c);
        }
    }

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
