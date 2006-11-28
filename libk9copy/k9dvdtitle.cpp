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

#include "k9dvdtitle.h"

#include <qapplication.h>

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qvaluelist.h>


//*******************k9DVDAudioStream**************
k9DVDAudioStream::k9DVDAudioStream() {
    size_mb=0;
    selected=false;
}

/** Read property of QString format. */
const QString& k9DVDAudioStream::getformat() {
    return format;
}

/** Read property of QString frequency. */
const QString& k9DVDAudioStream::getfrequency() {
    return frequency;
}
/** Read property of QString quantization. */
const QString& k9DVDAudioStream::getquantization() {
    return quantization;
}
/** Read property of int channels. */
const int& k9DVDAudioStream::getchannels() {
    return channels;
}
/** Read property of int appMode. */
const int& k9DVDAudioStream::getappMode() {
    return appMode;
}
/** Read property of QString content. */
const QString& k9DVDAudioStream::getcontent() {
    return content;
}

/** Read property of float size_mb. */
const float& k9DVDAudioStream::getsize_mb() {
    return size_mb;
}


/** Read property of bool selected. */
const bool& k9DVDAudioStream::getselected() {
    return selected;
}
/** Write property of bool selected. */
void k9DVDAudioStream::setselected( const bool& _newVal) {
    selected = _newVal;
    title->gettitleset()->updateSelection();
}

/** Read property of QString langCod. */
const QString& k9DVDAudioStream::getlangCod() {
    return langCod;
}
/** Read property of QString language. */
const QString& k9DVDAudioStream::getlanguage() {
    return language;
}

k9DVDTitle * k9DVDAudioStream::gettitle() {
	return title;
}

int k9DVDAudioStream::getID() {
    return(id);
}

//*******************k9DVDSubtitle *******************

k9DVDSubtitle::k9DVDSubtitle() {
    size_mb=0;
    selected=false;
}


/** Read property of QString langCod. */
const QString& k9DVDSubtitle::getlangCod() {
    return langCod;
}
/** Read property of QString language. */
const QString& k9DVDSubtitle::getlanguage() {
    return language;
}
/** Read property of int num. */
const int& k9DVDSubtitle::getnum() {
    return num;
}
/** Read property of QString content. */
const QString& k9DVDSubtitle::getcontent() {
    return content;
}

/** Read property of bool selected. */
const bool& k9DVDSubtitle::getselected() {
    return selected;
}
/** Write property of bool selected. */
void k9DVDSubtitle::setselected( const bool& _newVal) {
    selected = _newVal;
    title->gettitleset()->updateSelection();
}

/** Read property of float size_mb. */
const float& k9DVDSubtitle::getsize_mb() {
    return size_mb;
}

k9DVDTitle * k9DVDSubtitle::gettitle() {
	return title;
}


/*!
\fn k9DVDSubtitle::getID()
*/
int k9DVDSubtitle::getID() {
    return(id);
}

//************************* k9DVDChapter************************
/** Read property of int num. */
const int& k9DVDChapter::getnum() {
    return num;
}
/** Read property of QTime length. */
const QTime& k9DVDChapter::getlength() {
    return length;
}

/** Read property of int sectors. */
const int& k9DVDChapter::getsectors() {
    return sectors;
}

const QTime& k9DVDChapter::gettime() {
    return time;
}

k9DVDChapter::k9DVDChapter() {
    length.setHMS(0,0,0);
    num=0;
    sectors=0;
    startSector=0;
    endSector=0;
    time.setHMS(0,0,0);
    cells.setAutoDelete(true);
}

//************************* k9DVDTitle ***************************

/** No descriptions */
k9DVDAudioStream* k9DVDTitle::addAudioStream() {
    k9DVDAudioStream *auds;
    auds = new k9DVDAudioStream;
    auds->title=this;
    audioStreams.append(auds);
    return(auds);
}

/** No descriptions */
k9DVDSubtitle* k9DVDTitle::addSubtitle(int num) {
    k9DVDSubtitle *subt;
    subt = new k9DVDSubtitle;
    subt->title=this;
    subt->num=num;
    subtitles.append(subt);
    return(subt);
}

/** No descriptions */
k9DVDChapter* k9DVDTitle::addChapter(int num) {
    k9DVDChapter *chap;
    chap = new k9DVDChapter;
    chap->num = num;
    chap->id=k9DVDChapter::getcptChapter();
    chapters.append(chap);
    return(chap);
}

