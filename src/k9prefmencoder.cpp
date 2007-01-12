//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9prefmencoder.h"
#include "k9config.h"
#include <qtable.h>

k9prefMencoder::k9prefMencoder(QWidget* parent, const char* name, WFlags fl)
: prefMencoder(parent,name,fl)
{
  QStringList laudio;
  QStringList llabels;
  QStringList lvideo;

  k9Config config;
  laudio=config.getCodecAudio();
  llabels=config.getCodecLabels();
  lvideo=config.getCodecVideo();
  
  int row=0;
  for ( QStringList::Iterator it = llabels.begin(); it != llabels.end(); ++it )
  {
    tblOptions->setNumRows(row+1);
    tblOptions->setText(row,0,optValue( (*it).latin1()));
    QStringList::Iterator it3=lvideo.at(row);
    tblOptions->setText(row,1,optValue((*it3).latin1()));
    QStringList::Iterator it2=laudio.at(row);
    tblOptions->setText(row,2,optValue((*it2).latin1()));
    row++;
  }


}

QString k9prefMencoder::optValue(QString _value) {
   if (_value=="#") 
	return "";
   else
	return _value;
}

void k9prefMencoder::save()
{
  QStringList laudio;
  QStringList llabels;
  QStringList lvideo;
  
  k9Config config;
  laudio.clear();
  llabels.clear();
  lvideo.clear();
  for (int i=0;i<tblOptions->numRows();i++)
  {
    QString c=tblOptions->text(i,0);
    if (!c.isEmpty())
    {
        llabels.append(tblOptions->text(i,0));
        c=tblOptions->text(i,1);
        if (c.isEmpty()) c="#";
        lvideo.append(c);
        c=tblOptions->text(i,2);
        if (c.isEmpty()) c="#";
        laudio.append(c);
    }
  }
  
  config.setCodecAudio( laudio);
  config.setCodecLabels( llabels);
  config.setCodecVideo(  lvideo);
  config.save();
}




k9prefMencoder::~k9prefMencoder()
{
}

void k9prefMencoder::bAddClick()
{
  tblOptions->setNumRows(tblOptions->numRows()+1);
}


void k9prefMencoder::bRemoveClick()
{
  tblOptions->removeRow(tblOptions->currentRow());
}



/*$SPECIALIZATION$*/


#include "k9prefmencoder.moc"

