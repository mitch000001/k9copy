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


#include "k9mencodercmdgen.h"
#include <qscrollview.h>
#include <qtabwidget.h>
#include <qfile.h>
#include <qdom.h>
#include <qlabel.h>
#include <knuminput.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qwidgetstack.h>
#include <klocale.h>
#include <qheader.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qtooltip.h>
#include <kstandarddirs.h>
#include <kapplication.h>
class _k9CheckListItem: public QCheckListItem {
public:
    _k9CheckListItem(QListViewItem *_item):QCheckListItem(_item,"",QCheckListItem::CheckBox) {};
    QString root;
    QDomDocument doc;
    int page;
    QFrame *frame;
    int rtti () const {return 1001;}
protected:
    void stateChange(bool _state);
};

void _k9CheckListItem::stateChange(bool _state) {
    QDomElement eRoot=doc.documentElement().elementsByTagName(root).item(0).toElement();
    QDomElement eCodec=eRoot.elementsByTagName(text()).item(0).toElement();
    eCodec.setAttribute("selected",_state?"true":"false");
}


k9MencoderCmdGen::k9MencoderCmdGen(QWidget* parent, const QString &_cmd, bool modal, WFlags fl)
: MyDialog1(parent,"", modal,fl)
{
    setCaption(kapp->makeStdCaption(i18n("MEncoder options")));
    m_row=0; m_page=1;
    loadXml();
    parseCmd(_cmd);

    fillListView();
}

void k9MencoderCmdGen::fillListView() {
    listView->clear();
    listView->header()->hide();
    QListViewItem *item=new QListViewItem(listView);
    item->setOpen(true);
    item->setText(0,i18n("Audio Codec"));
    QDomElement root=m_doc.documentElement().elementsByTagName("AUDIO").item(0).toElement();
    for (int i=0; i< root.childNodes().count();i++) {
        QDomElement eChild=root.childNodes().item(i).toElement();
        _k9CheckListItem *child=new _k9CheckListItem(item);
        child->setText(0,eChild.nodeName());
        child->root="AUDIO";
        child->doc=m_doc;
        QDomAttr attr=eChild.attributeNode("selected");
        if (!attr.isNull())
            child->setOn(attr.value()=="true");

        addWidgets(child, "AUDIO",eChild.nodeName());

    }
    item=new QListViewItem(listView);
    item->setText(0,i18n("Video Codec"));
    item->setOpen(true);
    root=m_doc.documentElement().elementsByTagName("VIDEO").item(0).toElement();
    for (int i=0; i< root.childNodes().count();i++) {
        QDomElement eChild=root.childNodes().item(i).toElement();
        _k9CheckListItem *child=new _k9CheckListItem(item);
        child->setText(0,eChild.nodeName());
        child->root="VIDEO";
        child->doc=m_doc;
        QDomAttr attr=eChild.attributeNode("selected");
        if (!attr.isNull())
            child->setOn(attr.value()=="true");

        addWidgets(child, "VIDEO",eChild.nodeName());


    }

}

k9MencoderCmdGen::~k9MencoderCmdGen()
{
}

/*$SPECIALIZATION$*/

void k9MencoderCmdGen::listViewCurrentChanged(QListViewItem *_item) {
    if (_item->parent() == NULL)
        wsOptions->raiseWidget(0);
    else if (_item->rtti()==1001) {
        _k9CheckListItem *item=(_k9CheckListItem*) _item;
        wsOptions->raiseWidget(item->page);
    

    }    
}

void k9MencoderCmdGen::loadXml() {
    QFile file(KGlobal::dirs()->findResource( "data", "k9copy/mencoder.xml"));
    if ( !file.open( IO_ReadOnly ) )
        return;
    if ( !m_doc.setContent( &file ) ) {
        file.close();
        return;
    }
    file.close();


 }

