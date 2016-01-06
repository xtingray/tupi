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

#include "tuppaletteimporter.h"

struct TupPaletteImporter::Private
{
    TupPaletteDocument *document;
    QString paletteName;
    QString filePath;
};

TupPaletteImporter::TupPaletteImporter() : k(new Private)
{
}

TupPaletteImporter::~TupPaletteImporter()
{
}

bool TupPaletteImporter::import(const QString &file, PaletteType type)
{
    switch (type) {
            case Gimp:
            {
                 return importGimpPalette(file);
            }
            break;
    }

    return false;
}

bool TupPaletteImporter::importGimpPalette(const QString &file)
{
    QFile input(file);

    if (input.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&input);

        QFileInfo info(file);
        QString extension = info.suffix().toLower();

        if (extension.compare("gpl") == 0) {
            if (! stream.readLine().contains("GIMP Palette")) {
                #ifdef K_DEBUG
                    QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.gpl) -> " + file;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }

            QString line = stream.readLine();
            if (!line.startsWith("Name:")) {
                #ifdef K_DEBUG
                    QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.gpl) -> " + file;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }

            k->paletteName = line.section("Name:", 1).trimmed();
            k->document = new TupPaletteDocument(k->paletteName, false);

            line = stream.readLine();
            while(!line.contains("#"))
                  line = stream.readLine();

            while (!stream.atEnd()) {
                   line = stream.readLine().left(11).trimmed();
                   QStringList rgb = getColorArray(line);

                   if (rgb.count() != 3) {
                       #ifdef K_DEBUG
                           QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.gpl) -> " + line;
                           #ifdef Q_OS_WIN
                               qDebug() << msg;
                           #else
                               tError() << msg;
                           #endif
                       #endif
                       return false;
                   }

                   QColor color;
                   for (int i = 0; i < 3; ++i) {
                        bool ok;
                        int item = rgb.at(i).toInt(&ok, 10);
                        if (ok) {
                            if (i == 0)
                                color.setRed(item);
                            if (i == 1)
                                color.setGreen(item);
                            if (i == 2)
                                color.setBlue(item);
                        } else {
                            #ifdef K_DEBUG
                                QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.gpl) -> " + line;
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif
                            return false;
                        }
                   }

                   if (color.isValid()) {
                       k->document->addColor(color);
                   } else {
                       #ifdef K_DEBUG
                           QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid color format (.gpl) -> " + line;
                           #ifdef Q_OS_WIN
                               qDebug() << msg;
                           #else
                               tError() << msg;
                           #endif
                       #endif
                       return false;
                   }
            }
        } else {
            k->paletteName = info.baseName().toLower();
            k->document = new TupPaletteDocument(k->paletteName, false);

            if (extension.compare("txt") == 0) {
                while (!stream.atEnd()) {
                       QString line = stream.readLine();
                       if (line.startsWith("#")) {
                           QColor color(line);
                           if (color.isValid()) {
                               k->document->addColor(color);
                           } else {
                               #ifdef K_DEBUG
                                   QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid color format (.txt) -> " + line;
                                   #ifdef Q_OS_WIN
                                       qDebug() << msg;
                                   #else
                                       tError() << msg;
                                   #endif
                                #endif
                               return false;
                           }
                       }
                }
            } else if (extension.compare("css") == 0) {
                       while (!stream.atEnd()) {
                              QString line = stream.readLine();
                              int init = line.indexOf("(") + 1;
                              int end = line.indexOf(")");
                              if (init == -1 || end == -1) {
                                  #ifdef K_DEBUG
                                      QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.css) -> " + line;
                                      #ifdef Q_OS_WIN
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return false;
                              }

                              QString text = line.mid(init, end - init); 
                              QStringList rgb = text.split(",");
                              if (rgb.count() != 3) {
                                  #ifdef K_DEBUG
                                      QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.css) -> " + line;
                                      #ifdef Q_OS_WIN
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return false;
                              }

                              QColor color;
                              for (int i = 0; i < 3; ++i) {
                                   bool ok;
                                   int item = rgb.at(i).trimmed().toInt(&ok, 10);
                                   if (ok) {
                                       if (i == 0)
                                           color.setRed(item);
                                       if (i == 1)
                                           color.setGreen(item);
                                       if (i == 2)
                                           color.setBlue(item);
                                   } else {
                                       #ifdef K_DEBUG
                                           QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid palette format (.css) -> " + line;
                                           #ifdef Q_OS_WIN
                                               qDebug() << msg;
                                           #else
                                               tError() << msg;
                                           #endif
                                       #endif
                                       return false;
                                   }
                              }

                              if (color.isValid()) {
                                  k->document->addColor(color);
                              } else {
                                  #ifdef K_DEBUG
                                      QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid color format (.css) -> " + line;
                                      #ifdef Q_OS_WIN
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return false;
                              }
                       }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid extension! -> " + extension;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }

    return true;
}

bool TupPaletteImporter::saveFile(const QString &path)
{
    if (k->paletteName.isNull()) {
        #ifdef K_DEBUG
            QString msg = "TupPaletteImporter::saveFile() - Fatal Error: Palette name is null!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }

    QString pathName = path + QDir::separator() + k->paletteName.replace(" ", "_") + ".tpal";
    QFile file(pathName);
    if (file.exists()) {
        #ifdef K_DEBUG
            QString msg = "TupPaletteImporter::saveFile() - Warning: Palette file already exists! -> " + pathName;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts << k->document->toString();
        k->filePath = pathName;
    } else {
        #ifdef K_DEBUG
            QString msg = "TupPaletteImporter::saveFile() - Fatal Error: Insufficient permission to save palette file -> " + pathName;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }

    return true;
}

QString TupPaletteImporter::filePath() const
{
    return k->filePath;
}

QStringList TupPaletteImporter::getColorArray(const QString &line)
{
    QByteArray array = line.toLocal8Bit();
    QStringList rgb;
    QString color = "";
    bool gotIt = false;
    for (int i=0; i<array.size(); i++) {
         QChar letter(array.at(i));
         if (letter.isDigit()) {
             color += letter;
             if (gotIt)
                 gotIt = false;
             if (i == array.size() - 1)
                 rgb << color;
         } else {
                 if (!gotIt) {
                     rgb << color;
                     color = "";
                     gotIt = true;
                 }
         }
    }

    return rgb;
}

