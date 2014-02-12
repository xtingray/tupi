/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#define INT64_C
#define __STDC_CONSTANT_MACROS
#include <stdint.h>

#include "tffmpegmoviegenerator.h"
#include "tdebug.h"
#include "talgorithm.h"

#include <QDir>

#ifdef __cplusplus
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
}
#endif

struct TFFMpegMovieGenerator::Private
{
    AVFrame *picture;
    AVFrame *tmpPicture;

    QString movieFile;
    int fps;
    double video_pts;
    uint8_t *videOutbuf;
    int frameCount;
    int videOutbufSize;
    double streamDuration;
    bool exception;
    const char *errorMsg;

    AVStream *video_st;
    AVFormatContext *oc;
    AVOutputFormat *fmt;

    void chooseFileExtension(int format);
    bool openVideo(AVFormatContext *oc, AVStream *st);
    void RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint iRGBIncrement, bool bSwapRGB, int width, int height);
    bool writeVideoFrame(const QImage &image);
    void closeVideo(AVStream *st);
};

static AVStream *addVideoStream(AVFormatContext *oc, int codec_id, int width, int height, int fps, const char *errorMsg)
{
    AVCodecContext *c;
    AVStream *st;

    int w = width;
    int h = height;

    // st = av_new_stream(oc, 0);
    st = avformat_new_stream(oc, 0);
    if (!st) {
        errorMsg = "ffmpeg error: Could not alloc stream. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::addVideoStream() - " << errorMsg;
        #endif
        return 0;
    }

    c = st->codec;
    c->codec_id = CodecID(codec_id);
    c->codec_type = AVMEDIA_TYPE_VIDEO;   

    /* put sample parameters */
    // c->bit_rate = 800000;
    c->bit_rate = 6000000;

    /* resolution must be a multiple of two */
    c->width = w;  
    c->height = h; 

    c->time_base.den = fps;
    c->time_base.num = 1;
    // c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->gop_size = 2;
    c->pix_fmt = PIX_FMT_YUV420P;

    // c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
	/* just for testing, we also add B frames */
	c->max_b_frames = 2;
    }

    if (c->codec_id == CODEC_ID_MPEG1VIDEO) {
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
           c->mb_decision=2;
    }

    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	
    return st;
}

static AVFrame *allocPicture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;
 
    picture = avcodec_alloc_frame();

    if (!picture) 
        return 0;
	
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = (uint8_t *)malloc(size);

    if (!picture_buf) {
	av_free(picture);
	return 0;
    }

    avpicture_fill((AVPicture *)picture, picture_buf, pix_fmt, width, height);

    return picture;
}

void TFFMpegMovieGenerator::Private::chooseFileExtension(int format)
{
    switch (format) {
            case WEBM:
                 movieFile += ".webm";
                 break;
            case OGV:
                 movieFile += ".ogv";
                 break;
            case SWF:
                 movieFile += ".swf";
                 break;
            case MPEG:
                 movieFile += ".mpg";
                 break;
            case ASF:
                 movieFile += ".asf";
                 break;
            case AVI:
                 movieFile += ".avi";
                 break;
            case MOV:
                 movieFile += ".mov";
                 break;
            case GIF:
                 movieFile += ".gif";
                 break;
            default:
                 movieFile += ".mpg";
                 break;
    }
}

bool TFFMpegMovieGenerator::Private::openVideo(AVFormatContext *oc, AVStream *st)
{
    AVCodec *codec;
    AVCodecContext *c;

    c = st->codec;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);

    if (!codec) {
        errorMsg = "ffmpeg error: Video codec not found. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. It's very possible your system is missing codecs. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
        #endif

        return false;
    }

    /* open the codec */
    // if (avcodec_open(c, codec) < 0) {
    if (avcodec_open2(c, codec, NULL) < 0) {
        errorMsg = "ffmpeg error: Could not open video codec. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
        #endif
        return false;
    }

    videOutbuf = 0;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        videOutbufSize = 200000;
        videOutbuf = (uint8_t *) av_malloc(videOutbufSize);
    }

    /* allocate the encoded raw m_picture */
    picture = allocPicture(c->pix_fmt, c->width, c->height);

    if (!picture) {
        errorMsg = "ffmpeg error: Could not allocate m_picture. This is not a problem directly related to Tupi. \
                    Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
        #endif 
        return false;
    }

    tmpPicture = 0;

    if (c->pix_fmt != PIX_FMT_YUV420P) {
        tmpPicture = allocPicture(PIX_FMT_YUV420P, c->width, c->height);
        if (!tmpPicture) {
            errorMsg = "ffmpeg error: Could not allocate temporary picture. This is not a problem directly related to Tupi. \
                        Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
            #ifdef K_DEBUG
                   tError() << "TFFMpegMovieGenerator::openVideo() - " << errorMsg;
            #endif
            return false;
        }
    }
	
    return true;
}

