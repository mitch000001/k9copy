//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9chapteredit.h"
#include "k9drawimage.h"
#include <ktimewidget.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qlayout.h>
#include <kurlrequester.h>
#include <qcheckbox.h>
#include "k9title.h"

k9ChapterEdit::k9ChapterEdit(QWidget* parent, const char* name, WFlags fl)
: chapterEdit(parent,name,fl)
{
  m_wimage=new k9DrawImage(image,0);
  QGridLayout *l=new QGridLayout(image,1,1);
  l->addWidget(m_wimage,0,0);
  m_wimage->setPaletteBackgroundColor(Qt::black);
  connect(&m_aviDecode,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));

}

k9ChapterEdit::~k9ChapterEdit()
{
}

/*$SPECIALIZATION$*/

void k9ChapterEdit::sliderReleased() {
 //  m_aviDecode.seek((double)slider->value() / 100);
 //  m_aviDecode.readFrame();
}
void k9ChapterEdit::sliderChanged() {
   if (m_aviDecode.opened()) {
        //m_aviDecode.seek((double)slider->value() / 100);
        m_aviDecode.readFrame((double)slider->value() / 1000);
        QTime t;
        t=t.addMSecs((double)slider->value());
        lPosition->setText(t.toString("hh:mm:ss"));
    }
}


void k9ChapterEdit::urlSelected(const QString& _fileName) {
  m_aviDecode.open(_fileName);
  if (!m_noUpdate) {
    if (_fileName !=m_aviFile->getFileName()) {
        ckBreakEnd->setChecked(false);
        ckBreakStart->setChecked(false);
        ckBreakEndClick();
        ckBreakStartClick();
    } 
    m_aviFile->setFileName(_fileName);
  }

  QTime t;
  int sec=t.msecsTo(m_aviFile->getStart());
  slider->setMaxValue((int)(m_aviDecode.getDuration()*1000));

  slider->setValue(sec);
  
}

void k9ChapterEdit::drawImage(QImage * _image) {
  //QPixmap p(*_image);
  //image->setPixmap(p);
  m_wimage->setImage(*_image);
  m_image=*_image;
}

void k9ChapterEdit::bStartClick() {
   QTime t;
   int secs=slider->value()/1000;
   t=t.addSecs(secs);
   twstart->setTime(t);
}

void k9ChapterEdit::bEndClick() {
   QTime t;
   int secs=slider->value()/1000;
   t=t.addSecs(secs);
   twend->setTime(t);
}

void k9ChapterEdit::setAviFile(k9AviFile *_aviFile) {
    this->setEnabled(_aviFile!=NULL);
    m_aviFile=_aviFile;
    if (_aviFile) {
        m_noUpdate=true;
        k9AviFile *p=m_aviFile->getPrevious();
        k9AviFile *n=m_aviFile->getNext();
        if (p) {
            if(p->getFileName() !=_aviFile->getFileName()) {
                ckBreakStart->setEnabled(false);
                m_minTime.setHMS(0,0,0,0);
            } else {
                ckBreakStart->setEnabled(true);
                m_minTime=p->getStart();
            }
        }
        if (n) {
            if(n->getFileName() !=_aviFile->getFileName()) {
                ckBreakEnd->setEnabled(false);
                m_maxTime=_aviFile->getEnd();
            } else {
                ckBreakEnd->setEnabled(true);
                m_maxTime=n->getEnd();
            }
        }


        twstart->setTime(m_aviFile->getStart());
        twend->setTime(m_aviFile->getEnd());
        urFile->setURL(m_aviFile->getFileName());
        urlSelected(m_aviFile->getFileName());
        ckBreakStart->setChecked(m_aviFile->getBreakPrevious());
        ckBreakEnd->setChecked(m_aviFile->getBreakNext());
    }
    m_noUpdate=false;

}

void k9ChapterEdit::startChanged(const QTime& _time) {
    if (!m_noUpdate) {
        QTime t=_time;
        if (t > m_maxTime )
            t=m_maxTime;
        else if (t < m_minTime)
            t=m_minTime;
        else if (t >m_aviFile->getEnd())
            t=m_aviFile->getEnd();
        if (t!=_time)
            twstart->setTime(t);
        m_aviFile->setStart(t);
        if (!m_aviFile->getBreakPrevious() && m_aviFile->getPrevious()!=NULL) {
            m_aviFile->getPrevious()->setEnd(t);
        }
        QTime t2;
        slider->setValue(t2.msecsTo(t));
        m_aviFile->setImage(m_image);
    }
}

void k9ChapterEdit::endChanged(const QTime& _time) {
    if (!m_noUpdate) {
        QTime t=_time;
        if (t > m_maxTime )
            t=m_maxTime;
        else if (t < m_minTime)
            t=m_minTime;
        else if (t <m_aviFile->getStart())
            t=m_aviFile->getStart();
        if (t!=_time)
            twend->setTime(t);        
        m_aviFile->setEnd(t);
        QTime t2;
        if (!m_aviFile->getBreakNext() && m_aviFile->getNext()!=NULL) {
            QImage i=m_image;
            m_aviDecode.readFrame((double)t2.msecsTo(t)/1000.0);
            m_aviFile->getNext()->setImage(m_image);
            m_image=i;
            m_aviFile->getNext()->setStart(t);
        }   
        slider->setValue(t2.msecsTo(t));
    }
}

void k9ChapterEdit::ckBreakStartClick() {
    m_aviFile->setBreakPrevious(ckBreakStart->isOn());  

}

void k9ChapterEdit::bTitleButtonClick() {
   m_aviFile->getTitle()->getButton()->setImage(m_image);
}

void k9ChapterEdit::ckBreakEndClick() {
    m_aviFile->setBreakNext(ckBreakEnd->isOn());
}

#include "k9chapteredit.moc"

