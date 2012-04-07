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

#include "tupprojectmanagerparams.h"

struct TupProjectManagerParams::Private
{
    QString projectName;
    QString author;
    QColor bgcolor;
    QString description;
    QSize dimension;
    int fps;
};

TupProjectManagerParams::TupProjectManagerParams(): k(new Private)
{
}

TupProjectManagerParams::~TupProjectManagerParams()
{
}

void TupProjectManagerParams::setProjectName(const QString &name)
{
    k->projectName = name;
}

QString TupProjectManagerParams::projectName() const
{
    return k->projectName;
}

void TupProjectManagerParams::setAuthor(const QString &author)
{
    k->author = author;
}

QString TupProjectManagerParams::author() const
{
    return k->author;
}

void TupProjectManagerParams::setBgColor(const QColor color)
{
    k->bgcolor = color;
}

QColor TupProjectManagerParams::bgColor()
{
    return k->bgcolor;
}

void TupProjectManagerParams::setDescription(const QString &description)
{
    k->description = description;
}

QString TupProjectManagerParams::description() const
{
    return k->description;
}

void TupProjectManagerParams::setDimension(const QSize &dimension)
{
    k->dimension = dimension;
}

QSize TupProjectManagerParams::dimension() const
{
    return k->dimension;
}

void TupProjectManagerParams::setFPS(const int fps)
{
    k->fps = fps;
}

int TupProjectManagerParams::fps() const
{
    return k->fps;
}
