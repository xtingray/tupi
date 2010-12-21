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

#include "genericexportplugin.h"

#include "kglobal.h"
#include "kdebug.h"

#include <QImage>
#include <QPainter>

#include "ktlayer.h"
#include "ktanimationrenderer.h"

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

KTExportInterface::Formats GenericExportPlugin::availableFormats()
{
    return KTExportInterface::PNG | KTExportInterface::JPEG | KTExportInterface::XPM;
}

bool GenericExportPlugin::exportToFormat(const QColor color, const QString &filePath, const QList<KTScene *> &scenes, 
                                         KTExportInterface::Format format, const QSize &size, int fps)
{
    QFileInfo fileInfo(filePath);

    QDir dir = fileInfo.dir();
    if (!dir.exists())
        dir.mkdir(dir.path());

    m_baseName = fileInfo.baseName();
    const char *fmt = "PNG";

    switch (format) {
            case KTExportInterface::JPEG:
                 fmt = "JPEG";
                 break;
            case KTExportInterface::XPM:
                 fmt = "XPM";
                 break;
            default:
                 break;
    }

    KTAnimationRenderer renderer(color);

    foreach (KTScene *scene, scenes) {
             renderer.setScene(scene);

             int photogram = 0;
             while (renderer.nextPhotogram()) {
                    QImage img(size, QImage::Format_RGB32);
                    {
                     QPainter painter(&img);
                     painter.setRenderHint(QPainter::Antialiasing, true);
                     renderer.render(&painter);
                    }
                    QString index = "";
                    if (photogram < 10)
                        index = "0";

                    index += QString("%1").arg(photogram);

                    img.save(fileInfo.absolutePath() + "/" + QString(m_baseName + "%1.%2").arg(index).arg(QString(fmt).toLower()), fmt);

                    photogram++;
             }
    }

    return true;
}

const char* GenericExportPlugin::getExceptionMsg() {
    return errorMsg;
}

Q_EXPORT_PLUGIN(GenericExportPlugin);

