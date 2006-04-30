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

#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9cellcopylist.h"
#include "k9dvdprogress.h"
#include "k9ifo.h"
#include <qapplication.h>
#include <klocale.h>

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

k9DVDTitle* k9DVD::gettitle(int num) {
    return ((k9DVDTitle*)titles.at(num));
}

k9DVDTitle* k9DVD::gettitleByNum(int num) {
    int j=-1;
    for (int i=0; i < titles.count();i++) {
        k9DVDTitle *track=(k9DVDTitle*)titles.at(i);
        if (track->getIndexed()) {
            j++;
            if (j==num)
                return track;
        }
    }
    return (NULL);
}


/** Read property of int longestTitle. */
k9DVDTitle *k9DVD::getlongestTitle() {
    return longestTitle;
}

/** Read property of QString title. */
const QString& k9DVD::getDVDTitle() {
    return title;
}

void k9DVD::setDVDTitle(const QString &_newVal) {
    title=_newVal;
}

/** Read property of int titleCount. */
const int& k9DVD::gettitleCount() {
    return titleCount;
}

/** Read property of int titlesetCount. */
const int& k9DVD::gettitlesetCount() {
    return titlesetCount;
}

k9DVDTitleset *k9DVD::gettitleset(int num) {
    return titlesets.at(num);
}

int k9DVD::getmenuSize() {
    return menuSize;
}

k9DVD::k9DVD(QObject  *parent, const char *name,const QStringList args)  {
    error=false;
    opened=false;
    titles.setAutoDelete(true);
    titlesets.setAutoDelete(true);
    lvideoFormat.append("NTSC");
    lvideoFormat.append("PAL");

    laspectRatio.append("4:3");
    laspectRatio.append("16:9");
    laspectRatio.append("?:?");
    laspectRatio.append("16:9");

    lquantization.append("16bps");
    lquantization.append("20bps");
    lquantization.append("24bps");
    lquantization.append("drc");

    lmpegVersion.append("mpeg1");
    lmpegVersion.append("mpeg2");

    lvideoHeight.append("480");
    lvideoHeight.append("576");
    lvideoHeight.append("???");
    lvideoHeight.append("576");

    lvideoWidth.append("720");
    lvideoWidth.append("704");
    lvideoWidth.append("352");
    lvideoWidth.append("352");

    lpermittedDf.append("Pan&Scan");
    lpermittedDf.append("Letterbox");
    lpermittedDf.append("Pan&Scan");
    lpermittedDf.append("Letterbox");
    lpermittedDf.append("?");

    laudioFormat.append("ac3");
    laudioFormat.append("?");
    laudioFormat.append("mp2");
    laudioFormat.append("mp2");
    laudioFormat.append("pcm");
    laudioFormat.append("sdds");
    laudioFormat.append("dts");

    lsampleFreq.append("48kHz");
    lsampleFreq.append("48kHz");

    laudioType.append("");
    laudioType.append("Normal");
    laudioType.append("Impaired");
    laudioType.append("Comments1");
    laudioType.append("Comments2");

    lsubpType.append("");
    lsubpType.append("Normal");
    lsubpType.append("Large");
    lsubpType.append("Children");
    lsubpType.append("reserved");
    lsubpType.append("Normal_CC");
    lsubpType.append("Large_CC");
    lsubpType.append("Children_CC");
    lsubpType.append("reserved");
    lsubpType.append("Forced");
    lsubpType.append("reserved");
    lsubpType.append("reserved");
    lsubpType.append("reserved");
    lsubpType.append("Director");
    lsubpType.append("Large_Director");
    lsubpType.append("Children_Director");


    frames_per_s[0]=-1.0;
    frames_per_s[1]=25.00;
    frames_per_s[2]=-1.0;
    frames_per_s[3]=29.97;
    connect(this, SIGNAL(sigVobProgress(unsigned int,unsigned int)), this, SLOT(slotVobProgress(unsigned int,unsigned int)));
    connect(this, SIGNAL(sigTitleProgress(unsigned int,unsigned int)), this, SLOT(slotTitleProgress(unsigned int,unsigned int)));
    connect(this, SIGNAL(sigTitleText(QString&)), this, SLOT(slotTitleText(QString&)));
    connect(this, SIGNAL(sigTotalText(QString&)), this, SLOT(slotTotalText(QString&)));
    start=NULL;
}
k9DVD::~k9DVD() {}

int k9DVD::dvdtime2msec(dvd_time_t *dt) {
    double fps = frames_per_s[(dt->frame_u & 0xc0) >> 6];
    long   ms;
    ms  = (((dt->hour &   0xf0) >> 3) * 5 + (dt->hour   & 0x0f)) * 3600000;
    ms += (((dt->minute & 0xf0) >> 3) * 5 + (dt->minute & 0x0f)) * 60000;
    ms += (((dt->second & 0xf0) >> 3) * 5 + (dt->second & 0x0f)) * 1000;

    if(fps > 0)
        ms += ((dt->frame_u & 0x30) >> 3) * 5 + (dt->frame_u & 0x0f) * 1000.0 / fps;

    return ms;
}

/*
*  The following method is based on code from vobcopy, by Robos, with thanks.
*/

