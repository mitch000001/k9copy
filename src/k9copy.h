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

#include "k9common.h"

#include <kapplication.h>
#include <kmdimainfrm.h>


enum eStatusBarId {sbMessage=0,sbFactor=1};


class k9Main;
class kViewMPEG2;

class k9Copy : public KMdiMainFrm
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
    void closeEvent( QCloseEvent* ce );

private slots:
    void ActionCopy();
    void ActionMP4();

   void ActionPlayTitle();
    void changeStatusbar(const QString& text,int id);
    void changeCaption(const QString& text);
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void newToolbarConfig();
public slots:
    void fileOpen();
    void preferences();
private:
    void setupAccel();
    void setupActions();
    KAction *CopyAction;
    KAction *PlayTitleAction;
    KAction *mkMP4Action;

private:
   k9Main  *m_k9Main;
   KMdiChildView *m_childView;
   KMdiToolViewAccessor *m_previewAcc;
   kViewMPEG2 *m_mp2;
};



#endif 
