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


#include "k9processlist.h"
#include <qstyle.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qrect.h>
#include <qprogressbar.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <kpushbutton.h>

class _k9ProcessListItem : public QListViewItem {
public:
    _k9ProcessListItem(QListView *_listview,const QString & _text):QListViewItem(_listview,_text) {
        m_fileName="";
        m_pos=0;
        m_num=_listview->childCount();
    }
    void setFileName(const QString &_fileName) { m_fileName=_fileName;}
    void setPos(double _pos) { m_pos=_pos;}

    QString getFileName() const { return m_fileName;}

    double getPos() const { return m_pos;}
    int compare ( QListViewItem * i, int col, bool ascending ) const {
        return m_num-((_k9ProcessListItem*)i)->m_num;
    }
private:
    QString m_fileName;
    double m_pos;
    int m_num;
};


k9ProcessList::k9ProcessList(QWidget* parent, const char* name, WFlags fl) : processList(parent,name,fl)
{
    m_processes.setAutoDelete(true);
    m_maxProcess=4;  
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    bCancel->setEnabled(false);
}

k9ProcessList::~k9ProcessList()
{
}
/*$SPECIALIZATION$*/
void k9ProcessList::timeout() {
   if (m_waitSync) {
   	QApplication::eventLoop()->exitLoop();
   	m_waitSync=false;	
   }
}

void k9ProcessList::bCancelClick() {
    m_cancel=true;
    for (k9Process *p=m_processes.first();p;p=m_processes.next()) {
        if (p->isRunning()) 
                p->kill();
    }

}

void k9ProcessList::wait(int _msec) {
  m_waitSync=true;
  m_timer.start(_msec,true);
  QApplication::eventLoop()->enterLoop();
}

int k9ProcessList::getNbRunning() {
    int res=0;
    for (int i=0;i<m_processes.count();i++) {
        k9Process *p=m_processes.at(i);
        if (p->isRunning())
            res++;
    }
    qDebug(QString("%1 process en cours").arg(res));
    return res;
}


void k9ProcessList::execute() {
    bCancel->setEnabled(true);
    m_cancel=false;
    m_error=false;
    k9Process *p=NULL;
    for (QPtrListStdIterator <k9Process> it=m_processes.begin() ;it!=m_processes.end() ;++it ) {
        p=(*it);
        while (getNbRunning() >=m_maxProcess && ! m_cancel) {
            wait(1000);
        }
        if (m_cancel)
            break;
        if (!p->start(KProcess::OwnGroup,KProcess::All))
            m_error=true;
     }
     //waiting for processes
        for (p=m_processes.first();p;p=m_processes.next()) {
            if (p->isRunning()) 
                    p->sync();
        }
    bCancel->setEnabled(false);
}

void k9ProcessList::addProgress(const QString &_text) {
    QListViewItem *item = new _k9ProcessListItem(lProcess,_text);
    QProgressBar b(this);
    b.setProgress(100,100);
    b.resize(100,40);
    item->setPixmap(0,QPixmap::grabWidget(&b,0,0,b.width(),b.height()));
}

void k9ProcessList::setProgress (k9Process * _process,int _position, int _total) {
    QProgressBar b(this);
    b.setProgress(_position,_total);
    b.resize(100,40);
    QListViewItem *it =m_items[_process];
    it->setPixmap(0,QPixmap::grabWidget(&b,0,0,b.width(),b.height()));
}

void k9ProcessList::setText(k9Process *_process, const QString &_text,int _col) {
    QListViewItem *it =m_items[_process];
    it->setText(_col,_text);
}

void k9ProcessList::setFileName(k9Process *_process,const QString &_fileName) {
    _k9ProcessListItem *it = (_k9ProcessListItem*)m_items[_process];
    it->setFileName(_fileName);

}

void k9ProcessList::setPos(k9Process *_process,double _pos) {
    _k9ProcessListItem *it = (_k9ProcessListItem*)m_items[_process];
    it->setPos(_pos);
}

k9Process *k9ProcessList::addProcess(const QString &label) {
    QString name=QString("process%1").arg(m_items.count()) ;
    k9Process *process=new k9Process(this,name.latin1());
    m_processes.append(process);
    QListViewItem *item = new _k9ProcessListItem(lProcess,label);
    m_items[process]=item;
    setProgress(process,0,100);
    connect(process,SIGNAL(processExited( KProcess* )),this,SLOT(processExited(KProcess*)));
    return process;
}

void k9ProcessList::processExited(KProcess *_process){
    if (!_process->normalExit())
        m_cancel=true;
    else if (_process->exitStatus() !=0 ) 
        m_error=true;
        
}

#include "k9processlist.moc"

void k9ProcessList::clear() {
    m_processes.clear();
    m_items.clear();
}

int k9ProcessList::getMaxProcess() const {
    return m_maxProcess;
}


void k9ProcessList::setMaxProcess(int _value) {
    m_maxProcess = _value;
}


bool k9ProcessList::getCancel() const {
    return m_cancel;
}


bool k9ProcessList::getError() const {
    return m_error;
}
