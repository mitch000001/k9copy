//
// C++ Implementation: k9menubutton
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9menubutton.h"

k9CanvasSpriteRedim::k9CanvasSpriteRedim (ePosition _position,int _x,int _y,int _w,int _h,QCanvas *canvas,k9MenuButton *button):QCanvasRectangle(_x,_y,_w,_h,canvas) {
    m_position=_position;
    m_button=button;
}

void k9CanvasSpriteRedim::moveBy(double _x,double _y) {
   m_offsetX=_x;
   m_offsetY=_y;
   QCanvasRectangle::moveBy(_x,_y);
}

void k9CanvasSpriteRedim::update() {
    int w,h;
    switch (m_position) {
        case RedimTopLeft:
            w=m_button->getWidth()-m_offsetX;
            h=m_button->getHeight()-m_offsetY;
            if (w >0 && h >0) {
                m_button->setWidth(w,false);
                m_button->setHeight(h,false);
                m_button->moveBy(m_offsetX,m_offsetY);
            }
            break;
        case RedimMiddleRight :
            w= m_button->getWidth()+ m_offsetX;
            if (w>0)
                m_button->setWidth(w);       
            break;
        case RedimTopMiddle:
            h=m_button->getHeight()-m_offsetY;
            if (h>0) {
                m_button->setHeight(h,false);
                m_button->moveBy(0,m_offsetY);
            }
            break;
        case RedimTopRight:
            h=m_button->getHeight()-m_offsetY   ;
            w=m_button->getWidth()+ m_offsetX;
            if (h>0 && w>0) {
                m_button->setHeight(h,false);
                m_button->setWidth( w,false);       
                m_button->moveBy(0,m_offsetY);
            }
            break;
        case RedimMiddleLeft:
            w=m_button->getWidth()-m_offsetX;
            if (w > 0) {
                m_button->setWidth(w,false);
                m_button->moveBy(m_offsetX,0);
            }
            break;
        case RedimBottomLeft:
            w=m_button->getWidth()-m_offsetX;
            h=m_button->getHeight()+m_offsetY;
            if (h>0 && w>0) {
                m_button->setWidth(w,false);
                m_button->setHeight(h,false);
                m_button->moveBy(m_offsetX,0);
            }
            break;
        case RedimBottomMiddle:
            h=m_button->getHeight()+m_offsetY;
            if (h > 0)
                m_button->setHeight(h);
            break;

        case RedimBottomRight:
            w=m_button->getWidth()+m_offsetX;
            h=m_button->getHeight()+m_offsetY;
            if (h>0 && w >0) {
                m_button->setWidth(w,false);
                m_button->setHeight(h);
            }
            break;
    }
}

k9CanvasSprite::k9CanvasSprite (QCanvasPixmapArray *a,QCanvas *canvas,k9MenuButton *button) : QCanvasSprite(a,canvas) {
    m_button=button;
}

void k9CanvasSprite::update() {
    m_button->update();
}


k9MenuButton* k9CanvasSprite::getButton() const {
    return m_button;
}

void k9MenuButton::update() {
    int x=m_sprite->x()-2;
    int y=m_sprite->y()-2;
    int w=m_width;
    int h=m_height;

    int count=m_canvas->allItems().count();

    m_lt->move(x,y);
    m_lt->setZ(count--);
    m_lt->show();

    m_mt->move(x+w/2,y);
    m_mt->setZ(count--);
    m_mt->show();

    m_rt->move(x+w,y);
    m_rt->setZ(count--);
    m_rt->show();

    m_lm->move(x,y+h/2);
    m_lm->setZ(count--);
    m_lm->show();

    m_rm->move(x+w,y+h/2);
    m_rm->setZ(count--);
    m_rm->show();

    m_lb->move(x,y+h);
    m_lb->setZ(count--);
    m_lb->show();

    m_mb->move(x+w/2,y+h);
    m_mb->setZ(count--);
    m_mb->show();

    m_rb->move(x+w,y+h);
    m_rb->setZ(count--);
    m_rb->show();

    switch(m_textPosition) {
        case RIGHT:
            m_text->move(x+2+w,y+2+h/2-m_text->boundingRect().height()/2);
            break;
        case BOTTOM:
            m_text->move(x+2+w/2 - m_text->boundingRect().width()/2,y+2+h);
            break;
    }
    m_text->show();
    m_text->setZ(count --);

    m_sprite->setZ(count--);
    emit sigsetLeft(getLeft());
    emit sigsetTop(getTop());
    emit sigsetWidth(getWidth());
    emit sigsetHeight(getHeight());
}

