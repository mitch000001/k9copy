//
// C++ Implementation: k9menu
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9menu.h"
#include "k9menubutton.h"
#include "k9process.h"
#include <qpainter.h>
#include <qbitmap.h>
#include <kapplication.h>
#include <qfile.h>
#include <qdom.h>

k9Menu::k9Menu(QObject *parent, const char *name)
        : QObject(parent, name),m_format(PAL) {
    m_buttons.setAutoDelete(false);
    m_canvas=new QCanvas(this);
    QPixmap pix(720,576);
    pix.fill(Qt::black);
    m_canvas->setBackgroundPixmap(pix);

}


k9Menu::~k9Menu() {}


#include "k9menu.moc"



k9MenuButton *k9Menu::addButton() {
    k9MenuButton *button=new k9MenuButton(m_canvas);
    m_buttons.append(button);
    return button;

}



void k9Menu::createMenus(QDomElement *_rootNode) {
    int height=m_format==PAL?576:480;

    QImage img;

    //create menu image
    QCanvasItemList l=m_canvas->allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() > 1001 ) 
            (*it)->hide();
    }
    QPixmap pixbg(720,height);
    QPainter pbg(&pixbg);
    m_canvas->drawArea(QRect(0,0,720,height),&pbg);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() > 1001 ) 
            (*it)->show();
    }

    //draw background
/*    img=m_Background.smoothScale(720,height,QImage::ScaleMax);
    pbg.drawImage(0,0,img);
    //draw Text
    pbg.setFont(m_textFont);
    pbg.setPen(m_textColor);
    QPoint p=m_textPos;
    p.setY(p.y()+pbg.fontMetrics().height());
    pbg.drawText(p,m_text);
*/
    //create the hilite picture
    int lineWidth=2;
    img.create(720,height,8,3,QImage::IgnoreEndian);
    QPixmap pixhi;
    pixhi.convertFromImage(img,0);
    pixhi.fill(Qt::white);
    QPainter phi(&pixhi);
    QPen penhi;
    penhi.setColor(Qt::yellow);
    penhi.setWidth(lineWidth*2);
    phi.setPen(penhi);
    phi.setBrush(Qt::NoBrush);
    //create the hilite mask
    QBitmap mask(720,height);
    mask.fill(Qt::color0);
    QPainter pmk(&mask);
    QPen penmk;
    penmk.setColor(Qt::color1);
    penmk.setWidth(lineWidth*2);
    pmk.setPen(penmk);
    pmk.setBrush(Qt::NoBrush);

    //draw buttons
    for (k9MenuButton *button=m_buttons.first();button;button=m_buttons.next()) {
  /*      QImage imgBtn;
        imgBtn= button->getImage().smoothScale(button->getWidth(),button->getHeight(),QImage::ScaleFree);
        pbg.drawImage(button->getLeft(),button->getTop(),imgBtn);
        pbg.setPen(button->getColor());
        pbg.setFont(button->getFont());
        int top,left,w;
        switch (button->getTextPosition()) {
            case k9MenuButton::BOTTOM:
                w=pbg.fontMetrics().width(button->getText());
                left=button->getLeft() +button->getWidth() /2 - w/2;
                top=button->getTop()+button->getHeight()+pbg.fontMetrics().height();
                break;
            case k9MenuButton::RIGHT:
                left=button->getLeft()+button->getWidth();
                top=button->getTop()+button->getHeight() /2 +pbg.fontMetrics().height()/2;

                break;
        }


        pbg.drawText(left,top,button->getText());
*/        
        //draw hilight
        phi.drawRect(button->getLeft()+lineWidth,button->getTop()+lineWidth,button->getWidth()-lineWidth,button->getHeight()-lineWidth);
        pmk.drawRect(button->getLeft()+lineWidth,button->getTop()+lineWidth,button->getWidth()-lineWidth,button->getHeight()-lineWidth);
    }
    pbg.end();
    phi.end();
    pmk.end();
    pixhi.setMask(mask);
    //save menu image

    QString bgFileName=m_workDir+KApplication::randomString(8)+".jpg";
    QString hiFileName=m_workDir+KApplication::randomString(8)+".png";
    QString m2vFileName= m_workDir+KApplication::randomString(8)+".m2v";
    QString mpaFileName= m_workDir+KApplication::randomString(8)+".m2v";
    QString mpgFileName= m_workDir+KApplication::randomString(8)+".mpg";

    pixbg.save(bgFileName,"JPEG",-1);
    pixhi.save(hiFileName,"PNG",-1);

    convertJpegToMpeg(bgFileName,m2vFileName);
    createAudio(mpaFileName);
    multiplex(mpaFileName,m2vFileName,mpgFileName);
    spumux(hiFileName,mpgFileName);
    appendMenu(_rootNode);

//    mask.save(m_workDir+"/mask.bmp","BMP",-1);

    QFile::remove(bgFileName);
    QFile::remove(m2vFileName);
    QFile::remove(mpaFileName);
    QFile::remove(mpgFileName);
    QFile::remove(hiFileName);
}

