//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9PROCESSLIST_H
#define K9PROCESSLIST_H
#include "k9common.h"
#include "../libk9copy/processList.h"
#include "k9process.h"
#include <qmap.h>
#include <qtimer.h>
class k9ProcessList : public processList {
    Q_OBJECT

public:
    k9ProcessList(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~k9ProcessList();
    /*$PUBLIC_FUNCTIONS$*/
    void execute();
    void addProgress(const QString &_text);
    void setProgress (k9Process * _process,int _position, int _total);
    void setText(k9Process *_process, const QString &_text,int _col);
    void setFileName(k9Process *_process,const QString &_fileName);
    void setPos(k9Process *_process,double _pos);
    k9Process *addProcess(const QString &label);

    void setMaxProcess(int _value);
    int getMaxProcess() const;
    bool getCancel() const;
    void clear();
public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    int m_maxProcess;
    QMap <k9Process*,QListViewItem*> m_items;
    QPtrList <k9Process> m_processes;
    QTimer m_timer;
    bool m_cancel;
    bool m_waitSync;
    int getNbRunning();
    void wait(int _msec);

protected slots:
    /*$PROTECTED_SLOTS$*/
    void timeout();
    virtual void bCancelClick();
};

#endif

