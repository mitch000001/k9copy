//
// C++ Implementation: k9dvdtitleset
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9dvdtitleset.h"
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qvaluelist.h>


const QString k9DVDTitleset::getsize_mb()
{
  double size_mb=(double)m_size /512;
  QString c;
  c.sprintf("%.2f",size_mb);
  return c;
}

int k9DVDTitleset::getnum()
{
  return m_num;
}

void k9DVDTitleset::add(k9DVDTitle * _title)
{
  titles.append(_title);
}

int k9DVDTitleset::count()
{
  return titles.count();
}

k9DVDTitle * k9DVDTitleset::gettitle(uint _pos)
{
  return titles.at(_pos);
}

uint32_t k9DVDTitleset::getsize()
{
  return m_size;
}

bool k9DVDTitleset::getselected()
{
  return m_selected;
}

void k9DVDTitleset::setselected(bool _state)
{
  // selectionner / d�electionner tous les titres
  for (uint i=0; i<count();i++)
  {
    k9DVDTitle *title=titles.at(i);
    for (uint j=0; j <title->getaudioStreamCount();j++)
      title->getaudioStream(j)->setselected(_state);
    for (uint j=0;j<title->getsubPictureCount();j++)
      title->getsubtitle(j)->setselected(_state);
    title->setforceSelection(_state);
  }
  m_selected=_state;;
}

void k9DVDTitleset::updateSelection()
{
  if (m_updating)
    return;
  m_updating=true;
  bool selected=m_selected;
  m_selected=false;
  //check if at least on title is selected
  for (uint i=0; i<count() && (!m_selected);i++)
  {
    k9DVDTitle *title=titles.at(i);
    if (title->isSelected() && title->getIndexed())
      m_selected=true;
  }

  //if titleset selected, we select all features. otherwise they are unselected
  //TODO select parts of titles only
  if (selected !=m_selected)
    for (uint i=0; i<count();i++)
    {
      k9DVDTitle *title=titles.at(i);
      //FIXED : parts of title selection
      if (title->getIndexed() && title->isSelected())
        for (int iTitle=0;iTitle <title->getTitles().count();iTitle++)
        {
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
