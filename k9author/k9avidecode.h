//
// C++ Interface: k9avidecode
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9AVIDECODE_H
#define K9AVIDECODE_H

#include "k9common.h"
#include <qobject.h>
#include <ffmpeg/avformat.h>
#include <qimage.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/


//typedef dvd_file_t * (*DVDOpenFile_t) ( dvd_reader_t *, int, dvd_read_domain_t );
typedef void (*av_register_all_t) (void);
typedef int64_t (*av_gettime_t) (void);
typedef int (*av_open_input_file_t)(AVFormatContext **, const char *,AVInputFormat *,int, AVFormatParameters *);
typedef int (*av_find_stream_info_t)(AVFormatContext *);
typedef AVCodec* (*avcodec_find_decoder_t)(enum CodecID);
typedef int (*avcodec_open_t)(AVCodecContext *, AVCodec *);
typedef AVFrame * (*avcodec_alloc_frame_t)(void);

typedef int (*avpicture_get_size_t)(int , int , int );
typedef void * (*av_malloc_t)(unsigned int );
typedef int (*avpicture_fill_t)(AVPicture *, uint8_t *,int , int , int);
typedef int (*av_read_frame_t)(AVFormatContext *, AVPacket *);

typedef int (*avcodec_decode_video_t)(AVCodecContext *, AVFrame *,int *, uint8_t *, int );
typedef int (*img_convert_t)(AVPicture *, int , const AVPicture *, int ,int, int);
typedef void (*av_free_t)(void *);
typedef int (*avcodec_close_t)(AVCodecContext *);
typedef void (*av_close_input_file_t)(AVFormatContext *);
typedef int (*av_seek_frame_t)(AVFormatContext *,int,int64_t timestamp,int flags); 		typedef int64_t (*av_rescale_q_t)(int64_t , AVRational , AVRational )	;




class k9AviDecode : public QObject
{
Q_OBJECT
public:
    k9AviDecode(QObject *parent = 0, const char *name = 0);

    ~k9AviDecode();
    bool open(const QString & _fileName);
    void readFrame(double _seconds);
    void seek(double _seconds);
    void close();
    double getDuration() const;
    bool opened() const;
	
	
private:
    av_register_all_t av_register_all;
    av_open_input_file_t av_open_input_file;
    av_find_stream_info_t av_find_stream_info;
    avcodec_find_decoder_t avcodec_find_decoder;
    avcodec_open_t avcodec_open;
    avcodec_alloc_frame_t avcodec_alloc_frame;
    av_seek_frame_t av_seek_frame;
    avpicture_get_size_t avpicture_get_size;
    av_malloc_t av_malloc;
    avpicture_fill_t avpicture_fill;
    av_read_frame_t av_read_frame;
    avcodec_decode_video_t avcodec_decode_video;
    img_convert_t img_convert;
    av_free_t av_free;
    avcodec_close_t avcodec_close;
    av_close_input_file_t av_close_input_file;
    av_rescale_q_t av_rescale_q;
    av_gettime_t av_gettime;

    AVFormatContext *m_FormatCtx;
    AVCodecContext *m_CodecCtx;
    AVCodec *m_Codec;
    AVFrame *m_Frame,*m_FrameRGB;
    uint8_t *m_buffer;
    int m_videoStream;
    bool m_opened;
    double m_duration;
    void SaveFrame(AVFrame *pFrame, int width, int height);
signals:
    void drawFrame(QImage*);
};

#endif
