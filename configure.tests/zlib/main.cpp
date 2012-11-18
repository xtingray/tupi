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

#include <stdio.h>
#include <string.h>
#include "zlib.h"

int main()
{
    char sentence[50] = "Tupi: Open 2D Magic!";
    char compressedSentence[50];
    char uncompressedSentence[50];

    z_stream compression;
    compression.zalloc = Z_NULL;
    compression.zfree = Z_NULL;
    compression.opaque = Z_NULL;
    compression.avail_in = (uInt)strlen(sentence) + 1;
    compression.next_in = (Bytef *)sentence;
    compression.avail_out = (uInt)sizeof(compressedSentence);
    compression.next_out = (Bytef *)compressedSentence;

    deflateInit(&compression, Z_DEFAULT_COMPRESSION);
    deflate(&compression, Z_FINISH);
    deflateEnd(&compression);

    z_stream uncompression;
    uncompression.zalloc = Z_NULL;
    uncompression.zfree = Z_NULL;
    uncompression.opaque = Z_NULL;
    uncompression.avail_in = (uInt)((char*)compression.next_out - compressedSentence);
    uncompression.next_in = (Bytef *) compressedSentence;
    uncompression.avail_out = (uInt)sizeof(uncompressedSentence);
    uncompression.next_out = (Bytef *)uncompressedSentence;

    inflateInit(&uncompression);
    inflate(&uncompression, Z_NO_FLUSH);
    inflateEnd(&uncompression);
}
