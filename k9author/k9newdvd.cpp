//
// C++ Implementation: k9newdvd
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9newdvd.h"
#include "k9title.h"
#include "k9tools.h"
#include <qfile.h>
#include <stdio.h>
#include <qtextstream.h>
#include <kstandarddirs.h>
#include <qapplication.h>
#include <ktempfile.h>
#include <kapplication.h>
#include <klocale.h>
#include <qimage.h>
#include <kmessagebox.h>
#include "k9menu.h"
#include "k9menubutton.h"
#include "k9processlist.h"
#include <qthread.h>
#include <qfileinfo.h>

k9NewDVD::k9NewDVD(QObject *parent, const char *name)
        : QObject(parent, name) {
    m_workDir=locateLocal("tmp", "k9copy/" ) ;
    m_rootMenu=new k9Menu(this);
    m_format=PAL;
}


k9NewDVD::~k9NewDVD() {}

int k9NewDVDItems::compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2) {
    k9Title *_i1=(k9Title*) item1;
    k9Title *_i2=(k9Title*) item2;

    return (_i1->getNum() - _i2->getNum());


}

void k9NewDVD::execute() {
    m_cancel=false;
    m_error="";
    m_config=new k9Config();
  //  connect(m_process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));

    connect(&m_aviDecode,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));
    k9Tools::clearOutput(m_workDir+"dvd");
    createXML();
    disconnect(&m_aviDecode,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));
    delete m_config;
}

#include "k9newdvd.moc"


void k9NewDVD::drawImage(QImage * _image) {
//    m_progress->setImage(*_image);
}

void k9NewDVD::setFormat ( const eFormat& _value ) {
    m_format = _value;
    m_rootMenu->setFormat((k9Menu::eFormat)_value);
    for (k9Title *title=m_titles.first();title;title=m_titles.next()) {
        title->getMenu()->setFormat((k9Menu::eFormat)_value);
    }

}

void k9NewDVD::createXML() {

    m_rootMenu->setWorkDir(m_workDir);

    QString menuFileName=m_workDir+KApplication::randomString(8)+".mpg";
    m_rootMenu->setMenuFileName(menuFileName);

    m_xml=new QDomDocument();
    QDomElement root = m_xml->createElement( "dvdauthor" );
    root.setAttribute ("dest",m_workDir+"dvd");
    m_xml->appendChild( root );

    // Create vmgm menu
    QDomElement vmgm = m_xml->createElement("vmgm");
    root.appendChild(vmgm);
    m_processList->addProgress(i18n("Creating root menu"));
    m_rootMenu->createMenus(&vmgm);

    addTitles(root);
    m_processList->execute();
    m_totalEncodedSize=0;
    m_offset=0;
    m_lastvalue=0;
    for ( QStringList::Iterator it = m_tmpFiles.begin(); it != m_tmpFiles.end(); ++it ) {
        QString file= *it;
	if (file.endsWith(".mpeg")) {
		QFileInfo f(file);
		m_totalEncodedSize+=f.size();
	}
    }
    m_totalEncodedSize/=1024*1024;
    m_cancel=m_processList->getCancel();
    bool error=false;
    if (!m_cancel) {
        QString dvdAuthor(m_workDir+"/"+KApplication::randomString(8)+".xml");
        QFile file( dvdAuthor);

        file.open(IO_WriteOnly);
        QTextStream stream( &file );
        m_xml->save(stream,1);
        file.close();
  
        m_processList->clear();
        k9Process *process=m_processList->addProcess(i18n("authoring"));
        connect(process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));
        *process << "dvdauthor" << "-x" << dvdAuthor;
        m_processList->execute();
        m_cancel=m_processList->getCancel();
        error=m_processList->getError();
       // else
       //     m_error=i18n("An error occured while running dvdauthor");

        file.remove();
    }
    if (m_cancel)
        m_error=i18n("The dvd authoring was canceled");
    else if (error)
        m_error=i18n("An error occured while running DVDAuthor:\n")+ m_stdout;
    QFile::remove(menuFileName);
    for ( QStringList::Iterator it = m_tmpFiles.begin(); it != m_tmpFiles.end(); ++it ) {
        QFile::remove(*it);
    }
    m_tmpFiles.clear();
    if (m_error !="") {
        KMessageBox::error( 0, m_error, i18n("Authoring"));
    }
    delete m_xml;

}

