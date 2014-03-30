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
#include "tdebug.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

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
       k->paletteName = info.baseName().toLower();
       QString exttension = info.suffix().toLower();
       k->document = new TupPaletteDocument(k->paletteName, false);

       tError() << "TupPaletteImporter::importGimpPalette() - Filename: " << k->paletteName;

       if (exttension.compare("txt") == 0) {
           while (!stream.atEnd()) {
                  QString line = stream.readLine();
                  if (line.startsWith("#")) {
                      QColor color(line);
                      if (color.isValid()) {
                          k->document->addColor(color);
                      } else {
                          #ifdef K_DEBUG
                                 tError() << "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid color format -> " << line;
                          #endif
                          return false;
                      }
                  }
           }
       } else if (exttension.compare("css") == 0) {
                  while (!stream.atEnd()) {
                         QString line = stream.readLine();
                  }
       } else {
           #ifdef K_DEBUG
                  tError() << "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid extension!";
           #endif
           return false;
       }

       /*
       if (! string.contains("#"))
           stream.readLine();
		
       // QRegExp rgb("\\s*([\\d]{0,3})\\s+([\\d]{0,3})\\s+([\\d]{0,3})\\s+.*$");
       while (!stream.atEnd()) {
              QString line = stream.readLine();

              if (rgb.indexIn(line) != -1) {
                  QStringList capturedTexts = rgb.capturedTexts();

                  if (capturedTexts.count() != 4)
                      continue;

                  int r = capturedTexts[1].toInt();
                  int g = capturedTexts[2].toInt();
                  int b = capturedTexts[3].toInt();

                  QColor c(r, g, b);

                  if (c.isValid()) {
                      k->document->addColor(c);
                  } else {
                      #ifdef K_DEBUG
                             tError() << "TupPaletteImporter::importGimpPalette() - Fatal Error: Invalid color format";
                      #endif
                      return false;
                  }
              } else {
                  #ifdef K_DEBUG
                         tError() << "TupPaletteImporter::importGimpPalette() - Fatal Error: Couldn't find a color value";
                  #endif
                  return false;
              }
       }
       */
    }

    return true;
}

bool TupPaletteImporter::saveFile(const QString &path)
{
    if (k->paletteName.isNull()) {
        #ifdef K_DEBUG
               tError() << "TupPaletteImporter::saveFile() - Fatal Error: Palette name is null!";
        #endif
        return false;
    }

    QString pathName = path + QDir::separator() + k->paletteName.replace(" ", "_") + ".tpal";

    QFile file(pathName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts << k->document->toString();
        k->filePath = pathName;
    } else {
        #ifdef K_DEBUG
               tError() << "TupPaletteImporter::saveFile() - Fatal Error: Insufficient permission to save palette file -> " << pathName;
        #endif
        return false;
    }

    return true;
}

QString TupPaletteImporter::filePath() const
{
    return k->filePath;
}