void k9Menu::convertJpegToMpeg(const QString &_imageJpg,const QString &_imageMpg) {
    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
    process->setUseShell(true);
    if (m_format==NTSC) {
        *process << "jpeg2yuv" << "-n" << "50" << "-I" << "p" << "-f" << "29.97" << "-j" << _imageJpg << "|";
        *process << "mpeg2enc" << "-n" << "n"  <<"-f" << "8" << "-o" << _imageMpg;
    } else {
        *process << "jpeg2yuv" << "-n" << "50" << "-I" << "p" << "-f" << "25" << "-j" << _imageJpg << "|";
        *process << "mpeg2enc" << "-n" << "p"  <<"-f" << "8" << "-o" << _imageMpg;
    }
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error converting jpeg to mpeg");
    }
    qDebug(process->debug().latin1());
}

void k9Menu::createAudio(const QString & _audioFile) {
    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
    process->setUseShell(true);
    *process << "dd" << "if=/dev/zero" << "bs=4" << "count=1920" << "|" << "toolame" << "-b" << "128"  << "-s" << "48" << "/dev/stdin" << _audioFile;
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error creating audio stream");
    }
}
void k9Menu::multiplex(const QString &_audio, const QString &_video, const QString _result) {
    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
    process->setUseShell(true);
    *process << "mplex" <<  "-f" << "8" << "-o" << _result << _video << _audio;
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error multiplexing audio and video");
    }
}

void k9Menu::spumux(const QString &_hiFileName,const QString &_mpgFileName) {
    QDomElement stream,spu;

    QDomDocument xml;

    QDomElement root = xml.createElement( "subpictures" );
    xml.appendChild( root );

    stream=xml.createElement("stream");
    root.appendChild(stream);

    spu=xml.createElement("spu");
    stream.appendChild(spu);

    spu.setAttribute("start","00:00:00.0");
    spu.setAttribute("end","00:00:00.0");
    spu.setAttribute("highlight",_hiFileName);
    spu.setAttribute("select",_hiFileName);

    //spu.setAttribute("autooutline","infer");
    spu.setAttribute("force","yes");
 //   spu.setAttribute("autoorder","rows");
    spu.setAttribute("xoffset","0");
    spu.setAttribute("yoffset","0");

    int i=1;
    for (k9MenuButton *button=m_buttons.first();button;button=m_buttons.next()) {
        QDomElement eButton = xml.createElement("button");
        spu.appendChild(eButton);
        eButton.setAttribute("name",QString("button%1").arg(i));
        eButton.setAttribute("x0", QString::number(button->getLeft()));
        eButton.setAttribute("y0",QString::number(button->getTop()));
        eButton.setAttribute("x1",QString::number(button->getLeft()+button->getWidth()));
        eButton.setAttribute("y1",QString::number(button->getTop()+button->getHeight()));
        i++;
    }

    QString spuFileName(m_workDir + KApplication::randomString(8)+ ".xml");
    QFile file( spuFileName );
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream stream( &file );
        xml.save(stream,1);
        file.close();
    }


    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
    process->setUseShell(true);
//    *process << "mplex" <<  "-f" << "8" << "-o" << _result << _video << _audio;
    *process << "spumux" << spuFileName << "<" << _mpgFileName << ">" << m_menuFileName;
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error multiplexing audio and video");
    }

    file.remove();
}

void k9Menu::appendMenu(QDomElement *_rootNode) {
    QDomDocument doc=_rootNode->ownerDocument();
    QDomNodeList l=_rootNode->elementsByTagName("menus");
    QDomNode menus;
    if (l.count()==0) {
        menus=doc.createElement("menus");
        _rootNode->appendChild(menus);
    } else {
        menus=l.item(0);
    }
    QDomElement pgc=doc.createElement("pgc");
    menus.appendChild(pgc);
    QDomElement vob=doc.createElement("vob");
    pgc.appendChild(vob);
    vob.setAttribute("file",m_menuFileName);
    vob.setAttribute("pause","inf");
    int i=1;
    for (k9MenuButton *button=m_buttons.first();button;button=m_buttons.next()) {
        QDomElement eButton = doc.createElement("button");
        pgc.appendChild(eButton); 
        QDomText eButtonTxt=doc.createTextNode(button->getScript());
        eButton.appendChild(eButtonTxt);
        eButton.setAttribute("name",QString("button%1").arg(i));
        i++;
    }
    

}

QString k9Menu::getWorkDir() const {
    return m_workDir;
}


void k9Menu::setWorkDir(const QString& _value) {
    m_workDir = _value;
}


void k9Menu::setFormat(const eFormat& _value) {
    m_format = _value;
}


QString k9Menu::getMenuFileName() const {
    return m_menuFileName;
}


void k9Menu::setMenuFileName(const QString& _value) {
    m_menuFileName = _value;
}




QCanvas* k9Menu::getCanvas() const {
    return m_canvas;
}


void k9Menu::setCanvas(QCanvas* _value) {
    m_canvas = _value;
}
