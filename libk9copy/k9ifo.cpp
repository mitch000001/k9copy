/***************************************************************************
 *   Copyright (C) 2005 by Jean-Michel Petit                               *
 *   jm_petit@laposte.net                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "k9ifo.h"
#include "bswap.h"


k9Ifo::k9Ifo(k9DVDRead *_dvdread)
        : QObject(NULL,"") {
    _ifo=NULL;
    m_dvd=_dvdread;
}


k9Ifo::~k9Ifo() {
    closeIFO();


}


#include "k9ifo.moc"

/*!
    \fn k9Ifo::setDevice(QString &_device)
 */
void k9Ifo::setDevice(QString &_device) {
    device=_device;
}


/*!
    \fn k9Ifo::setOutput(QString &_output)
 */
void k9Ifo::setOutput(QString &_output) {
    output=_output;
}


/*!
    \fn k9Ifo::openIFO(int _num)
 */
ifo_handle_t * k9Ifo::openIFO(int _num) {
    _ifo = ifoOpen(m_dvd->getDvd(), _num);
    numIfo=_num;
    return _ifo;

}


/*!
    \fn k9Ifo::IFOClose( ifo_handle_t* _ifo)
 */
void k9Ifo::closeIFO() {
    if(_ifo!=NULL) {
        ifoClose(_ifo);
        _ifo=NULL;
    }
}

ifo_handle_t * k9Ifo::getIFO() {
    return _ifo;
}
/*!
    \fn k9Ifo::saveIFO(ifo_handle_t* _ifo)
 */
void k9Ifo::saveIFO() {
    uint32_t size;
    /* DVD handler */
    k9DVDFile *dvdfile;
    bool mainIfo =(numIfo==0);

    QString filename,filename2;
    if (mainIfo) {
        filename="/VIDEO_TS/VIDEO_TS.IFO";
    } else {
        filename.sprintf("/VIDEO_TS/VTS_%02d_0.IFO",numIfo);
    }
    if (numIfo==0) 
	size=_ifo->vmgi_mat->vmgi_last_sector+1;
    else
	size=_ifo->vtsi_mat->vtsi_last_sector +1;

    size*=2048;
 //   if (UDFFindFile(dvd,(char*) filename.latin1(), &size)) {
    if (size >0) {
        uchar *buffer ;
        buffer= (uchar *) malloc (size);
        bufCopy=(uchar *) malloc (size);
        memset(buffer,0,size);
        //Lecture du fichier IFO original et sauvegarde dans buffer
        if ((dvdfile = m_dvd->openIfo(  numIfo))== 0) {
            fprintf(stderr, "Failed opening IFO for titleset %d\n", 0);
            free(buffer);
            return;
        }

        /*	 if(DVDFileSeek(dvd_file, 0)!=0) {
            	return;
          	}
        */

        if ( dvdfile->readBytes(buffer,size) != size) {
            fprintf(stderr, "Error reading IFO for titleset %d\n", 0);
            free(buffer);
            dvdfile->close();
            return;
        }

        memcpy(bufCopy,buffer,size);
        bufCopySize=size;
        dvdfile->close();
        if (mainIfo) {

            checkBuffer("",buffer);
            updateVMG(buffer);
            checkBuffer("updateVMG",buffer);
            updateFP_PGC(buffer);
            checkBuffer("updateFP_PGC",buffer);

            updateTT_SRPT(buffer);
            checkBuffer("updateTT_SRPT",buffer);
            updatePGCI_UT(buffer);
            checkBuffer("updatePGCI_UT",buffer);
            updatePTL_MAIT(buffer);
            checkBuffer("updatePTL_MAIT",buffer);

            updateVTS_ATRT(buffer);
            checkBuffer("updateVTS_ATRT",buffer);
            updateTXTDT_MGI(buffer);
            checkBuffer("updateTXTDT_MGI",buffer);
            updateC_ADT(buffer);
            checkBuffer("updateC_ADT",buffer);
            updateVOBU_ADMAP(buffer);
            checkBuffer("updateVOBU_ADMAP",buffer);
        } else {
            updateVTS(buffer);
            checkBuffer("updateVTS",buffer);

            updateVTS_PTT_SRPT(buffer);
            checkBuffer("updateVTS_PTT_SRPT",buffer);

            updatePGCIT(buffer);
            checkBuffer("updatePGCIT",buffer);

            updatePGCI_UT(buffer);
            checkBuffer("updatePGCI_UT",buffer);

            updateVTS_TMAPT(buffer);
            checkBuffer("updateVTS_TMAPT",buffer);

            updateC_ADT(buffer);
            checkBuffer("updateC_ADT",buffer);

            updateVOBU_ADMAP(buffer);
            checkBuffer("updateVOBU_ADMAP",buffer);

        }

        //sauvegarder buffer dans fichier
        if (mainIfo) {
            filename=output+ "/VIDEO_TS.IFO";
            filename2=output+ "/VIDEO_TS.BUP";
        } else {
            filename.sprintf("/VTS_%02d_0.IFO",numIfo);
            filename=output + filename;

            filename2.sprintf("/VTS_%02d_0.BUP",numIfo);
            filename2=output + filename2;
        }

        QFile ifofile (filename);
        ifofile.open(IO_WriteOnly);
        if (ifofile.writeBlock((char*)buffer,size) ==-1) {
            QString sError("erreur");
        }
        ifofile.close();

        QFile ifofile2 (filename2);
        ifofile2.open(IO_WriteOnly);
        if (ifofile2.writeBlock((char*)buffer,size) ==-1) {
            QString sError("erreur");
        }
        ifofile2.close();


        free(buffer);
        free(bufCopy);
    }
}


