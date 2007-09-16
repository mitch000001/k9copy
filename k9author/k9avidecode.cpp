//
// C++ Implementation: k9avidecode
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9avidecode.h"
#include <ffmpeg/avcodec.h>
#include <qimage.h>
#include <dlfcn.h>

#include "ac.h"

void *CodecHandle=0;
void *FormatHandle=0;
int glibref=0;

k9AviDecode::k9AviDecode(QObject *parent, const char *name)
        : QObject(parent, name) {
    if (glibref==0) {
        CodecHandle=dlopen("libavcodec.so",RTLD_LAZY | RTLD_GLOBAL);
        FormatHandle=dlopen("libavformat.so",RTLD_LAZY | RTLD_GLOBAL);
    }

    av_register_all = (av_register_all_t)dlsym(FormatHandle,"av_register_all");
    av_open_input_file = (av_open_input_file_t)dlsym(FormatHandle,"av_open_input_file");
    av_find_stream_info = (av_find_stream_info_t)dlsym(FormatHandle,"av_find_stream_info");
    avcodec_find_decoder =(avcodec_find_decoder_t) dlsym(CodecHandle,"avcodec_find_decoder");
    avcodec_open = (avcodec_open_t)dlsym(CodecHandle,"avcodec_open");
    avcodec_alloc_frame = (avcodec_alloc_frame_t)dlsym(CodecHandle,"avcodec_alloc_frame");
    avpicture_get_size = (avpicture_get_size_t)dlsym(CodecHandle,"avpicture_get_size");
    av_malloc = (av_malloc_t)dlsym(CodecHandle,"av_malloc");
    avpicture_fill = (avpicture_fill_t)dlsym(CodecHandle,"avpicture_fill");
    av_read_frame = (av_read_frame_t)dlsym(FormatHandle,"av_read_frame");
    avcodec_decode_video = (avcodec_decode_video_t)dlsym(CodecHandle,"avcodec_decode_video");
    img_convert = (img_convert_t)dlsym(CodecHandle,"img_convert");
    av_free = (av_free_t)dlsym(CodecHandle,"av_free");
    avcodec_close = (avcodec_close_t)dlsym(FormatHandle,"avcodec_close");
    av_close_input_file = (av_close_input_file_t)dlsym(FormatHandle,"av_close_input_file");
    av_seek_frame=(av_seek_frame_t)dlsym(FormatHandle,"av_seek_frame");
    av_rescale_q=(av_rescale_q_t)dlsym(FormatHandle,"av_rescale_q");
    av_gettime=(av_gettime_t)dlsym(FormatHandle,"av_gettime");
    av_register_all();
    m_opened=false;

    glibref++;
}


k9AviDecode::~k9AviDecode() {
    if (m_opened)
        close();
    glibref--;
    if (glibref==0) {
        dlclose(FormatHandle);
        dlclose(CodecHandle);
    }

}


#include "k9avidecode.moc"


bool k9AviDecode::open(const QString & _fileName) {
    if (m_opened)
        close();

    // Open video file
    if (av_open_input_file(&m_FormatCtx, _fileName.latin1(), NULL, 0, NULL)!=0)
        return false; // Couldn't open file}

// Retrieve stream information
    if (av_find_stream_info(m_FormatCtx)<0)
        return false; // Couldn't find stream information

    int i;

// Find the first video stream
    m_videoStream=-1;
    for (i=0; i<m_FormatCtx->nb_streams; i++)
        if (m_FormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
            m_videoStream=i;
            break;
        }
    if (m_videoStream==-1)
        return false; // Didn't find a video stream

    // Get a pointer to the codec context for the video stream
    m_CodecCtx=m_FormatCtx->streams[m_videoStream]->codec;


// Find the decoder for the video stream
    m_Codec=avcodec_find_decoder(m_CodecCtx->codec_id);
    if (m_Codec==NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return false; // Codec not found
    }
// Open codec
    if (avcodec_open(m_CodecCtx, m_Codec)<0)
        return false; // Could not open codec


// Allocate video frame
    m_Frame=avcodec_alloc_frame();

// Allocate an AVFrame structure
    m_FrameRGB=avcodec_alloc_frame();
    if (m_FrameRGB==NULL)
        return false;


    int numBytes;
// Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, m_CodecCtx->width,
                                m_CodecCtx->height);
    m_buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

