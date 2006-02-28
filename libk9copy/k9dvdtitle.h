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
#ifndef K9DVDTITLE_H
#define K9DVDTITLE_H

#include "k9common.h"
#include "k9dvd.h"
#include <qdatetime.h>
#include <qstringlist.h>

static int cptChapter=0;
enum angleBlock_t {angleNone=0,angleInside=1,angleStart=3,angleEnd=5};

class k9ChapterCell :public QObject{
Q_OBJECT
public:
   k9ChapterCell(uint _id,uint _angle){ m_id=_id;m_angle=_angle;m_angleBlock=angleNone;};
   virtual uint getid() {return m_id;};
   virtual uint getangle(){return m_angle;};
   virtual uchar getangleBlock() {return m_angleBlock;};
   virtual void setangleBlock(uchar _angleBlock) {m_angleBlock=_angleBlock;};
private:
   uint m_id;
   uint m_angle;
   uchar m_angleBlock;
};

class k9DVDChapter : public QObject{
Q_OBJECT
friend class k9DVDTitle;
friend class k9DVD;
private: // Private attributes
  /**  */
  int num;
  int id;
  /**  */
  QTime length;
  QTime time;
  /**  */
  int sectors;
  unsigned long startSector;
  unsigned long endSector;
  static int getcptChapter() { cptChapter ++; return cptChapter;};
  static void setcptChapter(int _newValue) { cptChapter=_newValue;};
public: // Public methods
  k9DVDChapter();
  /** Read property of int num. */
  virtual const int& getnum();
  /** Read property of QTime length. */
  virtual const QTime& getlength();
  /** Read property of int sectors. */
  virtual const int& getsectors();
  QPtrList<k9ChapterCell> cells;
  QValueList<uint32_t> startSectors;
  virtual const QTime & gettime();
  unsigned long getstartSector() { return startSector;};
  unsigned long getendSector() {return endSector;};
};
  
class k9DVDVideoStream : public QObject{
Q_OBJECT
friend class k9DVDTitle;
private: // Private attributes
public: // Public methods

};
  
class k9DVDAudioStream : public QObject{
Q_OBJECT
friend class k9DVDTitle;
friend class k9DVD;
private: // Private attributes
  /**  */
  QString langCod;
  /**  */
  QString language;
  /**  */
  QString format;
  /**  */
  QString frequency;
  /**  */
  QString quantization;
  /**  */
  int channels;
  /**  */
  int appMode;
  /**  */
  QString content;
  /**  */
  float size_mb;
  /**  */
  bool selected;
    int id;
   k9DVDTitle *title;

public: // Public methods
  k9DVDAudioStream();
  /** Read property of QString langCod. */
  virtual const QString& getlangCod();
  /** Read property of QString language. */
  virtual const QString& getlanguage();
  /** Read property of QString format. */
  virtual const QString& getformat();
  /** Read property of QString frequency. */
  virtual const QString& getfrequency();
  /** Read property of QString quantization. */
  virtual const QString& getquantization();
  /** Read property of int channels. */
  virtual const int& getchannels();
  /** Read property of int appMode. */
  virtual const int& getappMode();
  /** Read property of QString content. */
  virtual const QString& getcontent();
  /** Read property of float size_mb. */
  virtual const float& getsize_mb();
  /** Write property of bool selected. */
  virtual void setselected( const bool& _newVal);
  /** Read property of bool selected. */
  virtual const bool& getselected();

  virtual k9DVDTitle* gettitle();
  virtual  int getID();
};

class k9DVDSubtitle : public QObject{
Q_OBJECT
friend class k9DVDTitle;
friend class k9DVD;
private: // Private attributes
  /**  */
  QString langCod;
  /**  */
  QString language;
  /**  */
  int num;
  /**  */
  QString content;
  /**  */
  float size_mb;
  /**  */
  bool selected;
    int id;
   k9DVDTitle *title;
public: // Public methods
  k9DVDSubtitle();
  /** Read property of QString langCod. */
  virtual const QString& getlangCod();
  /** Read property of QString language. */
  virtual const QString& getlanguage();
  /** Read property of int num. */
  virtual const int& getnum();
  /** Read property of QString content. */
  virtual const QString& getcontent();
  /** Read property of float size_mb. */
  virtual const float& getsize_mb();
  /** Write property of bool selected. */
  virtual void setselected( const bool& _newVal);
  /** Read property of bool selected. */
  virtual const bool& getselected();
  virtual k9DVDTitle* gettitle();
  virtual  int getID();
};
  



class k9DVDTitle : public QObject{
Q_OBJECT  
friend class k9DVD;
private: // Public attributes
  unsigned int ts_nr;
  int chapterCount;
  int streamCount;
  int audioStreamCount;
  int subPictureCount;
  QTime length;
  k9DVDVideoStream videoStream;
  QObjectList audioStreams;
  QObjectList subtitles;
  int VTS;
  int TTN;
  float FPS;
  QString format;
  QString aspectRatio;
  int numTitle;
  QString width;
  QString height;
  QStringList palette;
  QString DF;
  float size_mb;
  int angleCount;
  float videosize_mb;
  float vobusize_mb;
  QString name;
   k9DVDTitle* nextTitle;
  int id;
  k9DVDTitleset *m_titleset;
public: // Public methods
  k9DVDTitle();
  virtual k9DVDTitleset *gettitleset() {return m_titleset;};
  virtual const int& getchapterCount();
  virtual const int& getstreamCount();
  virtual const int& getaudioStreamCount();
  virtual const int& getsubPictureCount();    
  virtual const QTime& getlength();
  virtual const int& getVTS();
  virtual const int& getTTN();
  virtual const float& getFPS();
  virtual const QString& getformat();
  virtual const QString& getaspectRatio();
  virtual const QString& getwidth();
  virtual const QString& getheight();
  virtual void getpalette(int i,QString & c);
  virtual const QString& getDF();
  virtual const int& getangleCount();
  virtual const int& getnumTitle();
  virtual k9DVDAudioStream *getaudioStream(int num);
  virtual k9DVDSubtitle *getsubtitle(int num);
  virtual const float& getsize_mb();
  virtual const float& getvideosize_mb();
  virtual const float& getvobusize_mb();
  virtual bool isSelected();
  virtual void setname( const QString& _newVal);
  virtual const QString& getname();
  virtual int getpgc();
  virtual void setnextTitle(k9DVDTitle * nTitle);
  virtual k9DVDTitle* getnextTitle();
  virtual k9DVDChapter *getChapter(int num);
  virtual bool getIndexed() {return indexed;};
  virtual void setforceSelection(bool _state);
  virtual int getId() {return id;};
private: // Private methods
  k9DVDAudioStream* addAudioStream();
  k9DVDChapter* addChapter(int num);
  k9DVDSubtitle* addSubtitle(int num);
  QObjectList chapters;
  bool indexed;
  bool forceSelection;
   int pgc;

public: // Public attributes
};
  

class k9DVDTitleset :public QObject{
Q_OBJECT
private:
  int m_num;
  uint32_t m_size;
  QPtrList <k9DVDTitle> titles;
  bool m_selected;
  bool m_updating;
public:
  k9DVDTitleset( uint _num,uint32_t _size) {m_num=_num;m_size=_size;m_selected=false;m_updating=false;};
  virtual void add(k9DVDTitle *_title);
  virtual int count();
  virtual k9DVDTitle *gettitle(uint _pos);
  virtual uint32_t getsize();
  virtual const QString getsize_mb();
  virtual bool getselected();
  virtual void setselected(bool state);
  virtual void updateSelection();
  virtual int getnum();
};

#endif

