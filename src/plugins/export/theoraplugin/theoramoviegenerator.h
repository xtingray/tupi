/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#ifndef THEORAMOVIEGENERATOR_H
#define THEORAMOVIEGENERATOR_H

#include "tglobal.h"
#include "tmoviegenerator.h"
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

/**
    @author Gustav Gonzalez 
*/

class TUPI_EXPORT TheoraMovieGenerator : public TMovieGenerator
{
    public:
        TheoraMovieGenerator(const QSize &size, int fps = 24, double duration = 0, int frames = 0);
        ~TheoraMovieGenerator();
        virtual bool movieHeaderOk();
        virtual const char* getErrorMsg();

    protected:
        void createMovieFile(const QString &fileName);
        virtual void handle(const QImage &image);
        virtual bool begin();
        virtual void end();

    private:
        void writeTheoraFrame(unsigned long w, unsigned long h, unsigned char *yuv, int last);
        unsigned char clamp(double d);
        struct Private;
        Private *const k;
};

#endif