#define RGBtoYUV(r, g, b, y, u, v) \
  y = (uint8_t)(((int)30*r + (int)59*g + (int)11*b)/100); \
  u = (uint8_t)(((int)-17*r - (int)33*g + (int)50*b + 12800)/100); \
  v = (uint8_t)(((int)50*r - (int)42*g - (int)8*b + 12800)/100);

void TFFMpegMovieGenerator::Private::RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint iRGBIncrement, bool bSwapRGB, int width, int height)
{
    const unsigned iPlaneSize = width * height;
    const unsigned iHalfWidth = width >> 1;

    // get pointers to the data
    uint8_t *yplane  = bufferYUV;
    uint8_t *uplane  = bufferYUV + iPlaneSize;
    uint8_t *vplane  = bufferYUV + iPlaneSize + (iPlaneSize >> 2);
    const uint8_t *bufferRGBIndex = bufferRGB;

    int iRGBIdx[3];
    iRGBIdx[0] = 0;
    iRGBIdx[1] = 1;
    iRGBIdx[2] = 2;
    if (bSwapRGB)  {
        iRGBIdx[0] = 2;
        iRGBIdx[2] = 0;
    }
 
    for (int y = 0; y < (int) height; y++) {
         uint8_t *yline  = yplane + (y * width);
         uint8_t *uline  = uplane + ((y >> 1) * iHalfWidth);
         uint8_t *vline  = vplane + ((y >> 1) * iHalfWidth);

         for (int x=0; x<width; x+=2) {
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += iRGBIncrement;
              yline++;
              RGBtoYUV(bufferRGBIndex[iRGBIdx[0]], bufferRGBIndex[iRGBIdx[1]], bufferRGBIndex[iRGBIdx[2]], *yline, *uline, *vline);
              bufferRGBIndex += iRGBIncrement;
              yline++;
              uline++;
              vline++;
         }
    }
}

bool TFFMpegMovieGenerator::Private::writeVideoFrame(const QImage &image)
{
    #ifdef K_DEBUG
           tWarning() << "TFFMpegMovieGenerator::writeVideoFrame() - Generating frame #" << frameCount;
    #endif

    AVCodecContext *c = video_st->codec;
    AVFrame *picturePtr = 0;
    double nbFrames = ((int)(streamDuration * fps));

    if (frameCount < nbFrames) {
        int w = c->width;
        int h = c->height;

        int size = avpicture_get_size(PIX_FMT_YUV420P, w, h);
        uint8_t *pic_dat = (uint8_t *) av_malloc(size);
        RGBtoYUV420P(image.bits(), pic_dat, image.depth()/8, true, w, h);

        picturePtr = avcodec_alloc_frame();
        picturePtr->quality = 1;

        avpicture_fill((AVPicture *)picturePtr, pic_dat,
                   PIX_FMT_YUV420P, w, h);
    }

    int out_size = -1, ret = -1;

    if (oc->oformat->flags & AVFMT_RAWPICTURE) { // Exporting images array
        AVPacket pkt;
        av_init_packet(&pkt);

        // pkt.flags |= PKT_FLAG_KEY;
	pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index = video_st->index;
        pkt.data= (uint8_t *)picturePtr;
        pkt.size= sizeof(AVPicture);
        
        //ret = av_write_frame(oc, &pkt);
        ret = av_interleaved_write_frame(oc, &pkt);

    } else { // Exporting movies
        out_size = avcodec_encode_video(c, videOutbuf, videOutbufSize, picturePtr);

        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            if (c->coded_frame->pts != (int64_t) AV_NOPTS_VALUE) 
                pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);

            if (c->coded_frame->key_frame)
		pkt.flags |= AV_PKT_FLAG_KEY;
                // pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index = video_st->index;
            pkt.data = videOutbuf;
            pkt.size = out_size;

            /* write the compressed frame in the media file */
            ret = av_interleaved_write_frame(oc, &pkt);
            //ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
   }

   if (ret != 0) {
       errorMsg = "ffmpeg error: Could not write video frame. This is not a problem directly related to Tupi. \
                   Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";

       #ifdef K_DEBUG
              tError() << "TFFMpegMovieGenerator::writeVideoFrame() - " << errorMsg;
       #endif
       return false;
   }

   frameCount++;

   return true;
}

void TFFMpegMovieGenerator::Private::closeVideo(AVStream *st)
{
    AVCodecContext *c;
    c = st->codec;
    avcodec_close(c);
    av_free(picture->data[0]);
    av_free(picture);

    if (tmpPicture) {
        av_free(tmpPicture->data[0]);
        av_free(tmpPicture);
    }
    av_free(videOutbuf);
}

TFFMpegMovieGenerator::TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, int width, int height, int fps, double duration)
 : TMovieGenerator(width, height), k(new Private)
{
    k->movieFile = QDir::tempPath() + QDir::separator() + "tupi_video_" + TAlgorithm::randomString(12);
    k->chooseFileExtension(format);
    k->fps = fps;
    k->streamDuration = duration;
    k->exception = begin();
}

