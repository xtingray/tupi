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

#include "theoramoviegenerator.h"
#include "tdebug.h"
#include "talgorithm.h"

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#include <ogg/ogg.h>
#include <theora/codec.h>
#include <theora/theoraenc.h>
}
#endif

#include <QDir>
#include <QFile>
#include <QByteArray>

struct TheoraMovieGenerator::Private
{
    int fps;
    unsigned int width;
    unsigned int height; 
    double streamDuration;
    int frames;
    int framesCounter;
    QString movieFile;
    bool exception;
    const char *errorMsg;

    FILE *ogg_fp;
    ogg_stream_state ogg_os;
    ogg_packet op;
    ogg_page og;
    th_enc_ctx *td;
    th_info ti;
    th_comment tc;
};

TheoraMovieGenerator::TheoraMovieGenerator(const QSize &size, int fps, double duration, int frames) : TMovieGenerator(size.width(), size.height()), k(new Private)
{
    k->width = size.width();
    k->height = size.height();
    k->fps = fps;
    k->streamDuration = duration;
    k->frames = frames;
    k->movieFile = QDir::tempPath() + QDir::separator() + "tupi_video_" + TAlgorithm::randomString(12);
    k->movieFile += ".ogv";
    k->framesCounter = 0;

    k->exception = begin();
}

TheoraMovieGenerator::~TheoraMovieGenerator()
{
    if (QFile::exists(k->movieFile)) 
        QFile::remove(k->movieFile);

    delete k;
}

bool TheoraMovieGenerator::begin()
{ 
    QByteArray string = k->movieFile.toLocal8Bit();
    const char *file = string.data();

    k->ogg_fp = fopen(file, "wb");
    if (!k->ogg_fp) {
        tError() << "TheoraMovieGenerator::begin() - Error: couldn't open video file";
        return false;
    }

    srand(time(NULL));
    if (ogg_stream_init(&k->ogg_os, rand())) {
        tError() << "TheoraMovieGenerator::begin() - Error: couldn't create ogg stream state";
        return false;
    }

    th_info_init(&k->ti);
    k->ti.frame_width = ((k->width + 15) >> 4) << 4;
    k->ti.frame_height = ((k->height + 15) >> 4) << 4;
    k->ti.pic_width = k->width;
    k->ti.pic_height = k->height;
    k->ti.pic_x = 0;
    k->ti.pic_y = 0;
    k->ti.fps_numerator = k->fps;
    k->ti.fps_denominator = 1;
    k->ti.aspect_numerator = 0;
    k->ti.aspect_denominator = 0;
    k->ti.colorspace = TH_CS_UNSPECIFIED;
    k->ti.pixel_fmt = TH_PF_420;
    k->ti.target_bitrate = 6000000;
    k->ti.quality = 0;
    k->ti.keyframe_granule_shift = 6;

    k->td = th_encode_alloc(&k->ti);
    th_info_clear(&k->ti);

    ogg_uint32_t keyframe_frequency = 64;
    int buf_delay = -1;

    int ret = th_encode_ctl(k->td, TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE, &keyframe_frequency, sizeof(keyframe_frequency - 1));

    if (ret < 0) {
        tError() << "TheoraMovieGenerator::begin() - Error: could not set keyframe interval to " << (int)keyframe_frequency;
    }

    int arg = TH_RATECTL_CAP_UNDERFLOW;
    ret = th_encode_ctl(k->td, TH_ENCCTL_SET_RATE_FLAGS, &arg, sizeof(arg));
    if (ret < 0)
        tError() << "TheoraMovieGenerator::begin() - Error: could not set encoder flags for soft encoding";

    if (buf_delay < 0) {
        if (((int) keyframe_frequency*7>>1) > 5*k->fps/1)
            arg = keyframe_frequency*7>>1;
        else
            arg = 5*k->fps;

        ret = th_encode_ctl(k->td, TH_ENCCTL_SET_RATE_BUFFER, &arg, sizeof(arg));
        if (ret < 0)
            tError() << "TheoraMovieGenerator::begin() - Error: Could not set rate control buffer for soft encoding";
    }

    if (buf_delay >= 0) {
        ret = th_encode_ctl(k->td, TH_ENCCTL_SET_RATE_BUFFER, &buf_delay, sizeof(buf_delay));
        if (ret < 0) {
            tError() << "TheoraMovieGenerator::begin() - Error: could not set desired buffer delay";
        }
    }

    th_comment_init(&k->tc);

    if (th_encode_flushheader(k->td, &k->tc, &k->op) <= 0){
        tError() << "TheoraMovieGenerator::begin() - Error: internal Theora library error";
        return false;
    }

    th_comment_clear(&k->tc);

    ogg_stream_packetin(&k->ogg_os, &k->op);
    if (ogg_stream_pageout(&k->ogg_os, &k->og) != 1) {
        tError() << "TheoraMovieGenerator::begin() - Error: internal Ogg library error";
        return false;
    }
    fwrite(k->og.header, 1, k->og.header_len, k->ogg_fp);
    fwrite(k->og.body, 1, k->og.body_len, k->ogg_fp);

    for(;;) {
        ret = th_encode_flushheader(k->td, &k->tc, &k->op);
        if (ret < 0) {
            tError() << "TheoraMovieGenerator::begin() - Error: internal Theora library error";
            return false;
        } else if (!ret)
                   break;

        ogg_stream_packetin(&k->ogg_os, &k->op);
    }

    for(;;) {
        int result = ogg_stream_flush(&k->ogg_os, &k->og);
        if (result < 0) {
            tError() << "TheoraMovieGenerator::begin() - Error: internal Ogg library error";
            return false;
        }
        if (result == 0)
            break;

        fwrite(k->og.header, 1, k->og.header_len, k->ogg_fp);
        fwrite(k->og.body, 1, k->og.body_len, k->ogg_fp);
    }

    return true;
}