/*!
    \fn k9Ifo::updateVMG(uchar * _buffer)
 */
void k9Ifo::updateVMG(uchar * _buffer) {
    vmgi_mat_t vmgi_mat;
    memcpy(&vmgi_mat,_ifo->vmgi_mat,sizeof(vmgi_mat_t));

    B2N_32(vmgi_mat.vmg_last_sector);
    B2N_32(vmgi_mat.vmgi_last_sector);
    B2N_32(vmgi_mat.vmg_category);
    B2N_16(vmgi_mat.vmg_nr_of_volumes);
    B2N_16(vmgi_mat.vmg_this_volume_nr);
    B2N_16(vmgi_mat.vmg_nr_of_title_sets);
    B2N_64(vmgi_mat.vmg_pos_code);
    B2N_32(vmgi_mat.vmgi_last_byte);
    B2N_32(vmgi_mat.first_play_pgc);
    B2N_32(vmgi_mat.vmgm_vobs);
    B2N_32(vmgi_mat.tt_srpt);
    B2N_32(vmgi_mat.vmgm_pgci_ut);
    B2N_32(vmgi_mat.ptl_mait);
    B2N_32(vmgi_mat.vts_atrt);
    B2N_32(vmgi_mat.txtdt_mgi);
    B2N_32(vmgi_mat.vmgm_c_adt);
    B2N_32(vmgi_mat.vmgm_vobu_admap);
    B2N_16(vmgi_mat.vmgm_audio_attr.lang_code);
    B2N_16(vmgi_mat.vmgm_subp_attr.lang_code);

    memcpy(_buffer,&vmgi_mat,sizeof(vmgi_mat_t));
}


/*!
    \fn k9Ifo::updateFP_PGC(uchar * _buffer)
 */
void k9Ifo::updateFP_PGC(uchar * _buffer) {
    int offset=_ifo->vmgi_mat->first_play_pgc;

    updatePGC(_buffer,_ifo->first_play_pgc,offset);


}

/*!
    \fn k9Ifo::updatePGC(uchar *_buffer,pgc_t *_pgc,int _offset)
 */
void k9Ifo::updatePGC(uchar *_buffer,pgc_t *_pgc,int _offset) {
    // pgc_t2 pgc;
    pgc_t pgc;
    //memcpy(&pgc,_pgc,sizeof(pgc_t2));
    memcpy(&pgc,_pgc,sizeof(pgc_t));
    if(pgc.command_tbl_offset != 0) {
        updatePGC_COMMAND_TBL(_buffer ,pgc.command_tbl  ,_offset+pgc.command_tbl_offset);
    }
    if(pgc.program_map_offset != 0) {
        updatePGC_PROGRAM_MAP(_buffer, pgc.program_map,pgc.nr_of_programs ,_offset + pgc.program_map_offset );
    }
    if(pgc.cell_playback_offset != 0) {
        updatePGC_CELL_PLAYBACK_TBL(_buffer ,pgc.cell_playback,pgc.nr_of_cells,_offset + pgc.cell_playback_offset);
    }

    if(pgc.cell_position_offset != 0) {
        updatePGC_CELL_POSITION_TBL(_buffer,pgc.cell_position,pgc.nr_of_cells,_offset + pgc.cell_position_offset);
    }
    B2N_16(pgc.next_pgc_nr);
    B2N_16(pgc.prev_pgc_nr);
    B2N_16(pgc.goup_pgc_nr);
    B2N_16(pgc.command_tbl_offset);
    B2N_16(pgc.program_map_offset);
    B2N_16(pgc.cell_playback_offset);
    B2N_16(pgc.cell_position_offset);

    int i;
    for(i = 0; i < 8; i++)
        B2N_16(pgc.audio_control[i]);
    for(i = 0; i < 32; i++)
        B2N_32(pgc.subp_control[i]);
    for(i = 0; i < 16; i++)
        B2N_32(pgc.palette[i]);

    //memcpy(_buffer+_offset,&(pgc.pgc),sizeof(pgc_t1));
    memcpy(_buffer+_offset,&pgc,PGC_SIZE);
}



