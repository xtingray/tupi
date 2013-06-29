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

#ifdef __cplusplus
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ogg/ogg.h>
#include <theora/codec.h>
#include <theora/theoraenc.h>
}
#endif

int main()
{
    static FILE *ogg_fp = NULL;
    static ogg_stream_state ogg_os;
    static ogg_packet op;  
    static ogg_page og;

    static th_enc_ctx *td;
    static th_info ti;
    th_comment tc;

    int buf_delay=-1;
    static int video_fps_numerator = 24;
    static int video_fps_denominator = 1;
    ogg_uint32_t keyframe_frequency = 64;

    unsigned int w = 520;
    unsigned int h = 380;

    // Starting process

    ogg_fp = fopen("test.ogv", "wb");
    if (!ogg_fp) {
        fprintf(stderr, "error creating ogv file\n");
        return 1;
    }

    srand(time(NULL));
    if (ogg_stream_init(&ogg_os, rand())) {
        fprintf(stderr, "error: couldn't create ogg stream state\n");
        return 1;
    }

    th_info_init(&ti);
    ti.frame_width = 528;
    ti.frame_height = 384;
    ti.pic_width = w;
    ti.pic_height = h;
    ti.pic_x = 0;
    ti.pic_y = 0;
    ti.fps_numerator = 24;
    ti.fps_denominator = 1;
    ti.aspect_numerator = 0;
    ti.aspect_denominator = 0;
    ti.colorspace = TH_CS_UNSPECIFIED;
    ti.pixel_fmt = TH_PF_444;
    ti.target_bitrate = 6000000;
    ti.quality = 0;
    ti.keyframe_granule_shift = 6;

    td = th_encode_alloc(&ti);
    th_info_clear(&ti);

    int ret = th_encode_ctl(td, TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE, &keyframe_frequency, sizeof(keyframe_frequency-1));
    if (ret < 0) {
        fprintf(stderr, "Could not set keyframe interval to %d.\n", (int)keyframe_frequency);
    }

    int arg = TH_RATECTL_CAP_UNDERFLOW;
    ret = th_encode_ctl(td, TH_ENCCTL_SET_RATE_FLAGS, &arg, sizeof(arg));
    if (ret < 0)
        fprintf(stderr,"Could not set encoder flags for --soft-target\n");

    if (buf_delay < 0) {
        int var1 = keyframe_frequency*7>>1;
        int var2 = 5*video_fps_numerator/video_fps_denominator;
        if (var1 > var2)
            arg = keyframe_frequency*7>>1;
        else
            arg = 5*video_fps_numerator/video_fps_denominator;
        ret = th_encode_ctl(td, TH_ENCCTL_SET_RATE_BUFFER, &arg, sizeof(arg));

        if (ret < 0)
            fprintf(stderr,"Could not set rate control buffer for --soft-target\n");
    }

    if (buf_delay >= 0) {
        ret = th_encode_ctl(td, TH_ENCCTL_SET_RATE_BUFFER, &buf_delay, sizeof(buf_delay));
        if (ret < 0) {
            fprintf(stderr,"Warning: could not set desired buffer delay.\n");
        }
    }

    /* write the bitstream header packets with proper page interleave */
    th_comment_init(&tc);

    /* first packet will get its own page automatically */
    if (th_encode_flushheader(td, &tc, &op) <= 0){
        fprintf(stderr,"Internal Theora library error.\n");
        exit(1);
    }

    ogg_stream_packetin(&ogg_os,&op);
    if (ogg_stream_pageout(&ogg_os, &og) != 1) {
        fprintf(stderr,"Internal Ogg library error.\n");
        exit(1);
    }
    fwrite(og.header, 1, og.header_len, ogg_fp);
    fwrite(og.body, 1, og.body_len, ogg_fp);

    /* create the remaining theora headers */
    for(;;) {
        ret = th_encode_flushheader(td, &tc, &op);
        if (ret < 0) {
            fprintf(stderr,"Internal Theora library error.\n");
            exit(1);
        } else if (!ret)
                   break;

        ogg_stream_packetin(&ogg_os, &op);
    }

    for(;;) {
        int result = ogg_stream_flush(&ogg_os, &og);
        if (result < 0) {
            /* can't get here */
            fprintf(stderr,"Internal Ogg library error.\n");
            exit(1);
        }
        if (result == 0)
            break;
        fwrite(og.header, 1, og.header_len, ogg_fp);
        fwrite(og.body, 1, og.body_len, ogg_fp);
    }

    // Ending process 

    th_encode_free(td);

    if (ogg_stream_flush(&ogg_os, &og)) {
        fwrite(og.header, og.header_len, 1, ogg_fp);
        fwrite(og.body, og.body_len, 1, ogg_fp);
    } 

    if (ogg_fp) {
        fflush(ogg_fp);
        if (ogg_fp != stdout)
            fclose(ogg_fp);
    }

    ogg_stream_clear(&ogg_os);

    return 0;
}