void k9MencoderCmdGen::addWidgets(_k9CheckListItem *_listItem,QString _root,QString _cat) {
    QVBox *vbox=new QVBox(wsOptions);
    wsOptions->addWidget(vbox,m_page);
    _listItem->page=m_page;
    m_page++;

    QFrame *fr=new QFrame(vbox);
    _listItem->frame=fr;

    
    QGridLayout *m_grid1=new QGridLayout(fr,1,1,0,0);
    m_scrollView=new QScrollView(fr,0);
    m_scrollView->viewport()->setPaletteBackgroundColor(this->paletteBackgroundColor());
    m_scrollView->setVScrollBarMode(QScrollView::AlwaysOn);
    m_grid1->addWidget(m_scrollView,0,0);

    m_grid=new QGrid(2,m_scrollView->viewport());
    m_grid->setSpacing(2);
    m_scrollView->addChild(m_grid);


   //finds the VIDEO node
    QDomElement eVideo=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    //gets the codec list
    QDomNodeList lCodecs=eVideo.elementsByTagName(_cat);
    for (int i=0; i<lCodecs.count();i++) {
        QDomElement eCodec=lCodecs.item(i).toElement();
        //gets codec options
        QDomNodeList lOptions=eCodec.elementsByTagName("opt");
        for (int j=0;j<lOptions.count();j++) {
            QDomElement eOpt=lOptions.item(j).toElement();
            QDomAttr aType=eOpt.attributeNode("type");
            if (aType.value() == "int")
                addInt(eOpt);
            else if(aType.value()=="float")
                addFloat(eOpt);
            else if (aType.value()=="bool")
                addBool(eOpt);
            else if (aType.value()=="string")
                addString(eOpt);
            else if (aType.value()=="")
                addList(eOpt);
        }
    }
}

void k9MencoderCmdGen::addList(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool bSel=false;
   QDomAttr aSel=_eOpt.attributeNode("selected");
   if (!aSel.isNull())
       bSel=aSel.value()=="true";


   QCheckBox *ckLabel=new QCheckBox(sName,m_grid,QString("ck%1").arg(m_row++) );
   ckLabel->setChecked(bSel);
   m_hbox=new QHBox(m_grid);
   if ( !_eOpt.attributeNode("description").isNull()) 
        QToolTip::add(ckLabel,_eOpt.attributeNode("description").value());


    QDomNodeList lOption=_eOpt.elementsByTagName("subopt");
    for (int i=0; i<lOption.count();i++) {
        QDomElement eSubOpt=lOption.item(i).toElement();
        QDomAttr aType=eSubOpt.attributeNode("type");
        if (aType.value() == "int")
            addInt(eSubOpt);
        else if(aType.value()=="float")
            addFloat(eSubOpt);
        else if (aType.value()=="bool")
            addBool(eSubOpt);
        else if (aType.value()=="string")
            addString(eSubOpt);
    }

  _eOpt.setAttribute("checkbox",ckLabel->name());

}


void k9MencoderCmdGen::addInt(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool blist=false;
   if (sName=="") {
        sName=_eOpt.parentNode().nodeName();
        blist=true;
    }

   int iMin=_eOpt.attributeNode("min").value().toInt();
   int iMax=_eOpt.attributeNode("max").value().toInt();
   int iDefault=_eOpt.attributeNode("default").value().toInt();

   QWidget *parent;
   if (!blist) {
       bool bSel=false;
       QDomAttr aSel=_eOpt.attributeNode("selected");
       if (!aSel.isNull())
           bSel=aSel.value()=="true";

       QCheckBox *ckLabel=new QCheckBox(sName,m_grid,QString("ck%1").arg(m_row++) );
       ckLabel->setChecked(bSel);

       if ( !_eOpt.attributeNode("description").isNull()) 
           QToolTip::add(ckLabel,_eOpt.attributeNode("description").value());
      _eOpt.setAttribute("checkbox",ckLabel->name());
       parent=m_grid;
   } else
       parent=m_hbox;
   KIntSpinBox *sb= new KIntSpinBox(iMin,iMax,1,iDefault,10,parent,QString("int%1").arg(m_row++));
   if (iMax <1000)
        sb->setFixedWidth(50);
    else   
        sb->setFixedWidth(100);
    if( !_eOpt.attributeNode("special").isNull()){
        sb->setSpecialValueText(_eOpt.attributeNode("special").value());
    }

   _eOpt.setAttribute("widget",sb->name());

   //m_grid->addWidget(label,m_row++,0);
}
void k9MencoderCmdGen::addFloat(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   double dMin=_eOpt.attributeNode("min").value().toDouble();
   double dMax=_eOpt.attributeNode("max").value().toDouble();
   double dDefault=_eOpt.attributeNode("default").value().toDouble();
   bool bSel=false;
   QDomAttr aSel=_eOpt.attributeNode("selected");
   if (!aSel.isNull())
       bSel=aSel.value()=="true";

   QCheckBox *ckLabel=new QCheckBox(sName,m_grid,QString("ck%1").arg(m_row++));
   ckLabel->setChecked(bSel);
   if ( !_eOpt.attributeNode("description").isNull()) 
        QToolTip::add(ckLabel,_eOpt.attributeNode("description").value());

   KDoubleSpinBox *sb= new KDoubleSpinBox(dMin,dMax,0.01,dDefault,2,m_grid,QString("float%1").arg(m_row++));
   if (dMax <1000)
        sb->setFixedWidth(80);
    else   
        sb->setFixedWidth(120);
    if( !_eOpt.attributeNode("special").isNull()){
        sb->setSpecialValueText(_eOpt.attributeNode("special").value());
    }
   
   _eOpt.setAttribute("widget",sb->name());
   _eOpt.setAttribute("checkbox",ckLabel->name());

}