void k9NewDVD::addTitles (QDomElement &_root) {
    calcVideoBitrate();
    for (k9Title *title=m_titles.first();title && !m_cancel;title=m_titles.next()) {
        QDomElement titleSet = m_xml->createElement("titleset");
        _root.appendChild(titleSet);
        QDomElement pgc;
        k9Menu *menu=title->getMenu();
        menu->setWorkDir(m_workDir);
        QString menuFileName=m_workDir+KApplication::randomString(8)+".mpg";
        m_tmpFiles << menuFileName,
        menu->setMenuFileName(menuFileName);
        m_processList->addProgress(i18n("Creating menu for title %1").arg(title->getNum()+1));
        menu->createMenus(&titleSet);

        QDomElement eTitle=m_xml->createElement("titles");
        titleSet.appendChild(eTitle);
        QDomElement e=m_xml->createElement("video");
        e.setAttribute("aspect","16:9");
        e.setAttribute("format",m_format==PAL?"PAL":"NTSC");
//        if (l_track->getaspectRatio()!="4:3") {
        e.setAttribute("widescreen","nopanscan");
//       }setProgressWindow
        eTitle.appendChild(e);

        e=m_xml->createElement("audio");
        e.setAttribute("format","ac3");
        e.setAttribute("channels","2");
        eTitle.appendChild(e);

        pgc=m_xml->createElement("pgc");
        eTitle.appendChild(pgc);
        QDomElement post=m_xml->createElement("post");
        pgc.appendChild(post);
        QDomText txt=m_xml->createTextNode(title->getMenu()->getEndScript());
        post.appendChild(txt);

        QPtrList <k9AviFile > *l=title->getFiles();
        for (k9AviFile *aviFile= l->first();aviFile && !m_cancel;aviFile=l->next()) {
            if ( aviFile->getPrevious()==NULL || aviFile->getBreakPrevious()) {
                QString cmd="",chapters="";
                createMencoderCmd(cmd,chapters,aviFile);
                e=m_xml->createElement("vob");
                e.setAttribute("file",cmd);
                e.setAttribute("chapters",chapters);
                pgc.appendChild(e);
                m_tmpFiles << cmd;
            }
        }
    }

}

void k9NewDVD::setWorkDir ( const QString& _value ) {
    m_workDir = _value;
}

