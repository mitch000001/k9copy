//
// C++ Implementation: k9dvdread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvdread.h"


k9DVDRead::k9DVDRead(){
	m_dvd=NULL;
	files.setAutoDelete(true);
}


k9DVDRead::~k9DVDRead() {
	if (m_dvd !=NULL)
		close();
}

k9DVDFile::k9DVDFile(k9DVDRead *_dvd) {
	m_file=NULL;
	m_dvd=_dvd;
}

k9DVDFile::~k9DVDFile() {
	if (m_file != NULL)
		close();
}

/*!
    \fn k9DVDRead::openDevice(const QString & _device)
 */
void k9DVDRead::openDevice(const QString & _device) {
	m_dvd=DVDOpen(_device.latin1());
	//turn UDF cache off
	//DVDUDFCacheLevel(m_dvd, 0 );
}

QString k9DVDRead::getDiscId() {
uchar ID[17];
QString id="";
if (DVDDiscID(m_dvd,ID) !=-1) {
	ID[16]=0;
	id=QString::fromLatin1((const char*)ID);
}
return id;
}
/*!
    \fn k9DVDRead::close()
 */
void k9DVDRead::close()
{
	DVDClose(m_dvd);
	files.clear();
	m_dvd=NULL;
}

bool k9DVDRead::opened() {
	return (m_dvd !=NULL);
}


k9DVDFile * k9DVDRead::openIfo(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openIfo(_vts);
	files.append(file);
	return file;
}

k9DVDFile *k9DVDRead::openMenu(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openMenu(_vts);
	files.append(file);
	return file;
}

k9DVDFile *k9DVDRead::openTitle(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openTitle(_vts);
	files.append(file);
	return file;
}

/*!
    \fn k9DVDFile::openIfo(uint _vts)
 */
void k9DVDFile::openIfo(uint _vts) {
	m_file=DVDOpenFile(m_dvd->getDvd(),_vts,DVD_READ_INFO_FILE);
}


/*!
    \fn k9DVDFile::openMenu(uint _vts)
 */
void k9DVDFile::openMenu(uint _vts)
{
	m_file=DVDOpenFile(m_dvd->getDvd() ,_vts,DVD_READ_MENU_VOBS);
}


/*!
    \fn k9DVDFile::openTitle(uint _vts)
 */
void k9DVDFile::openTitle(uint _vts)
{
    	m_file=DVDOpenFile(m_dvd->getDvd(),_vts,DVD_READ_TITLE_VOBS);
}


/*!
    \fn k9DVDFile::close()
 */
void k9DVDFile::close()
{
    if (m_file !=NULL) {
	DVDCloseFile(m_file);
	m_file=NULL;
    }
}


/*!
    \fn k9DVDFile::read(uchar *_buffer,uint32_t _size)
 */
int k9DVDFile::readBytes(uchar *_buffer,uint32_t _size)
{
	if (m_file !=NULL)
		return  DVDReadBytes(m_file,_buffer,_size);
	else 
		return -1;
}

int k9DVDFile::readBlocks(uint32_t _sector,uint32_t _size,uchar*_buffer) {
	if (m_file !=NULL)  {
	   return DVDReadBlocks(m_file,_sector,_size,_buffer);
	}
	else 
		return -1;
}




