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

#include "themedocument.h"

ThemeDocument::ThemeDocument() : QDomDocument()
{
    QDomProcessingInstruction header = this->createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    this->appendChild(header);
    
    QDomElement root = createElement("Theme");
    
    appendChild(root);
}

ThemeDocument::ThemeDocument(const QString &name, const QString &version) : QDomDocument()
{
    QDomProcessingInstruction header = this->createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    this->appendChild(header);
    
    QDomElement root = createElement("Theme");
    root.setAttribute("name", name);
    root.setAttribute("version", version);
    
    appendChild( root );
}

ThemeDocument::~ThemeDocument()
{
}

void ThemeDocument::addGeneralSection(ThemeKey tk)
{
    QDomElement general = createElement("General");
    QStringList keys = tk.keys();
    QStringList values = tk.values();

    for (int i = 0; i < keys.count(); i++) {
         QDomElement e = createElement(keys[i]);
         e.setAttribute("color", values[i]);
         general.appendChild(e);
    }

    documentElement().appendChild(general);
}

void ThemeDocument::addEffectsSection(ThemeKey tk)        
{
    QDomElement general = createElement("Effects");
    QStringList keys = tk.keys();
    QStringList values = tk.values();

    for (int i = 0; i < keys.count(); i++) {
         QDomElement e = createElement(keys[i]);
         e.setAttribute("color", values[i]);
         general.appendChild(e);
    }

    documentElement().appendChild(general);
}

void ThemeDocument::addSelections(ThemeKey tk)
{
    QDomElement general = createElement("Selections");
    QStringList keys = tk.keys();
    QStringList values = tk.values();

    for (int i = 0; i < keys.count(); i++) {
         QDomElement e = createElement(keys[i]);
         e.setAttribute("color", values[i]);
         general.appendChild(e);
    }

    documentElement().appendChild(general);
}

void ThemeDocument::addTextEffect(ThemeKey tk)
{
    QDomElement general = createElement("TextEffects");
    QStringList keys = tk.keys();
    QStringList values = tk.values();

    for (int i = 0; i < keys.count(); i++) {
         QDomElement e = createElement(keys[i]);
         e.setAttribute("color", values[i]);
         general.appendChild(e);
    }

    documentElement().appendChild(general);
}