int k9DVD::get_title_name(const char* dvd_device, char* title) {
    FILE *filehandle = 0;
    int  i;
    QString c;
    if (! (filehandle = fopen(dvd_device, "r"))) {
        c=i18n("Couldn't open %1 for title\n").arg(dvd_device);
        // setError(c );
        strcpy(title, "unknown");
        return -1;
    }

    if ( fseek(filehandle, 32808, SEEK_SET )) {
        fclose(filehandle);
        c=i18n("Couldn't seek in %1 for title\n").arg( dvd_device);
        setError(c);
        strcpy(title, "unknown");
        return -1;
    }

    if ( 32 != (i = fread(title, 1, 32, filehandle)) ) {
        fclose(filehandle);
        //        c=tr2i18n("Couldn't read enough bytes for title.\n");
        //        setError(c);
        strcpy(title, "unknown");
        return 0;
    }

    fclose (filehandle);

    title[32] = '\0';
    while(i-- > 2)
        if(title[i] == ' ')
            title[i] = '\0';
    return 0;
}

QString k9DVD::lang_name(const QString& code) {
    QString c;
    int i=0;
    lng arrLng[] = {
                       { "  ", i18n("Not Specified") }, { "aa", i18n("Afar") },	{ "ab", i18n("Abkhazian") }, { "af", i18n("Afrikaans") },	{ "am", i18n("Amharic")},
                       { "ar", i18n("Arabic") }, { "as", i18n("Assamese") },	{ "ay", i18n("Aymara") }, { "az", i18n("Azerbaijani") }, { "ba", i18n("Bashkir") },
                       { "be", i18n("Byelorussian") }, { "bg", i18n("Bulgarian") }, { "bh", i18n("Bihari") }, { "bi", i18n("Bislama") }, { "bn", i18n("Bengali; Bangla") },
                       { "bo", i18n("Tibetan") }, { "br", i18n("Breton") }, { "ca", i18n("Catalan") }, { "co", i18n("Corsican") }, { "cs", i18n("Czech") },
                       { "cy", i18n("Welsh") }, { "da", i18n("Dansk") }, { "de", i18n("Deutsch") }, { "dz", i18n("Bhutani") }, { "el", i18n("Greek") }, { "en", i18n("English") },
                       { "eo", i18n("Esperanto") }, { "es", i18n("Espanol") }, { "et", i18n("Estonian") }, { "eu", i18n("Basque") }, { "fa", i18n("Persian") },
                       { "fi", i18n("Suomi") }, { "fj", i18n("Fiji") }, { "fo", i18n("Faroese") }, { "fr", i18n("Francais") }, { "fy", i18n("Frisian") }, { "ga", i18n("Gaelic") },
                       { "gd", i18n("Scots Gaelic") }, { "gl", i18n("Galician") }, { "gn", i18n("Guarani") }, { "gu", i18n("Gujarati") }, { "ha", i18n("Hausa") },
                       { "he", i18n("Hebrew") }, { "hi", i18n("Hindi") }, { "hr", i18n("Hrvatski") }, { "hu", i18n("Magyar") }, { "hy", i18n("Armenian") },
                       { "ia", i18n("Interlingua") }, { "id", i18n("Indonesian") }, { "ie", i18n("Interlingue") }, { "ik", i18n("Inupiak") }, { "in", i18n("Indonesian") },
                       { "is", i18n("Islenska") }, { "it", i18n("Italiano") }, { "iu", i18n("Inuktitut") }, { "iw", i18n("Hebrew") }, { "ja", i18n("Japanese") },
                       { "ji", i18n("Yiddish") }, { "jw", i18n("Javanese") }, { "ka", i18n("Georgian") }, { "kk", i18n("Kazakh") }, { "kl", i18n("Greenlandic") },
                       { "km", i18n("Cambodian") }, { "kn", i18n("Kannada") }, { "ko", i18n("Korean") }, { "ks", i18n("Kashmiri") }, { "ku", i18n("Kurdish") },
                       { "ky", i18n("Kirghiz") }, { "la", i18n("Latin") }, { "ln", i18n("Lingala") }, { "lo", i18n("Laothian") }, { "lt", i18n("Lithuanian") },
                       { "lv", i18n("Latvian, Lettish") }, { "mg", i18n("Malagasy") }, { "mi", i18n("Maori") }, { "mk", i18n("Macedonian") }, { "ml", i18n("Malayalam") },
                       { "mn", i18n("Mongolian") }, { "mo", i18n("Moldavian") }, { "mr", i18n("Marathi") }, { "ms", i18n("Malay") }, { "mt", i18n("Maltese") },
                       { "my", i18n("Burmese") }, { "na", i18n("Nauru") }, { "ne", i18n("Nepali") }, { "nl", i18n("Nederlands") }, { "no", i18n("Norsk") }, { "oc", i18n("Occitan") },
                       { "om", i18n("Oromo") }, { "or", i18n("Oriya") }, { "pa", i18n("Punjabi") }, { "pl", i18n("Polish") }, { "ps", i18n("Pashto, Pushto") },
                       { "pt", i18n("Portugues") }, { "qu", i18n("Quechua") }, { "rm", i18n("Rhaeto-Romance") }, { "rn", i18n("Kirundi") }, { "ro", i18n("Romanian")  },
                       { "ru", i18n("Russian") }, { "rw", i18n("Kinyarwanda") }, { "sa", i18n("Sanskrit") }, { "sd", i18n("Sindhi") }, { "sg", i18n("Sangho") },
                       { "sh", i18n("Serbo-Croatian") }, { "si", i18n("Sinhalese") }, { "sk", i18n("Slovak") }, { "sl", i18n("Slovenian") }, { "sm",i18n( "Samoan") },
                       { "sn", i18n("Shona")  }, { "so", i18n("Somali") }, { "sq", i18n("Albanian") }, { "sr", i18n("Serbian") }, { "ss", i18n("Siswati") },
                       { "st", i18n("Sesotho") }, { "su", i18n("Sundanese") }, { "sv", i18n("Svenska") }, { "sw", i18n("Swahili") }, { "ta", i18n("Tamil") },
                       { "te", i18n("Telugu") }, { "tg", i18n("Tajik") }, { "th", i18n("Thai") }, { "ti", i18n("Tigrinya") }, { "tk", i18n("Turkmen") }, { "tl", i18n("Tagalog") },
                       { "tn", i18n("Setswana") }, { "to", i18n("Tonga") }, { "tr", i18n("Turkish") }, { "ts", i18n("Tsonga") }, { "tt", i18n("Tatar") }, { "tw", i18n("Twi") },
                       { "ug", i18n("Uighur") }, { "uk", i18n("Ukrainian") }, { "ur", i18n("Urdu") }, { "uz", i18n("Uzbek") }, { "vi", i18n("Vietnamese") },
                       { "vo", i18n("Volapuk") }, { "wo", i18n("Wolof") }, { "xh", i18n("Xhosa") }, { "yi", i18n("Yiddish") }, { "yo", i18n("Yoruba") }, { "za", i18n("Zhuang") },
                       { "zh", i18n("Chinese") }, { "zu", i18n("Zulu") }, { "xx", i18n("Unknown") }, { "\0", i18n("Unknown") } };

    c=i18n("Unknown");
    for (i=0 ; arrLng[i].code[0]!=0;i++) {
        lng l =arrLng[i];
        if (strcmp(code.latin1(),l.code)==0) {
            c = l.name;
        }
    }

    return c;
}


