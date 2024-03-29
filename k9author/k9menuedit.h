//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MENUEDIT_H
#define K9MENUEDIT_H

#include "../k9author/menuEdit.h"
#include <qcanvas.h>
#include <qimage.h>

class k9MenuButton;
class k9MenuEdit;
class k9Title;
class k9NewDVD;
class k9MenuEditor;


class k9MenuEdit : public menuEdit {
    Q_OBJECT
public:
    enum eFormat {PAL=1,NTSC=2};
    enum eMenuType {ROOTMENU,TITLEMENU};

    k9MenuEdit(QWidget* parent = 0, const char* name = 0,QCanvas *_canvas=0);
    ~k9MenuEdit();
    /*$PUBLIC_FUNCTIONS$*/
    void setBackgroundImage(const QImage &_image);
    void setText(const QString &_value);
    void setFont(const QFont &_value);
    void setColor(const QColor &_value);
    void updateTextPos(const QPoint &_point);
public slots:
    /*$PUBLIC_SLOTS$*/
    void itemSelected();
    QCanvasText* getText() const;
    void setFormat(const eFormat& _value);
    k9MenuEdit::eFormat getFormat() const;
    QCanvas *getCanvas();
    void setCanvas(QCanvas* _value);
    void titleSelected(k9Title *);
    void rootSelected(k9NewDVD *);

protected:
    /*$PROTECTED_FUNCTIONS$*/
    virtual void bFontClick();
    virtual void urBackgroundSelected(const QString &_fileName);
    virtual void cbColorChanged(const QColor &_color);
    virtual void leTitleChanged(const QString &_value);
    virtual void cbPosTitleActivated(int _value);
    virtual void bAddTextClick();
    virtual void cbStartActivated (int _value); 
    virtual void cbEndActivated (int _value);
    k9MenuEditor *m_menuEditor;
    QCanvas *m_canvas;
    QImage m_background;
    QCanvasText *m_text;
    eFormat m_format;
    int m_imageHeight;
    eMenuType m_menuType;
    QStringList m_startScripts;
    QStringList m_endScripts;
    bool m_noUpdate;
protected slots:
    /*$PROTECTED_SLOTS$*/
signals:
    void backgroundImageChanged(const QImage &);
    void textChanged(const QString&);
    void startScriptChanged(const QString&);
    void endScriptChanged(const QString&);
    void textColorChanged(const QColor &);
    void textFontChanged(const QFont&);
    void updatePos(const QPoint &);
};

#endif