/*!
    \fn k9Ifo::updatePGC_COMMAND_TBL(uchar  *_buffer,pgc_command_tbl_t *_cmd_tbl,int offset)
 */
void k9Ifo::updatePGC_COMMAND_TBL(uchar  *_buffer,pgc_command_tbl_t *_cmd_tbl,int _offset) {
    struct {
        uint16_t nr_of_pre;
        uint16_t nr_of_post;
        uint16_t nr_of_cell;
        uint16_t zero_1;
    }
    ATTRIBUTE_PACKED cmd_tbl;

    memcpy(&cmd_tbl,_cmd_tbl,sizeof(cmd_tbl));
    B2N_16(cmd_tbl.nr_of_pre);
    B2N_16(cmd_tbl.nr_of_post);
    B2N_16(cmd_tbl.nr_of_cell);
    memcpy(_buffer+_offset,&cmd_tbl,sizeof(cmd_tbl));

    //moves the offset to save vm_cmd
    _offset +=sizeof(cmd_tbl);

    if(_cmd_tbl->nr_of_pre != 0) {
        unsigned int pre_cmds_size  = _cmd_tbl->nr_of_pre * COMMAND_DATA_SIZE;
        memcpy(_buffer + _offset,_cmd_tbl->pre_cmds,pre_cmds_size);
        _offset +=pre_cmds_size;
    }
    if(_cmd_tbl->nr_of_post != 0) {
        unsigned int post_cmds_size = _cmd_tbl->nr_of_post * COMMAND_DATA_SIZE;
        memcpy(_buffer + _offset,_cmd_tbl->post_cmds,post_cmds_size);
        _offset += post_cmds_size;
    }
    if(_cmd_tbl->nr_of_cell != 0) {
        unsigned int cell_cmds_size = _cmd_tbl->nr_of_cell * COMMAND_DATA_SIZE;
        memcpy(_buffer +_offset,_cmd_tbl->cell_cmds,cell_cmds_size);
    }
}


/*!
    \fn k9Ifo::updatePGC_PROGRAM_MAP(uchar *_buffer,pgc_program_map_t *_program_map, int _nr,int_offset
 */
void k9Ifo::updatePGC_PROGRAM_MAP(uchar *_buffer,pgc_program_map_t *_program_map, int _nr,int _offset) {
    int size = _nr * sizeof(pgc_program_map_t);
    memcpy(_buffer+_offset, _program_map,size);

}


/*!
    \fn k9Ifo::updatePGC_CELL_PLAYBACK_TBL(uchar *_buffer, cell_playback_t *_cell_playback,int _nr, int _offset)
 */
void k9Ifo::updatePGC_CELL_PLAYBACK_TBL(uchar *_buffer, cell_playback_t *_cell_playback,int _nr, int _offset) {
    cell_playback_t *cell_playback;
    int size = _nr * sizeof(cell_playback_t);
    cell_playback=(cell_playback_t*) malloc(size);
    memcpy(cell_playback,_cell_playback,size);

    for( int i = 0; i < _nr; i++) {
        B2N_32(cell_playback[i].first_sector);
        B2N_32(cell_playback[i].first_ilvu_end_sector);
        B2N_32(cell_playback[i].last_vobu_start_sector);
        B2N_32(cell_playback[i].last_sector);
    }

    memcpy(_buffer + _offset,cell_playback,size);

    free(cell_playback);
}


/*!
    \fn k9Ifo::updatePGC_CELL_POSITION_TBL(uchar *_buffer,cell_position_t *_cell_position,int _nr, int _offset)
 */
void k9Ifo::updatePGC_CELL_POSITION_TBL(uchar *_buffer,cell_position_t *_cell_position,int _nr, int _offset) {
    cell_position_t * cell_position;
    int size = _nr * sizeof(cell_position_t);
    cell_position=(cell_position_t*)malloc(size);
    memcpy(cell_position,_cell_position,size);
    for( int i = 0; i < _nr; i++) {
        B2N_16(cell_position[i].vob_id_nr);
    }
    memcpy(_buffer + _offset,cell_position,size);
    free(cell_position);
}


/*!
    \fn k9Ifo::updateTT_SRPT(uchar *_buffer)
 */
