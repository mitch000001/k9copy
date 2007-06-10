//
// C++ Interface: k9config
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CONFIG_H
#define K9CONFIG_H
#include "k9common.h"

#include <ksimpleconfig.h>
#include <kstandarddirs.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Config{
private:
    KSimpleConfig *m_config;
    int  m_InputDev;
    int  m_OutputDev;
    bool m_keepMenus;
    QString m_prefOutput;
    bool m_useDvdAuthor;
    bool m_quickScan;
    int  m_prefSize;
    bool m_prefK3b;
    bool m_prefMenu;
    bool m_prefAutoBurn;
    int  m_prefMp4Codec;
    int  m_prefMp4AudioCodec;
    int  m_prefMp4AudioGain;
    int  m_prefMp4Size;
    int  m_prefMp4NumberCD;
    bool m_prefMp4AspectRatio;
    bool m_prefMp42Passes;
    QString  m_prefMp4Width;
    QString  m_prefMp4Height;
    QString  m_prefMp4AudioBitrate;
    QString  m_prefMp4VideoBitrate;
    QStringList m_codecAudio;
    QStringList m_codecLabelsAudio;
    QStringList m_codecLabels;
    QStringList m_codecVideo;
    QString m_prefCodecAudio,m_prefCodecVideo,m_prefCodecLabel;

    QStringList m_devices;
    QStringList m_devicesLabels;
    QStringList m_devicesIO;
    
    bool m_useMplayer;
    bool m_useGL;
    int  m_MplayerVout;
    int m_MplayerAout;
public:
    k9Config();
    void read();
    void save();
    ~k9Config();

	void setInputDev(const int& _value) {
	    m_InputDev = _value;
	}
	

	int getInputDev() const {
	    return m_InputDev;
	}

	void setOutputDev(const int& _value) {
	    m_OutputDev = _value;
	}
	

	int getOutputDev() const {
	    return m_OutputDev;
	}

	void setKeepMenus(bool _value) {
	    m_keepMenus = _value;
	}
	

	bool getKeepMenus() const {
	    return m_keepMenus;
	}

	void setPrefOutput(const QString& _value) {
	    m_prefOutput = _value;
	}
	

	QString getPrefOutput() const {
	    return m_prefOutput;
	}

	void setUseDvdAuthor(bool _value) {
	    m_useDvdAuthor = _value;
	}
	

	bool getUseDvdAuthor() const {
	    return m_useDvdAuthor;
	}

	void setQuickScan(bool _value) {
	    m_quickScan = _value;
	}
	

	bool getQuickScan() const {
	    return m_quickScan;
	}

	void setPrefSize(const int& _value) {
	    m_prefSize = _value;
	}
	

	int getPrefSize() const {
	    return m_prefSize;
	}

	void setPrefK3b(bool _value) {
	    m_prefK3b = _value;
	}
	

	bool getPrefK3b() const {
	    return m_prefK3b;
	}

	void setPrefMenu(bool _value) {
	    m_prefMenu = _value;
	}
	

	bool getPrefMenu() const {
	    return m_prefMenu;
	}

	void setPrefAutoBurn(bool _value) {
	    m_prefAutoBurn = _value;
	}
	

	bool getPrefAutoBurn() const {
	    return m_prefAutoBurn;
	}

	void setPrefMp4Codec(const int& _value) {
	    m_prefMp4Codec = _value;
	}

	int getPrefMp4Codec() const {
	    return m_prefMp4Codec;
	}

	void setPrefMp4AudioCodec(const int& _value) {
	    m_prefMp4AudioCodec = _value;
	}

	int getPrefMp4AudioCodec() const {
	    return m_prefMp4AudioCodec;
	}


	void setPrefMp4Size(const int& _value) {
	    m_prefMp4Size = _value;
	}
	

	int getPrefMp4Size() const {
	    return m_prefMp4Size;
	}

	void setPrefMp4AudioGain(const int& _value) {
	    m_prefMp4AudioGain = _value;
	}
	

	int getPrefMp4AudioGain() const {
	    return m_prefMp4AudioGain;
	}

	void setPrefMp4NumberCD(const int& _value) {
	    m_prefMp4NumberCD = _value;
	}
	

	int getPrefMp4NumberCD() const {
	    return m_prefMp4NumberCD;
	}

	void setPrefMp4Width(const QString& _value) {
	    m_prefMp4Width = _value;
	}
	

	QString getPrefMp4Width() const {
	    return m_prefMp4Width;
	}

	void setPrefMp4Height(const QString& _value) {
	    m_prefMp4Height = _value;
	}
	

	QString getPrefMp4Height() const {
	    return m_prefMp4Height;
	}

	void setPrefMp4AudioBitrate(const QString& _value) {
	    m_prefMp4AudioBitrate = _value;
	}
	

	QString getPrefMp4AudioBitrate() const {
	    return m_prefMp4AudioBitrate;
	}

	void setPrefMp4VideoBitrate(const QString& _value) {
	    m_prefMp4VideoBitrate = _value;
	}
	

	QString getPrefMp4VideoBitrate() const {
	    return m_prefMp4VideoBitrate;
	}

	void setCodecAudio(const QStringList& _value) {
	    m_codecAudio = _value;
	}
	

	QStringList getCodecAudio() const {
	    return m_codecAudio;
	}

	void setCodecLabels(const QStringList& _value) {
	    m_codecLabels = _value;
	}
	

	QStringList getCodecLabels() const {
	    return m_codecLabels;
	}

	void setCodecVideo(const QStringList& _value) {
	    m_codecVideo = _value;
	}
	

	QStringList getCodecVideo() const {
	    return m_codecVideo;
	}


	void setCodecLabelsAudio(const QStringList& _value) {
	    m_codecLabelsAudio = _value;
	}
	

	QStringList getCodecLabelsAudio() const {
	    return m_codecLabelsAudio;
	}


	void setDevices(const QStringList& _value) {
	    m_devices = _value;
	}
	

	QStringList getDevices() const {
	    return m_devices;
	}

	void setDevicesLabels(const QStringList& _value) {
	    m_devicesLabels = _value;
	}
	

	QStringList getDevicesLabels() const {
	    return m_devicesLabels;
	}

	void setDevicesIO(const QStringList& _value) {
	    m_devicesIO = _value;
	}
	

	QStringList getDevicesIO() const {
	    return m_devicesIO;
	}

	void setUseMplayer(bool _value) {
	    m_useMplayer = _value;
	}
	

	bool getUseMplayer() const {
	    return m_useMplayer;
	}

	void setUseGL(bool _value) {
	    m_useGL = _value;
	}
	

	bool getUseGL() const {
	    return m_useGL;
	}

	void setPrefMp4AspectRatio(bool _value) {
	    m_prefMp4AspectRatio = _value;
	}
	

	bool getPrefMp4AspectRatio() const {
	    return m_prefMp4AspectRatio;
	}

	void setPrefMp42Passes(bool _value) {
	    m_prefMp42Passes = _value;
	}
	

	bool getPrefMp42Passes() const {
	    return m_prefMp42Passes;
	}

	void setMplayerVout(const int& _value) {
	    m_MplayerVout = _value;
	}
	

	int getMplayerVout() const {
	    return m_MplayerVout;
	}

	void setMplayerAout(const int& _value) {
	    m_MplayerAout = _value;
	}
	

	int getMplayerAout() const {
	    return m_MplayerAout;
	}
	
};

#endif
