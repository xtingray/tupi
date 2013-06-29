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

#ifndef TFFMPEGMOVIEGENERATOR_H
#define TFFMPEGMOVIEGENERATOR_H

#include "tmoviegenerator.h"

/**
    @author David Cuadrado
*/

class TFFMpegMovieGenerator : public TMovieGenerator
{
    public:
        TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, int width, int height, int fps = 24, double duration = 0);
        TFFMpegMovieGenerator(TMovieGeneratorInterface::Format format, const QSize &size, int fps = 24, double duration = 0);
        ~TFFMpegMovieGenerator();
        virtual bool movieHeaderOk();
        virtual const char* getErrorMsg();

    protected:
        void __saveMovie(const QString &fileName);
        virtual void handle(const QImage &image);
        virtual bool begin();
        virtual void end();

    private:
        struct Private;
        Private *const k;
};

#endif
