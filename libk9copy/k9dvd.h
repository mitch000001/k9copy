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
#ifndef K9DVD_H
#define K9DVD_H
#include "k9common.h"
#include "k9dvdread.h"
#include <qstringlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>


struct streamSize {
   unsigned char id;
   float size_mb;
   float percent;
};

struct stream_counter {
   unsigned char id;
   int count;
   float size_mb;
   float percent;
};

class k9DVDProgress;
class k9DVDChapter;
class k9DVDTitle;
class k9DVDTitleset;

/** get the DVD structure with titles lengths and streams lengths.
  *@author 
  */

class k9DVD : public QObject  {
Q_OBJECT  
private:
  QObjectList titles;
  QPtrList<k9DVDTitleset> titlesets;
  QString title;
  int titleCount;
  k9DVDTitle *longestTitle;
  QStringList lvideoFormat;
  QStringList laspectRatio;
  QStringList lquantization;
  QStringList lmpegVersion;
  QStringList lvideoHeight;
  QStringList lvideoWidth;
  QStringList lpermittedDf;
  QStringList laudioFormat;
  QStringList lsampleFreq;
  QStringList laudioType;
  QStringList lsubpType;
  QStringList langCode;
  QStringList langName;
  double frames_per_s[4];
  uint menuSize;
  k9DVDProgress *progressDlg; 
  QString Device;
  bool error;
  QString errMsg;
  bool opened;
  QString format;
  k9DVDTitle* start;
  int titlesetCount;
public: 
  k9DVD(QObject  *parent=0,const char *name=0,const QStringList args=0) ;
  ~k9DVD();

  /**
   * Opens and estimates the size of each stream
   * @param device input device
   * @return return code (0=ok)
   */
  virtual int scandvd (const QString & device,bool _quickScan);
 
  /**
   * gets the title of the DVD
   * @return DVD title
   */
  virtual const QString& getDVDTitle();
  
  /**
   * set the title of the DVD 
   * @param _newVal the new DVD title
   */
  virtual void setDVDTitle(const QString &_newVal);
  
  /**
   * 
   * @return returns the number of titles
   */
  virtual const int& gettitleCount();

  /**
   * 
   * @return returns the number of titlesets
   */
  virtual const int& gettitlesetCount();


  /**
   * returns a title of the DVD
   * @param num the title number
   * @return the title
   */
  virtual k9DVDTitle* gettitle(int num);
  
  /**
   * returns a title (which is in tt_srpt)  
   * @param num the title number 
   * @return the title
   */
  virtual k9DVDTitle* gettitleByNum(int num);
  virtual k9DVDTitle *getlongestTitle();
  virtual float getfactor(bool withMenus,bool _streams);
  virtual const QString& getDevice();
  virtual const bool& geterror();
  virtual const QString& geterrMsg();
  virtual const bool& getopened();
  virtual float getsizeSelected(bool _streams);
  virtual QString &getFormat();
  virtual k9DVDTitle* getstart();
  virtual void setstart(k9DVDTitle* title);
  virtual int getnewTitleNum(k9DVDTitle *title);
  virtual int getmenuSize();
  virtual void close();
  virtual k9DVDTitleset *gettitleset(int num);
private: // Private methods
  k9DVDRead  m_dvd;
  k9DVDTitle* addTitle(k9DVDTitleset *titleset,int id,int num,int _VTS,int _pgc,uint32_t _startSector, bool _indexed);
  float calcVobuSize(ifo_handle_t *_ifo,k9DVDChapter *_chapter);
  long stream_vob( int title, unsigned long startblock, unsigned long lastblock, struct stream_counter *sc);
  int identify_stream( unsigned char *buffer ) ;
  int dvdtime2msec(dvd_time_t *dt);
  int get_title_name(const char* dvd_device, char* title);
  QString lang_name(const QString & code);
  void calcStreamSize(k9DVDTitle & title);
  void setError(const QString &err);
  int getVampsID(int type);
  int calcNumTitle(ifo_handle_t *ifo,int _vts,int _ttn);
public slots: // Public slots
  void slotVobProgress(unsigned int position,unsigned int total);
  void slotTitleProgress(unsigned int position,unsigned int total);
  void slotTitleText(QString &text);
  void slotTotalText(QString &text);
signals: // Signals
  void sigVobProgress(unsigned int position,unsigned int total);
  void sigTitleProgress(unsigned int position,unsigned int total);
  void sigTitleText(QString &text);
  void sigTotalText(QString &text);
};

typedef struct lng { char code[3];	QString name;};

#endif

