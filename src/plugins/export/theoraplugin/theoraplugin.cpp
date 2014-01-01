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

#include "theoraplugin.h"

// Tupi Framework
#include "tdebug.h"
#include "tglobal.h"
#include "theoramoviegenerator.h"

#include "tuplayer.h"
#include "tupanimationrenderer.h"

#include <QImage>
#include <QPainter>

TheoraPlugin::TheoraPlugin()
{
}

TheoraPlugin::~TheoraPlugin()
{
}

QString TheoraPlugin::key() const
{
    return "Open Video Format";
}

TupExportInterface::Formats TheoraPlugin::availableFormats()
{
    return TupExportInterface::OGV;
}

bool TheoraPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<TupScene *> &scenes, TupExportInterface::Format fmt, const QSize &size, int fps)
{
    Q_UNUSED(fmt);

    int frames = 0;
    qreal duration = 0;
    foreach (TupScene *scene, scenes) {
             duration += (qreal) scene->framesTotal() / (qreal) fps;
             frames += scene->framesTotal();
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

    return true;
}

bool TheoraPlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, TupScene *scene, const QSize &size)
{
    Q_UNUSED(frameIndex);
    Q_UNUSED(color);
    Q_UNUSED(filePath);
    Q_UNUSED(scene);
    Q_UNUSED(size);

    return false;
}

const char* TheoraPlugin::getExceptionMsg() {
    return errorMsg;
}

// #ifdef HAVE_THEORA
//        Q_EXPORT_PLUGIN( TheoraPlugin );
// #endif
