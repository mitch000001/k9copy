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
#include "k9config.h"
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
#include "kviewmpeg2.h"
#include "k9titlefactor.h"
#include <qdom.h>
#include <ksimpleconfig.h>	
#include "k9mp4title.h"
#include "k9tools.h"
	
k9Copy::k9Copy()
    : KMdiMainFrm( 0, "k9Copy" ,KMdi::IDEAlMode )
{
    k9Config config;
    m_useXine=config.getUseMplayer();
    
    m_useDvdAuthor=config.getUseDvdAuthor();

    if (config.getPrefVersion() != VERSION) {
        initCodecs();
        config.read();
        config.setPrefVersion(VERSION);
        config.save();
    }
        
    m_k9Main=new k9Main(this);
    m_childView=createWrapper( m_k9Main,"","");
    addWindow(m_childView, KMdi::StandardAdd);

    // tell the KMainWindow that this is indeed the main widget
    setToolviewStyle(KMdi::TextAndIcon);
    
    m_options=new k9PlaybackOptions(m_k9Main,this);
    addToolWindow(m_options, KDockWidget::DockRight, getMainDockWidget(),0,i18n("DVD playback options"),i18n("DVD playback options"))->show();;

    m_lang=new k9LangSelect(m_k9Main,this);
    addToolWindow(m_lang, KDockWidget::DockRight, getMainDockWidget(),0,i18n("Selection"),i18n("Selection"));
   
    if (m_useXine) {
	    m_mp2=new K9Mplayer(this);
    }
    else
	    m_mp2=new kViewMPEG2();

    m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
//    m_k9Main->setViewer( (kViewMPEG2*) m_mp2);

//    kViewMPEG2 * v=(kViewMPEG2*) m_mp2;
    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));
       
    m_factors =new k9TitleFactor( this);  
    m_factors->setUseDvdAuthor(m_useDvdAuthor);
    int h=m_factors->height();
    addToolWindow(m_factors,KDockWidget::DockBottom,getMainDockWidget(),10,i18n("Shrink Factor"),i18n("Shrink Factor"));
      
    
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_factors,SLOT(SelectionChanged( k9DVD*,bool )));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_factors,SLOT(changedTitle( k9DVDTitle* )));    
                  			    
    KDockWidget *dw=dockManager->findWidgetParentDock (m_factors);
    dw->setForcedFixedHeight(h);
    
    addMp4TitleOptions();
    
    
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
    KSimpleConfig settings("K9Copy");
    if (settings.hasGroup("dock"))
    	dockManager->readConfig((KConfig*)&settings,"dock");
}

k9Copy::~k9Copy()
{
    k9Config config;
    if (config.getPrefDelTmpFiles())
        k9Tools::clearOutput(config.getPrefOutput());
}

void k9Copy::initCodecs() {
    k9VideoCodecs v(0,0);
    v.reset();
    k9AudioCodecs a(0,0);
    a.reset();
}

bool k9Copy::queryClose   (    ) {	
	return true;
}

void k9Copy::setupActions()
{
    
    m_actions["open"]=KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    m_actions["quit"]=KStdAction::quit(this, SLOT(quit()), actionCollection());
    m_actions["preferences"]=KStdAction::preferences(this,SLOT(preferences()),actionCollection());
    KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());


    setStandardToolBarMenuEnabled(TRUE);
    createStandardStatusBarAction();

    PlayTitleAction = new KAction(i18n("Play title"), 0,
                                  this, SLOT(ActionPlayTitle()),
                                  actionCollection(), "PlayTitle");
    m_actions["playtitle"]=PlayTitleAction;
    
    QImage img;
    img.loadFromData( img_preview, sizeof( img_preview ), "PNG" );	
    PlayTitleAction->setIconSet(QIconSet(img));   

    CopyAction = new KAction(i18n("Copy"), 0,
                                  this, SLOT(ActionCopy()),
                                  actionCollection(), "Copy");
    m_actions["copy"]=CopyAction;
   CopyAction->setIcon("dvdcopy");


   mkMP4Action = new KAction(i18n("Create MPEG-4"),0,
				  this, SLOT(ActionMP4()),
				  actionCollection(),"MakeMPEG4");
   m_actions["mp4"]=mkMP4Action;

   mkMP4Action->setIcon("mp4");

   ejectAction = new KAction(i18n("Eject"),0,
				  this, SLOT(ActionEject()),
				  actionCollection() , "Eject");
   ejectAction->setIcon("player_eject");

   m_actions["eject"]=ejectAction;

   createGUI(0);
   
}

void k9Copy::setActions( bool enabled) {
   m_actions["open"]->setEnabled(enabled);
   m_actions["quit"]->setEnabled(enabled);
   m_actions["preferences"]->setEnabled(enabled);
   m_actions["playtitle"]->setEnabled(enabled);
   m_actions["copy"]->setEnabled(enabled);
   m_actions["mp4"]->setEnabled(enabled);
   m_actions["eject"]->setEnabled(enabled);
   m_mp2->setEnabled(enabled);
   m_mp4->setEnabled(enabled);
   m_factors->setEnabled(enabled);
   m_lang->setEnabled(enabled);
   m_options->setEnabled(enabled);
   


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


void k9Copy::addMp4TitleOptions() {
    m_mp4=new k9MP4Title(this);
    addToolWindow(m_mp4,KDockWidget::DockBottom,getMainDockWidget(),10,i18n("MPEG4 Encoding options"),i18n("MPEG4 Encoding Options"));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_mp4,SLOT(titleChanged( k9DVDTitle* )));
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_mp4,SLOT(selectionChanged( k9DVD*,bool )));

}

void k9Copy::preferences() {
  k9settings settings(this,i18n("Settings"));
  settings.exec();
  m_k9Main->readSettings();

  m_mp2->close();
  deleteToolWindow(m_previewAcc);
  
   k9Config config;
   m_useXine=config.getUseMplayer();

    if (m_useXine)
	m_mp2=new K9Mplayer(this);
    else
    	m_mp2=new kViewMPEG2();
    m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
    //kViewMPEG2 * v=(kViewMPEG2*) m_mp2;
    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));

    m_mp4->load();
}


void k9Copy::ActionCopy()
{   
 if (m_useXine) 
    ((K9Mplayer*)m_mp2)->bStopClick();
 else
    ((kViewMPEG2*)m_mp2)->bStopClick();
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


KMdiToolViewAccessor * k9Copy::setToolWindow(QWidget *_widget,KDockWidget::DockPosition _pos,const QString &tabToolTip,const QString &tabCaption) {
   return addToolWindow(_widget, _pos, getMainDockWidget(),0,tabToolTip,tabCaption);
}

void k9Copy::removeToolWindow(KMdiToolViewAccessor *toolWin) {
   deleteToolWindow(toolWin);
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


void k9Copy::quit() {
    close( true);	
}

void k9Copy::closeEvent( QCloseEvent* ce ) {    
    KConfig config("K9Copy");
    dockManager->writeConfig(&config,"dock");

    m_k9Main->saveSettings();
    ce->accept();
    kapp->quit();
}

KDockWidget* k9Copy::getVisibleDock() {
    KDockWidget *w= manager()->findWidgetParentDock(m_options);
    if (w->isVisible())
    	return w;
    
    w= manager()->findWidgetParentDock(m_lang);
    if (w->isVisible())
    	return w;

    w= manager()->findWidgetParentDock(m_mp2);
    if (w->isVisible())
    	return w;

    return NULL;
}



