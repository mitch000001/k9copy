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
#include <k9process.h>
#include <qtimer.h>
#include <qstringlist.h>
class k9MP4Dlg;
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9MP4Enc : public QObject {
    Q_OBJECT
private:
    k9Process *m_process;
    k9MP4Dlg  *m_progress;
    k9DVDTitle *m_title;
    int getBitRate(k9DVDTitle *_title);
    QString m_txt;
    QString m_device;
    QString m_filename;
    QString m_height;
    QString m_width;
    QString m_size;
    QString m_audioBitrate;
    QString m_videoBitrate;
    QString m_audioGain;
    QString m_fourcc;
    QString m_stderr;
    int m_parts;
    float m_percent;
    QTimer *timer;
    QString m_remain;
    uint32_t m_totalSize;
    bool m_2pass;
    int m_pass;
    bool m_canceled;
    int m_codec;
    int m_audioCodec;
    bool m_usecache;
    QStringList m_lstVideo,m_lstAudio,m_lstCodecs;
    int m_cpt;
    QTime *time,m_timer3;
    QString replaceParams(QString _input);
    QString round16(QString _wh);
    QString getChapterList(k9DVDTitle *_title);
    int getselectedSubp(k9DVDTitle *_title);

private slots:
    void getStdout(KProcess *proc, char *buffer, int buflen);
    void getStderr(KProcess *proc, char *buffer, int buflen);
    void timerDone();
    void slotCancel();

public:
    k9MP4Enc(QObject *parent = 0, const char *name = 0,const QStringList& args=0);
    ~k9MP4Enc();

    virtual void execute(k9DVDTitle *_title );

    virtual void setNumberCD(const QString & _value) {
	m_parts = _value.toInt();
	if (m_parts==0) m_parts=1;
    }

    virtual void setDevice(const QString& _value) {
        m_device = _value.stripWhiteSpace();
    };
    virtual void setFilename(const QString& _value) {
        m_filename = _value.stripWhiteSpace();
    };
    virtual void setHeight(const QString& _value) {
        m_height = _value.stripWhiteSpace();
    };
    virtual void setWidth(const QString& _value) {
        m_width = _value.stripWhiteSpace();
    };
    virtual void setSize(const QString& _value) {
        m_size = _value.stripWhiteSpace();
    };
    virtual void setAudioBitrate(const QString& _value) {
        m_audioBitrate = _value.stripWhiteSpace();
    };
    virtual void setVideoBitrate(const QString& _value) {
        m_videoBitrate = _value.stripWhiteSpace();
    };
    virtual void setAudioGain(const QString& _value) {
	m_audioGain = _value.stripWhiteSpace();
    };
    virtual void setfourcc(const QString& _value) {
        m_fourcc = _value.stripWhiteSpace();
    };

    virtual void set2Passes(bool _value) {
    	m_2pass=_value;
    }

    virtual void setUseCache(bool _value) {
    	m_usecache=_value;
    }

    virtual void setCodec(const int& _value) {m_codec = _value;};

    virtual void setAudioCodec(const int& _value) {m_audioCodec = _value;};
	
    virtual QWidget *getDialog() {return (QWidget*)m_progress;};

    virtual bool isCanceled();
};

#endif
