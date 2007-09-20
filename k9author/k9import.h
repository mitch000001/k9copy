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

#ifndef K9IMPORT_H
#define K9IMPORT_H

#include "k9common.h"
#include <qlistview.h>
#include "k9copy.h"
#include "../k9author/import.h"
#include "../k9author/k9newdvd.h"


class k9AviFile;
class k9MenuButton;
class k9MenuEdit;
class k9Import : public import {
    Q_OBJECT
private:
    QListViewItem *m_root;
    k9NewDVD m_newDVD;
    KMdiToolViewAccessor *m_toolView;
    KDockWidget *m_dockWidget;
    k9Copy *m_parent;
    QPtrList <k9CdDrive> recorderList;
    k9CdDrives *drives;
    k9MenuEdit *m_menuEdit;

public:
    k9Import(QWidget* parent = 0, const char* name = 0,k9CdDrives *_drives=0 );
    ~k9Import();
    /*$PUBLIC_FUNCTIONS$*/
    void init();
    void execute();
    QListViewItem* getRoot() const;
    k9NewDVD *getNewDVD() {
        return &m_newDVD;
    }

public slots:
    /*$PUBLIC_SLOTS$*/
    void aviFileUpdated(k9AviFile *_aviFile);
    void buttonUpdated(k9MenuButton *_button, const QImage &_image);
    void setMenuEdit(k9MenuEdit* _value);
    void setEnableCreate(bool _state);
    void updateTotalTime();
    void titleAdded();
protected:
    /*$PROTECTED_FUNCTIONS$*/
    void setProgressWindow(QWidget *_widget);
    void removeProgressWindow();
    void readDrives();
    void addDrive (k9CdDrive *_drive);

protected slots:
    /*$PROTECTED_SLOTS$*/
    void lvDVDItemSelected (QListViewItem *);
    void cbFormatActivated(const QString &_format);
    virtual void deviceAdded(k9CdDrive *_drive);
    virtual void deviceRemoved(k9CdDrive *_drive);
    virtual void volumeChanged(const QString &device,const QString &volumeName);

signals:
    void aviFileSelected(k9AviFile *_aviFile);
    void titleSelected(k9Title *);
    void rootSelected(k9NewDVD *);
};

#endif

