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
#include "k9importfiles.h"
#include "k9import.h"
#include "k9menuedit.h"
#include "k9chapteredit.h"
#include "k9newtitle.h"
#include "k9menu.h"
#include <kdockwidget.h>

k9Copy::k9Copy()
        : KMdiMainFrm( 0, "k9Copy" ,KMdi::IDEAlMode ) {
    // tell the KMainWindow that this is indeed the main widget
    m_k9MainView=NULL;
    m_k9ImportView =NULL;

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

    setToolviewStyle(KMdi::TextAndIcon);
    createCopyWindows();
    backupAction->setChecked(true);
    //createImportWindows();
}


void k9Copy::createCopyWindows() {
    saveDock();
    if (m_k9ImportView) {
        m_k9ImportView->close();
        m_k9Import=NULL;
        m_k9ImportView=NULL;
    }
    removeToolWindows();

    m_actions["open"]->setEnabled(true);
    m_actions["quit"]->setEnabled(true);
    m_actions["preferences"]->setEnabled(true);
    m_actions["playtitle"]->setEnabled(true);
    m_actions["copy"]->setEnabled(true);
    m_actions["create"]->setEnabled(false);
    m_actions["mp4"]->setEnabled(true);
    m_actions["eject"]->setEnabled(true);


    k9Config config;
    m_useXine=config.getUseMplayer();

    m_useDvdAuthor=config.getUseDvdAuthor();

    if (config.getPrefVersion() != VERSION) {
        initCodecs();
        config.read();
        config.setPrefVersion(VERSION);
        config.save();
    }

    m_k9Main=new k9Main(this,0,&m_drives);
    m_k9MainView=createWrapper( m_k9Main,"","");
    addWindow(m_k9MainView, KMdi::StandardAdd);

    m_options=new k9PlaybackOptions(m_k9Main,this);
    KMdiToolViewAccessor *acc;
    acc=addToolWindow(m_options, KDockWidget::DockRight, getMainDockWidget(),0,i18n("DVD playback options"),i18n("DVD playback options"));
    acc->show();

    m_ToolViews.append(acc);

    m_lang=new k9LangSelect(m_k9Main,this);
    acc=addToolWindow(m_lang, KDockWidget::DockRight, getMainDockWidget(),0,i18n("Selection"),i18n("Selection"));
    m_ToolViews.append(acc);

    if (m_useXine) {
        m_mp2=new K9Mplayer(this);
    } else
        m_mp2=new kViewMPEG2();

    m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
    m_ToolViews.append(m_previewAcc);

    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));

    m_factors =new k9TitleFactor( this);
    m_factors->setUseDvdAuthor(m_useDvdAuthor);
    int h=m_factors->height();
    acc=addToolWindow(m_factors,KDockWidget::DockBottom,getMainDockWidget(),10,i18n("Shrink Factor"),i18n("Shrink Factor"));
    m_ToolViews.append(acc);

    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_factors,SLOT(SelectionChanged( k9DVD*,bool )));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_factors,SLOT(changedTitle( k9DVDTitle* )));

    KDockWidget *dw=dockManager->findWidgetParentDock (m_factors);
    dw->setForcedFixedHeight(h);

    addMp4TitleOptions();

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

void k9Copy::removeToolWindows() {
    for (KMdiToolViewAccessor *acc=m_ToolViews.first();acc;acc=m_ToolViews.next()) {
        deleteToolWindow(acc);
    }
    m_ToolViews.clear();
}

