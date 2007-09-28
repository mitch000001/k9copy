//
// C++ Interface: k9process
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PROCESS_H
#define K9PROCESS_H

#include <kprocess.h>
#include <qdatetime.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Process : public KProcess
{
Q_OBJECT
public:
    k9Process(QObject *parent = 0, const char *name = 0);
    void sync();
    const QString & debug();
    ~k9Process();
    int getElapsed() const;
    virtual bool start (RunMode runmode=NotifyOnExit, Communication comm=NoCommunication);
private:
    bool m_waitSync;
    QString m_debug;
    QTime m_elapsed;
private slots:
   void slotProcessExited(KProcess *proc);
};

#endif
