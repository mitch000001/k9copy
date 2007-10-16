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
#include "k9dvdtitleset.h"
#include "k9titleencopt.h"

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
    bool before=title->isSelected();
    selected = _newVal;
    title->gettitleset()->setAudio(getID(),_newVal);
    title->gettitleset()->updateSelection();
    bool after=title->isSelected();
    if (after!=before)
    	title->selectChapters( after);
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
    bool before=title->isSelected();
    selected = _newVal;
    title->gettitleset()->updateSelection();
    bool after=title->isSelected();
    if (after!=before)
    	title->selectChapters(after);
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
QValueList<int>& k9DVDSubtitle::getID() {
    return id;
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
    chap->m_title=this;
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
    for (uint i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
   	 size +=tmp->getsize_mb();
    }
    return size;

}

//If _selected=true, returns the size of selected chapters, otherwise returns size of
//unselected ones
const float k9DVDTitle::getChaptersSize_mb(bool _selected){
   return (getChaptersSize(_selected)/512);
}

//If _selected=true, returns the size of selected chapters, otherwise returns size of
//unselected ones
const uint64_t k9DVDTitle::getChaptersSize(bool _selected){
	uint64_t total=0;
	for (int i=0;i<chapterCount ;i++) {
		k9DVDChapter *chap = getChapter( i);
		if (chap->getSelected()==_selected) {
			total+= chap->getsectors();
		}
	}
	for (uint i=0;i<m_titles.count();i++) {
		k9DVDTitle * title=m_titles.at(i);
		total+=title->getChaptersSize(_selected);
	}
	return total;

}

const float k9DVDTitle::gettotalvideosize_mb() {
    float size =videosize_mb;
    for (uint i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
 	 size+=tmp->getvideosize_mb();
    }
    return size;

}


QTime k9DVDTitle::gettotallength() {
    QTime totalTime=getlength();
    for (uint i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
	 QTime t;
	 int msecs= t.msecsTo(tmp->getlength());
	 totalTime=totalTime.addMSecs(msecs);
    }
    return totalTime;

}

QTime k9DVDTitle::getSelectedLength() {
    QTime totalTime(0,0);
    for (k9DVDChapter *chapter=chapters.first();chapter;chapter=chapters.next()) {
    	if (chapter->getSelected()) {
		QTime t;
		int msecs= t.msecsTo(chapter->getlength());
		totalTime=totalTime.addMSecs(msecs);
    	}
    }
    
    
    for (uint i=0;i<m_titles.count();i++) {
         k9DVDTitle *tmp=m_titles.at(i);
         QPtrList <k9DVDChapter> tchapters=tmp->getChapters();
          for (k9DVDChapter *chapter=tchapters.first();chapter;chapter=tchapters.next()) {
		if (chapter->getSelected()) {
			QTime t;
			int msecs= t.msecsTo(chapter->getlength());
			totalTime=totalTime.addMSecs(msecs);
		}
	}           
    }
    return totalTime;

}


/** Read property of float videosize_mb. */
const float& k9DVDTitle::getvideosize_mb() {
    return videosize_mb;
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
    c=*palette.at(i);
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
    uint i;

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
	bool before=isSelected();
	forceSelection = _state;    
	gettitleset()->updateSelection();
	bool after=isSelected();
	if (after!=before) {
	    selectChapters(after);
	    emit selectionChanged();
	}
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
    m_encOpt=NULL;
}

k9TitleEncOpt* k9DVDTitle::getEncOpt()  { 
  if (!m_encOpt)
  	m_encOpt=new k9TitleEncOpt(this);
  return m_encOpt;
  
}


void k9DVDTitle::selectChapters(bool _state) {
    for (int i=0;i < chapterCount;i++) {
	getChapter(i)->setSelected( _state);    
    }
    
    for (uint iTitle=0;iTitle<m_titles.count();iTitle++) {
	k9DVDTitle * title=m_titles.at(iTitle);
	title->selectChapters(_state);     
    }
}




