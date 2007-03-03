/*
// C++ Interface: dvdread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
*/
#ifndef DVDREAD_H_INCLUDED
#define DVDREAD_H_INCLUDED



#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


/*** dvd_reader  ***/
typedef dvd_reader_t * (*DVDOpen_t) ( const char * );
typedef void (*DVDClose_t)( dvd_reader_t * );
typedef dvd_file_t * (*DVDOpenFile_t) ( dvd_reader_t *, int, dvd_read_domain_t );
typedef void (*DVDCloseFile_t) ( dvd_file_t * );
typedef ssize_t (*DVDReadBlocks_t) ( dvd_file_t *, int, size_t, unsigned char * );
typedef int (*DVDFileSeek_t)( dvd_file_t *, int );
typedef ssize_t (*DVDReadBytes_t) ( dvd_file_t *, void *, size_t );
typedef ssize_t (*DVDFileSize_t)( dvd_file_t * );
typedef int (*DVDDiscID_t)( dvd_reader_t *, unsigned char * );
typedef int (*DVDUDFVolumeInfo_t)( dvd_reader_t *, char *, unsigned int, unsigned char *, unsigned int );
typedef int (*DVDISOVolumeInfo_t)( dvd_reader_t *, char *, unsigned int,unsigned char *, unsigned int );
typedef int (*DVDUDFCacheLevel_t)( dvd_reader_t *, int );

/*** nav_read ***/
typedef void (*navRead_PCI_t)(pci_t *, unsigned char *);
typedef void (*navRead_DSI_t)(dsi_t *, unsigned char *);

/*** ifo_read ***/

typedef ifo_handle_t * (*ifoOpen_t)(dvd_reader_t *, int );
typedef ifo_handle_t * (*ifoOpenVMGI_t)(dvd_reader_t *);
typedef ifo_handle_t * (*ifoOpenVTSI_t)(dvd_reader_t *, int);
typedef void (*ifoClose_t)(ifo_handle_t *);
typedef int (*ifoRead_PTL_MAIT_t)(ifo_handle_t *);
typedef int (*ifoRead_VTS_ATRT_t)(ifo_handle_t *);
typedef int (*ifoRead_TT_SRPT_t)(ifo_handle_t *);
typedef int (*ifoRead_VTS_PTT_SRPT_t)(ifo_handle_t *);
typedef int (*ifoRead_FP_PGC_t)(ifo_handle_t *);
typedef int (*ifoRead_PGCIT_t)(ifo_handle_t *);
typedef int (*ifoRead_PGCI_UT_t)(ifo_handle_t *);
typedef int (*ifoRead_VTS_TMAPT_t)(ifo_handle_t *);
typedef int (*ifoRead_C_ADT_t)(ifo_handle_t *);
typedef int (*ifoRead_TITLE_C_ADT_t)(ifo_handle_t *);
typedef int (*ifoRead_VOBU_ADMAP_t)(ifo_handle_t *);
typedef int (*ifoRead_TITLE_VOBU_ADMAP_t)(ifo_handle_t *);
typedef int (*ifoRead_TXTDT_MGI_t)(ifo_handle_t *);

typedef void (*ifoFree_PTL_MAIT_t)(ifo_handle_t *);
typedef void (*ifoFree_VTS_ATRT_t)(ifo_handle_t *);
typedef void (*ifoFree_TT_SRPT_t)(ifo_handle_t *);
typedef void (*ifoFree_VTS_PTT_SRPT_t)(ifo_handle_t *);
typedef void (*ifoFree_FP_PGC_t)(ifo_handle_t *);
typedef void (*ifoFree_PGCIT_t)(ifo_handle_t *);
typedef void (*ifoFree_PGCI_UT_t)(ifo_handle_t *);
typedef void (*ifoFree_VTS_TMAPT_t)(ifo_handle_t *);
typedef void (*ifoFree_C_ADT_t)(ifo_handle_t *);
typedef void (*ifoFree_TITLE_C_ADT_t)(ifo_handle_t *);
typedef void (*ifoFree_VOBU_ADMAP_t)(ifo_handle_t *);
typedef void (*ifoFree_TITLE_VOBU_ADMAP_t)(ifo_handle_t *);
typedef void (*ifoFree_TXTDT_MGI_t)(ifo_handle_t *);


