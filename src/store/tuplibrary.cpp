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

#include "tuplibrary.h"
#include "tupproject.h"

TupLibrary::TupLibrary(const QString &id, TupProject *parent) : TupLibraryFolder(id, parent)
{
}

TupLibrary::~TupLibrary()
{
}

void TupLibrary::fromXml(const QString &xml)
{
    QDomDocument document;
    if (!document.setContent(xml))
        return;
    
    QDomElement root = document.documentElement();
    QDomNode n = root.firstChild();
    while (!n.isNull()) {
           QDomElement e = n.toElement();
        
           if (!e.isNull()) {
               if (e.tagName() == "folder") {
                   QString doc;
                   {
                       QTextStream ts(&doc);
                       ts << n;
                   }
                   TupLibraryFolder::fromXml(doc);
               }

               // SQA: Check why this weird if :S
               /*
               if (e.tagName() == "library") {

               } else if (e.tagName() == "folder") {
                          QString doc;
                          {
                             QTextStream ts(&doc);
                             ts << n;
                          }
                          TupLibraryFolder::fromXml(doc);
              }
              */
           }
           n = n.nextSibling();
    }
}

QDomElement TupLibrary::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("library");
    root.appendChild(TupLibraryFolder::toXml(doc));
    return root;
}
