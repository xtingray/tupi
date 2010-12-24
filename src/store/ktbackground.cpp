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

#include "ktbackground.h"
#include "ktserializer.h"

#include "kdebug.h"

KTBackground::KTBackground(KTScene *parent)
    : QObject(parent)
{
    landscape = new KTFrame(this);
    landscape->setFrameName(tr("landscape"));
}

KTBackground::~KTBackground()
{
}

void KTBackground::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();

    QDomNode n = root.firstChild();

    QDomElement e = n.toElement();

    if (!e.isNull()) {
        if (e.tagName() == "frame") {

            landscape = new KTFrame(this);
            landscape->setFrameName(tr("landscape"));

            if (landscape) {
                QString newDoc;

                {
                  QTextStream ts(&newDoc);
                  ts << n;
                }

                landscape->fromXml(newDoc);
            }
        }
    }
}

QDomElement KTBackground::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("background");
    doc.appendChild(root);

    root.appendChild(landscape->toXml(doc));

    return root;
}

KTFrame *KTBackground::frame()
{
    return landscape;
}