void k9Ifo::updateTT_SRPT(uchar *_buffer) {
    if(_ifo->vmgi_mat->tt_srpt != 0) {
        tt_srpt_t * tt_srpt;
        tt_srpt=(tt_srpt_t*) malloc(sizeof(tt_srpt_t));
        int offset= _ifo->vmgi_mat->tt_srpt * DVD_BLOCK_LEN;
        memcpy (tt_srpt,_ifo->tt_srpt,sizeof(tt_srpt_t));


        int  info_length = tt_srpt->last_byte + 1 - TT_SRPT_SIZE;
        title_info_t * title_info;
        title_info =(title_info_t*) malloc(info_length);
        memcpy(title_info, tt_srpt->title,info_length);

        for(int i =  0; i < tt_srpt->nr_of_srpts; i++) {
            B2N_16(title_info[i].nr_of_ptts);
            B2N_16(title_info[i].parental_id);
            B2N_32(title_info[i].title_set_sector);
        }
        memcpy(_buffer+offset+TT_SRPT_SIZE,title_info,info_length);

        free(title_info);

        B2N_16(tt_srpt->nr_of_srpts);
        B2N_32(tt_srpt->last_byte);
        memcpy(_buffer+offset,tt_srpt,TT_SRPT_SIZE);
        free (tt_srpt);
    }

}


void k9Ifo::updatePGCI_UT(uchar *_buffer) {
    int sector,sector2;
    if(_ifo->vmgi_mat) {
        if(_ifo->vmgi_mat->vmgm_pgci_ut == 0)
            return;
        sector = _ifo->vmgi_mat->vmgm_pgci_ut *DVD_BLOCK_LEN;
    } else if(_ifo->vtsi_mat) {
        if(_ifo->vtsi_mat->vtsm_pgci_ut == 0)
            return;
        // A vï¿½ifier : si sector = secteur physique ou offset
        sector = _ifo->vtsi_mat->vtsm_pgci_ut * DVD_BLOCK_LEN;
    } else {
        return;
    }
    sector2=sector;
    pgci_ut_t * pgci_ut;
    pgci_ut = (pgci_ut_t*) malloc( sizeof(pgci_ut_t));
    memcpy (pgci_ut,_ifo->pgci_ut,sizeof(pgci_ut_t));
    B2N_16(pgci_ut->nr_of_lus);
    B2N_32(pgci_ut->last_byte);

    memcpy(_buffer+sector,pgci_ut,PGCI_UT_SIZE);


    int info_length = _ifo->pgci_ut->nr_of_lus * PGCI_LU_SIZE;
    sector += PGCI_UT_SIZE;
    free(pgci_ut);

    pgci_lu_t *pgci_lu;
    pgci_lu = (pgci_lu_t*)malloc(sizeof(pgci_lu_t));

    for(int i = 0; i < _ifo->pgci_ut->nr_of_lus; i++) {
        memcpy(pgci_lu,&(_ifo->pgci_ut->lu[i]), PGCI_LU_SIZE);
        B2N_16(pgci_lu->lang_code);
        B2N_32(pgci_lu->lang_start_byte);
        memcpy(_buffer+sector,pgci_lu,PGCI_LU_SIZE);
        updatePGCIT_internal(_buffer,_ifo->pgci_ut->lu[i].pgcit,sector2 + _ifo->pgci_ut->lu[i].lang_start_byte);
        sector += PGCI_LU_SIZE;

    }
    free (pgci_lu);

    // A FAIRE :
//    for(int i = 0; i < _ifo->pgci_ut->nr_of_lus; i++) {
//        updatePGCIT_internal(_buffer,_ifo->pgci_ut->lu[i].pgcit,sector2  + _ifo->pgci_ut->lu[i].lang_start_byte);
 //   }

}

void k9Ifo::updatePGCIT(uchar *_buffer) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_pgcit == 0) /* mandatory */
        return ;

    updatePGCIT_internal(_buffer,_ifo->vts_pgcit,_ifo->vtsi_mat->vts_pgcit * DVD_BLOCK_LEN);
}


void k9Ifo::updatePGCIT_internal(uchar *_buffer, pgcit_t *_pgcit, int _offset) {
    pgcit_t * pgcit;
    pgcit=(pgcit_t*)malloc(sizeof(pgcit_t));
    memcpy(pgcit,_pgcit,sizeof(pgcit_t));
    B2N_16(pgcit->nr_of_pgci_srp);
    B2N_32(pgcit->last_byte);
    memcpy(_buffer+_offset ,pgcit,PGCIT_SIZE);
    free(pgcit);

    int info_length =_pgcit->nr_of_pgci_srp * PGCI_SRP_SIZE;
    pgci_srp_t * pgci_srp;;
    pgci_srp=(pgci_srp_t*)malloc(sizeof(pgci_srp_t));

    int offset=_offset+PGCIT_SIZE;

    for(int i = 0; i < _pgcit->nr_of_pgci_srp; i++) {
        memcpy(pgci_srp,&(_pgcit->pgci_srp[i]) , PGCI_SRP_SIZE);
        B2N_16(pgci_srp->ptl_id_mask);
        B2N_32(pgci_srp->pgc_start_byte);
        memcpy(_buffer+offset,pgci_srp,PGCI_SRP_SIZE);

        offset+=PGCI_SRP_SIZE;
// JMP : a vérifier, utilisation de _offset
        updatePGC(_buffer,_pgcit->pgci_srp[i].pgc,_offset + _pgcit->pgci_srp[i].pgc_start_byte);
    }
    free(pgci_srp);


}