void k9MencoderCmdGen::addBool(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool bSel=false;
   QDomAttr aSel=_eOpt.attributeNode("selected");
   if (!aSel.isNull())
       bSel=aSel.value()=="true";

   QCheckBox *ckLabel=new QCheckBox(sName,m_grid,QString("ck%1").arg(m_row++));
   //QCheckBox *ck= new QCheckBox("selected",m_grid,QString("bool%1").arg(m_row++));
   if ( !_eOpt.attributeNode("description").isNull()) 
        QToolTip::add(ckLabel,_eOpt.attributeNode("description").value());

   QWidget *w=new QWidget(m_grid);
    if ((_eOpt.attributeNode("default").value()=="true") && bSel)
        ckLabel->setChecked(true);   


//   _eOpt.setAttribute("widget",ck->name());
   _eOpt.setAttribute("checkbox",ckLabel->name());

}

void k9MencoderCmdGen::addString(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool blist=false;
   if (sName=="") {
        blist=true;
        sName=_eOpt.parentNode().nodeName();
   }
    
   QString sDefault=_eOpt.attributeNode("default").value();
   QWidget *parent;
   if( !blist) {
        bool bSel=false;
        QDomAttr aSel=_eOpt.attributeNode("selected");
        if (!aSel.isNull())
            bSel=aSel.value()=="true";
        
        QCheckBox *ckLabel=new QCheckBox(sName,m_grid,QString("ck%1").arg(m_row++));
        ckLabel->setChecked(bSel);
        if ( !_eOpt.attributeNode("description").isNull()) 
                QToolTip::add(ckLabel,_eOpt.attributeNode("description").value());
        parent=m_grid;
       _eOpt.setAttribute("checkbox",ckLabel->name());
   } else
        parent=m_hbox;

   QComboBox *cb=new QComboBox(parent,QString("string%1").arg(m_row++));
   
   QDomNodeList values=_eOpt.elementsByTagName("value");
   int def=0;
   if (values.count()==0)
        cb->setEditable(true);

   for (int i=0;i<values.count();i++) {
        QDomElement e=values.item(i).toElement();
        cb->insertItem(e.attributeNode("name").value());
        if (e.attributeNode("name").value() ==sDefault)
            def=cb->count()-1;
    }
    cb->setCurrentItem(def);
   _eOpt.setAttribute("widget",cb->name());

}