TFFMpegMovieGenerator::TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, const QSize &size, int fps, double duration) : TMovieGenerator(size.width(), size.height()), k(new Private)
{
    k->movieFile = QDir::tempPath() + QDir::separator() + "tupi_video_" + TAlgorithm::randomString(12);
    k->chooseFileExtension(format);
    k->fps = fps;
    k->streamDuration = duration;
    k->exception = begin();
}

TFFMpegMovieGenerator::~TFFMpegMovieGenerator()
{
    if (QFile::exists(k->movieFile)) 
        QFile::remove(k->movieFile);

    delete k;
}

bool TFFMpegMovieGenerator::begin()
{
    av_register_all();

    #ifdef K_LUCID
        k->fmt = guess_format(0, k->movieFile.toLocal8Bit().data(), 0);
    #else
        k->fmt = av_guess_format(0, k->movieFile.toLocal8Bit().data(), 0);
    #endif

    if (!k->fmt) {
        #ifdef K_LUCID
            k->fmt = guess_format("mpeg", NULL, NULL);
        #else
            k->fmt = av_guess_format("mpeg", NULL, NULL);
        #endif
    }

    if (! k->fmt) {
        k->errorMsg = "ffmpeg error: Cannot find a valid format for " + k->movieFile.toLocal8Bit() + ". This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }

    k->oc = avformat_alloc_context();

    if (!k->oc) {
        k->errorMsg = "ffmpeg error: Error while doing export. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }
	
    k->oc->oformat = k->fmt;
    snprintf(k->oc->filename, sizeof(k->oc->filename), "%s", k->movieFile.toLocal8Bit().data());
    k->video_st = addVideoStream(k->oc, k->fmt->video_codec, width(), height(), k->fps, k->errorMsg);
	
    if (!k->video_st) {
        k->errorMsg = "ffmpeg error: Can't add video stream. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }

    /*
    // if (av_set_parameters(k->oc, 0) < 0) {
    if (avformat_write_header(k->oc, NULL) < 0) {
        k->errorMsg = "ffmpeg error: Invalid output format parameters. This is not a problem directly related to Tupi. \
                       Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
        #endif
        return false;
    }
    */

    // dump_format(k->oc, 0, k->movieFile.toLocal8Bit().data(), 1);
    av_dump_format(k->oc, 0, k->movieFile.toLocal8Bit().data(), 1);

    if (!k->openVideo(k->oc, k->video_st)) {
        #ifdef K_DEBUG
               tError() << "TFFMpegMovieGenerator::begin() - [ Fatal Error ] - Can't open video";
        #endif
        return false;
    }

    if (!(k->fmt->flags & AVFMT_NOFILE)) {
        // if (url_fopen(&k->oc->pb, k->movieFile.toLocal8Bit().data(), URL_WRONLY) < 0) {
        if (avio_open(&k->oc->pb, k->movieFile.toLocal8Bit().data(), URL_WRONLY) < 0) {
            k->errorMsg = "ffmpeg error: Could not open " + k->movieFile.toLocal8Bit() + ". This is not a problem directly related to Tupi. \
                           Please, check your ffmpeg installation and codec support. More info: http://ffmpeg.org/";
            #ifdef K_DEBUG
                   tError() << "TFFMpegMovieGenerator::begin() - " << k->errorMsg;
            #endif
            return false;
        }
    }

    // av_write_header(k->oc);
    avformat_write_header(k->oc, NULL);

    k->video_pts = 0.0;
    k->frameCount = 0;

    return true;
}

bool TFFMpegMovieGenerator::movieHeaderOk() 
{ 
    return k->exception;
}

const char* TFFMpegMovieGenerator::getErrorMsg() 
{
    return k->errorMsg;
}

void TFFMpegMovieGenerator::handle(const QImage& image)
{
    if (k->video_st) 
        k->video_pts = (double)k->video_st->pts.val * k->video_st->time_base.num / k->video_st->time_base.den;
    else 
        k->video_pts = 0.0;

    if (!k->video_st || k->video_pts >= k->streamDuration)
        return;

    k->writeVideoFrame(image);
}

void TFFMpegMovieGenerator::end()
{
    k->closeVideo(k->video_st);
    av_write_trailer(k->oc);

    int streams_total = k->oc->nb_streams;
    for (int i = 0; i < streams_total; i++) {
         av_freep(&k->oc->streams[i]->codec);  
         av_freep(&k->oc->streams[i]);
    }

    if (!(k->fmt->flags & AVFMT_NOFILE)) {
        // url_fclose(k->oc->pb);
        avio_close(k->oc->pb);
    }

    av_free(k->oc);
}


void TFFMpegMovieGenerator::__saveMovie(const QString &fileName)
{
    if (QFile::exists(fileName)) 
        QFile::remove(fileName);

    QFile::copy(k->movieFile, fileName);
}