void k9Ifo::updatePTL_MAIT(uchar *_buffer) {
    if(!_ifo->vmgi_mat)
        return;

    if(_ifo->vmgi_mat->ptl_mait == 0)
        return;

    ptl_mait_t* ptl_mait;
    ptl_mait=(ptl_mait_t*)malloc(sizeof(ptl_mait_t));
    memcpy(ptl_mait,_ifo->ptl_mait,sizeof(ptl_mait_t));

    B2N_16(ptl_mait->nr_of_countries);
    B2N_16(ptl_mait->nr_of_vtss);
    B2N_32(ptl_mait->last_byte);

    int offset=_ifo->vmgi_mat->ptl_mait * DVD_BLOCK_LEN;
    memcpy(_buffer+offset,ptl_mait,PTL_MAIT_SIZE);
    free(ptl_mait);

    offset+=PTL_MAIT_SIZE;

    int info_length = _ifo->ptl_mait->nr_of_countries * sizeof(ptl_mait_country_t);
    ptl_mait_country_t *ptl_mait_country;;
    ptl_mait_country = (ptl_mait_country_t*) malloc (sizeof(ptl_mait_country_t));


    for(int i = 0; i < ptl_mait->nr_of_countries; i++) {
        memcpy(ptl_mait_country,&(_ifo->ptl_mait->countries[i]),PTL_MAIT_COUNTRY_SIZE);
        B2N_16(ptl_mait_country->country_code);
        B2N_16(ptl_mait_country->pf_ptl_mai_start_byte);
        memcpy(_buffer +offset,ptl_mait_country,PTL_MAIT_COUNTRY_SIZE);
        offset+=PTL_MAIT_COUNTRY_SIZE;

    }
    free(ptl_mait_country);
    /* A VOIR
    pf_level_t * pf_level,*ptr2;
    pf_level =(pf_level_t*)malloc(sizeof(pf_level_t));

    for(int i = 0; i < _ifo->ptl_mait->nr_of_countries; i++) {
       int offset2=_ifo->vmgi_mat->ptl_mait * DVD_BLOCK_LEN + _ifo->ptl_mait->countries[i].pf_ptl_mai_start_byte;
    for (int j = 0; j < ((_ifo->ptl_mait->nr_of_vtss + 1) * 8); j++) {
    	ptr2 =  &(_ifo->ptl_mait->countries[i].pf_ptl_mai[j]);
    	memcpy(pf_level,ptr2,sizeof(pf_level_t));
           	B2N_16(pf_level[0]);
    	memcpy(_buffer+offset2,pf_level,sizeof(pf_level_t));
    	offset2+=sizeof(pf_level_t);
       }
    }
    free(pf_level);
    */
}

void k9Ifo::updateVTS_ATRT(uchar *_buffer) {
    if(_ifo->vmgi_mat->vts_atrt == 0)
        return;

    int sector = _ifo->vmgi_mat->vts_atrt * DVD_BLOCK_LEN;
    vts_atrt_t *vts_atrt;
    vts_atrt = (vts_atrt_t*)malloc(sizeof(vts_atrt_t));
    memcpy(vts_atrt,_ifo->vts_atrt,VTS_ATRT_SIZE);
    B2N_16(vts_atrt->nr_of_vtss);
    B2N_32(vts_atrt->last_byte);
    memcpy(_buffer+sector,vts_atrt,VTS_ATRT_SIZE);
    free(vts_atrt);

    sector += VTS_ATRT_SIZE;
    vts_atrt=_ifo->vts_atrt;
    int info_length;
    info_length = vts_atrt->nr_of_vtss * sizeof(uint32_t);
    uint32_t *data;
    data=(uint32_t*) malloc(info_length);
    memcpy (data,_ifo->vts_atrt->vts_atrt_offsets,info_length);

    for(int i = 0; i < _ifo->vts_atrt->nr_of_vtss; i++) {
        B2N_32(data[i]);
    }
    memcpy(_buffer+sector,data,info_length);
    free(data);

    data = (uint32_t*)_ifo->vts_atrt->vts_atrt_offsets;
    info_length = vts_atrt->nr_of_vtss * sizeof(vts_attributes_t);
    for(int i = 0; i < _ifo->vts_atrt->nr_of_vtss; i++) {
        unsigned int offset = data[i];
        // A VOIR
        // updateVTS_ATTRIBUTES(_buffer+sector+offset,&(_ifo->vts_atrt->vts[i]));
    }
}

