//
// C++ Implementation: k9config
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9config.h"

k9Config::k9Config() {
    m_config=NULL;
    read();
}

void k9Config::read() {
  // if (! m_config->hasGroup( "options"))
  // 	readOldConfig();
  if (m_config!=NULL)
    delete m_config;
  m_config=new KSimpleConfig("K9Copy");


  m_config->setGroup( "dir");
  m_prefOutput=m_config->readEntry("output",locateLocal("tmp","k9copy/",true));
  
  m_config->setGroup("devices");
  m_InputDev=m_config->readNumEntry("input",0);
  m_OutputDev=m_config->readNumEntry("output",0);

  m_config->setGroup("options");
  m_prefK3b=m_config->readBoolEntry("usek3b",false);
  m_prefAutoBurn=m_config->readBoolEntry("autoburn",false);
  m_quickScan=m_config->readBoolEntry("quickscan",true);
  m_prefSize=m_config->readNumEntry("dvdsize",4400);
  m_useDvdAuthor=m_config->readBoolEntry("useDvdAuthor",true);
  m_keepMenus=m_config->readBoolEntry("keepMenus",true);
  m_prefDelTmpFiles=m_config->readBoolEntry("deltmpfiles",false);

  m_config->setGroup("mp4");
  m_prefMp4Codec=m_config->readNumEntry("codec",0);
  m_prefMp4Size=m_config->readNumEntry("size",700);
  m_prefMp4NumberCD=m_config->readNumEntry("numberCD",1);
  m_prefMp4Width=m_config->readEntry("width","640");
  m_prefMp4Height=m_config->readEntry("height","");
  m_prefMp4AspectRatio=m_config->readBoolEntry("aspectRatio",true);
  m_prefMp42Passes=m_config->readBoolEntry("2passes",false);
  m_prefMp4AudioBitrate=m_config->readEntry("audiobitrate","128");
  m_prefMp4VideoBitrate=m_config->readEntry("videobitrate" "");
  m_prefMp4AudioCodec=m_config->readNumEntry("audiocodec",0);
  m_prefMp4AudioGain=m_config->readNumEntry("audiogain",7);
  m_prefUseCellCache=m_config->readBoolEntry("usecellcache",true);

  m_config->setGroup("mencoder");
  m_prefVersion=m_config->readEntry("version","");
  m_codecAudio=m_config->readListEntry("audio");
  m_codecLabels=m_config->readListEntry("labels");
  m_codecLabelsAudio=m_config->readListEntry("audiolabels");
  

  m_codecVideo=m_config->readListEntry("video");
  
  m_config->setGroup("devices");
  m_devices=m_config->readListEntry("dev");
  m_devicesLabels=m_config->readListEntry("labels");
  m_devicesIO=m_config->readListEntry("IO");

  m_config->setGroup("preview");
  m_useMplayer=m_config->readBoolEntry("useMplayer",false);
  m_useGL=m_config->readBoolEntry("useGL",false);
  m_MplayerVout=m_config->readNumEntry("MplayerVout",0);
  m_MplayerAout=m_config->readNumEntry("MplayerAout",0);
}

void k9Config::save() {
  m_config->setGroup( "dir");
  m_config->writeEntry("output",m_prefOutput);
  
  m_config->setGroup("devices");
  m_config->writeEntry("input", m_InputDev);
  m_config->writeEntry("output",m_OutputDev);

  m_config->setGroup("options");
  m_config->writeEntry("usek3b",m_prefK3b);
  m_config->writeEntry("autoburn",m_prefAutoBurn);
  m_config->writeEntry("quickscan",m_quickScan);
  m_config->writeEntry("dvdsize",m_prefSize);
  m_config->writeEntry("useDvdAuthor",m_useDvdAuthor);
  m_config->writeEntry("keepMenus",m_keepMenus);
  m_config->writeEntry("deltmpfiles",m_prefDelTmpFiles);
  
  m_config->setGroup("mp4");
  m_config->writeEntry("codec",m_prefMp4Codec);
  m_config->writeEntry("size",m_prefMp4Size);
  m_config->writeEntry("numberCD",m_prefMp4NumberCD);
  m_config->writeEntry("width",m_prefMp4Width);
  m_config->writeEntry("height",m_prefMp4Height);
  m_config->writeEntry("audiobitrate",m_prefMp4AudioBitrate);
  m_config->writeEntry("videobitrate",m_prefMp4VideoBitrate);
  m_config->writeEntry("aspectRatio",m_prefMp4AspectRatio);
  m_config->writeEntry("2passes",m_prefMp42Passes);
  m_config->writeEntry("audiogain",m_prefMp4AudioGain);
  m_config->writeEntry("audiocodec", m_prefMp4AudioCodec);
  m_config->writeEntry("usecellcache",m_prefUseCellCache);


  m_config->setGroup("mencoder");
  m_config->writeEntry("version",m_prefVersion);
  m_config->writeEntry("audio",m_codecAudio);
  m_config->writeEntry("labels",m_codecLabels);
  m_config->writeEntry("audiolabels",m_codecLabelsAudio);
  m_config->writeEntry("video",m_codecVideo);

  m_config->setGroup("devices");
  m_config->writeEntry("dev",m_devices);
  m_config->writeEntry("labels",m_devicesLabels);
  m_config->writeEntry("IO",m_devicesIO);

  m_config->setGroup("preview");
  m_config->writeEntry("useMplayer",m_useMplayer);
  m_config->writeEntry("useGL",m_useGL);
  m_config->writeEntry("MplayerVout",m_MplayerVout);
  m_config->writeEntry("MplayerAout",m_MplayerAout);

}


k9Config::~k9Config(){
   delete m_config;

}