void k9Copy::createImportWindows() {
    saveDock();
    if (m_k9MainView) {
        m_k9MainView->close();
        m_k9Main=NULL;
        m_k9MainView=NULL;
    }

    removeToolWindows();
  
    m_actions["open"]->setEnabled(false);
    m_actions["quit"]->setEnabled(true);
    m_actions["preferences"]->setEnabled(true);
    m_actions["playtitle"]->setEnabled(false);
    m_actions["copy"]->setEnabled(false);
    m_actions["create"]->setEnabled(false);
    m_actions["mp4"]->setEnabled(false);
    m_actions["eject"]->setEnabled(false);
  
    KMdiToolViewAccessor *acc;
    m_k9Import=new k9Import(this,0,&m_drives);
    m_k9ImportView=createWrapper( m_k9Import,"","");
    addWindow(m_k9ImportView, KMdi::StandardAdd);
    
    m_menuEdit=new k9MenuEdit(this,0,m_k9Import->getNewDVD()->getRootMenu()->getCanvas());
    acc=addToolWindow(m_menuEdit, KDockWidget::DockRight, getMainDockWidget(),20,i18n("Edit menu"),i18n("Edit Menu"));
    m_ToolViews.append(acc);

    connect (m_k9Import,SIGNAL(titleSelected(k9Title*)),m_menuEdit,SLOT(titleSelected(k9Title*)));
    connect (m_k9Import,SIGNAL(rootSelected(k9NewDVD*)),m_menuEdit,SLOT(rootSelected(k9NewDVD*)));
    m_k9Import->setMenuEdit(m_menuEdit);

    k9NewTitle *newTitle=new k9NewTitle(this);
    newTitle->setK9Import(m_k9Import);
    acc=addToolWindow(newTitle, KDockWidget::DockBottom, getMainDockWidget(),20,i18n("Add new title"),i18n("Add new title"));
    m_ToolViews.append(acc);
    acc->show();


    k9ChapterEdit *ch=new k9ChapterEdit(this);
    acc=addToolWindow(ch, KDockWidget::DockBottom, getMainDockWidget(),20,i18n("Chapter properties"),i18n("Chapter properties"));
    m_ToolViews.append(acc);
    connect(m_k9Import,SIGNAL(aviFileSelected(k9AviFile*)),ch,SLOT(setAviFile(k9AviFile*)));



    // allow the view to change the statusbar and caption
/*
    connect(m_k9Main, SIGNAL(changeStatusbar(const QString&,int )),
            this,   SLOT(changeStatusbar(const QString&,int)));
    connect(m_k9Main, SIGNAL(changeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));
*/
    setAutoSaveSettings();
    KSimpleConfig settings("K9Copy");
    if (settings.hasGroup("dockImport"))
        dockManager->readConfig((KConfig*)&settings,"dockImport");

    m_k9Import->init();

}


k9Copy::~k9Copy() {
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

void k9Copy::setupActions() {

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

    
    backupAction= new KRadioAction(i18n("DVD Backup"),0,
                              this, SLOT(ActionBackup()),
                              actionCollection(), "backup");
    backupAction->setExclusiveGroup("mode");
    m_actions["backup"]=backupAction;

    authorAction= new KRadioAction(i18n("DVD Author"),0,
                              this, SLOT(ActionAuthor()),
                              actionCollection(), "author");
    authorAction->setExclusiveGroup("mode");
    m_actions["author"]=authorAction;

    m_actions["create"]= new KAction(i18n("Create DVD"),0,
                              this, SLOT(ActionCreate()),
                              actionCollection(), "Create");

    createGUI(0);

}

void k9Copy::setActions( bool enabled) {
    if (m_k9MainView) {
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
    } else {
        m_actions["create"]->setEnabled(enabled);
    }


}

void k9Copy::optionsConfigureKeys() {
    KKeyDialog::configure(actionCollection());
}

void k9Copy::optionsConfigureToolbars() {
    // use the standard toolbar editor
    saveMainWindowSettings(KGlobal::config(), autoSaveGroup());
    KEditToolbar dlg(factory());
    connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(newToolbarConfig()));
    dlg.exec();
}

