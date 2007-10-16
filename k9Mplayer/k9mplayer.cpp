
#include "k9common.h"
#include "k9config.h"
#include <qlayout.h>
#include <qslider.h>
#include <kselect.h>
#include "k9mplayer.h"
#include <qcombobox.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <kprocess.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qevent.h>
#include <klocale.h>
#include <kmessagebox.h>

K9Mplayer::K9Mplayer(QObject  *parent,const char *name,const QStringList args):MPlayer((QWidget*)parent) {
    m_seeking=FALSE;
    m_initVol=TRUE;
    m_title=0;

    m_process=new KProcess(this);
    m_ratio=4.0/3.0;

    slider->setRange(0, 100);
    slider->setSteps(1, 1);
    bPlay->setPixmap(SmallIcon("player_play"));
    bStop->setPixmap(SmallIcon("player_stop"));
    bSwitchAudio->setPixmap(SmallIcon("cycle"));
    connect(m_process,SIGNAL(receivedStdout( KProcess*, char*, int )),this,SLOT(receivedStdout( KProcess*, char*, int )));
    m_timer=new QTimer(this);
    connect(m_process,SIGNAL(wroteStdin( KProcess* )),this,SLOT(wroteStdin( KProcess* )));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    m_container=new QWidget(Label);
}


K9Mplayer::~K9Mplayer() {}

void K9Mplayer::resizeEvent ( QResizeEvent * _resizeEvent) {
    int w,h;
    w=Label->width();
    h=Label->width() /m_ratio;
    if (h>Label->height()) {
        h=Label->height();
        w=Label->height()*m_ratio;
    }

    m_container->resize(w,h);
    m_container->move((Label->width()-w)/2,(Label->height()-h)/2);
}


void K9Mplayer::wroteStdin( KProcess *_process) {
    m_canwrite = TRUE;
}

void K9Mplayer::timeout() {
    sendCmd( "get_percent_pos");

}

void K9Mplayer::setDevice(const QString & _device) {
    m_device=_device;
}


void K9Mplayer::setTitle( const QString & _numTitle,const QString &_numChapter) {
    if (m_process->isRunning()) {
        sendCmd("quit");
        m_process->wait( 1000);
    }
    k9Config config;
    QString vout[]= {"x11","xv","gl2","sdl"};
    QString aout[]= {"alsa","oss","sdl"};
    QString vo=vout[config.getMplayerVout()];
    QString ao=aout[config.getMplayerAout()];


    m_process->clearArguments();
    *m_process << "mplayer";
    *m_process << "-vo" << vo << "-ao" << ao << "-sws" << "0";
    *m_process << "-framedrop";
    *m_process << "-wid" << QString::number(m_container->winId()) ;
    *m_process << "-slave" ;
    *m_process <<  "-idle";
    *m_process << "-dvd-device" << m_device;
    if (_numChapter !="")
        *m_process << "-chapter" << _numChapter;

    *m_process << QString("dvd://%1").arg(_numTitle);
    if (!m_process->start( KProcess::NotifyOnExit,KProcess::All)) {
         KMessageBox::error (qApp->mainWidget(),i18n("Unable to run %1").arg("mplayer") , i18n("Preview"));
    }
    m_canwrite=TRUE;

    m_position=0;
    slider->setValue(m_position);
}




/*$SPECIALIZATION$*/

void  K9Mplayer::receivedStdout (KProcess *proc, char *buffer, int buflen) {
    QString buf = QString::fromLatin1(buffer, buflen);
    sscanf(buf.latin1(),"ANS_PERCENT_POSITION=%d",&m_position);
    if (!m_seeking && m_position>0) {
        slider->setValue(m_position);
    }
    int audio=0;
    if (buf.contains("ANS_switch_audio"))
        sscanf(buf.latin1(),"ANS_switch_audio=%d",&audio);
    if (audio >0) {
        for (int i=0;i < m_dvdTitle->getaudioStreamCount();i++) {
            k9DVDAudioStream * str=m_dvdTitle->getaudioStream(i);
            if (str->getStreamId() == audio) {
                cbAudio->setCurrentItem(i);
            }
        }
    }
}

void K9Mplayer::slotLengthChanged() {}

void K9Mplayer::slotNewPosition(int _pos,const QTime & _time) {
    slider->setValue(_pos);

}

void K9Mplayer::sliderReleased() {
    sendCmd( QString("seek %1 1").arg((int)slider->value()));
    m_seeking=FALSE;
}

void K9Mplayer::sliderChanged( int _value) {}

void K9Mplayer::sliderPressed() {
    m_seeking=TRUE;

}

void K9Mplayer::bPlayClick() {
    setTitle(QString::number( m_title),"");
}

void K9Mplayer::bStopClick() {
    sendCmd("quit");
}


void K9Mplayer::bDownClick() {
    sendCmd( QString("volume -1"));

}

void K9Mplayer::bSwitchAudioClick() {
    sendCmd( QString("switch_audio"));
    sendCmd( QString("get_property switch_audio"));
}

void K9Mplayer::bUpClick() {
    sendCmd( QString("volume +1"));

}


void K9Mplayer::open( k9DVD *_dvd,k9DVDTitle *_title,int chapter) {
    cbSub->clear();
    cbAudio->clear();
    for (int i=0; i< _title->getaudioStreamCount();i++) {
        k9DVDAudioStream *aud=_title->getaudioStream(i);
        cbAudio->insertItem(NULL,QString("%1-%2").arg(aud->getID()).arg(aud->getlanguage()),-1);
    }

    for (int i=0; i< _title->getsubPictureCount();i++) {
        k9DVDSubtitle *sub=_title->getsubtitle(i);
        cbSub->insertItem(NULL,QString("%1-%2").arg(sub->getID().first()).arg(sub->getlanguage()),-1);
    }

    if(_title->getaspectRatio()=="16:9")
        m_ratio=16.0/9.0;
    else
        m_ratio=4.0/3.0;
    resizeEvent( NULL);

    setDevice( _dvd->getDevice());
    m_dvdTitle=_title;
    m_title=_title->getnumTitle();
    setTitle( QString::number(m_title),QString::number(chapter));
    if (_title->getaudioStreamCount() >0)
        cbAudioActivated( 0);
    if (_title->getsubPictureCount() >0)
        cbSubActivated( 0);
    if (!m_timer->isActive())
        m_timer->start(200,FALSE);

}

void K9Mplayer::titleChanged() {}

void K9Mplayer::cbAudioActivated( int _value) {
    if (m_dvdTitle) {
       int value=m_dvdTitle->getaudioStream( _value)->getStreamId();
       sendCmd(QString( "switch_audio %1").arg(value));
    }
}

void K9Mplayer::cbSubActivated( int _value) {
    sendCmd(QString("sub_select %1").arg(_value));

}


void K9Mplayer::sendCmd( QString _cmd) {
    while (!m_canwrite) {
        qApp->processEvents();
        if ( !m_process->isRunning())
            return;
    }

    _cmd +="\n";
    m_canwrite=FALSE;
    m_process->writeStdin( _cmd.latin1() ,_cmd.length());

}

#include "k9mplayer.moc"