int k9DVD::calcNumTitle(ifo_handle_t *ifo,int _vts,int _ttn) {
    for (int i=0; i< ifo->tt_srpt->nr_of_srpts;i++) {
        if (ifo->tt_srpt->title[i].title_set_nr==_vts && ifo->tt_srpt->title[i].vts_ttn == _ttn)
            return (i+1);
    }
   return 0;

}

bool k9DVD::isTitleIndex(ifo_handle_t *ifo,int _vts,int _ttn) {
    for (int i=0; i< ifo->tt_srpt->nr_of_srpts;i++) {
        if (ifo->tt_srpt->title[i].title_set_nr==_vts && ifo->tt_srpt->title[i].vts_ttn == _ttn)
            return true;
    }
    return false;

}

int k9DVD::scandvd (const QString & device,bool _quickScan) {
    char ctitle[255];
    k9DVDChapter::setcptChapter(0);
    ifo_handle_t *ifo_zero, *ifo;
    pgcit_t *vts_pgcit;
    vtsi_mat_t *vtsi_mat;
    vmgi_mat_t *vmgi_mat;
    audio_attr_t *audio_attr;
    video_attr_t *video_attr;
    subp_attr_t *subp_attr;
    pgc_t *pgc;
    int i, j,  ltitles, cell, vts_ttn, title_set_nr;
    char lang_code[2];
    int has_title = 0, ret = 0;
    int max_length = 0;
    struct stat dvd_stat;
    bool ok;
    tt_srpt_t   *tt_srpt;
    QString txt,c;

    int menuSizes[100];
    for ( j=0; j<100;j++)
        menuSizes[j]=0;

    titles.clear();
    Device=device;

    error=false;
    errMsg="";

    progressDlg= new k9DVDProgress(qApp->mainWidget(),"progress",true);
    progressDlg->setpbTitleStep(0);
    progressDlg->setpbTotalStep(0);

    if (!_quickScan)
        progressDlg->show();
    qApp->processEvents();

    k9DVDTitle *l_track;
    k9DVDAudioStream *l_auds;
    k9DVDChapter *l_chap;
    k9DVDSubtitle *l_sub;
    QString sh,sm,ss;
    /*
        ret = stat(device.latin1(), &dvd_stat);
        if ( ret < 0 ) {
            c=i18n("Can't find device %1\n").arg( device);
            setError(c);
            return 1;
        }
    */
    m_dvd.openDevice(device);
    if( !m_dvd.opened() ) {
        c=i18n("Can't open disc %1!\n").arg(device);
        setError(c);
        return 2;
    }

    k9Ifo kifo_zero(&m_dvd),kifo(&m_dvd);
    kifo_zero.openIFO( 0);

    ifo_zero = kifo_zero.getIFO();
    if ( !ifo_zero ) {
        c=tr2i18n("Can't open main ifo!\n");
        setError(c);
        return 3;
    }

    ltitles = ifo_zero->tt_srpt->nr_of_srpts;
    titleCount = 0;
    int indexedCount=0;
    has_title = get_title_name(device.latin1(), ctitle);

    vmgi_mat = ifo_zero->vmgi_mat;
    titlesetCount = vmgi_mat->vmg_nr_of_title_sets;

    menuSizes[0]=vmgi_mat->vmg_last_sector;

    title=( has_title ? tr2i18n("unknown") : ctitle);

    for (int ts=1;ts <=titlesetCount;ts++) {
        tt_srpt = ifo_zero->tt_srpt;
        kifo.openIFO(ts);
        ifo = kifo.getIFO();
        if (ifo==NULL)
            continue;
        //add the titleset in the titleset list
        k9DVDTitleset *titleset = new k9DVDTitleset(ts,ifo->vtsi_mat->vts_last_sector -ifo->vtsi_mat->vtstt_vobs-1);
        titlesets.append(titleset);

        for (j=0; j < ifo->vts_pgcit->nr_of_pgci_srp; j++) {
            // tt_srpt->title[j].title_set_nr);
            // GENERAL
            vtsi_mat   = ifo->vtsi_mat;
            vts_pgcit  = ifo->vts_pgcit;
            pgc = vts_pgcit->pgci_srp[j].pgc;
            if (vtsi_mat && (pgc->nr_of_cells >0)) {
                titleCount++;
                vts_ttn =  j+1;//ifo->vts_ptt_srpt->title[j].ptt[0].pgcn; //ifo_zero->tt_srpt->title[j].vts_ttn;
                if (isTitleIndex(ifo_zero,ts,vts_ttn))
                    indexedCount++;
		
                txt=i18n("Title %1").arg(indexedCount);
                emit sigTotalText (txt);
                emit sigTitleProgress(indexedCount,ltitles);
                video_attr = &vtsi_mat->vts_video_attr;

                vmgi_mat = ifo_zero->vmgi_mat;
                title_set_nr = ts; //ifo_zero->tt_srpt->title[j].title_set_nr;
                menuSizes[title_set_nr]=vtsi_mat->vtstt_vobs + vtsi_mat->vtsi_last_sector +1;

                //vts_pgcit->pgci_srp[ifo->vts_ptt_srpt->title[vts_ttn - 1].ptt[0].pgcn - 1].pgc;

                int titleStartSector=pgc->cell_playback[0].first_sector;
                //l_track=addTitle(j+1,title_set_nr,ifo->vts_ptt_srpt->title[vts_ttn - 1].ptt[0].pgcn - 1,titleStartSector,isTitleIndex(ifo_zero,ts,vts_ttn));
                bool titleIndexed=isTitleIndex(ifo_zero,ts,vts_ttn);
                l_track=addTitle(titleset,titleCount, calcNumTitle(ifo_zero,ts,vts_ttn),title_set_nr,j,titleStartSector,titleIndexed);
                titleset->add
                (l_track);

                sh.sprintf("%02x",pgc->playback_time.hour);
                sm.sprintf("%02x",pgc->playback_time.minute);
                ss.sprintf("%02x",pgc->playback_time.second);
                l_track->length.setHMS(sh.toInt(&ok,10),sm.toInt(&ok,10),ss.toInt(&ok,10));

                //printf(tr2i18n("Title: %02d, Length: %02x:%02x:%02x "), j+1, pgc->playback_time.hour, pgc->playback_time.minute, pgc->playback_time.second);

                if (dvdtime2msec(&pgc->playback_time) > max_length && isTitleIndex(ifo_zero,ts,vts_ttn)) {
                    max_length = dvdtime2msec(&pgc->playback_time);
                    longestTitle = l_track;
                }

                l_track->chapterCount =  pgc->nr_of_programs;//  ifo_zero->tt_srpt->title[j].nr_of_ptts;
                l_track->audioStreamCount = vtsi_mat->nr_of_vts_audio_streams;
                l_track->subPictureCount = vtsi_mat->nr_of_vts_subp_streams;
                l_track->VTS = ts;//  ifo_zero->tt_srpt->title[j].title_set_nr;
                l_track->TTN = j+1; // ifo_zero->tt_srpt->title[j].vts_ttn;
                l_track->FPS = frames_per_s[(pgc->playback_time.frame_u & 0xc0) >> 6];
                l_track->format= (*lvideoFormat.at(video_attr->video_format)).latin1();
                format = l_track->format;
                /*      QStringList::Iterator it;
                it= videoFormat.at(video_attr->video_format);
                c= (*it).latin1();
                */

                l_track->aspectRatio = (*laspectRatio.at(video_attr->display_aspect_ratio)).latin1();
                l_track->width = (*lvideoWidth.at(video_attr->picture_size)).latin1();
                ;
                l_track->height = (*lvideoHeight.at(video_attr->video_format)).latin1();
                l_track->DF = (*lpermittedDf.at(video_attr->permitted_df)).latin1();

                for (i=0; i<16; i++) {
                    QString pal;
                    pal.sprintf("%08x",pgc->palette[i]);
                    l_track->palette.append(pal);
                }

                // ANGLES
                l_track->angleCount =  ifo_zero->tt_srpt->title[j].nr_of_angles;

                // AUDIO
                //       		for (i=0; i<vtsi_mat->nr_of_vts_audio_streams; i++) {
                l_track->audioStreamCount=0;
                for (i=0; i<vtsi_mat->nr_of_vts_audio_streams; i++) {
                    if (pgc->audio_control[i]>>8 !=0) {
                        l_track->audioStreamCount++;
                        l_auds=l_track->addAudioStream();
                        //JMP l_auds->setselected(!titleIndexed);
                        audio_attr = &vtsi_mat->vts_audio_attr[i];
                        sprintf(lang_code, "%c%c", audio_attr->lang_code>>8, audio_attr->lang_code & 0xff);
                        if (!lang_code[0]) {
                            lang_code[0] = 'x';
                            lang_code[1] = 'x';
                        }
                        l_auds->langCod=lang_code;
                        l_auds->language=lang_name(lang_code);
                        l_auds->format= (*laudioFormat.at(audio_attr->audio_format)).latin1();
                        l_auds->frequency = (*lsampleFreq.at(audio_attr->sample_frequency)).latin1();
                        l_auds->quantization = (*lquantization.at(audio_attr->quantization)).latin1();
                        l_auds->channels = audio_attr->channels+1;
                        l_auds->appMode = audio_attr->application_mode;
                        l_auds->content = (*laudioType.at(audio_attr->lang_extension)).latin1();
                        //if (((pgc->audio_control[i]>>8) & 0x80) ==0x80) {
                        l_auds->id = 1+ ((pgc->audio_control[i]>>8) & 0x7) ;
			l_auds->m_streamId = (pgc->audio_control[i]>>8);
                        //} else {
                        //    l_auds->id=1;
                        //}
                    }
                }
                l_track->m_defAudio=NULL;
                l_track->m_defSubtitle=NULL;
                // CHAPTERS
                cell = 0;
                unsigned long total_sectors = 0;

                l_track->chapterCount= pgc->nr_of_programs;
                for (i=0;i < l_track->chapterCount;i++)
                    l_track->addChapter(i+1);

                k9DVDChapter *l_pchap=NULL;
                for (i=0; i<pgc->nr_of_programs; i++) {
                    int second=0, minute=0, hour=0, tmp;
                    char hex[2];
                    int next = pgc->program_map[i+1];
                    unsigned long sectors = 0;
                    l_chap = l_track->getChapter(i);
                    //first sector
                    l_chap->startSector=pgc->cell_playback[cell].first_sector;
                    ;
                    l_chap->endSector=0;
                    if (i == pgc->nr_of_programs - 1)
                        next = pgc->nr_of_cells + 1;

                    int iangleStart=-1,iangleEnd=-1;
                    while (cell < next - 1) {
                        //using c2 otherwise the value of cell were lost
                        //int c2=cell;
                        //l_chap->cells.append(c2);
                        int startSect=pgc->cell_playback[cell].first_sector;
                        l_chap->startSectors.append(startSect);

                        sprintf(hex, "%02x", pgc->cell_playback[cell].playback_time.second);
                        tmp = second + atoi(hex);
                        minute = minute + (tmp / 60);
                        second = tmp % 60;
                        sprintf(hex, "%02x", pgc->cell_playback[cell].playback_time.minute);
                        tmp = minute + atoi(hex);
                        hour = hour + (tmp / 60);
                        minute = tmp % 60;

                        l_chap->endSector= pgc->cell_playback[cell].last_sector;
                        //last sector

                        /* Check if we're entering an angle block. (vamp_play-title) */
                        int cell2=cell;
                        if ( pgc->cell_playback[ cell ].block_type
                                == BLOCK_TYPE_ANGLE_BLOCK  && cell>iangleEnd ) {
                            iangleStart=cell;
                            for( int idc = 0;; ++idc ) {
                                k9ChapterCell * chapterCell=new k9ChapterCell(cell2+idc,idc+1);
                                l_chap->cells.append(chapterCell);
                                chapterCell->setstartSector(pgc->cell_playback[cell2+idc].first_sector);
                                chapterCell->setlastSector(pgc->cell_playback[cell2+idc].last_sector);
                                if (idc==0)
                                    chapterCell->setangleBlock(angleStart);
                                else
                                    chapterCell->setangleBlock(angleInside);
                                if( pgc->cell_playback[ cell2 + idc ].block_mode
                                        == BLOCK_MODE_LAST_CELL ) {
                                    iangleEnd=cell2+idc;
                                    chapterCell->setangleBlock(angleEnd);
                                    sectors += pgc->cell_playback[iangleEnd].last_sector - pgc->cell_playback[iangleStart].first_sector + 1;

                                    break;
                                }
                            }
                        } else {
                            if (!(cell>=iangleStart  && cell <=iangleEnd)) {
                                k9ChapterCell * chapterCell=new k9ChapterCell(cell,1);
                                l_chap->cells.append(chapterCell);
                                chapterCell->setstartSector(pgc->cell_playback[cell].first_sector);
                                chapterCell->setlastSector(pgc->cell_playback[cell].last_sector);

                                sectors += pgc->cell_playback[cell].last_sector - pgc->cell_playback[cell].first_sector + 1;

                            }
                        }
                        cell++;

                    }

                    l_track->vobusize_mb += calcVobuSize(ifo,l_chap);
                    total_sectors += sectors;
                    l_chap->length.setHMS(hour,minute,second);
                    l_chap->sectors=sectors;
                    if (l_pchap!=NULL) {
                        QTime t;
                        int position;
                        t.setHMS(0,0,0);
                        position=t.secsTo(l_chap->length);
                        t=l_pchap->time.addSecs(position);
                        l_chap->time=t;
                    }
                    l_pchap=l_chap;
                }
                l_track->size_mb =((float)total_sectors *2048)/(1024*1024);

                // SUBTITLES
                l_track->subPictureCount=0;
                for (i=0; i<vtsi_mat->nr_of_vts_subp_streams; i++) {
                    if (pgc->subp_control[i]>>24 !=0) {
                        l_track->subPictureCount++;
                        subp_attr = &vtsi_mat->vts_subp_attr[i];
                        sprintf(lang_code, "%c%c", subp_attr->lang_code>>8, subp_attr->lang_code & 0xff);
                        if (!lang_code[0]) {
                            lang_code[0] = 'x';
                            lang_code[1] = 'x';
                        }
                        l_sub=l_track->addSubtitle(i+1);
                        //JMP : l_sub->setselected(!titleIndexed);
                        l_sub->langCod=lang_code;
                        l_sub->language=lang_name(lang_code);
                        l_sub->content= (*lsubpType.at(subp_attr->lang_extension)).latin1();
                        unsigned char subpc;
                        subpc=pgc->subp_control[i]>>24;
                        if ((subpc & 0x80)==0x80) {
                            l_sub->id=1;
                            if ((subpc &0x1F) !=0)
                                l_sub->id = 1+ (subpc & 0x1F);
                            subpc=pgc->subp_control[i]>>16;
                            if ((subpc &0x1F) !=0)
                                l_sub->id = 1+ (subpc & 0x1F);
                            subpc=pgc->subp_control[i]>>8;
                            if ((subpc &0x1F) !=0)
                                l_sub->id = 1+ (subpc & 0x1F);
                            subpc=pgc->subp_control[i];
                            if ((subpc &0x1F) !=0)
                                l_sub->id = 1+ (subpc & 0x1F);

                        }
                    }
                }
                if (isTitleIndex(ifo_zero,ts,vts_ttn) && !_quickScan)
                    calcStreamSize(*l_track);
            }

        }
        kifo.closeIFO();
    }
    menuSize=0;
    for (j=0;j<100;j++)
        menuSize+=menuSizes[j];
    kifo_zero.closeIFO();

    delete progressDlg;
    progressDlg=0;
    opened=true;
    m_dvd.close();
    return 0;
}

