//
// C++ Implementation: k9audiocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9audiocodecs.h"

#include <qstringlist.h>

class _k9AudioCodec
{
public:
    _k9AudioCodec():name(""),options(""){};
   _k9AudioCodec(QString _name,QString _options) {
      name=_name;
      options=_options; 
   }
   QString name;
   QString options;
}; 


k9AudioCodecs::k9AudioCodecs(QObject *parent, const char *name)
 : QObject(parent, name)
{
   m_config=new k9Config();
   QStringList slLabels=m_config->getCodecLabelsAudio();
   QStringList slCodecs=m_config->getCodecAudio();

   //adds default codecs
   if (slLabels.count()==0) {
      m_codecs[0]=_k9AudioCodec("copy","-oac copy");
      m_codecs[1]=_k9AudioCodec("mp3","-oac lavc -lavcopts acodec=mp3:abitrate=$AUDBR");
      m_codecs[2]=_k9AudioCodec("mp2","-oac lavc -lavcopts acodec=mp2:abitrate=$AUDBR");
      m_codecs[3]=_k9AudioCodec("ac3","-oac lavc -lavcopts acodec=ac3:abitrate=$AUDBR");
      m_codecs[4]=_k9AudioCodec("IMA Adaptive PCM","-oac lavc -lavcopts acodec=adpcm_ima_wav:abitrate=$AUDBR");
      m_codecs[5]=_k9AudioCodec("sonic","-oac lavc -lavcopts acodec=sonic:abitrate=$AUDBR");
      m_codecs[6]=_k9AudioCodec("aac","-oac faac -faacopts br=$AUDBR");

      save();
      m_config=new k9Config();
      slLabels=m_config->getCodecLabelsAudio();
      slCodecs=m_config->getCodecAudio();
    }
   QStringList::iterator c=slCodecs.begin();
   int cpt=0;
   for (QStringList::iterator i=slLabels.begin();i!=slLabels.end() ;++i) {
        QString o1=(*c);
        c++;
        m_codecs[cpt++]=_k9AudioCodec((*i),o1);
   }
   delete m_config;

}

void k9AudioCodecs::save() {
   m_config=new k9Config();

    QStringList labels;
    QStringList options;
    for (QMap<int,_k9AudioCodec>::iterator i=m_codecs.begin();i!=m_codecs.end();++i) {
        labels << i.data().name;
        options << i.data().options;
    }
    m_config->setCodecLabelsAudio(labels);
    m_config->setCodecAudio(options);
    m_config->save();
    delete m_config;
}




int k9AudioCodecs::count() {
   return m_codecs.count();
}

void k9AudioCodecs::setOptions(int _num,QString _value) {
    m_codecs[_num].options=_value;
}

void k9AudioCodecs::setCodecName(int _num,QString _value) {
    m_codecs[_num].name=_value;
}

QString k9AudioCodecs::getOptions(int _num) {
   return m_codecs[_num].options;
}

QString k9AudioCodecs::getCodecName(int _num) {
   return m_codecs[_num].name;
}

void k9AudioCodecs::remove(int _num) {
    int nb=count();
    for(int i=_num;i<nb;i++) {
        m_codecs[i]=m_codecs[i+1];
        m_codecs.remove(i+1);
    }
}

k9AudioCodecs::~k9AudioCodecs()
{
}


#include "k9audiocodecs.moc"
