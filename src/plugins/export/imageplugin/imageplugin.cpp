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

#include "imageplugin.h"

ImagePlugin::ImagePlugin()
{
}

ImagePlugin::~ImagePlugin()
{
}

QString ImagePlugin::key() const
{
    return tr("Image Sequence");
}

TupExportInterface::Formats ImagePlugin::availableFormats()
{
    return TupExportInterface::PNG | TupExportInterface::JPEG | TupExportInterface::SVG | TupExportInterface::XPM;
}

bool ImagePlugin::exportToFormat(const QColor bgColor, const QString &filePath, const QList<TupScene *> &scenes, 
                                         TupExportInterface::Format format, const QSize &size, int fps, TupLibrary *library)
{
    Q_UNUSED(fps);

    QFileInfo fileInfo(filePath);

    QDir dir = fileInfo.dir();
    if (!dir.exists())
        dir.mkdir(dir.path());

    m_baseName = fileInfo.baseName();
    const char *extension = "";
    QImage::Format imageFormat = QImage::Format_RGB32;
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
            case TupExportInterface::SVG:
                 extension = "SVG";
                 break;
            default:
                 imageFormat = QImage::Format_RGB32;
                 break;
    }

    TupAnimationRenderer renderer(bgColor, library);

    foreach (TupScene *scene, scenes) {
             renderer.setScene(scene, size);

             int photogram = 0;
             while (renderer.nextPhotogram()) {
                    QString index = "";
                    if (photogram < 10) {
                        index = "000";
                    } else if (photogram < 100) {
                               index = "00";
                    } else {
                        index = "0";
                    }

                    index += QString("%1").arg(photogram);

                    if (QString(extension).compare("SVG") == 0) {
                        QString path = fileInfo.absolutePath() + QDir::separator() + QString(m_baseName + "%1.%2").arg(index).arg(QString(extension).toLower());

                        QSvgGenerator generator;
                        generator.setFileName(path);
                        generator.setSize(size);
                        generator.setViewBox(QRect(0, 0, size.width(), size.height()));
                        QFileInfo info(path);
                        generator.setTitle(info.fileName());
                        generator.setDescription(scene->sceneName());

                        QPainter painter;
                        painter.begin(&generator);
                        painter.setRenderHint(QPainter::Antialiasing, true);
                        renderer.render(&painter);
                        painter.end();
                    } else {
                       QImage image(size, imageFormat);
                       if (bgColor.alpha() == 0)
                           image.fill(Qt::transparent);
                       else
                           image.fill(bgColor);

                       {
                           QPainter painter(&image);
                           painter.setRenderHint(QPainter::Antialiasing, true);
                           renderer.render(&painter);
                       }

                       image.save(fileInfo.absolutePath() + QDir::separator() + QString(m_baseName + "%1.%2").arg(index).arg(QString(extension).toLower()), extension, 100);
                    }          

                    photogram++;
             }
    }

    return true;
}

bool ImagePlugin::exportFrame(int frameIndex, const QColor color, const QString &filePath, TupScene *scene, const QSize &size, TupLibrary *library)
{
    bool result = false;
    QString path = filePath;
    const char *extension;
    QImage::Format imageFormat;
    QColor bgColor = color;
    bgColor.setAlpha(255);

    TupAnimationRenderer renderer(bgColor, library);
    renderer.setScene(scene, size);
    renderer.renderPhotogram(frameIndex);

    if (filePath.endsWith(".SVG", Qt::CaseInsensitive)) {
        QSvgGenerator generator;
        generator.setFileName(path);
        generator.setSize(size);
        generator.setViewBox(QRect(0, 0, size.width(), size.height()));
        QFileInfo info(path);
        generator.setTitle(info.fileName());
        generator.setDescription(scene->sceneName());

        QPainter painter;
        painter.begin(&generator);
        painter.setRenderHint(QPainter::Antialiasing, true);
        renderer.render(&painter);
        result = painter.end();
    } else {
        if (filePath.endsWith(".PNG", Qt::CaseInsensitive)) {
            extension = "PNG";
            imageFormat = QImage::Format_ARGB32;
        } else if (filePath.endsWith(".JPG", Qt::CaseInsensitive) || filePath.endsWith("JPEG", Qt::CaseInsensitive)) {
                   extension = "JPEG";
                   imageFormat = QImage::Format_RGB32;
        } else {
            extension = "PNG"; 
            path += ".png";
            imageFormat = QImage::Format_ARGB32;
        }

        QImage image(size, imageFormat);
        {
            QPainter painter(&image);
            painter.setRenderHint(QPainter::Antialiasing, true);
            renderer.render(&painter);
        }

        result = image.save(path, extension, 100);
    }

    return result;
}

QString ImagePlugin::getExceptionMsg() const {
    return errorMsg;
}

// Q_EXPORT_PLUGIN(ImagePlugin);

