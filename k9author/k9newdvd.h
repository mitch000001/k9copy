//
// C++ Interface: k9newdvd
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9NEWDVD_H
#define K9NEWDVD_H

#include "k9common.h"
#include <qobject.h>
#include <qptrlist.h>
#include <qdom.h>
#include <qdatetime.h>
#include "k9process.h"
#include "k9progress.h"
#include "k9avidecode.h"


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Title;
class k9AviFile;
class k9Menu;
class k9ProcessList;
class k9NewDVDItems : public QPtrList<k9Title> {
protected:
    virtual int compareItems ( QPtrCollection::Item item1,QPtrCollection::Item item2 );
};

class k9NewDVD : public QObject {
    Q_OBJECT
public:
    enum eFormat {NTSC=2,PAL=1};
    k9NewDVD ( QObject *parent = 0, const char *name = 0 );
    ~k9NewDVD();
    k9NewDVDItems* getTitles() {
        return &m_titles;
    }
    eFormat m_format;

    void execute();
    void createXML();
    void addTitles ( QDomElement &_root );
    void setFormat ( const eFormat& _value );

    void setWorkDir ( const QString& _value );
    void appendTitle ( k9Title *_title );

    void setProcessList(k9ProcessList *_value);
    eFormat getFormat() const;

    int getTotalTime();
    k9Menu* getRootMenu() const;

    QString getError() const;

private:
    k9NewDVDItems m_titles;
    QStringList m_tmpFiles;
    QDomDocument *m_xml;
    QString m_workDir;
    k9ProcessList *m_processList;
    QTime m_timer;
    QTime m_timer2;
    QTime m_timer3;
    k9Menu *m_rootMenu;
    bool m_cancel;
    QString m_error;
    int m_videoBitrate;
    k9AviDecode m_aviDecode;
    int m_totalEncodedSize;
    int m_offset;
    int m_lastvalue;
    QMap <k9Process*,QTime> m_timers;
    void calcVideoBitrate();
    void createMencoderCmd ( QString &_cmd,QString &_chapters, k9AviFile *_aviFile );
protected slots:
    void getStdout ( KProcess *, char *, int );
    void getStderr ( KProcess *, char *, int );
    void drawImage ( QImage * _image );
signals:
    void sigAddTitle();
};

#endif
