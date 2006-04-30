//
// C++ Interface: k9mp4enc
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MP4ENC_H
#define K9MP4ENC_H
#include "k9common.h"
#include "k9dvdtitle.h"

#include <qobject.h>
#include <kprocess.h>


class k9MP4Dlg;
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9MP4Enc : public QObject {
    Q_OBJECT
public:
    enum Codec {
	xvid =0,
        lavc_mp4=1
    };
private:
    KProcess *m_process;
    k9MP4Dlg  *m_progress;
    int getBitRate(k9DVDTitle *_title);
    QString m_txt;
    QString m_device;
    QString m_filename;
    QString m_height;
    QString m_width;
    QString m_size;
    QString m_audioBitrate;
    QString m_stderr;
    Codec m_codec;
    int m_cpt;
private slots:
    void getStdout(KProcess *proc, char *buffer, int buflen);
    void getStderr(KProcess *proc, char *buffer, int buflen);
    void exited(KProcess *proc);
public:
    k9MP4Enc(QObject *parent = 0, const char *name = 0,const QStringList& args=0);
    ~k9MP4Enc();

    virtual void execute(k9DVDTitle *_title );

    virtual void setDevice(const QString& _value) {
        m_device = _value;
    };
    virtual void setFilename(const QString& _value) {
        m_filename = _value;
    };
    virtual void setHeight(const QString& _value) {
        m_height = _value;
    };
    virtual void setWidth(const QString& _value) {
        m_width = _value;
    };
    virtual void setSize(const QString& _value) {
        m_size = _value;
    };
    virtual void setAudioBitrate(const QString& _value) {
        m_audioBitrate = _value;
    };

    virtual void setCodec(const Codec& _value) {m_codec = _value;};
	






};

#endif