typedef struct {
	DVDOpen_t 	DVDOpen;
	DVDClose_t 	DVDClose;
	DVDOpenFile_t	DVDOpenFile;
	DVDCloseFile_t 	DVDCloseFile;
	DVDReadBlocks_t DVDReadBlocks;
	DVDFileSeek_t	DVDFileSeek;
	DVDReadBytes_t	DVDReadBytes;
	DVDFileSize_t	DVDFileSize;
	DVDDiscID_t	DVDDiscID;
	DVDUDFVolumeInfo_t DVDUDFVolumeInfo;
	DVDISOVolumeInfo_t DVDISOVolumeInfo;
	DVDUDFCacheLevel_t DVDUDFCacheLevel;
	navRead_PCI_t	navRead_PCI;
	navRead_DSI_t	navRead_DSI;
	
	ifoOpen_t 	ifoOpen;
	ifoOpenVMGI_t 	ifoOpenVMGI;
	ifoOpenVTSI_t 	ifoOpenVTSI;
	ifoClose_t 	ifoClose;
	ifoRead_PTL_MAIT_t ifoRead_PTL_MAIT;
	ifoRead_VTS_ATRT_t ifoRead_VTS_ATRT;
	ifoRead_TT_SRPT_t  ifoRead_TT_SRPT;
	ifoRead_VTS_PTT_SRPT_t ifoRead_VTS_PTT_SRPT;
	ifoRead_FP_PGC_t ifoRead_FP_PGC;
	ifoRead_PGCIT_t ifoRead_PGCIT;
	ifoRead_PGCI_UT_t ifoRead_PGCI_UT;
	ifoRead_VTS_TMAPT_t ifoRead_VTS_TMAPT;
	ifoRead_C_ADT_t ifoRead_C_ADT;
	ifoRead_TITLE_C_ADT_t ifoRead_TITLE_C_ADT;
	ifoRead_VOBU_ADMAP_t ifoRead_VOBU_ADMAP;
	ifoRead_TITLE_VOBU_ADMAP_t ifoRead_TITLE_VOBU_ADMAP;
	ifoRead_TXTDT_MGI_t ifoRead_TXTDT_MGI;
	
	ifoFree_PTL_MAIT_t ifoFree_PTL_MAIT;
	ifoFree_VTS_ATRT_t ifoFree_VTS_ATRT;
	ifoFree_TT_SRPT_t ifoFree_TT_SRPT;
	ifoFree_VTS_PTT_SRPT_t ifoFree_VTS_PTT_SRPT;
	ifoFree_FP_PGC_t ifoFree_FP_PGC;
	ifoFree_PGCIT_t ifoFree_PGCIT;
	ifoFree_PGCI_UT_t ifoFree_PGCI_UT;
 	ifoFree_VTS_TMAPT_t  ifoFree_VTS_TMAPT;
	ifoFree_C_ADT_t ifoFree_C_ADT;
	ifoFree_TITLE_C_ADT_t ifoFree_TITLE_C_ADT;
	ifoFree_VOBU_ADMAP_t ifoFree_VOBU_ADMAP;
	ifoFree_TITLE_VOBU_ADMAP_t ifoFree_TITLE_VOBU_ADMAP;
	ifoFree_TXTDT_MGI_t ifoFree_TXTDT_MGI;
	
	
} DvdreadFunctions_t;

DvdreadFunctions_t *DvdreadF(void);
void loadDvdread(void);
void unloadDvdread(void);
#ifdef __cplusplus
}
#endif
#endif