void k9NewDVD::createMencoderCmd(QString &_cmd,QString &_chapters, k9AviFile *_aviFile) {
 //   m_aviDecode.open(_aviFile->getFileName());
    m_timer.start();
    m_timer2.start();
    m_timer3.start();
    QTime end;
    k9AviFile *file=_aviFile;
    bool bEnd;
    _chapters="0";
    do {
        end=file->getEnd();
        bEnd= (file->getNext()==NULL) || (file->getBreakNext());
        file=file->getNext();
        if (!bEnd) {
            int lt=_aviFile->getStart().msecsTo(end);
            QTime t;
            t=t.addMSecs(lt);
            _chapters +="," + t.toString("hh:mm:ss");
        }
    } while (!bEnd);

    QString fileName= m_workDir + KApplication::randomString(8)+".mpeg";
    QString t1=_aviFile->getStart().toString("hh:mm:ss.zzz");
    int length=_aviFile->getStart().msecsTo(end);
    QTime l;
    l=l.addMSecs(length);
    QString t2=l.toString("hh:mm:ss.zzz");
    QString scale;
    QString fps;
    switch (m_format) {
    case PAL:
        scale="720:576";
        fps="25";
        break;
    case NTSC:
        scale="720:480";
        fps="30000/1001";
        break;
    }

    k9Process *process=m_processList->addProcess(i18n("Encoding %1").arg(_aviFile->getFileName()));
    m_processList->setFileName(process,_aviFile->getFileName());

    QTime t(0,0);
    t.start();
    m_timers[process]=t;
    connect(process, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(getStdout(KProcess *, char *, int) ));
    //m_progress->setTitle(i18n("Encoding file"));
    //m_process->clearArguments();
    *process << "mencoder" << "-oac" << "lavc" << "-ovc" << "lavc" << "-of" << "mpeg";
    *process << "-mpegopts" << "format=dvd" << "-vf" << "scale="+scale+",harddup" << "-srate" << "48000" << "-af" << "lavcresample=48000";
    *process << "-lavcopts" << QString("vcodec=mpeg2video:vrc_buf_size=1835:vrc_maxrate=9800:vbitrate=%1:keyint=15:acodec=%3:abitrate=%2:aspect=16/9").arg(m_videoBitrate).arg(m_config->getPrefAudioBitrate()).arg(m_config->getPrefAudioFormat().lower());
    *process << "-ofps" << fps << "-o" << fileName << "-ss" << t1 << "-endpos" << t2 << _aviFile->getFileName();
    qDebug(process->debug()); 
/*
    if (!m_progress->execute()) {
        m_cancel=true;
        if (m_progress->getCanceled())
            m_error=i18n("The dvd authoring was canceled");
        else
            m_error=i18n("An error occured while transcoding video");
    }
*/

    _cmd=fileName;
//    m_aviDecode.close();
}

void k9NewDVD::getStderr(KProcess *_process, char *_buffer, int _length) {
    QCString tmp(_buffer,_length);
    m_stdout=tmp;
    int pos;
    if (tmp.contains("STAT:")) {
        pos=tmp.find("fixing VOBU");
        if (pos!=-1) {  
            QString tmp2=tmp;
//            m_progress->setTitle(i18n("Authoring"));
//            m_progress->setLabelText(i18n("Fixing VOBUS"));
            int end=tmp2.find("%");
            if (end!=-1) {
                pos =end -2;
                tmp2=tmp2.mid(pos,end-pos);
                tmp2=tmp2.stripWhiteSpace();
//                m_progress->setProgress(tmp2.toInt(),100);
            }
        } else {
	   pos=tmp.find("STAT: VOBU ");
	   if (pos !=-1) {
	        QCString tmp2(_buffer+pos,_length-pos);
		int vobu,mb;
		sscanf(tmp2.data(),"STAT: VOBU %d at %dMB",&vobu,&mb);
		if (mb <m_lastvalue) 
		   m_offset+=m_lastvalue;
		m_lastvalue=mb;
		m_processList->setProgress((k9Process*)_process,mb+m_offset,m_totalEncodedSize);
	   }
	} 

//STAT: VOBU 16 at 3MB, 1 PGCS

    }

}

void k9NewDVD::getStdout(KProcess *_process, char *_buffer, int _length) {
    k9Process *process=(k9Process*) _process;
    if (m_timers[process].elapsed() >500) {
        QCString tmp(_buffer,_length);
        int pos=tmp.find("Pos:");
        if (pos!=-1) {
            QString tmp2=tmp.mid(pos);
            tmp2=tmp2.replace(":",": ").replace("(","").replace(")","").simplifyWhiteSpace();
            QStringList sl=QStringList::split(" ",tmp2);
            float position;
            sscanf(sl[1].latin1(),"%fs",&position);
            int frame;
            sscanf(sl[2].latin1(),"%df",&frame);
            int percent;
            sscanf(sl[3].latin1(),"%d",&percent);
            int fps;
            sscanf(sl[4].latin1(),"%d",&fps);
            m_processList->setProgress(process,percent,100);
            m_processList->setPos(process,position);
            //m_progress->setProgress(percent,100);
    //        if (percent>0 &&m_timer3.elapsed() >1000 ) {
            if (percent>0 ) {
                int elapsed=process->getElapsed();
                QTime time2(0,0);
                time2=time2.addMSecs(elapsed);
                QTime time3(0,0);
                float fprc=percent/100.0;
                time3=time3.addMSecs((uint32_t)(elapsed*(1.0/fprc)));
                m_processList->setText(process,time2.toString("hh:mm:ss") +" / " + time3.toString("hh:mm:ss"),1);
                m_timer3.restart();
            }
    
            QString text;//=i18n("filename") + " : " + m_aviDecode.getFileName();
            text=i18n("fps")+ " : "+QString::number(fps);
            m_processList->setText(process,text,2);
    /*
            m_progress->setLabelText(text);
            if (m_timer.elapsed() > 5000) {
                m_timer.restart();
                if (m_aviDecode.opened()) {
                    m_aviDecode.readFrame(position);
                }
            }
    */
        }
        m_timers[process].restart();
    }
}