k9DVDChapter* k9DVDTitle::getChapter(int num) {
    return (k9DVDChapter*)chapters.at(num);
}

/** No descriptions */
k9DVDAudioStream *k9DVDTitle::getaudioStream(int num) {
    return (k9DVDAudioStream*)audioStreams.at(num);
}


/** Read property of int chapterCount. */
const int& k9DVDTitle::getchapterCount() {
    return chapterCount;
}
/** Read property of int streamCount. */
const int& k9DVDTitle::getstreamCount() {
    return streamCount;
}

/** Read property of int audioStreamCount. */
const int& k9DVDTitle::getaudioStreamCount() {
    return audioStreamCount;
}

/** Read property of int audioStreamCount. */
const int& k9DVDTitle::getsubPictureCount() {
    return subPictureCount;
}
/** Read property of int streamCount. */
const QTime& k9DVDTitle::getlength() {
    return length;
}

/** Read property of int streamCount. */
const int& k9DVDTitle::getnumTitle() {
    return numTitle;
}

/** No descriptions */
k9DVDSubtitle *k9DVDTitle::getsubtitle(int num) {
    return (k9DVDSubtitle*)subtitles.at(num);
}

/** Read property of float size_mb. */
const float& k9DVDTitle::getsize_mb() {
    return size_mb;
}

const float k9DVDTitle::gettotalsize_mb() {
    float size =size_mb;
    for (int i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
   	 size +=tmp->getsize_mb();
    }
    return size;

}

const float k9DVDTitle::gettotalvideosize_mb() {
    float size =videosize_mb;
    for (int i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
 	 size+=tmp->getvideosize_mb();
    }
    return size;

}


QTime k9DVDTitle::gettotallength() {
    QTime totalTime=getlength();
    for (int i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
	 QTime t;
	 int msecs= t.msecsTo(tmp->getlength());
	 totalTime=totalTime.addMSecs(msecs);
    }
    return totalTime;

}



/** Read property of float videosize_mb. */
const float& k9DVDTitle::getvideosize_mb() {
    return videosize_mb;
}

/** Read property of float vobusize_mb. */
const float& k9DVDTitle::getvobusize_mb() {
    return vobusize_mb;
}


/** Read property of int VTS. */
const int& k9DVDTitle::getVTS() {
    return VTS;
}
/** Read property of int TTN. */
const int& k9DVDTitle::getTTN() {
    return TTN;
}
/** Read property of float FPS. */
const float& k9DVDTitle::getFPS() {
    return FPS;
}
/** Read property of QString format. */
const QString& k9DVDTitle::getformat() {
    return format;
}
/** Read property of QString aspectRatio. */
const QString& k9DVDTitle::getaspectRatio() {
    return aspectRatio;
}
/** Read property of QString width. */
const QString& k9DVDTitle::getwidth() {
    return width;
}
/** Read property of QString height. */
const QString& k9DVDTitle::getheight() {
    return height;
}
/** Read property of QString palette. */
void k9DVDTitle::getpalette(int i,QString & c) {
    c=(*palette.at(i)).latin1();
}
/** Read property of QString DF. */
const QString& k9DVDTitle::getDF() {
    return DF;
}
/** Read property of int angleCount. */
const int& k9DVDTitle::getangleCount() {
    return angleCount;
}


bool k9DVDTitle::isSelected() {
    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    int i;

    if (forceSelection)
        return true;

    for (i=0;i<audioStreams.count();i++) {
        l_auds=getaudioStream(i);
        if (l_auds->getselected()) {
            return(true);
        }
    }

    for (i=0;i<subtitles.count();i++) {
        l_sub=getsubtitle(i);
        if (l_sub->getselected()) {
            return(true);
        }
    }
    return(false);
}

 void k9DVDTitle::setforceSelection(bool _state) { 
	forceSelection = _state;    
	gettitleset()->updateSelection();
}


/** Read property of QString name. */
const QString& k9DVDTitle::getname() {
    return name;
}
/** Write property of QString name. */
void k9DVDTitle::setname( const QString& _newVal) {
    name = _newVal;
}

/*!
\fn k9DVDTitle::getpgc()
*/
int k9DVDTitle::getpgc() {
    return(pgc);
}


