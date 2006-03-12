//
// C++ Interface: k9copy
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef _K9COPY_H_
#define _K9COPY_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kmainwindow.h>
class k9Main;

class k9Copy : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    k9Copy();

    void clone(QString _input,QString _output);
    void setInput(QString _input);
    void setOutput(QString _output);

    /**
     * Default Destructor
     */
    virtual ~k9Copy();

protected:
    bool queryClose( );
private slots:
    void ActionCopy();
   void ActionPlayTitle();
    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);
public slots:
    void fileOpen();

private:
    void setupAccel();
    void setupActions();
    KAction *CopyAction;
    KAction *PlayTitleAction;
private:
   k9Main  *m_k9Main;

};



#endif 
