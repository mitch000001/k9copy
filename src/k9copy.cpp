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

#include <kdeversion.h>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>


k9Copy::k9Copy()
    : KMainWindow( 0, "k9Copy" ),
      m_k9Main(new k9Main(this))
{
    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_k9Main);

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();

    // Apply the create the main window and ask the mainwindow to
		// automatically save settings if changed: window size, toolbar
    // position, icon size, etc.  Also to add actions for the statusbar
		// toolbar, and keybindings if necessary.
    setupGUI();

    // allow the view to change the statusbar and caption

    connect(m_k9Main, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_k9Main, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));



}

k9Copy::~k9Copy()
{
}
bool k9Copy::queryClose   (    ) {
	m_k9Main->saveSettings();
	return true;
}

void k9Copy::setupActions()
{
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());;
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());


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


   mkMP4Action = new KAction(i18n("Create MPEG-4 from title"),0,
				  this, SLOT(ActionMP4()),
				  actionCollection(),"MakeMPEG4");

   mkMP4Action->setIcon("mp4");
}

void k9Copy::fileOpen()
{
  m_k9Main->Open();

}

void k9Copy::ActionCopy()
{
 m_k9Main->Copy();
}

void k9Copy::ActionPlayTitle()
{
 m_k9Main->PreviewTitle();
}

void k9Copy::ActionMP4() {
  m_k9Main->CreateMP4();
}


void k9Copy::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->message(text);
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

