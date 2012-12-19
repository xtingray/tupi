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

#include "tupstoryboardparser.h"
#include <QTextStream>

struct TupStoryboardParser::Private
{
    QDomDocument request;
    int sceneIndex;
    int checksum;
    QString storyboard;
};

TupStoryboardParser::TupStoryboardParser(const QString &package) : k(new Private())
{
    k->sceneIndex = -1;
    k->checksum = 0;
    k->storyboard = "";

    if (k->request.setContent(package)) {
        QDomElement root = k->request.documentElement();
        QDomNode n = root.firstChild();
        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (e.tagName() == "sceneIndex") {
                   k->sceneIndex = e.text().toInt();
                   k->checksum++;
               } else if (e.tagName() == "storyboard") {
                          QString input = "";
                          QTextStream text(&input);
                          text << n;
                          k->storyboard = input;
                          k->checksum++;
               }
               n = n.nextSibling();
        }
    }
}

TupStoryboardParser::~TupStoryboardParser()
{
    delete k;
}

bool TupStoryboardParser::checksum()
{
    if (k->checksum == 2)
        return true;

    return false;
}

int TupStoryboardParser::sceneIndex()
{
    return k->sceneIndex;
}

QString TupStoryboardParser::storyboardXml() const
{
    return k->storyboard;
}

QDomDocument TupStoryboardParser::request() const
{
    return k->request;
}
