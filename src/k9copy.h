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
class k9PlaybackOptions;
class k9LangSelect;
class k9MP4Title;
class k9TitleFactor;

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
    
    KDockWidget* getVisibleDock();
    KMdiToolViewAccessor * setToolWindow(QWidget *_widget,KDockWidget::DockPosition _pos,const QString &tabToolTip,const QString &tabCaption) ;
    void removeToolWindow(KMdiToolViewAccessor *toolWin);
    
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
    void ActionEject();

   void ActionPlayTitle();
    void changeStatusbar(const QString& text,int id);
    void changeCaption(const QString& text);
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void newToolbarConfig();
public slots:
    void fileOpen();
    void preferences();
    void quit();
    void setActions(bool enabled);
private:
    void setupAccel();
    void setupActions();
    KAction *CopyAction;
    KAction *PlayTitleAction;
    KAction *mkMP4Action;
    KAction *ejectAction;
private:
   k9Main  *m_k9Main;
   KMdiChildView *m_childView;
   KMdiToolViewAccessor *m_previewAcc;
   k9PlaybackOptions *m_options;
   k9TitleFactor *m_factors;
   k9LangSelect *m_lang;
   k9MP4Title *m_mp4;
   QWidget *m_mp2;
   bool m_useXine;
   bool m_useDvdAuthor;
   QMap  <QString,KAction*> m_actions;
};



#endif 
