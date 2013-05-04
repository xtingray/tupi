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
 *   the Free Software Foundation; either version 3 of the License, or     *
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
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
}
#endif

#include <iostream>

int main()
{
    av_register_all();

    #if defined(K_LUCID)
        AVOutputFormat *fmt = guess_format("mpeg", NULL, NULL);
    #else
        AVOutputFormat *fmt = av_guess_format("mpeg", NULL, NULL);
    #endif

    AVFormatContext *oc = avformat_alloc_context();
    oc->oformat = fmt;

    AVCodecContext *c;
    AVStream *st;

    // st = av_new_stream(oc, 0);
    st = avformat_new_stream(oc, 0);

    c = st->codec;
    c->time_base.den = 24;
    c->time_base.num = 1;
    c->gop_size = 12;
    c->pix_fmt = PIX_FMT_YUV420P;

    av_free(oc);

    return 0;
}

