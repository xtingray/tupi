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

#include "apngplugin.h"

APNGPlugin::APNGPlugin()
{
}

APNGPlugin::~APNGPlugin()
{
}

QString APNGPlugin::key() const
{
    return "Animated Image";
}

TupExportInterface::Formats APNGPlugin::availableFormats()
{
    return TupExportInterface::APNG;
}

bool APNGPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<TupScene *> &scenes, TupExportInterface::Format fmt, const QSize &size, int fps)
{
    Q_UNUSED(color);
    Q_UNUSED(filePath);
    Q_UNUSED(scenes);
    Q_UNUSED(fmt);
    Q_UNUSED(size);
    Q_UNUSED(fps);

    /*
    int frames = 0;
    qreal duration = 0;
    foreach (TupScene *scene, scenes) {
             duration += (qreal) scene->framesCount() / (qreal) fps;
             frames += scene->framesCount();
    }

    TheoraMovieGenerator *generator = 0;
    generator = new TheoraMovieGenerator(size, fps, duration, frames);

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
    */

    return true;
}

bool APNGPlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, TupScene *scene, const QSize &size)
{
    Q_UNUSED(frameIndex);
    Q_UNUSED(color);
    Q_UNUSED(filePath);
    Q_UNUSED(scene);
    Q_UNUSED(size);

    return false;
}

QString APNGPlugin::getExceptionMsg() const {
    return errorMsg;
}

// #ifdef HAVE_THEORA
//        Q_EXPORT_PLUGIN( APNGPlugin );
// #endif
