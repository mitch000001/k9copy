
#ifndef K9Mplayer_H
#define K9Mplayer_H

#include "k9common.h"
#include "../k9Mplayer/mplayer.h"


class k9DVD;
class k9DVDTitle;
class KProcess;
class QTimer;
class QResizeEvent;

class K9Mplayer : public MPlayer
{
  Q_OBJECT
private:
  KProcess    *m_process;
  bool m_seeking,m_initVol;
  int m_title;
  k9DVDTitle *m_dvdTitle;
  QString m_device;
  QTimer *m_timer;
  void sendCmd(QString _cmd);
  int m_position;
  bool m_canwrite;
  QWidget *m_container;
  float m_ratio;
public:
  //K9Mplayer(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  K9Mplayer(QObject  *parent=0,const char *name=0,const QStringList args=0);
  ~K9Mplayer();
  /*$PUBLIC_FUNCTIONS$*/
  void setDevice(const QString & _device);
  void setTitle(const QString & _numTitle,const QString &_numChapter);
private slots:
  void slotLengthChanged();
  void slotNewPosition(int _pos,const QTime & _time);
  void receivedStdout (KProcess *proc, char *buffer, int buflen);
  void wroteStdin(KProcess *_process);
  void timeout();
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          sliderReleased();
  virtual void          bPlayClick();
  virtual void          bStopClick();
  virtual void 		sliderPressed();
  virtual void		sliderChanged(int _value);
  virtual void 		open(k9DVD *_dvd,k9DVDTitle *_title,int chapter);
  virtual void		titleChanged();
  virtual void		cbAudioActivated (int _value);
  virtual void 		cbSubActivated (int _value);
  virtual void		bUpClick();
  virtual void 		bDownClick();
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void resizeEvent ( QResizeEvent * _resiseEvent);
protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