void k9Copy::newToolbarConfig() {
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
    createGUI(0);

    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void k9Copy::fileOpen() {
    m_k9Main->Open();

}


void k9Copy::addMp4TitleOptions() {
    m_mp4=new k9MP4Title(this);
    m_ToolViews.append(addToolWindow(m_mp4,KDockWidget::DockBottom,getMainDockWidget(),10,i18n("MPEG4 Encoding options"),i18n("MPEG4 Encoding Options")));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_mp4,SLOT(titleChanged( k9DVDTitle* )));
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_mp4,SLOT(selectionChanged( k9DVD*,bool )));

}

void k9Copy::preferences() {
    k9settings settings(this,i18n("Settings"));
    settings.exec();
    k9Config config;
    m_useXine=config.getUseMplayer();

    if (m_k9MainView) {
        m_k9Main->readSettings();
        m_mp2->close();
        deleteToolWindow(m_previewAcc);
        m_ToolViews.removeRef(m_previewAcc);

        if (m_useXine)
            m_mp2=new K9Mplayer(this);
        else
            m_mp2=new kViewMPEG2();
        m_previewAcc=addToolWindow(m_mp2,KDockWidget::DockRight,getMainDockWidget(),0,i18n("Preview"),i18n("Preview"));
        m_ToolViews.append(m_previewAcc);
        //kViewMPEG2 * v=(kViewMPEG2*) m_mp2;
        connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
        connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));
    
        m_mp4->load();
    }
}


void k9Copy::ActionCopy() {
    if (m_useXine)
        ((K9Mplayer*)m_mp2)->bStopClick();
    else
        ((kViewMPEG2*)m_mp2)->bStopClick();
    m_k9Main->Copy();
}

void k9Copy::ActionPlayTitle() {
    m_previewAcc->show();
    m_k9Main->PreviewTitle();

}

void k9Copy::ActionMP4() {
    m_k9Main->CreateMP4();
}

void k9Copy::ActionEject() {
    m_k9Main->eject();
}

void k9Copy::ActionBackup() {
   createCopyWindows();
}

void k9Copy::ActionAuthor() {
    createImportWindows();
}

void k9Copy::ActionCreate() {
    m_k9Import->execute();
}

KMdiToolViewAccessor * k9Copy::setToolWindow(QWidget *_widget,KDockWidget::DockPosition _pos,const QString &tabToolTip,const QString &tabCaption) {
    KMdiToolViewAccessor *acc=addToolWindow(_widget, _pos, getMainDockWidget(),0,tabToolTip,tabCaption);
    m_ToolViews.append(acc);
    return acc;
}

void k9Copy::removeToolWindow(KMdiToolViewAccessor *toolWin) {
    deleteToolWindow(toolWin);
    m_ToolViews.removeRef(toolWin);
}


void k9Copy::changeStatusbar(const QString& text,int id) {
    // display the text on the statusbar
    statusBar()->changeItem(text,id);
}

void k9Copy::changeCaption(const QString& text) {
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

void k9Copy::saveDock() {
    KConfig config("K9Copy");
    if (m_k9MainView) {
        dockManager->writeConfig(&config,"dock");
        m_k9Main->saveSettings();
    } else if (m_k9ImportView) {
        dockManager->writeConfig(&config,"dockImport");
    }
}

void k9Copy::closeEvent( QCloseEvent* ce ) {
    saveDock();
    ce->accept();
    kapp->quit();
}

KDockWidget* k9Copy::getVisibleDock() {
    if (m_k9MainView) {
        KDockWidget *w= manager()->findWidgetParentDock(m_options);
        if (w->isVisible())
            return w;
    
        w= manager()->findWidgetParentDock(m_lang);
        if (w->isVisible())
            return w;
    
        w= manager()->findWidgetParentDock(m_mp2);
        if (w->isVisible())
            return w;
    } else {
        KDockWidget *w= manager()->findWidgetParentDock(m_menuEdit);
        if (w->isVisible())
            return w;
    }

    return NULL;
}

void k9Copy::setEnabledCreateDVD(bool _state) {
	m_actions["create"]->setEnabled(_state);
}

