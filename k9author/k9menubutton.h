//
// C++ Interface: k9menubutton
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MENUBUTTON_H
#define K9MENUBUTTON_H

#include <qobject.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qcanvas.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9MenuButton;
class k9CanvasSprite : public QCanvasSprite {
public:
    k9CanvasSprite (QCanvasPixmapArray *a,QCanvas *canvas,k9MenuButton *button);
    virtual int rtti() const {
        return 1000;
    }
    void update();

    k9MenuButton* getButton() const;

private:
    k9MenuButton *m_button;
};

class k9CanvasSpriteRedim : public QCanvasRectangle {
public:
    enum ePosition  { RedimTopLeft=1002,RedimTopMiddle=1003,RedimTopRight=1004,RedimMiddleLeft=1005,RedimBottomLeft=1006,RedimBottomMiddle=1007,RedimBottomRight=1008,RedimMiddleRight=1009} ;
    k9CanvasSpriteRedim (ePosition _position,int _x,int _y,int _w,int _h,QCanvas *canvas,k9MenuButton *button);
    void update();
    virtual int rtti() const {
        return m_position;
    }
    virtual void moveBy(double _x,double _y);
private:
    k9MenuButton *m_button;
    ePosition m_position;
    double m_offsetX,m_offsetY;
};



class k9MenuButton : public QObject {
    Q_OBJECT
public:
    enum eTextPosition {BOTTOM=1,RIGHT=2};

    k9MenuButton(QCanvas *parent = 0, const char *name = 0);

    ~k9MenuButton();
    void setWidth(int _value,bool _reload=true);
    int getWidth();
    void setHeight(int _value,bool _reload=true);
    int getHeight();
    void setTop(int _value,bool _reload=true);
    void setLeft(int _value,bool _reload=true);
    void setFont(QFont _font);
    QFont getFont();
    void setColor(QColor _color);
    QColor getColor();
    void setText(QString _text);
    QString getText();
    void select(bool);
    int getTop();
    int getLeft();

    void moveBy(int x,int y,bool _reload=true);
    void update();
public slots:
    void setImage(const QImage &_image);
    void setImage(const QString &_fileName);

    void setTextPosition(const eTextPosition& _value);
    eTextPosition getTextPosition() const;

    QImage getImage() const;

    void setScript(const QString& _value);
    QString getScript() const;

    void setAngle(int _value);
    int getAngle() const;

	k9CanvasSprite* getSprite() const;
	

private:
    QCanvas *m_canvas;
    QCanvasPixmapArray m_pixmapArray;
    QCanvasPixmap *m_pixmap;
    k9CanvasSprite *m_sprite;

    k9CanvasSpriteRedim *m_lt,*m_mt,*m_rt,*m_lm,*m_rm,*m_lb,*m_mb,*m_rb;
    QCanvasText *m_text;
    QImage m_image;
    int  m_width;
    int m_height;
    int m_angle;
    eTextPosition m_textPosition;
    QString m_script;
    void loadImage();
signals:
    void sigsetTop(int);
    void sigsetLeft(int);
    void sigsetWidth(int);
    void sigsetHeight(int);

    void sigsetFont(const QFont &);
    void sigsetColor(const QColor &);
    void sigsetImage(k9MenuButton *,const QImage &);
    void sigsetText(const QString &);
    void sigsetTextPosition(int);
};

#endif
