//
// C++ Interface: k9titleencopt
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9TITLEENCOPT_H
#define K9TITLEENCOPT_H

#include <qobject.h>


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9DVDTitle;

class k9TitleEncOpt : public QObject
{
Q_OBJECT
public:
    k9TitleEncOpt(k9DVDTitle *parent );
    ~k9TitleEncOpt();
   k9DVDTitle *getTitle() { return (k9DVDTitle*)parent();};
    
public:
   int getMaxSize();
   void setMaxSize(int);
   
   int getNumParts();
   void setNumParts(int);
   
   const QString & getWidth();
   void setWidth(QString);
   
   const QString & getHeight();
   void setHeight(QString);
   
   const QString & getAudioBr();
   void setAudioBr(QString);
   
   bool getKeepAspectRatio();
   void setKeepAspectRatio(bool);
   
   bool get2Passes();
   void set2Passes(bool);
   
   int getCodec();
   void setCodec(int);


private:
   int m_maxSize;
   int m_numParts;
   QString m_width;
   QString m_height;
   bool m_keepAspectRatio;
   bool m_2Passes;
   QString m_audioBr;
   int m_codec;
   
public slots:
   void load();
   
};

#endif