float k9DVD::calcVobuSize(ifo_handle_t *_ifo,k9DVDChapter *_chapter) {
    vobu_admap_t * vobu_admap = _ifo->vts_vobu_admap;
    int length = vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    float total=0;
    for(uint32_t i = 0; i < length/sizeof(uint32_t); i++) {
        if(vobu_admap->vobu_start_sectors[i] >= _chapter->startSector
                &&vobu_admap->vobu_start_sectors[i] <= _chapter->endSector)
            total++;
    }

    total=total*2048;
    total=total/(1024*1024);
    return total;

}

/** Adds a new track in the titles list the list is sorted by VTS and pgc*/
k9DVDTitle* k9DVD::addTitle(k9DVDTitleset *_titleset,int id,int num,int _VTS,int _pgc,int _startSector,bool _indexed) {
    k9DVDTitle *track,*tmp;
    track = new k9DVDTitle;
    track->numTitle=num;
    track->indexed=_indexed;
    track->id=id;
    track->m_titleset=_titleset;
    //JMP : modification
    track->forceSelection = false; // !_indexed;

    track->ts_nr=_VTS;
    track->pgc=_pgc;
    bool bappend=true;
    for (int i=0;i<titles.count();i++) {
        tmp=(k9DVDTitle*)titles.at(i);
        k9DVDChapter *chap =tmp->getChapter(0);

        if (tmp->ts_nr >_VTS) {
            bappend=false;
        } else if ( (tmp->ts_nr==_VTS) && ((tmp->pgc >_pgc) || (chap->getstartSector() > _startSector)) ) {
            bappend=false;
        }
        if (!bappend) {
            titles.insert(i,track);
            break;
        }

    }
    if (bappend)
        titles.append(track);
    track->name=i18n("Title %1").arg(num);
    return(track);

}

