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
#include <qtextstream.h>
#include <kstandarddirs.h>
#include <qapplication.h>
#include <ktempfile.h>
#include <kapplication.h>
#include <klocale.h>
#include <qimage.h>
#include "k9menu.h"
#include "k9menubutton.h"

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

    return (_i2->getNum() - _i1->getNum());


}

void k9NewDVD::execute() {

    m_process=m_progress->getProcess();
    connect(m_process, SIGNAL(receivedStdout(KProcess *, char *, int)),this, SLOT(getStdout(KProcess *, char *, int) ));
//            connect(m_process, SIGNAL(receivedStderr(KProcess *, char *, int)),this, SLOT(getStderr(KProcess *, char *, int) ));
//            //connect(m_process, SIGNAL(processExited(KProcess*)),this,SLOT(exited(KProcess*)));
    connect(m_progress,SIGNAL(sigCancel()),this,SLOT(slotCancel()));

    connect(&m_aviDecode,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));
    k9Tools::clearOutput(m_workDir+"dvd");
    createXML();
    disconnect(&m_aviDecode,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));
}

#include "k9newdvd.moc"


void k9NewDVD::drawImage(QImage * _image) {
  m_progress->setImage(*_image);
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
    m_rootMenu->createMenus(&vmgm);

    addTitles(root);
    QString dvdAuthor(m_workDir+"/"+KApplication::randomString(8)+".xml");
    QFile file( dvdAuthor);

    file.open(IO_WriteOnly);
    QTextStream stream( &file );
    m_xml->save(stream,1);
    file.close();

    m_process->clearArguments();
    *m_process << "dvdauthor" << "-x" << dvdAuthor;
    m_progress->execute();

    file.remove();
    QFile::remove(menuFileName);
    for ( QStringList::Iterator it = m_tmpFiles.begin(); it != m_tmpFiles.end(); ++it ) {
        QFile::remove(*it);
    }
    m_tmpFiles.clear();
    delete m_xml;
    delete m_progress;
}

void k9NewDVD::addTitles (QDomElement &_root) {
    calcVideoBitrate();
    for (k9Title *title=m_titles.first();title;title=m_titles.next()) {
        QDomElement titleSet = m_xml->createElement("titleset");
        _root.appendChild(titleSet);
        QDomElement pgc;
        k9Menu *menu=title->getMenu();
        menu->setWorkDir(m_workDir);
        QString menuFileName=m_workDir+KApplication::randomString(8)+".mpg";
        m_tmpFiles << menuFileName,
        menu->setMenuFileName(menuFileName);
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
        for (k9AviFile *aviFile= title->getFiles()->first();aviFile;aviFile=title->getFiles()->next()) {
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
    m_aviDecode.open(_aviFile->getFileName());
    m_timer.start();
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
    m_progress->setTitle(i18n("Encoding %1").arg(_aviFile->getFileName()));

    m_process->clearArguments();
    *m_process << "mencoder" << "-oac" << "lavc" << "-ovc" << "lavc" << "-of" << "mpeg";
    *m_process << "-mpegopts" << "format=dvd" << "-vf" << "scale="+scale+",harddup" << "-srate" << "48000" << "-af" << "lavcresample=48000";
    *m_process << "-lavcopts" << QString("vcodec=mpeg2video:vrc_buf_size=1835:vrc_maxrate=9800:vbitrate=%1:keyint=15:acodec=ac3:abitrate=192:aspect=16/9:threads=2").arg(m_videoBitrate);
    *m_process << "-ofps" << fps << "-o" << fileName << "-ss" << t1 << "-endpos" << t2 << _aviFile->getFileName();
    m_progress->execute();
    _cmd=fileName;
    m_aviDecode.close();
}


void k9NewDVD::getStdout(KProcess *, char *_buffer, int _length) {
    QCString c(_buffer,_length);
    if (c.mid(0,4) == "Pos:") {
        QString t=c.replace("(","").replace(")","").simplifyWhiteSpace();
        QString percent=t.section(' ',3,3);
        QString fps=t.section(' ',4,4);
        QString trem=t.section(' ',6,6);
        long pos=percent.replace("%","").toLong();
        m_progress->setProgress(pos,100);
        m_progress->setElapsed(trem);
        m_progress->setLabelText(i18n("fps")+ " : "+fps);
        if (m_timer.elapsed() > 5000) {
            m_timer.restart();
            if (m_aviDecode.opened()) {
                m_aviDecode.readFrame(m_aviDecode.getDuration()*pos /100);
            }
        }
    }
}

void k9NewDVD::appendTitle(k9Title *_title) {
    m_titles.append(_title);
    m_titles.sort();
    //create the menu button
    k9MenuButton *btn=m_rootMenu->addButton();
    _title->setButton(btn);
    QPixmap px(50,50);
    px.fill(Qt::black);
    QImage img=px.convertToImage();
    btn->setImage(img);

    btn->setTop(50+(_title->getNum())*120);
    btn->setLeft(20);
    btn->setWidth(100);
    btn->setHeight(100);
    btn->setScript(QString("jump titleset %1  menu;").arg(_title->getNum()+1));
    btn->setTextPosition(k9MenuButton::RIGHT);
    btn->setText(i18n("title %1").arg(_title->getNum()+1));
}


void k9NewDVD::setProgress(k9Progress* _value) {
    m_progress = _value;
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
    int size=4400;
    double sec=(double)length/1000.0;
    m_videoBitrate=(int)( (size * 8388.608)/sec  - 192);
    m_videoBitrate=QMIN(m_videoBitrate,5000);
}