// Assign appropriate parts of buffer to image planes in pFrameRGB
// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
// of AVPicture
    avpicture_fill((AVPicture *)m_FrameRGB, m_buffer, PIX_FMT_RGB24,
                   m_CodecCtx->width, m_CodecCtx->height);

    m_duration=(double)m_FormatCtx->duration / AV_TIME_BASE;
    m_opened=true;
    m_fileName=_fileName;
}

void k9AviDecode::seek(double _seconds) {
    AVRational time_base = m_FormatCtx->streams[m_videoStream]->time_base;
    int64_t fspos = (int64_t)(_seconds * AV_TIME_BASE);
    fspos=av_rescale_q(fspos, AV_TIME_BASE_Q,  time_base);
    int i=av_seek_frame(m_FormatCtx, m_videoStream, fspos, AVSEEK_FLAG_BACKWARD );
    double pos=av_gettime() / 1000000;
}

void k9AviDecode::readFrame(double _seconds) {
    AVRational time_base = m_FormatCtx->streams[m_videoStream]->time_base;
    int64_t fspos = (int64_t)(_seconds * AV_TIME_BASE);
    fspos=av_rescale_q(fspos, AV_TIME_BASE_Q,  time_base);
    int res=av_seek_frame(m_FormatCtx, m_videoStream, fspos, AVSEEK_FLAG_BACKWARD );

    int frameFinished=0;
    AVPacket packet;

    bool bFound=false;
    while (av_read_frame(m_FormatCtx, &packet)>=0 &&  !bFound) {
        // Is this a packet from the video stream?
        if (packet.stream_index==m_videoStream) {
            // Decode video frame
            avcodec_decode_video(m_CodecCtx, m_Frame, &frameFinished,
                                 packet.data, packet.size);

            // Did we get a video frame?
            if (frameFinished) {
//            if (m_Frame->pts >=fspos)
                if (m_FormatCtx->cur_st->cur_dts >=fspos) {
                    bFound=true;
                    // Convert the image from its native format to RGB
                    img_convert((AVPicture *)m_FrameRGB, PIX_FMT_RGB24,
                                (AVPicture*)m_Frame, m_CodecCtx->pix_fmt,
                                m_CodecCtx->width, m_CodecCtx->height);

                    // convert frame to QImage
                    SaveFrame(m_FrameRGB, m_CodecCtx->width,
                              m_CodecCtx->height);
                }
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

}

void k9AviDecode::SaveFrame(AVFrame *pFrame, int width, int height) {
    QImage pix;
    int len =(int) (3*width*height);
    char c[255];
    // Write header
    sprintf(c,"P6\n%d %d\n255\n", width, height);
    char *s= (char*) malloc(len+strlen(c));
    tc_memcpy(s,c,strlen(c));
    tc_memcpy(s+strlen(c),pFrame->data[0], len);
    pix.loadFromData((uchar*)s,strlen(c)+len);
    free(s);
    emit drawFrame( &pix);

}

void k9AviDecode::close() {
    if (m_opened) {
        // Free the RGB image
        av_free(m_buffer);
        av_free(m_FrameRGB);

        // Free the YUV frame
        av_free(m_Frame);

        // Close the codec
        avcodec_close(m_CodecCtx);

        // Close the video file
        av_close_input_file(m_FormatCtx);
        m_opened=false;
    }
}


double k9AviDecode::getDuration() const {
    return m_duration;
}


bool k9AviDecode::opened() const {
    return m_opened;
}


QString k9AviDecode::getFileName() const {
    return m_fileName;
}