void k9DVD::calcStreamSize(k9DVDTitle & track) {
    struct stream_counter streams[64];
    int stream_count;
    int start = 0;
    int stop = 0;
    k9DVDChapter *c_start,*c_stop;
    struct streamSize streamList[64];
    //   unsigned long maxSectors=(track.size_mb*1024*1024)/2048;
    int c, x;
    QString pg;
    //   int  *b;
    //   emit sig_progress("start");
    //   setLog("start");

    for( x=0 ; x<64 ; x++ ) {                                                   // init stream usage list
        streams[x].id = 0;
        streams[x].count = 0;
        streams[x].size_mb = 0;
        streams[x].percent = 0;
        streamList[x].id=0;
        streamList[x].percent=0;
        streamList[x].size_mb=0;
    }

    x = 0;
    c = 0;

    if( track.chapterCount == 2 ) {                                                   // select which chapters to probe for stream sizes
        start=0;
        stop=1;
    }
    if (track.chapterCount == 1) {
        start=0;
        stop=0;
    }
    if (track.chapterCount >2) {
        //     start=(int)(track.chapterCount/2 - 0.5);
        //     stop=(int)(track.chapterCount/2 + 0.5);
        start=0;
        stop=(int)(track.chapterCount-1);
    }
    c_start=(k9DVDChapter*)track.chapters.at(start);
    c_stop=(k9DVDChapter*)track.chapters.at(stop);
    pg=tr2i18n("reading title");
    emit sigTitleText(pg);

    //   c = stream_vob(dvd, track.ts_nr,c_start->startSector, c_start->endSector, streams);
    //probe stream usage
    c = stream_vob( track.ts_nr,c_start->startSector, c_stop->endSector, streams);
    /*  if(start!=stop) {
    pg.sprintf(tr2i18n("chapter %d"),stop+1);
    emit sigTitleText(pg);
    c += stream_vob(dvd, track.ts_nr, c_stop->startSector, c_stop->endSector, streams);
    }

    */

    for( x=0 ; x<64 ; x++ ) {
        if( streams[x].id == 0 )
            break;
        streams[x].percent = (float)streams[x].count / (float)(c / 100.);     // calc usage in percent
    }
    // create streamlist with all streams
    stream_count=0;
    for( c=0 ; c<64 ; c++ ) {
        if( streams[c].id == 0 )
            break;
        streamList[stream_count].id = streams[c].id;
        if( stream_count >= 64 )
            break;
        stream_count++;
        x++;
    }

    //   other_streams_mb = 0;                                                       // calc size of all selected streams
    for( x=0 ; x<stream_count ; x++ ) {
        for( c=0 ; c<64 ; c++ ) {
            if( streamList[x].id == streams[c].id ) {
                streamList[x].percent=streams[c].percent;
                streamList[x].size_mb= (float) ((track.size_mb / 100.) * streams[c].percent);
            }
        }
    }

    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    k9DVDTitle *l_track;
    int i;
    l_track=&track;

    for (x=0;x<64;x++) {
        if (streamList[x].id==0)
            break;
        if (streamList[x].id== 0xe0)
            l_track->videosize_mb = streamList[x].size_mb;
    }

    for (i=0;i<l_track->audioStreamCount;i++) {
        l_auds=(k9DVDAudioStream*)l_track->audioStreams.at(i);
        for (x=0;x<64;x++) {
            if (streamList[x].id==0)
                break;
            int id=streamList[x].id;
            if (( (id >=0x80) && (id <=0x8f)) || ((id >=0xa0) && (id <=0xa7)) || ((id >=0xc0) && (id <=0xdf))) {
                if ( getVampsID(id) == l_auds->id) {
                    l_auds->size_mb = streamList[x].size_mb;
                    //stop the loop, go to next audio stream
                    break;
                }
            }

        }
    }
    for (i=0;i<l_track->subPictureCount;i++) {
        l_sub=(k9DVDSubtitle*)l_track->subtitles.at(i);
        for (x=0;x<64;x++) {
            if (streamList[x].id==0)
                break;
            int id=streamList[x].id;
            if ( (id >=0x20) && (id <=0x3f)) {
                if (  (id - 0x20 +1) ==l_sub->id) {
                    l_sub->size_mb = streamList[x].size_mb;
                    break;
                }
            }
        }
    }


}

