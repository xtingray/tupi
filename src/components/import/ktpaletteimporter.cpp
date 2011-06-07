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

#include "ktpaletteimporter.h"
#include <QFile>
#include <QTextStream>

#include "tdebug.h"

KTPaletteImporter::KTPaletteImporter() : m_document(0)
{
}

KTPaletteImporter::~KTPaletteImporter()
{
    if (m_document) 
        delete m_document;
}

void KTPaletteImporter::import(const QString &file, PaletteType pt)
{
    switch (pt) {
            case Gimp:
             {
                 importGimpPalette(file);
             }
             break;
    }
}

void KTPaletteImporter::importGimpPalette(const QString &file)
{
   QFile f(file);

   if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
       QTextStream stream(&f);

       if (! stream.readLine().contains("GIMP Palette")) {
           #ifdef K_DEBUG
                  tError() << "Don't contains \"GIMP Palette\"";
           #endif
           return;
       }

       QString string = "";
       string = stream.readLine();

       m_paletteName = string.section("Name:", 1).trimmed();

       if (m_document) 
           delete m_document;

       m_document = new KTPaletteDocument(m_paletteName, false);

       stream >> string;

       if (! string.contains("#"))
           stream.readLine();
		
       QRegExp rgb("\\s*([\\d]{0,3})\\s+([\\d]{0,3})\\s+([\\d]{0,3})\\s+.*$");
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
                      m_document->addColor(c);
                  } else {
                      #ifdef K_DEBUG
                             tError() << "Bad color";
                      #endif
                  }
              } else {
                  #ifdef K_DEBUG
                         tError() << "No find";
                  #endif
              }
       }
    }
}

void KTPaletteImporter::saveFile(const QString &path)
{
    if (m_paletteName.isNull())
        return;

    QFile file(path + "/" + m_paletteName.remove(' ') + ".tpal");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts << m_document->toString();

        m_filePath = path+"/" + m_paletteName.remove(' ') + ".tpal";
    }
}

QString KTPaletteImporter::filePath() const
{
    return m_filePath;
}
