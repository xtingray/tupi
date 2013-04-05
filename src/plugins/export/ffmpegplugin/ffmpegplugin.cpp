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

#include "ffmpegplugin.h"

// Tupi Framework
#include "tdebug.h"
#include "tglobal.h"
#include "tffmpegmoviegenerator.h"

#include "tuplayer.h"
#include "tupanimationrenderer.h"

#include <QImage>
#include <QPainter>

FFMpegPlugin::FFMpegPlugin()
{
}

FFMpegPlugin::~FFMpegPlugin()
{
}

QString FFMpegPlugin::key() const
{
    return "Video Formats";
}

TupExportInterface::Formats FFMpegPlugin::availableFormats()
{
    return TupExportInterface::WEBM | TupExportInterface::OGV | TupExportInterface::MPEG | TupExportInterface::SWF | TupExportInterface::AVI 
           | TupExportInterface::RM | TupExportInterface::ASF | TupExportInterface::MOV | TupExportInterface::GIF;
}

TMovieGeneratorInterface::Format FFMpegPlugin::videoFormat(TupExportInterface::Format format)
{
    switch (format) {
            case TupExportInterface::WEBM:
                 {
                   return TFFMpegMovieGenerator::WEBM;
                 }
                 break;
            case TupExportInterface::OGV:
                 {
                   return TFFMpegMovieGenerator::OGV;
                 }
                 break;
            case TupExportInterface::SWF:
                 {
                   return TFFMpegMovieGenerator::SWF;
                 }
                 break;
            case TupExportInterface::MPEG:
                 {
                   return TFFMpegMovieGenerator::MPEG;
                 }
                 break;
            case TupExportInterface::AVI:
                 {
                   return TFFMpegMovieGenerator::AVI;
                 }
                 break;
            case TupExportInterface::RM:
                 {
                   return TFFMpegMovieGenerator::RM;
                 }
                 break;
            case TupExportInterface::MOV:
                 {
                   return TFFMpegMovieGenerator::MOV;
                 }
                 break;
            case TupExportInterface::ASF:
                 {
                   return TFFMpegMovieGenerator::ASF;
                 }
                 break;
            case TupExportInterface::GIF:
                 {
                   return TFFMpegMovieGenerator::GIF;
                 }
                 break;
            case TupExportInterface::PNG:
            case TupExportInterface::JPEG:
            case TupExportInterface::XPM:
            case TupExportInterface::SMIL:
            case TupExportInterface::NONE:
                 {
                   return TFFMpegMovieGenerator::NONE;
                 }
    }

    return TFFMpegMovieGenerator::NONE;
}

bool FFMpegPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<TupScene *> &scenes, TupExportInterface::Format fmt, const QSize &size, int fps)
{
    qreal duration = 0;
    foreach (TupScene *scene, scenes)
             duration += (qreal) scene->framesTotal() / (qreal) fps;

    TFFMpegMovieGenerator *generator = 0;
    TMovieGeneratorInterface::Format format = videoFormat(fmt);

    if (format == TFFMpegMovieGenerator::NONE)
        return false;

    generator = new TFFMpegMovieGenerator(format, size, fps, duration);

    TupAnimationRenderer renderer(color);
    {
         if (!generator->movieHeaderOk()) {
             errorMsg = generator->getErrorMsg();
             #ifdef K_DEBUG
                    tError() << "FFMpegPlugin::exportToFormat() - [ Fatal Error ] - Can't create video -> " << filePath;
             #endif
             delete generator;
             return false;
         }

         QPainter painter(generator);
         painter.setRenderHint(QPainter::Antialiasing, true);

         foreach (TupScene *scene, scenes) {
                  renderer.setScene(scene, size);

                  while (renderer.nextPhotogram()) {
                         renderer.render(&painter);
                         generator->nextFrame();
                         generator->reset();
                  }
         }
    }

    generator->saveMovie(filePath);
    delete generator;

    return true;
}

bool FFMpegPlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, TupScene *scene, const QSize &size)
{
    return false;
}

const char* FFMpegPlugin::getExceptionMsg() {
    return errorMsg;
}

#ifdef HAVE_FFMPEG
Q_EXPORT_PLUGIN( FFMpegPlugin );
#endif