int k9DVD::identify_stream( unsigned char *buffer ) {
    uint8_t packet_type = buffer[17];

    if( (packet_type >= 0xE0) && (packet_type <= 0xEF) ) {                 // video streams
        ;
    } else if( packet_type == 0xBB ) {                                       // system header
        ;
    } else if( packet_type == 0xBE ) {                                       // padding
        ;
    } else if( packet_type == 0xBF ) {                                       // nav pack
        ;
    } else if( (packet_type >= 0xC0) && (packet_type <= 0xDF) ) {            // mpeg audio
        ;
    } else if( packet_type == 0xBD ) {                                       // private stream, check content
        packet_type = buffer[23+buffer[22]];
    }

    return packet_type;
}

long k9DVD::stream_vob( int title, unsigned long startblock, unsigned long lastblock, struct stream_counter *sc) {

    k9DVDFile *dvdfile;
    //JMP unsigned char buffer[DVD_VIDEO_LB_LEN];
    unsigned char buffer[DVD_VIDEO_LB_LEN*800];
    unsigned char *ptrbuff;
    int blocksize=800,y;
    int step=(int)((lastblock-startblock)/800) / 10;// * 0.05;
    if (step==0)
        step=1;
    unsigned char stream_id;
    long total=0;
    unsigned long i, x ,nbread=0;
    ssize_t size;
    QString c;
    dvdfile =m_dvd.openTitle( title);
    if( !dvdfile ) {
        c=i18n("Error opening vobs for title %1\n").arg( title);
        setError(c);
        return 0;
    }

    i = startblock;
    while( (i >= startblock) && (i <= lastblock) && (blocksize>0)) {                         // read blocks
        size= dvdfile->readBlocks( i,blocksize , buffer);
        emit sigVobProgress(i-startblock,lastblock-startblock);
        total+=size;
        if( !size ) {
            c=i18n("ERROR reading block %1\n").arg(i);
            setError(c);
            break;
        }
        ptrbuff=buffer;
        for (y=0;y<blocksize;y++) {
            stream_id = identify_stream(ptrbuff);  // get stream id
            nbread++;
            for( x=0 ; x<64 ; x++ ) {             // scan for already present counter
                if( sc[x].id != 0 ) {
                    if( stream_id != sc[x].id ) {
                        continue;
                    }
                }

                sc[x].id = stream_id;             // increase stream packet counter
                sc[x].count++;
                break;
            }
            ptrbuff+=DVD_VIDEO_LB_LEN;
        }
        i+=blocksize*step;//   6;
        if (i+blocksize> lastblock)
            blocksize=lastblock-i;

    }

    dvdfile->close();
    return total;
}