void k9NewDVD::appendTitle(k9Title *_title) {
    m_config=new k9Config();
    m_titles.append(_title);
    m_titles.sort();
    //create the menu button
    k9MenuButton *btn=m_rootMenu->addButton();
    _title->setButton(btn);
    btn->setNum(_title->getNum()+1);
    QPixmap px(m_config->getPrefButtonWidth(),m_config->getPrefButtonHeight());
    px.fill(Qt::black);
    QImage img=px.convertToImage();
    btn->setImage(img);
    int nbColumn=(720-50)/(m_config->getPrefButtonWidth()+50);
    int top=(int) _title->getNum()/nbColumn ;
    int left=_title->getNum() %nbColumn;
    btn->setTop(top*(m_config->getPrefButtonHeight()+20) +50);
    btn->setLeft(left*(m_config->getPrefButtonWidth()+50) +50);
    btn->setWidth(m_config->getPrefButtonWidth());
    btn->setHeight(m_config->getPrefButtonHeight());
    btn->setScript(QString("g1=0;jump titleset %1  menu;").arg(_title->getNum()+1));
    btn->setTextPosition(k9MenuButton::RIGHT);
    btn->setText(i18n("title %1").arg(_title->getNum()+1));
    btn->setColor(m_config->getPrefButtonTextColor());
    btn->setFont(m_config->getPrefButtonFont());

    QString script="\n";
    for (k9Title *t = m_titles.first();t;t=m_titles.next()) {
        script +=QString("if (g6== %1) { g6=0; jump titleset %2 menu;}\n").arg(t->getNum()+1).arg(t->getNum()+1);
    }
    m_rootMenu->setStartScript2(script);
    emit sigAddTitle();
    delete m_config;
}



void k9NewDVD::setProcessList(k9ProcessList *_value) {
    m_processList=_value;
}


k9NewDVD::eFormat k9NewDVD::getFormat() const {
    return m_format;
}


k9Menu* k9NewDVD::getRootMenu() const {
    return m_rootMenu;
}

void k9NewDVD::calcVideoBitrate() {
    // bitrate video = (MB *8388.608) /SEC    - bitrate audio
    int length=0;
    for (k9Title *title=m_titles.first();title;title=m_titles.next()) {
        k9TitleItems *chapters=title->getFiles();
        for (k9AviFile *chapter=chapters->first();chapter;chapter=chapters->next()) {
            length+=chapter->getStart().msecsTo(chapter->getEnd());
        }
    }
    int size=m_config->getPrefSize();
    double sec=(double)length/1000.0;
   // m_videoBitrate=(int)( (size * 8388.608)/sec  - 192);
    m_videoBitrate=8*((size*1024 - (m_config->getPrefAudioBitrate() * sec/8))/sec);
    m_videoBitrate=QMIN(m_videoBitrate,9800);
}

int k9NewDVD::getTotalTime() {
    int total=0;
    for (k9Title * title=m_titles.first();title;title=m_titles.next()) {
        k9TitleItems *chapters=title->getFiles();
        for (k9AviFile *chapter=chapters->first();chapter;chapter=chapters->next()) {
            total+=chapter->getStart().secsTo(chapter->getEnd());
        }
    }
    return total;
}

QString k9NewDVD::getError() const {
    return m_error;
}
