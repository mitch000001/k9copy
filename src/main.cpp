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
#include "k9play.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include "ac.h"
#include "k9copy.h"
#include "dvdread.h"

static const char description[] =
    I18N_NOOP("A DVD Backup tool for KDE");

static const char version[] = "1.1.0";

static const KCmdLineOptions options[] = {
            { "input <device>", I18N_NOOP("input device"), 0
            },
            { "output <device>", I18N_NOOP("output device"), 0 },
            { "dvdtitle <number>", I18N_NOOP("title to play"), 0 },
            { "play", I18N_NOOP("play title to stdout"), 0 },
            { "startsector <number>", I18N_NOOP("start sector"),0},
            { "endsector <number>", I18N_NOOP("end sector"),0},
            { "audiofilter <number,number>", I18N_NOOP("list of audio streams"),0},
            { "subpicturefilter <number,number>", I18N_NOOP("list of spu streams"),0},
            { "vampsfactor <number>", I18N_NOOP("shrink factor"),0},
	    { "ffactor", I18N_NOOP("shrink factor forced"),0},
            { "inputsize <number>", I18N_NOOP("size of the cell to be copied"),0},
	    { "totalsize <number>", I18N_NOOP("total size of selected titles"),0},
	    { "chaptersize <number>", I18N_NOOP("size of the chapters being copied"),0},
	    { "dvdsize <number>", I18N_NOOP("new dvd size"),0},
            { "chapter <number>", I18N_NOOP("selected chapter"),0},
	    { "cell <number>", I18N_NOOP("cell number in selected chapter"),0},
	    { "inject <filename>", I18N_NOOP("status file name"),0},
	    { "initstatus" ,I18N_NOOP("initialize status file"),0},
	    { "continue" ,I18N_NOOP("continue playing from last sector"),0},



            KCmdLineLastOption // End of options.
        };


int main(int argc, char **argv) {
    KAboutData about("k9copy", I18N_NOOP("k9copy"), version,description,
                     KAboutData::License_GPL, "(C) 2004-2006 Jean-Michel PETIT", 0, 0, "k9copy@free.fr");
    about.addAuthor( "Jean-Michel PETIT", 0, "k9copy@free.Fr" );
    about.setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS\\nYour names")
                        ,I18N_NOOP("_: EMAIL OF TRANSLATORS\\nYour emails"));

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;
    //    kMainDlg *mainWin = 0;

    loadDvdread( );
    // see if we are starting with session management
    if (app.isRestored()) {
        RESTORE(k9Copy);
    } else {
        ac_mmtest();
        tc_memcpy_init( 0, ac_mmflag());

        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        QString TitleNumber(args->getOption("dvdtitle"));
        QString InputOptionArg( args->getOption("input"));
        QString OutputOptionArg( args->getOption("output"));
        QString startSectorArg(args->getOption("startsector"));
        QString endSectorArg(args->getOption("endsector"));
        QString audioFilterArg(args->getOption("audiofilter"));
        QString subpictureFilterArg(args->getOption("subpicturefilter"));
        QString vampsFactorArg(args->getOption("vampsfactor"));
        QString inputSizeArg(args->getOption("inputsize"));
        QString chapterArg(args->getOption("chapter"));
	QString cellArg(args->getOption("cell"));
	QString injectArg(args->getOption("inject"));
	QString totalSizeArg(args->getOption("totalsize"));
	QString dvdSizeArg(args->getOption("dvdsize"));
	QString chapterSizeArg(args->getOption("chaptersize"));
	bool ffactor=args->isSet("ffactor");
        bool play= args->isSet("play");
/*	
        InputOptionArg="/dev/hdb";
        play=true;
	TitleNumber="4";
	audioFilterArg="1";
        chapterArg="2";
	cellArg="1";
        injectArg="/tmp/kde-jmp/inject";
	vampsFactorArg ="1.00";
	inputSizeArg="8192";
	chapterSizeArg="8192";
	injectArg="/tmp/kde-jmp/k9copy/k9v102718";
	totalSizeArg="300320768";
	dvdSizeArg="4613734400";
*/	
        if (play) {
            k9play player;
	    player.setinitStatus( args->isSet("initstatus"));
	    //tmp
//	    player.setinitStatus( true);

	    player.setcontinue( args->isSet("continue"));
            player.setDevice(InputOptionArg);
            player.setTitle(TitleNumber.toInt());
            player.setstartSector(startSectorArg);
            player.setendSector(endSectorArg );
            player.setaudioFilter( audioFilterArg);
            player.setsubpictureFilter( subpictureFilterArg);
            player.setvampsFactor( vampsFactorArg);
            player.setinputSize( inputSizeArg);
            player.setchapter(chapterArg);
	    player.setcell(cellArg);
	    player.setinject(injectArg);
	    player.settotalSize(totalSizeArg);
	    player.setdvdSize(dvdSizeArg);
            player.setchapterSize( chapterSizeArg);
	    player.setforcedFactor(ffactor);
            player.execute();
            return 0;
        } else {

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
    }


    int ret= app.exec();
    unloadDvdread();
    return ret;
}