float k9DVD::getsizeSelected() {
    float selstreams=0,vidstreams=0;
    int i,x;
    k9DVDTitle *l_track;
    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    bool withvideo;

    for (i=0;i<titleCount;i++) {
        l_track=gettitle(i);

        withvideo=l_track->isSelected() && l_track->getIndexed();

        if ( withvideo) {
            vidstreams +=l_track->getsize_mb();
            for (x=0;x<l_track->audioStreamCount;x++) {
                l_auds=l_track->getaudioStream(x);
                if (!l_auds->selected)
                    selstreams += l_auds->size_mb;
            }
            for (x=0;x<l_track->subPictureCount;x++) {
                l_sub=l_track->getsubtitle(x);
                if (!l_sub->selected)
                    selstreams += l_sub->size_mb;
            }
        }
    }
    vidstreams -=selstreams;

    return (vidstreams );
}


float k9DVD::getfactor(bool _withMenus,bool _streams) {
    if (_withMenus) {
        m_dvd.openDevice(Device);
        k9CellCopyList *cellCopyList =new k9CellCopyList(&m_dvd,this);
        double factor=cellCopyList->getfactor(_withMenus,_streams);
        m_dvd.close();
        return (factor);
    } else {
        float selstreams=0,vidstreams=0,l_factor;
        int i,x;
        k9DVDTitle *l_track;
        k9DVDAudioStream *l_auds;
        k9DVDSubtitle *l_sub;
        bool withvideo;

        for (i=0;i<titleCount;i++) {
            l_track=gettitle(i);

            withvideo=l_track->isSelected() && l_track->getIndexed();

            if ( withvideo) {
                vidstreams +=l_track->getsize_mb();
                if (_streams) {
                    for (x=0;x<l_track->audioStreamCount;x++) {
                        l_auds=l_track->getaudioStream(x);
                        if (!l_auds->selected)
                            selstreams += l_auds->size_mb;
                    }
                    for (x=0;x<l_track->subPictureCount;x++) {
                        l_sub=l_track->getsubtitle(x);
                        if (!l_sub->selected)
                            selstreams += l_sub->size_mb;
                    }
                }
            }
        }
        vidstreams -=selstreams;

        l_factor = (float) vidstreams / (k9DVDSize::getMaxSize());
        l_factor = (int)((l_factor+0.01)*100);
        l_factor /=100;
        if (l_factor <1)
            l_factor=1;

        return(l_factor);
    }
}
void k9DVD::slotVobProgress(unsigned int position,unsigned int total) {
    progressDlg->setpbTitleStep(position);
    progressDlg->setpbTitleTotalSteps(total);
    qApp->processEvents();
}