bool TheoraMovieGenerator::movieHeaderOk() 
{ 
    return k->exception;
}

const char* TheoraMovieGenerator::getErrorMsg() 
{
    return k->errorMsg;
}

void TheoraMovieGenerator::writeTheoraFrame(unsigned long w, unsigned long h, unsigned char *yuv, int last)
{
    int chroma_format = TH_PF_420;
    th_ycbcr_buffer ycbcr;
    ogg_packet op;
    ogg_page og;

    unsigned long yuv_w;
    unsigned long yuv_h;

    unsigned char *yuv_y;
    unsigned char *yuv_u;
    unsigned char *yuv_v;

    unsigned int x;
    unsigned int y;

    yuv_w = (w + 15) & ~15;
    yuv_h = (h + 15) & ~15;

    ycbcr[0].width = yuv_w;
    ycbcr[0].height = yuv_h;
    ycbcr[0].stride = yuv_w;
    ycbcr[1].width = (chroma_format == TH_PF_444) ? yuv_w : (yuv_w >> 1);
    ycbcr[1].stride = ycbcr[1].width;
    ycbcr[1].height = (chroma_format == TH_PF_420) ? (yuv_h >> 1) : yuv_h;
    ycbcr[2].width = ycbcr[1].width;
    ycbcr[2].stride = ycbcr[1].stride;
    ycbcr[2].height = ycbcr[1].height;

    ycbcr[0].data = yuv_y = (unsigned char*) malloc(ycbcr[0].stride * ycbcr[0].height);
    ycbcr[1].data = yuv_u = (unsigned char*) malloc(ycbcr[1].stride * ycbcr[1].height);
    ycbcr[2].data = yuv_v = (unsigned char*) malloc(ycbcr[2].stride * ycbcr[2].height);

    for (y = 0; y < h; y++) {
         for (x = 0; x < w; x++) {
              yuv_y[x + y * yuv_w] = yuv[3 * (x + y * w) + 0];
         }
    }

    if (chroma_format == TH_PF_420) {
        for (y = 0; y < h; y += 2) {
             for (x = 0; x < w; x += 2) {
                  yuv_u[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * w) + 1];
                  yuv_v[(x >> 1) + (y >> 1) * (yuv_w >> 1)] = yuv[3 * (x + y * w) + 2];
             }
        }
    } else if (chroma_format == TH_PF_444) {
               for (y = 0; y < h; y++) {
                    for (x = 0; x < w; x++) {
                         yuv_u[x + y * ycbcr[1].stride] = yuv[3 * (x + y * w) + 1];
                         yuv_v[x + y * ycbcr[2].stride] = yuv[3 * (x + y * w) + 2];
                    }
               }
    } else {  // TH_PF_422
        for (y = 0; y < h; y += 1) {
             for (x = 0; x < w; x += 2) {
                  yuv_u[(x >> 1) + y * ycbcr[1].stride] = yuv[3 * (x + y * w) + 1];
                  yuv_v[(x >> 1) + y * ycbcr[2].stride] = yuv[3 * (x + y * w) + 2];
             }
        }
    }

    if (th_encode_ycbcr_in(k->td, ycbcr)) {
        tError() << "TheoraMovieGenerator::theoraWriteFrame() - Error: could not encode frame";
        return;
    }

    if (!th_encode_packetout(k->td, last, &op)) {
        tError() << "TheoraMovieGenerator::theoraWriteFrame() - Error: could not read packets";
        return;
    }

    ogg_stream_packetin(&k->ogg_os, &op);
    while (ogg_stream_pageout(&k->ogg_os, &og)) {
           fwrite(og.header, og.header_len, 1, k->ogg_fp);
           fwrite(og.body, og.body_len, 1, k->ogg_fp);
    }

    free(yuv_y);
    free(yuv_u);
    free(yuv_v);
}

