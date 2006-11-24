//
// C++ Implementation: k9copy
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9main.h"
#include "k9copy.h"
#include "k9settings.h"
#include "k9playbackoptions.h"
#include "k9langselect.h"

#include <kdeversion.h>
#include <kstatusbar.h> 
#include <kstdaccel.h> 
#include <kaction.h>
#include <kstdaction.h>
#include <kdockwidget.h>
#include <kmdidefines.h>
#include <kmdichildarea.h>
#include <qframe.h>
#include <kkeydialog.h>
#include <kedittoolbar.h>
#include <klibloader.h>
#include <ksimpleconfig.h>
#include "kviewmpeg2.h"
#include "k9titlefactor.h"
#include <qdom.h>
		
k9Copy::k9Copy()
    : KMdiMainFrm( 0, "k9Copy" ,KMdi::IDEAlMode )
{
    KSimpleConfig settings("K9Copy");
    m_useXine=settings.readEntry("/options/useMplayer",0).toInt();
    
    m_k9Main=new k9Main(this);
    m_childView=createWrapper( m_k9Main,"","");
    addWindow(m_childView, KMdi::StandardAdd);

    // tell the KMainWindow that this is indeed the main widget
    setToolviewStyle(KMdi::TextAndIcon);
    
    k9PlaybackOptions *opt=new k9PlaybackOptions(m_k9Main,this);
    addToolWindow(opt, KDockWidget::DockRight, getMainDockWidget(),0,i18n("DVD playback options"),i18n("DVD playback options"))->show();;

    k9LangSelect *lang=new k9LangSelect(m_k9Main,this);
    addToolWindow(lang, KDockWidget::DockRight, getMainDockWidget(),0,i18n("Selection"),i18n("Selection"));
   
    if (m_useXine) {
	    m_mp2=new K9Mplayer(this);
    }
    else
	    m_mp2=new kViewMPEG2();

    m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
//    m_k9Main->setViewer( (kViewMPEG2*) m_mp2);

//    kViewMPEG2 * v=(kViewMPEG2*) m_mp2;
    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle* )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle* )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));
       
    k9TitleFactor *Factors=new k9TitleFactor( this);  
    
    addToolWindow(Factors,KDockWidget::DockBottom,getMainDockWidget(),10,i18n("Shrink Factor"),i18n("Shrink Factor"));
      
    
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD* )),Factors,SLOT(SelectionChanged( k9DVD* )));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),Factors,SLOT(changedTitle( k9DVDTitle* )));    
                  			    
    KDockWidget *dw=dockManager->findWidgetParentDock (Factors);
    dw->setForcedFixedHeight( 100);
    // accept dnd
    setAcceptDrops(true);
   
    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();
    statusBar()->insertFixedItem("",sbFactor,TRUE);
    statusBar()->setItemFixed(sbFactor,100);
    statusBar()->insertItem("",sbMessage,1,FALSE);
    statusBar()->setItemAlignment(sbMessage,AlignLeft | AlignVCenter);

    // Apply the create the main window and ask the mainwindow to
		// automatically save settings if changed: window size, toolbar
    // position, icon size, etc.  Also to add actions for the statusbar
		// toolbar, and keybindings if necessary.
    resize(QSize(800,600));
 
    // allow the view to change the statusbar and caption

    connect(m_k9Main, SIGNAL(changeStatusbar(const QString&,int )),
            this,   SLOT(changeStatusbar(const QString&,int)));
    connect(m_k9Main, SIGNAL(changeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

    setAutoSaveSettings();
    KConfig config("K9Copy");
    dockManager->readConfig(&config,"dock");

}

k9Copy::~k9Copy()
{
}
bool k9Copy::queryClose   (    ) {	
	return true;
}

void k9Copy::setupActions()
{
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());
    KStdAction::preferences(this,SLOT(preferences()),actionCollection());
    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());


    setStandardToolBarMenuEnabled(TRUE);
    createStandardStatusBarAction();

    PlayTitleAction = new KAction(i18n("Play title"), 0,
                                  this, SLOT(ActionPlayTitle()),
                                  actionCollection(), "PlayTitle");
    QImage img;
    img.loadFromData( img_preview, sizeof( img_preview ), "PNG" );	
    PlayTitleAction->setIconSet(QIconSet(img));   

    CopyAction = new KAction(i18n("Copy"), 0,
                                  this, SLOT(ActionCopy()),
                                  actionCollection(), "Copy");
    
   CopyAction->setIcon("dvdcopy");


   mkMP4Action = new KAction(i18n("Create MPEG-4"),0,
				  this, SLOT(ActionMP4()),
				  actionCollection(),"MakeMPEG4");

   mkMP4Action->setIcon("mp4");

   ejectAction = new KAction(i18n("Eject"),0,
				  this, SLOT(ActionEject()),
				  actionCollection() , "Eject");
   ejectAction->setIcon("player_eject");


   createGUI(0);
   
}

void k9Copy::optionsConfigureKeys()
{
    KKeyDialog::configure(actionCollection());
}

void k9Copy::optionsConfigureToolbars()
{
    // use the standard toolbar editor
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
    KEditToolbar dlg(factory());
    connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(newToolbarConfig()));
    dlg.exec();
}

void k9Copy::newToolbarConfig()
{
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI(0);

    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void k9Copy::fileOpen()
{
  m_k9Main->Open();

}

void k9Copy::preferences() {
  k9settings settings(this,i18n("Settings"));
  settings.exec();
  m_k9Main->readSettings();

  m_mp2->close();
  deleteToolWindow(m_previewAcc);
  
   KSimpleConfig ksettings("K9Copy");
   m_useXine=ksettings.readEntry("/options/useMplayer",0).toInt();

    if (m_useXine)
	m_mp2=new K9Mplayer(this);
    else
    	m_mp2=new kViewMPEG2();
    m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
    //kViewMPEG2 * v=(kViewMPEG2*) m_mp2;
    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle* )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle* )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));

}


void k9Copy::ActionCopy()
{
( (kViewMPEG2*)m_mp2)->bStopClick();
 m_k9Main->Copy();
}

void k9Copy::ActionPlayTitle()
{
 m_previewAcc->show();
 m_k9Main->PreviewTitle();

}

void k9Copy::ActionMP4() {
  m_k9Main->CreateMP4();
}

void k9Copy::ActionEject() {
  m_k9Main->eject();
}




void k9Copy::changeStatusbar(const QString& text,int id)
{
    // display the text on the statusbar
    statusBar()->changeItem(text,id);
}

void k9Copy::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text);
}

void k9Copy::clone( QString _input,QString _output) {
   show();
   m_k9Main->Clone( _input,_output);
}

void k9Copy::setInput(QString _input) {
  m_k9Main->setInput(_input);
}

void k9Copy::setOutput(QString _output) {
  m_k9Main->setOutput(_output);
}

void k9Copy::closeEvent( QCloseEvent* ce ) {    
    KConfig config("K9Copy");
    dockManager->writeConfig(&config,"dock");
    m_k9Main->saveSettings();
    ce->accept();
    kapp->quit();
}


