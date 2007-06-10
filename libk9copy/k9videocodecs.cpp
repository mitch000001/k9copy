//
// C++ Implementation: k9videocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9videocodecs.h"
#include <qstringlist.h>

class _k9VideoCodec
{
public:
    _k9VideoCodec():name(""),optPass1(""),optPass2(""),optOnePass(""){};
   _k9VideoCodec(QString _name,QString _optOnePass,QString _optPass1,QString _optPass2) {
      name=_name;
      optOnePass=_optOnePass;
      optPass1=_optPass1;
      optPass2=_optPass2; 
   }
   QString name;
   QString optOnePass;
   QString optPass1;
   QString optPass2;
}; 


k9VideoCodecs::k9VideoCodecs(QObject *parent, const char *name)
 : QObject(parent, name)
{
   m_config=new k9Config();
   QStringList slLabels=m_config->getCodecLabels();
   QStringList slCodecs=m_config->getCodecVideo();

   //adds default codecs
   if (slLabels.count()==0) {
      m_codecs[0]=_k9VideoCodec("copy","-ovc copy","-ovc copy","-ovc copy");
      m_codecs[1]=_k9VideoCodec("XviD","-ovc xvid -xvidencopts bitrate=$VIDBR","-ovc xvid -xvidencopts bitrate=$VIDBR:turbo:pass=%1","-ovc xvid -xvidencopts bitrate=$VIDBR");
      m_codecs[2]=_k9VideoCodec("x264","-ovc x264 -x264encopts bitrate=$VIDBR","-ovc x264 -x264encopts bitrate=$VIDBR:turbo=1:pass=$PASS","-ovc x264 -x264encopts bitrate=$VIDBR:turbo=1:pass=$PASS");     
      m_codecs[3]=_k9VideoCodec("MJPEG","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[4]=_k9VideoCodec("LJPEG","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[5]=_k9VideoCodec("H261","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[6]=_k9VideoCodec("H263","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[7]=_k9VideoCodec("H263+","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[8]=_k9VideoCodec("MPEG-4 (DivX 4/5)","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR -ffourcc=DIVX","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS -ffourcc=DIVX","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR -ffourcc=DIVX");
      m_codecs[9]=_k9VideoCodec("MS MPEG-4 (DivX 3)","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[10]=_k9VideoCodec("MS MPEG-4 v2","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[11]=_k9VideoCodec("WMV7","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[12]=_k9VideoCodec("WMV8","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR");

      m_codecs[13]=_k9VideoCodec("RealVideo","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[14]=_k9VideoCodec("MPEG-1 Video","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[15]=_k9VideoCodec("MPEG-2 Video","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR");
      m_codecs[16]=_k9VideoCodec("Huffmann yuv","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:format=422p","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR::format=422p:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:format=422p");
      m_codecs[17]=_k9VideoCodec("ffv Huffmann","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[18]=_k9VideoCodec("ASUS v1","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[19]=_k9VideoCodec("ASUS v2","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");
      m_codecs[20]=_k9VideoCodec("flv","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:turbo:vpass=$PASS","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR");

      save();
      m_config=new k9Config();
      slLabels=m_config->getCodecLabels();
      slCodecs=m_config->getCodecVideo();
   }  

   QStringList::iterator c=slCodecs.begin();
   int cpt=0;
   for (QStringList::iterator i=slLabels.begin();i!=slLabels.end() ;++i) {
        QString o1=(*c);
        c++;
        QString o2=(*c);
        c++;
        QString o3=(*c);
        c++;

        m_codecs[cpt++]=_k9VideoCodec((*i),o1,o2,o3);
   }
   delete m_config;

}

void k9VideoCodecs::save() {
   m_config=new k9Config();

    QStringList labels;
    QStringList options;
    for (QMap<int,_k9VideoCodec>::iterator i=m_codecs.begin();i!=m_codecs.end();++i) {
        labels << i.data().name;
        options << i.data().optOnePass;
        options << i.data().optPass1;
        options << i.data().optPass2;
    }
    m_config->setCodecLabels(labels);
    m_config->setCodecVideo(options);
    m_config->save();
    delete m_config;
}




int k9VideoCodecs::count() {
   return m_codecs.count();
}

void k9VideoCodecs::setOptions0(int _num,QString _value) {
    m_codecs[_num].optOnePass=_value;
}
void k9VideoCodecs::setOptions1(int _num,QString _value) {
    m_codecs[_num].optPass1=_value;
}
void k9VideoCodecs::setOptions2(int _num,QString _value) {
    m_codecs[_num].optPass2=_value;
}

void k9VideoCodecs::setCodecName(int _num,QString _value) {
    m_codecs[_num].name=_value;
}

QString k9VideoCodecs::getOptions0(int _num) {
   return m_codecs[_num].optOnePass;
}

QString k9VideoCodecs::getOptions1(int _num) {
   return m_codecs[_num].optPass1;
}

QString k9VideoCodecs::getOptions2(int _num) {
   return m_codecs[_num].optPass2;
}
QString k9VideoCodecs::getCodecName(int _num) {
   return m_codecs[_num].name;
}

void k9VideoCodecs::remove(int _num) {
    int nb=count();
    for(int i=_num;i<nb;i++) {
        m_codecs[i]=m_codecs[i+1];
        m_codecs.remove(i+1);
    }
}

k9VideoCodecs::~k9VideoCodecs()
{

}


#include "k9videocodecs.moc"