const QString & k9MencoderCmdGen::getCmd(const QString &_root) {
    QString str;
    QTextStream ts(&str,IO_WriteOnly);
    m_doc.save(ts,4);
    QFile f ("/home/jmp/doc.xml");
    f.open(IO_WriteOnly);
    f.writeBlock(str.latin1(),str.length());
    f.close();


    m_cmd="";
    //finds the VIDEO node
    QDomElement eRoot=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    //gets the codec list
    QDomNodeList lCodecs=eRoot.childNodes();
    for (int i=0; i<lCodecs.count();i++) {
        QString sCmd="",sCmd1="";
        QDomElement eCodec=lCodecs.item(i).toElement();
        QDomAttr eAttr=eCodec.attributeNode("selected");
        if (eAttr.isNull())
            continue;
        if (eAttr.value()=="false")
            continue;

        //gets codec options
        QDomNodeList lOptions=eCodec.elementsByTagName("opt");
        for (int j=0;j<lOptions.count();j++) {
            QDomElement eOpt=lOptions.item(j).toElement();
            QDomAttr aType=eOpt.attributeNode("type");  
            QDomAttr aCheckBox=eOpt.attributeNode("checkbox");
            QDomAttr aWidget=eOpt.attributeNode("widget");
            QCheckBox *ck=(QCheckBox*)this->child(aCheckBox.value().latin1());
            qDebug(eOpt.attributeNode("name").value()+" : "+ck->name());
            if (ck->isChecked()) {
                if (sCmd1.isEmpty())
                    sCmd1=eCodec.tagName()+" -"+eCodec.attributeNode("options").value()+" ";
                if(aType.isNull()) {
                    //build list
                    QDomNodeList lSubOpt=eOpt.elementsByTagName("subopt");
                    QString sCmd2="";
                    for (int k=0;k<lSubOpt.count();k++) {
                        QDomElement eSOpt=lSubOpt.item(k).toElement();
                        QDomAttr aSType=eSOpt.attributeNode("type");  
                        QDomAttr aSWidget=eSOpt.attributeNode("widget");
                        qDebug(aSWidget.value());
                        if(aSType.value()=="int") {
                            KIntSpinBox *isb=(KIntSpinBox*) this->child(aSWidget.value().latin1(),"KIntSpinBox");
                            if (!sCmd2.isEmpty() && isb->text()!="")
                                sCmd2 +=",";
                            sCmd2+= isb->text();      
                        } else if(aSType.value()=="string") {
                            QComboBox *cb=(QComboBox*) this->child(aSWidget.value().latin1(),"QComboBox");
                            if (!sCmd2.isEmpty() && cb->currentText()!="")
                                sCmd2 +=",";
                           // sCmd2+= cb->text(cb->currentItem());
                            sCmd2+= cb->currentText();
                        }
                    }
                    if (!sCmd.isEmpty())
                        sCmd +=":";
                    sCmd+= eOpt.attributeNode("name").value() + "=" +sCmd2;
                }else if(aType.value()=="int") {
                    KIntSpinBox *isb=(KIntSpinBox*) this->child(aWidget.value().latin1(),"KIntSpinBox");
                    if (!sCmd.isEmpty())
                        sCmd +=":";
                    sCmd+= eOpt.attributeNode("name").value() + "=" + isb->text();
                } else if(aType.value()=="float") {
                    KDoubleSpinBox *isb=(KDoubleSpinBox*) this->child(aWidget.value().latin1(),"KDoubleSpinBox");
                    if (!sCmd.isEmpty())
                        sCmd +=":";
                    sCmd+= eOpt.attributeNode("name").value() + "=" + isb->text().replace(",",".");
                } else if(aType.value()=="string") {
                    QComboBox *cb=(QComboBox*) this->child(aWidget.value().latin1(),"QComboBox");
                    if (!sCmd.isEmpty())
                        sCmd +=":";
                    sCmd+= eOpt.attributeNode("name").value() + "=" + cb->text(cb->currentItem());
                } else if(aType.value()=="bool") {
                    if (!sCmd.isEmpty()) 
                            sCmd +=":";
                    sCmd+=eOpt.attributeNode("name").value() ;
                }
            }
        }
        m_cmd+=sCmd1+sCmd;

    }
    
    return m_cmd;
}

