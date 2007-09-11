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


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Title;
class k9AviFile;
class k9Menu;
class k9MenuEdit;
class k9NewDVDItems : public QPtrList<k9Title> {
protected:
    virtual int compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2);
};

class k9NewDVD : public QObject {
    Q_OBJECT
public:
    enum eFormat {NTSC=2,PAL=1};
    k9NewDVD(QObject *parent = 0, const char *name = 0);
    ~k9NewDVD();
    k9NewDVDItems* getTitles() {
        return &m_titles;
    }
    eFormat m_format;

    void execute();
    void createXML();
    void addTitles (QDomElement &_root);
    void setFormat ( const eFormat& _value );

    void setWorkDir ( const QString& _value );
    void appendTitle(k9Title *_title);
    void setMenuEdit(k9MenuEdit* _value);
    k9MenuEdit* getMenuEdit() const;

    void setProgress(k9Progress* _value);

private:
    k9NewDVDItems m_titles;
    QStringList m_tmpFiles;
    QDomDocument *m_xml;
    QString m_workDir;
    k9Process *m_process;
    k9Progress *m_progress;
    k9MenuEdit *m_menuEdit;
    QTime m_timer;
    k9Menu *m_rootMenu;
    void createMencoderCmd(QString &_cmd,QString &_chapters, k9AviFile *_aviFile);

protected slots:
    void getStdout(KProcess *, char *, int);

};

#endif
