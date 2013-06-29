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

#include "tupnewprojectpackage.h"

struct TupNewProjectPackage::Private
{
    QDomText name;
    QDomText author;
    QDomText description;
    QDomText bgcolor;
    QDomText dimension;
    QDomText fps;
};

TupNewProjectPackage::TupNewProjectPackage(const QString & name, const QString & author, const QString & description, const QString & bgcolor, const QString & dimension, const QString & fps): QDomDocument(), k(new Private)
{
    QDomElement root = createElement("project_new");
    root.setAttribute("version", "0");
    
    k->name = createTextNode(name);
    k->author = createTextNode(author);
    k->description = createTextNode(description);
    k->bgcolor = createTextNode(bgcolor);
    k->dimension = createTextNode(dimension);
    k->fps = createTextNode(fps);
    
    root.appendChild(createElement("name")).appendChild(k->name);
    root.appendChild(createElement("author")).appendChild(k->author);
    root.appendChild(createElement("description")).appendChild(k->description);
    root.appendChild(createElement("bgcolor")).appendChild(k->bgcolor);
    root.appendChild(createElement("dimension")).appendChild(k->dimension);
    root.appendChild(createElement("fps")).appendChild(k->fps);

    appendChild(root);
}

TupNewProjectPackage::~TupNewProjectPackage()
{
}

void TupNewProjectPackage::setName(const QString & name)
{
    k->name.setData(name);
}

void TupNewProjectPackage::setAuthor(const QString & author)
{
    k->author.setData(author);
}

void TupNewProjectPackage::setDescription(const QString & description)
{
    k->description.setData(description);
}

void TupNewProjectPackage::setBgColor(const QString & bgcolor)
{
    k->bgcolor.setData(bgcolor);
}

void TupNewProjectPackage::setDimension(const QString & dimension)
{
    k->dimension.setData(dimension);
}

void TupNewProjectPackage::setFps(const QString & fps)
{
    k->fps.setData(fps);
}