void k9Ifo::updateTXTDT_MGI(uchar * _buffer) {
    if(_ifo->vmgi_mat->txtdt_mgi == 0)
        return;
    int offset=_ifo->vmgi_mat->txtdt_mgi * DVD_BLOCK_LEN;
    memcpy(_buffer+offset,_ifo->txtdt_mgi,TXTDT_MGI_SIZE);
}

void k9Ifo::updateC_ADT(uchar * _buffer) {
    int sector;

    if(_ifo->vmgi_mat) {
        if(_ifo->vmgi_mat->vmgm_c_adt != 0) {
            sector = _ifo->vmgi_mat->vmgm_c_adt * DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->menu_c_adt,sector);
        }
    } else if(_ifo->vtsi_mat) {
        if(_ifo->vtsi_mat->vtsm_c_adt != 0) {
            sector = _ifo->vtsi_mat->vtsm_c_adt * DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->menu_c_adt,sector);
        }
        if (_ifo->vtsi_mat->vts_c_adt !=0) {
            sector = _ifo->vtsi_mat->vts_c_adt * DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->vts_c_adt,sector);
        }
    } else {
        return ;
    }

}

void k9Ifo::updateC_ADT_Internal(uchar *_buffer,c_adt_t *_c_adt,int _sector) {
    c_adt_t * c_adt;
    c_adt =(c_adt_t*) malloc (sizeof(c_adt_t));
    memcpy(c_adt,_c_adt,sizeof(c_adt_t));
    B2N_16(c_adt->nr_of_vobs);
    B2N_32(c_adt->last_byte);
    memcpy(_buffer+_sector,c_adt,C_ADT_SIZE);
    free(c_adt);

    int offset =_sector + C_ADT_SIZE;

    int info_length = _c_adt->last_byte + 1 - C_ADT_SIZE;

    cell_adr_t *cell_adr,*ptr;
    cell_adr=(cell_adr_t*) malloc(sizeof(cell_adr_t));

    ptr= _c_adt->cell_adr_table;
    for(int i = 0; i < info_length/sizeof(cell_adr_t); i++) {
        memcpy(cell_adr,&(ptr[i]),sizeof(cell_adr_t));
        B2N_16(cell_adr->vob_id);
        B2N_32(cell_adr->start_sector);
        B2N_32(cell_adr->last_sector);
        memcpy(_buffer+offset,cell_adr,sizeof(cell_adr_t));
        offset+=sizeof(cell_adr_t);
        //ptr+=sizeof(cell_adr_t);
    }

    free(cell_adr);
}

void k9Ifo::updateVOBU_ADMAP(uchar * _buffer) {
    int sector;
    if(_ifo->vmgi_mat) {

        if(_ifo->vmgi_mat->vmgm_vobu_admap == 0)
            return ;
        sector = _ifo->vmgi_mat->vmgm_vobu_admap * DVD_BLOCK_LEN;
        updateVOBU_ADMAP_Internal(_buffer,_ifo->menu_vobu_admap,sector);

    } else if(_ifo->vtsi_mat) {

        if(_ifo->vtsi_mat->vtsm_vobu_admap != 0) {
            sector = _ifo->vtsi_mat->vtsm_vobu_admap * DVD_BLOCK_LEN;
            updateVOBU_ADMAP_Internal(_buffer,_ifo->menu_vobu_admap,sector);
        }
        if (_ifo->vtsi_mat->vts_vobu_admap !=0) {
            sector = _ifo->vtsi_mat->vts_vobu_admap * DVD_BLOCK_LEN;
            updateVOBU_ADMAP_Internal(_buffer,_ifo->vts_vobu_admap,sector);
        }
    } else {
        return ;
    }
}

void k9Ifo::updateVOBU_ADMAP_Internal(uchar *_buffer,vobu_admap_t *_vobu_admap,int _sector) {
    vobu_admap_t *vobu_admap;
    vobu_admap=(vobu_admap_t*)malloc(sizeof(vobu_admap_t));
    memcpy(vobu_admap,_vobu_admap,sizeof(vobu_admap_t));
    B2N_32(vobu_admap->last_byte);
    memcpy(_buffer+_sector,vobu_admap,VOBU_ADMAP_SIZE);
    free(vobu_admap);

    int offset = _sector + VOBU_ADMAP_SIZE;

    int info_length = _vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    uint32_t *vobu_start_sectors;
    vobu_start_sectors=(uint32_t*)malloc(info_length);
    memcpy(vobu_start_sectors,_vobu_admap->vobu_start_sectors,info_length);

    for(int i = 0; i < info_length/sizeof(uint32_t); i++)
        B2N_32(vobu_start_sectors[i]);

    memcpy(_buffer+offset,vobu_start_sectors,info_length);

    free(vobu_start_sectors);
}

