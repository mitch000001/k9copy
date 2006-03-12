/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   jm_petit@laposte.net                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "k9common.h"
#include "k9copy.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "ac.h"

static const char description[] =
    I18N_NOOP("A DVD Backup tool for KDE");

static const char version[] = "1.0.4";

 static const KCmdLineOptions options[] =
  {
     { "input <device>", I18N_NOOP("input device"), 0 },
     { "output <device>", I18N_NOOP("output device"), 0 },

     KCmdLineLastOption // End of options.
  };


int main(int argc, char **argv)
{
    KAboutData about("k9copy", I18N_NOOP("k9copy"), version,description,
                     KAboutData::License_GPL, "(C) 2004-2006 Jean-Michel PETIT", 0, 0, "k9copy@free.fr");
    about.addAuthor( "Jean-Michel PETIT", 0, "k9copy@free.Fr" );
    about.setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS\\nYour names")
 ,I18N_NOOP("_: EMAIL OF TRANSLATORS\\nYour emails"));

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
	

    KApplication app;
//    kMainDlg *mainWin = 0;

     // see if we are starting with session management
    if (app.isRestored())
    {
        RESTORE(k9Copy);
    }
    else
    {
        ac_mmtest();
        tc_memcpy_init( 0, ac_mmflag());

        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
 	QString InputOptionArg( args->getOption("input"));
 	QString OutputOptionArg( args->getOption("output"));
	
        k9Copy  *widget = new k9Copy;
	if (InputOptionArg !="") {
		widget->setInput( InputOptionArg);
		widget->fileOpen();
	}
	if (OutputOptionArg !="")
		widget->setOutput( OutputOptionArg);

	if ((InputOptionArg !="") && (OutputOptionArg!="")) 
		widget->clone( InputOptionArg,OutputOptionArg);
        widget->show();
    }


    return app.exec();
}