unsigned char TheoraMovieGenerator::clamp(double d)
{
    if (d < 0)
        return 0;

    if (d > 255)
        return 255;

    return d;
}

void TheoraMovieGenerator::handle(const QImage &image)
{
    k->framesCounter++;
    QByteArray imageBytes((char *) image.bits(), image.byteCount());

    unsigned char *yuv = (unsigned char *) malloc(k->width * k->height * 3);
    unsigned int x;
    unsigned int y;

    for (y = 0; y < k->height; y++) {
         for (x = 0; x < k->width; x++) {
              uint8_t r;
              uint8_t g;
              uint8_t b;

              b = (uint8_t) imageBytes.at((y*k->width*4) + (4 * x) + 0);
              g = (uint8_t) imageBytes.at((y*k->width*4) + (4 * x) + 1);
              r = (uint8_t) imageBytes.at((y*k->width*4) + (4 * x) + 2);

              yuv[3 * (x + k->width * y)] = clamp(0.299 * r + 0.587 * g + 0.114 * b);
              yuv[3 * (x + k->width * y) + 1] = clamp((0.436 * 255 - 0.14713 * r - 0.28886 * g + 0.436 * b) / 0.872);
              yuv[3 * (x + k->width * y) + 2] = clamp((0.615 * 255 + 0.615 * r - 0.51499 * g - 0.10001 * b) / 1.230);
         }
    }

    #ifdef K_DEBUG
           tWarning() << "TheoraMovieGenerator::handle() -  Writing theora frame #" + QString::number(k->framesCounter);
    #endif

    int last = 0;
    if (k->framesCounter == k->frames) 
        last = 1;

    writeTheoraFrame(k->width, k->height, yuv, last);
    free(yuv);
}

void TheoraMovieGenerator::end()
{
    th_encode_free(k->td);

    if (ogg_stream_flush(&k->ogg_os, &k->og)) {
        fwrite(k->og.header, k->og.header_len, 1, k->ogg_fp);
        fwrite(k->og.body, k->og.body_len, 0, k->ogg_fp);
    }

    if (k->ogg_fp) {
        fflush(k->ogg_fp);
        if (k->ogg_fp != stdout)
            fclose(k->ogg_fp);
    }

    ogg_stream_clear(&k->ogg_os);
}

void TheoraMovieGenerator::__saveMovie(const QString &fileName)
{
    if (QFile::exists(fileName)) 
        QFile::remove(fileName);

    QFile::copy(k->movieFile, fileName);
}
