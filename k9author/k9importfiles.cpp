//
// C++ Implementation: k9importfiles
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9importfiles.h"
#include "k9import.h"
#include "k9newtitle.h"
#include "klocale.h"
#include "kapplication.h"
#include "k9chapteredit.h"
#include "k9avifile.h"
#include "k9menuedit.h"

k9ImportFiles::k9ImportFiles(QWidget *parent, const char *name)
 : KMdiMainFrm( parent, name ,KMdi::IDEAlMode )
{
    this->setCaption(kapp->makeStdCaption(i18n("Create new DVD"),true));
    m_k9Import=new k9Import(this);
    m_k9ImportView=createWrapper( m_k9Import,"","");
    addWindow(m_k9ImportView, KMdi::StandardAdd);
    

    k9MenuEdit *menu=new k9MenuEdit(this);
    addToolWindow(menu, KDockWidget::DockRight, getMainDockWidget(),0,i18n("Edit menu"),i18n("Edit Menu"));

    k9ChapterEdit *ch=new k9ChapterEdit(this);
    addToolWindow(ch, KDockWidget::DockBottom, getMainDockWidget(),0,i18n("Chapter properties"),i18n("Chapter properties"));

    connect(m_k9Import,SIGNAL(aviFileSelected(k9AviFile*)),ch,SLOT(setAviFile(k9AviFile*)));
    
    m_k9NewTitle=new k9NewTitle(this);
    m_k9NewTitle->setK9Import(m_k9Import);
    addToolWindow(m_k9NewTitle, KDockWidget::DockBottom, getMainDockWidget(),0,i18n("Add title"),i18n("Add title"));

    m_k9Import->init();

}


k9ImportFiles::~k9ImportFiles()
{
}


#include "k9importfiles.moc"
