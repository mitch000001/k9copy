//
// C++ Implementation: k9dvdchapter
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvdchapter.h"
#include "k9dvdtitle.h"

//************************* k9DVDChapter************************
/** Read property of int num. */
const int& k9DVDChapter::getnum()
{
  return num;
}
/** Read property of QTime length. */
const QTime& k9DVDChapter::getlength()
{
  return length;
}

/** Read property of int sectors. */
const int& k9DVDChapter::getsectors()
{
  return sectors;
}

const QTime& k9DVDChapter::gettime()
{
  return time;
}
k9DVDTitle *k9DVDChapter::getTitle()
{
  return m_title;
}

k9DVDChapter::k9DVDChapter()
{
  length.setHMS(0,0,0);
  num=0;
  sectors=0;
  startSector=0;
  endSector=0;
  m_selected=false;
  time.setHMS(0,0,0);
  cells.setAutoDelete(true);
}


bool k9DVDChapter::getSelected() const
{
  return m_selected;
}


void k9DVDChapter::setSelected(bool _value)
{
  m_selected = _value;
  if (_value &&  !m_title->isSelected() )
	  m_title->setforceSelection( true);
	
}