/*!
\fn k9DVDTitle::setNextTitle(k9DVDTitle * nextTitle)
*/
void k9DVDTitle::setnextTitle(k9DVDTitle * nTitle) {
    nextTitle=nTitle;
}


/*!
\fn k9DVDTitle::getnextTitle()
*/
k9DVDTitle* k9DVDTitle::getnextTitle() {
    return nextTitle;
}


k9DVDAudioStream* k9DVDTitle::getDefAudio() const {
    return m_defAudio;
}


void k9DVDTitle::setDefAudio(k9DVDAudioStream* _value) {
    m_defAudio = _value;
    m_defAudioSet = true;
}


k9DVDSubtitle* k9DVDTitle::getDefSubtitle() const {
    return m_defSubtitle;
}


void k9DVDTitle::setDefSubtitle(k9DVDSubtitle* _value) {
    m_defSubtitle = _value;
    m_defSubtitleSet=true;
}


k9DVDTitle::k9DVDTitle() {
    audioStreams.setAutoDelete(true);
    chapters.setAutoDelete(true);
    subtitles.setAutoDelete(true);
    size_mb=0;
    videosize_mb=0;
    vobusize_mb=0;
    nextTitle=NULL;
    m_defAudio=NULL;
    m_defSubtitle=NULL;
    m_defAudioSet=false;
    m_defSubtitleSet=false;
    forceSelection=false;
    m_forceFactor=false;
    m_factor=0;
}

void k9DVDTitleset::add(k9DVDTitle * _title) {
   titles.append(_title);
}

int k9DVDTitleset::count() {
  return titles.count();
}

k9DVDTitle * k9DVDTitleset::gettitle(uint _pos) {
	return titles.at(_pos);
}

uint32_t k9DVDTitleset::getsize(){
	return m_size;
}

bool k9DVDTitleset::getselected() {
	return m_selected;
}

void k9DVDTitleset::setselected(bool _state) {
// selectionner / d�electionner tous les titres
	for (uint i=0; i<count();i++) {
		k9DVDTitle *title=titles.at(i);
		for (uint j=0; j <title->getaudioStreamCount();j++)
			title->getaudioStream(j)->setselected(_state);
		for (uint j=0;j<title->getsubPictureCount();j++)
			title->getsubtitle(j)->setselected(_state);
		title->setforceSelection(_state);
	}
	m_selected=_state;;
}

void k9DVDTitleset::updateSelection() {
	if (m_updating)
		return;
	m_updating=true;
	bool selected=m_selected;
	m_selected=false;
	//check if at least on title is selected
	for (uint i=0; i<count() && (!m_selected);i++) {
		k9DVDTitle *title=titles.at(i);
		if (title->isSelected() && title->getIndexed())
			m_selected=true;
	}

	//if titleset selected, we select all features. otherwise they are unselected
	//TODO select parts of titles only
	if (selected !=m_selected) 
		for (uint i=0; i<count();i++) {
			k9DVDTitle *title=titles.at(i);
			//FIXED : parts of title selection
			if (title->getIndexed() && title->isSelected()) 
				for (int iTitle=0;iTitle <title->getTitles().count();iTitle++) {
				    k9DVDTitle *title2=title->getTitles().at(iTitle);
				    for (uint j=0; j <title2->getaudioStreamCount();j++)
					    title2->getaudioStream(j)->setselected(title->getaudioStream(j)->getselected());
				    for (uint j=0;j<title2->getsubPictureCount();j++)
					    title2->getsubtitle(j)->setselected(title2->getsubtitle(j)->getselected());
				    title2->setforceSelection(true);		    
				}
			/* 
			if (!title->getIndexed()) {
				for (uint j=0; j <title->getaudioStreamCount();j++)
					title->getaudioStream(j)->setselected(m_selected);
				for (uint j=0;j<title->getsubPictureCount();j++)
					title->getsubtitle(j)->setselected(m_selected);
				title->setforceSelection(m_selected);
			}*/
		}
	m_updating=false;
}

const QString k9DVDTitleset::getsize_mb() {
	double size_mb=(double)m_size /512;
	QString c;
	c.sprintf("%.2f",size_mb);
	return c;
}

int k9DVDTitleset::getnum() {
	return m_num;
}