bool k9MencoderCmdGen::getMencoderOptions(QString &_cmd) {
    k9MencoderCmdGen * m_cmdGen=new k9MencoderCmdGen(NULL,_cmd);
    m_cmdGen->m_cmd=_cmd;
    bool res=m_cmdGen->exec();
    QString m_video=m_cmdGen->getCmd("VIDEO").stripWhiteSpace();
    QString m_audio=m_cmdGen->getCmd("AUDIO").stripWhiteSpace();
    if (!m_video.isEmpty())
        m_video="-ovc "+m_video;
    if (!m_audio.isEmpty())
        m_audio="-oac " +m_audio;
    _cmd=m_video+m_audio;
    qDebug(_cmd);

    delete m_cmdGen;
    return res;
}

void k9MencoderCmdGen::parseCmd(const QString &_cmd){
    QStringList cmdList=QStringList::split(" ",_cmd);
    for (QStringList::iterator it=cmdList.begin();it!=cmdList.end();++it) {
        if (*it=="-ovc") {
            ++it;
            selectCodec("VIDEO",*it);            
        } else if (*it=="-oac") {
            ++it;
            selectCodec("AUDIO",*it);            
        } else { //if (*it=="-lavcopts") {
            QDomNodeList lRoot=m_doc.documentElement().childNodes();
            bool bFound=false;
            QString sCodec=*it;
            for (int i=0;i<lRoot.count();i++) {
                QDomElement eRoot=lRoot.item(i).toElement();
                QDomNodeList lCodec=eRoot.childNodes();
                for (int j=0;j<lCodec.count();j++) {
                    QDomElement eCodec=lCodec.item(j).toElement();
                    if ( ("-"+eCodec.attributeNode("options").value()) ==sCodec) {
                        if (!bFound)
                            ++it;
                        parseCodecOptions(eRoot.nodeName(),eCodec.nodeName(),*it);
                        bFound=true;        
                    }
                }

            }
        }
    }
}

void k9MencoderCmdGen::parseCodecOptions(const QString &_root,const QString &_codec,const QString & _options) {
    QStringList slOptions=QStringList::split(":",_options);
    QDomElement eRoot=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    QDomElement eCodec=eRoot.elementsByTagName(_codec).item(0).toElement();
    QDomNodeList lOpt=eCodec.elementsByTagName("opt");
    for (int i=0;i <lOpt.count();i++) {
        QDomElement eOpt=lOpt.item(i).toElement();
        QString sName=eOpt.attributeNode("name").value();
        //extract the option (optname=value)
        QStringList sl=slOptions.grep(sName,false);
        if (sl.count()>0) {
            QString sOpt=*(sl.at(0));
            QString value;
            if (eOpt.attributeNode("type").isNull()) {
                //it's a list of values
                QString sValues=sOpt.section("=",1,1);
                QStringList slValues=QStringList::split(",",sValues);
                //loop on values nodes
                QDomNodeList lSOpt=eOpt.elementsByTagName("subopt");
                for (int j=0;j<lSOpt.count();j++) {
                    QDomElement eSOpt=lSOpt.item(j).toElement();
                    if (eSOpt.attributeNode("type").value()=="int") {
                        if (j< slValues.count()) 
                            eSOpt.setAttribute("default",*(slValues.at(j)));
                    } else if (eSOpt.attributeNode("type").value()=="string") {
                        QDomNodeList lValues=eSOpt.elementsByTagName("value");
                        for (int k=0;k<lValues.count();k++) {
                            QDomElement eValue=lValues.item(k).toElement();
                            int pos=slValues.findIndex(eValue.attributeNode("name").value());
                            if (pos!=-1) {
                                eSOpt.setAttribute("default",eValue.attributeNode("name").value());
                                slValues.remove(slValues.at(pos));
                            }   
                        }
                    }
                }
            } else if (eOpt.attributeNode("type").value()=="bool" )
                value="true";
            else
                value=sOpt.section("=",1,1);
            eOpt.setAttribute("default",value);
            eOpt.setAttribute("selected","true");
        }
    }
}

void k9MencoderCmdGen::selectCodec(const QString &_root,const QString &_codec) {
    QDomElement root=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    QDomElement codec=root.elementsByTagName(_codec).item(0).toElement();
    if (!codec.isNull())
        codec.setAttribute("selected","true");
}


#include "k9mencodercmdgen.moc"