void k9Ifo::updateVTS(uchar *_buffer) {
    vtsi_mat_t *vtsi_mat;
    vtsi_mat = (vtsi_mat_t *)malloc(sizeof(vtsi_mat_t));
    memcpy(vtsi_mat,_ifo->vtsi_mat,sizeof(vtsi_mat_t));

    B2N_32(vtsi_mat->vts_last_sector);
    B2N_32(vtsi_mat->vtsi_last_sector);
    B2N_32(vtsi_mat->vts_category);
    B2N_32(vtsi_mat->vtsi_last_byte);
    B2N_32(vtsi_mat->vtsm_vobs);
    B2N_32(vtsi_mat->vtstt_vobs);
    B2N_32(vtsi_mat->vts_ptt_srpt);
    B2N_32(vtsi_mat->vts_pgcit);
    B2N_32(vtsi_mat->vtsm_pgci_ut);
    B2N_32(vtsi_mat->vts_tmapt);
    B2N_32(vtsi_mat->vtsm_c_adt);
    B2N_32(vtsi_mat->vtsm_vobu_admap);
    B2N_32(vtsi_mat->vts_c_adt);
    B2N_32(vtsi_mat->vts_vobu_admap);
    B2N_16(vtsi_mat->vtsm_audio_attr.lang_code);
    B2N_16(vtsi_mat->vtsm_subp_attr.lang_code);
    for(int i = 0; i < 8; i++)
        B2N_16(vtsi_mat->vts_audio_attr[i].lang_code);
    for(int i = 0; i < 32; i++)
        B2N_16(vtsi_mat->vts_subp_attr[i].lang_code);

    memcpy(_buffer,vtsi_mat,sizeof(vtsi_mat_t));
    free(vtsi_mat);
}

void k9Ifo::updateVTS_PTT_SRPT(uchar *_buffer) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_ptt_srpt == 0) /* mandatory */
        return ;

    vts_ptt_srpt_t * vts_ptt_srpt;
    vts_ptt_srpt = (vts_ptt_srpt_t *)malloc(sizeof(vts_ptt_srpt_t));
    memcpy(vts_ptt_srpt,_ifo->vts_ptt_srpt,sizeof(vts_ptt_srpt_t));
    int offset = _ifo->vtsi_mat->vts_ptt_srpt * DVD_BLOCK_LEN;

    B2N_16(vts_ptt_srpt->nr_of_srpts);
    B2N_32(vts_ptt_srpt->last_byte);

    memcpy(_buffer+offset,vts_ptt_srpt,VTS_PTT_SRPT_SIZE);
    free(vts_ptt_srpt);

    /* A VOIR
    offset += VTS_PTT_SRPT_SIZE;

    int info_length = _ifo->vts_ptt_srpt->last_byte + 1 - VTS_PTT_SRPT_SIZE;
    uint32_t * data;
    data=(uint32_t*) malloc(info_length);
    memcpy(data,_ifo->vts_ptt_srpt->ttu_offset,info_length);
    for(int i = 0; i < _ifo->vts_ptt_srpt->nr_of_srpts; i++) {
      B2N_32(data[i]);
    }
    memcpy(_buffer+offset,data,info_length);
    free(data);

    ...
    */
}


/*!
    \fn k9Ifo::checkBuffer()
 */


