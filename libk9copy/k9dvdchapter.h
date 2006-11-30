//
// C++ Interface: k9dvdchapter
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DVDCHAPTER_H
#define K9DVDCHAPTER_H

#include "k9common.h"
#include "k9dvd.h"
#include <qptrlist.h>

static int cptChapter=0;


enum angleBlock_t {angleNone=0,angleInside=1,angleStart=3,angleEnd=5};

class k9DVDTitle;
class k9ChapterCell :public QObject
{
  Q_OBJECT
public:
  k9ChapterCell(uint _id,uint _angle){ m_id=_id;m_angle=_angle;m_angleBlock=angleNone;};
  virtual uint getid() {return m_id;};
  virtual uint getangle(){return m_angle;};
  virtual uchar getangleBlock() {return m_angleBlock;};
  virtual void setangleBlock(uchar _angleBlock) {m_angleBlock=_angleBlock;};
  virtual void setstartSector(uint32_t _value) { m_startSector=_value;};
  virtual uint32_t getstartSector() { return m_startSector;};
  virtual void setlastSector(uint32_t _value) { m_lastSector=_value;};
  virtual uint32_t getlastSector() { return m_lastSector;};

private:
  uint m_id;
  uint m_angle;
  uchar m_angleBlock;
  uint32_t m_startSector,m_lastSector;
};

class k9DVDChapter : public QObject
{
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
  k9DVDTitle *m_title;
  bool m_selected;
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
  virtual k9DVDTitle * getTitle();
  unsigned long getstartSector() { return startSector;};
  unsigned long getendSector() {return endSector;};

	void setSelected(bool _value);
	

	bool getSelected() const;
	
};

#endif