void k9DVD::slotTitleProgress(unsigned int position,unsigned int total) {
    progressDlg->setpbTotalStep(position);
    progressDlg->setpbTotalTotalSteps(total);
    qApp->processEvents();
}

void k9DVD::slotTitleText(QString& text) {
    progressDlg->setlblTitle(text);
    qApp->processEvents();
}

void k9DVD::slotTotalText(QString& text) {
    progressDlg->setlblTotal(text);
    qApp->processEvents();
}

const QString& k9DVD::getDevice() {
    return Device;
}

const bool& k9DVD::geterror() {
    return error;
}

const QString& k9DVD::geterrMsg() {
    return errMsg;
}

void k9DVD::setError(const QString & err) {
    error=true;
    errMsg=err;
    progressDlg->hide();
}

const bool& k9DVD::getopened() {
    return opened;
}


QString &k9DVD::getFormat() {
    return (format);
}

int k9DVD::getVampsID(int type) {
    int abase;
    if (type >= 0x80 && type <= 0x87) {
        // AC3 audio
        abase = 0x80;
    } else if (type >= 0x88 && type <= 0x8f) {
        // DTS audio
        abase = 0x88;
    } else if (type >= 0xa0 && type <= 0xbf) {
        // LPCM audio
        abase = 0xa0;
    } else if (type >= 0xc0 && type <= 0xdf) {
        // MPEG audio
        abase = 0xc0;
    }

    return (type-abase +1);

}


k9DVDTitle* k9DVD::getstart() {
    return start;
}

void k9DVD::setstart(k9DVDTitle* title) {
    start=title;
}


int k9DVD::getnewTitleNum(k9DVDTitle *title) {
    int num=0;
    k9DVDTitle *tr;
    for (int i=0 ;i < titleCount;i++) {
        tr=gettitle(i);
        if (tr->isSelected() && tr->getIndexed()) {
            ++num;
            if (tr->getnumTitle() == title->getnumTitle())
                return(num);
        }
    }

    return(-1);
}


void k9DVD::close() {
    opened=false;
    if (m_dvd.opened())
        m_dvd.close();
    titles.clear();
    titlesets.clear();
}