k9MenuButton::k9MenuButton(QCanvas *parent, const char *name)
 : QObject(parent, name),m_width(0),m_height(0)
{
    m_canvas=parent;
    m_sprite=NULL;
    m_pixmap=NULL;
    m_rm=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimMiddleRight,0,0,5,5,m_canvas,this);
    m_rm->setPen(Qt::black);
    m_rm->setBrush(Qt::red);

    m_lt=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimTopLeft,0,0,5,5,m_canvas,this);
    m_lt->setPen(Qt::black);
    m_lt->setBrush(Qt::red);

    m_mt=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimTopMiddle,0,0,5,5,m_canvas,this);
    m_mt->setPen(Qt::black);
    m_mt->setBrush(Qt::red);

    m_mt=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimTopMiddle,0,0,5,5,m_canvas,this);
    m_mt->setPen(Qt::black);
    m_mt->setBrush(Qt::red);

    m_rt=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimTopRight,0,0,5,5,m_canvas,this);
    m_rt->setPen(Qt::black);
    m_rt->setBrush(Qt::red);

    m_lm=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimMiddleLeft,0,0,5,5,m_canvas,this);
    m_lm->setPen(Qt::black);
    m_lm->setBrush(Qt::red);

    m_lb=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimBottomLeft,0,0,5,5,m_canvas,this);
    m_lb->setPen(Qt::black);
    m_lb->setBrush(Qt::red);

    m_mb=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimBottomMiddle,0,0,5,5,m_canvas,this);
    m_mb->setPen(Qt::black);
    m_mb->setBrush(Qt::red);

    m_rb=new k9CanvasSpriteRedim(k9CanvasSpriteRedim::RedimBottomRight,0,0,5,5,m_canvas,this);
    m_rb->setPen(Qt::black);
    m_rb->setBrush(Qt::red);

    m_text=new QCanvasText("",m_canvas);
    m_text->setColor(Qt::yellow);

}


k9MenuButton::~k9MenuButton()
{
}

void k9MenuButton::setImage(const QImage &_image) {
    m_image=_image;
    emit sigsetImage(this,_image);
    loadImage();

}

void k9MenuButton::setImage(const QString &_fileName){
    m_image.load(_fileName);
    emit sigsetImage(this,m_image);
    loadImage();
}

void k9MenuButton::loadImage() {
    QImage tmp=m_image.smoothScale(m_width,m_height);
    int x=0,y=0;
    if (m_sprite) {
        x=m_sprite->x();
        y=m_sprite->y();
        delete m_sprite;
    } 
    m_pixmap=new QCanvasPixmap(tmp);
    m_pixmapArray.setImage(0,m_pixmap);
    m_sprite = new k9CanvasSprite(&m_pixmapArray,m_canvas,this);
    m_sprite->move(x,y);
    m_sprite->show();
    update();
    m_canvas->update();
}

void k9MenuButton::setWidth(int _value,bool _reload) {
    m_width=_value;
    emit sigsetWidth(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getWidth() {
    return m_width;
}

void k9MenuButton::setHeight(int _value,bool _reload) {
    m_height=_value;
    emit sigsetHeight(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getHeight() {
    return m_height;
}

void k9MenuButton::setTop(int _value,bool _reload) {
    m_sprite->setY(_value);
    emit sigsetTop(_value);
    if (_reload)
        loadImage();
}

void k9MenuButton::setLeft(int _value,bool _reload) {
    m_sprite->setX(_value);
    emit sigsetLeft(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getTop() {
    return m_sprite->y();
}

int k9MenuButton::getLeft() {
    return m_sprite->x();
}

void k9MenuButton::setFont(QFont _font) {
    m_text->setFont(_font);
    update();
    m_canvas->update();
    emit sigsetFont(_font);
}

QFont k9MenuButton::getFont() {
    return m_text->font();
}

void k9MenuButton::setColor(QColor _color) {
    m_text->setColor(_color);
    m_canvas->update();
    emit sigsetColor(_color);
}

QColor k9MenuButton::getColor() {
    return m_text->color();
}

void k9MenuButton::setText(QString _text) {
    m_text->setText(_text);
    update();
    m_canvas->update();
    emit sigsetText(_text);
}

QString k9MenuButton::getText() {
    return m_text->text();
}



void k9MenuButton::moveBy(int _x,int _y,bool _reload) {
    int x=getLeft();
    int y=getTop();
    m_sprite->moveBy(_x,_y);
    emit sigsetLeft(x+_x);
    emit sigsetTop(y+_y);

    if(_reload)
        loadImage();
}
#include "k9menubutton.moc"




k9MenuButton::eTextPosition k9MenuButton::getTextPosition() const {
    return m_textPosition;
}


void k9MenuButton::setTextPosition(const eTextPosition& _value) {
    m_textPosition = _value;
    update();
    m_canvas->update();
    emit sigsetTextPosition(_value);
}


QImage k9MenuButton::getImage() const {
    return m_image;
}


QString k9MenuButton::getScript() const {
    return m_script;
}


void k9MenuButton::setScript(const QString& _value) {
    m_script = _value;
}
