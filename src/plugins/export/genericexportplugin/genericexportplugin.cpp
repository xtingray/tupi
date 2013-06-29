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

#include "genericexportplugin.h"
#include "tuplayer.h"
#include "tupanimationrenderer.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QImage>
#include <QPainter>

GenericExportPlugin::GenericExportPlugin()
{
}

GenericExportPlugin::~GenericExportPlugin()
{
}

QString GenericExportPlugin::key() const
{
    return "Image Arrays";
}

TupExportInterface::Formats GenericExportPlugin::availableFormats()
{
    return TupExportInterface::PNG | TupExportInterface::JPEG | TupExportInterface::XPM;
}

bool GenericExportPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<TupScene *> &scenes, 
                                         TupExportInterface::Format format, const QSize &size, int fps)
{
    Q_UNUSED(fps);

    QFileInfo fileInfo(filePath);

    QDir dir = fileInfo.dir();
    if (!dir.exists())
        dir.mkdir(dir.path());

    m_baseName = fileInfo.baseName();
    const char *extension = "";
    QImage::Format imageFormat;
    switch (format) {
            case TupExportInterface::JPEG:
                 extension = "JPEG";
                 imageFormat = QImage::Format_RGB32;
                 break;
            case TupExportInterface::PNG:
                 extension = "PNG";
                 imageFormat = QImage::Format_ARGB32;
                 break;
            case TupExportInterface::XPM:
                 extension = "XPM";
                 imageFormat = QImage::Format_RGB32;
                 break;
            default:
                 break;
    }

    TupAnimationRenderer renderer(color);

    foreach (TupScene *scene, scenes) {
             renderer.setScene(scene, size);

             int photogram = 0;
             while (renderer.nextPhotogram()) {
                    QImage image(size, imageFormat);
                    {
                     QPainter painter(&image);
                     painter.setRenderHint(QPainter::Antialiasing, true);
                     renderer.render(&painter);
                    }

                    QString index = "";
                    if (photogram < 10) {
                        index = "000";
                    } else if (photogram < 100) {
                               index = "00";
                    } else {
                        index = "0";
                    }

                    index += QString("%1").arg(photogram);

                    image.save(fileInfo.absolutePath() + QDir::separator() + QString(m_baseName + "%1.%2").arg(index).arg(QString(extension).toLower()), extension);

                    photogram++;
             }
    }

    return true;
}

bool GenericExportPlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, TupScene *scene, const QSize &size)
{
    QString path = filePath;
    const char *extension;
    QImage::Format imageFormat;
    QColor bgColor = color;

    if (filePath.endsWith(".PNG", Qt::CaseInsensitive)) {
        extension = "PNG";
        imageFormat = QImage::Format_ARGB32;
        bgColor.setAlpha(0);
    } else if (filePath.endsWith(".JPG", Qt::CaseInsensitive) || filePath.endsWith("JPEG", Qt::CaseInsensitive)) {
               extension = "JPG";
               imageFormat = QImage::Format_RGB32;
    } else {
        extension = "PNG"; 
        path += ".png";
        imageFormat = QImage::Format_ARGB32;
    }

    TupAnimationRenderer renderer(bgColor);
    renderer.setScene(scene, size);

    renderer.renderPhotogram(frameIndex);
    QImage image(size, imageFormat);

    {
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        renderer.render(&painter);
    }

    return image.save(path, extension);
}

const char* GenericExportPlugin::getExceptionMsg() {
    return errorMsg;
}

Q_EXPORT_PLUGIN(GenericExportPlugin);