void k9Ifo::updateVTS_TMAPT(uchar *_buffer) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_tmapt == 0) { /* optional(?) */
        return ;
    }

    vts_tmapt_t *vts_tmapt1,*vts_tmapt;
    vts_tmapt1 = (vts_tmapt_t *)malloc(sizeof(vts_tmapt_t));
    memcpy(vts_tmapt1,_ifo->vts_tmapt,sizeof(vts_tmapt_t));
    uint32_t offset = _ifo->vtsi_mat->vts_tmapt * DVD_BLOCK_LEN;
    B2N_16(vts_tmapt1->nr_of_tmaps);
    B2N_32(vts_tmapt1->last_byte);
    int offset0=offset;
    offset+=VTS_TMAPT_SIZE;

    vts_tmapt=_ifo->vts_tmapt;
    int info_length = vts_tmapt->nr_of_tmaps * 4;
    uint32_t *vts_tmap_srp = (uint32_t *)malloc(info_length);
    memcpy(vts_tmap_srp,vts_tmapt->tmap_offset,info_length);
    for (int i = 0; i < vts_tmapt->nr_of_tmaps; i++) {
        B2N_32(vts_tmap_srp[i]);
    }
    int offset1=offset;
    int info_length1=info_length;
    offset += info_length;

    info_length = vts_tmapt->nr_of_tmaps * sizeof(vts_tmap_t);
    vts_tmap_t * tmap = (vts_tmap_t *)malloc(info_length);
    memcpy(tmap,vts_tmapt->tmap,info_length);
    for(int i = 0; i < vts_tmapt->nr_of_tmaps; i++) {
        B2N_16(tmap[i].nr_of_entries);
        memcpy(_buffer+offset,&(tmap[i]),VTS_TMAP_SIZE);

        vts_tmapt1->last_byte=+offset+VTS_TMAP_SIZE-1;

        offset+=VTS_TMAP_SIZE;
        vts_tmap_srp[i]=offset-offset1+4;
        B2N_32(vts_tmap_srp[i]);
        if(vts_tmapt->tmap[i].nr_of_entries == 0) { // Early out if zero entries
            continue;
        }
        int info_length2 = vts_tmapt->tmap[i].nr_of_entries * sizeof(map_ent_t);
        map_ent_t *map_ent = (map_ent_t *)malloc(info_length2);
        memcpy(map_ent,vts_tmapt->tmap[i].map_ent,info_length2);
        for(int j = 0; j < vts_tmapt->tmap[i].nr_of_entries; j++) {
            B2N_32(map_ent[j]);
            memcpy(_buffer+offset,&(map_ent[j]),sizeof(map_ent_t));
            vts_tmapt1->last_byte=offset+sizeof(map_ent_t)-1;
            offset+=sizeof(map_ent_t);
        }
        free(map_ent);
    }
    memcpy(_buffer+offset1,vts_tmap_srp,info_length1);
    free(vts_tmap_srp);

    free(tmap);
    vts_tmapt1->last_byte-=offset0;
    B2N_32(vts_tmapt1->last_byte);
    memcpy(_buffer+offset0,vts_tmapt1,VTS_TMAPT_SIZE);
    free(vts_tmapt1);


}
void k9Ifo::checkBuffer(QString lib,uchar* _buffer) {
    /*     for (int j=0;j<bufCopySize;j++) {
           uchar *c,*c2;
    c=(uchar*) (_buffer+j);
    c2=(uchar*)(bufCopy+j);
    if (*c!=*c2) {
      return;
    }
       }
    */
    return;
    if (memcmp(bufCopy,_buffer,bufCopySize)!=0) {
        QString c;
        c="Buffer de sortie alï¿½ï¿½: " +lib;
        qDebug(c.latin1());
    }
}

void k9Ifo::navRead_PCI(pci_t *pci, uchar *buffer) {
  int i, j;

  memcpy(pci, buffer, sizeof(pci_t));

  /* Endian conversions  */

  /* pci pci_gi */
  B2N_32(pci->pci_gi.nv_pck_lbn);
  B2N_16(pci->pci_gi.vobu_cat);
  B2N_32(pci->pci_gi.vobu_s_ptm);
  B2N_32(pci->pci_gi.vobu_e_ptm);
  B2N_32(pci->pci_gi.vobu_se_e_ptm);

  /* pci nsml_agli */
  for(i = 0; i < 9; i++)
    B2N_32(pci->nsml_agli.nsml_agl_dsta[i]);

  /* pci hli hli_gi */
  B2N_16(pci->hli.hl_gi.hli_ss);
  B2N_32(pci->hli.hl_gi.hli_s_ptm);
  B2N_32(pci->hli.hl_gi.hli_e_ptm);
  B2N_32(pci->hli.hl_gi.btn_se_e_ptm);

  /* pci hli btn_colit */
  for(i = 0; i < 3; i++)
    for(j = 0; j < 2; j++)
      B2N_32(pci->hli.btn_colit.btn_coli[i][j]);

  /* NOTE: I've had to change the structure from the disk layout to get
   * the packing to work with Sun's Forte C compiler. */
  
  /* pci hli btni */
  for(i = 0; i < 36; i++) {
    char tmp[sizeof(pci->hli.btnit[i])], swap;
    memcpy(tmp, &(pci->hli.btnit[i]), sizeof(pci->hli.btnit[i]));
    /* Byte 4 to 7 are 'rotated' was: ABCD EFGH IJ is: ABCG DEFH IJ */
    swap   = tmp[6]; 
    tmp[6] = tmp[5];
    tmp[5] = tmp[4];
    tmp[4] = tmp[3];
    tmp[3] = swap;
    
    /* Then there are the two B2N_24(..) calls */
#ifndef WORDS_BIGENDIAN
    swap = tmp[0];
    tmp[0] = tmp[2];
    tmp[2] = swap;
    
    swap = tmp[4];
    tmp[4] = tmp[6];
    tmp[6] = swap;
#endif
    memcpy(&(pci->hli.btnit[i]), tmp, sizeof(pci->hli.btnit[i]));
  }

}
