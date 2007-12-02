/***************************************************************************
 *   Copyright (C) 2005 by Jean-Michel Petit                               *
 *   jm_petit@laposte.net                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "k9backupdlg.h"
#include "ac.h"
#include <qtimer.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include <kguiitem.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qlayout.h>
#include "k9drawimage.h"

k9BackupDlg::k9BackupDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
        : backupDlg(parent,name, modal,fl) {
    Abort=false;
    timer = new QTimer( this );
    time = new QTime(0,0);
    time->start();
    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
    timer->start(500, 0 );
    bAbort->setIconSet(KStdGuiItem::cancel().iconSet());
    bPlay->setIconSet(SmallIcon("player_play"));
    totalCopied=0;
    m_progressTotal=0;
    m_progressLabel="";
    m_totalSteps=0;
    m_factor="";
    m_progress=0;
    connect(&m_decoder, SIGNAL(pixmapReady(QImage *)), this, SLOT(drawPixmap(QImage *)));
    m_stop=false;
    m_playmovie=true;
    m_cpt=0;
    m_wimage=new k9DrawImage(image,0);
    QGridLayout *l=new QGridLayout(image,1,1);
    l->addWidget(m_wimage,0,0);
     m_data=NULL;
     m_dataSize=0;
}

void k9BackupDlg::drawPixmap(QImage *_image) {
	m_count++;
	if (m_count ==4) {
  
        m_wimage->setImage(*_image);

	m_stop=true;
	}
}

void k9BackupDlg::bPlayToggled( bool state) {
	m_playmovie=bPlay->isOn();
}

 void k9BackupDlg::playMovie(uchar *_data,uint32_t _size) {
	if (!m_playmovie || m_dataSize)
		return;
	m_count=0;
	m_dataSize=_size;
	m_data=(uchar*)malloc(_size);
	tc_memcpy(m_data,_data,_size);	
}


k9BackupDlg::~k9BackupDlg() {
    delete timer;
    delete time;
    if  (m_data)
        free(m_data);
}

void k9BackupDlg::update(){
    lblStep->setText(m_progressLabel);
    pbTotal->setProgress(m_progressTotal);
    pbStep->setTotalSteps(m_totalSteps);
    lblFactor->setText(m_factor);
    pbStep->setProgress(m_progress);

    if (m_data) {
	if (m_stop)
		m_decoder.start();
	m_stop=false;
	for (uint32_t i=0;i<m_dataSize-2048;i+=2048) {
		if (m_stop) {
			m_decoder.stop();
			break;
		}
		m_decoder.decode(m_data +i ,m_data+i+2048,0);
	}
	free(m_data);
	m_data=NULL;
	m_dataSize=0;
	
	m_playmovie=false;
     }

}

void k9BackupDlg::timerDone() {
   m_cpt++;
   if (m_cpt==10) {
	m_cpt=0;
	m_playmovie=bPlay->isOn();
   }
    QTime time2(0,0);
    time2=time2.addMSecs(time->elapsed());
    QString remain("--:--:--");
    if (m_progressTotal>0) {
	QTime time3(0,0);
	time3=time3.addMSecs((time->elapsed()/m_progressTotal)*pbTotal->totalSteps());
	remain=time3.toString("hh:mm:ss");
    }

    lblTime->setText(time2.toString("hh:mm:ss") +" / " +remain);
    update();
}

void k9BackupDlg::setTotalSteps(uint32_t _totalSteps) {
    m_totalSteps=_totalSteps;
}

void k9BackupDlg::setProgress(uint32_t _position) {
    m_progress=_position;
}

void k9BackupDlg::setTotalMax(uint32_t _max) {
    pbTotal->setTotalSteps(_max);
}

void k9BackupDlg::setProgressTotal(uint32_t _position) {
   totalCopied+=_position;
   uint64_t total=totalCopied*2048;
   total/=(1024*1024);
   m_progressTotal=total;

}

void k9BackupDlg::setProgressLabel(QString _text) {
	m_progressLabel=_text;
	update();
}

bool k9BackupDlg::getAbort() {
    return Abort;
}

void k9BackupDlg::bAbortClick() {
    Abort=true;
    reject();

}

void k9BackupDlg::setFactor(QString _factor) {
  m_factor=_factor;
}
/*$SPECIALIZATION$*/


#include "k9backupdlg.moc"
