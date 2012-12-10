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

#include "tupstoryboardexportpackage.h"

// <project_storyboard version="0">
//   <storyboard>
//     <title>Storyboard title</title>
//     <author>Artist name</author>
//     <summary>Storyboard Summary</summary>
//     <scene>
//       <title>Scene title</title>
//       <duration>Scene time</duration>
//       <description>Scene description</description>
//     </scene>
//     <scene>
//       <title>Scene title</title>
//       <duration>Scene time</duration>
//       <description>Scene description</description>
//     </scene>
//     ... 
//   </storyboard>
// </project_storyboard>

TupStoryboardExportPackage::TupStoryboardExportPackage(const QDomElement storyboard): QDomDocument()
{
    QDomElement root = createElement("project_storyboard");
    root.setAttribute("version", "0");
    appendChild(root);
    root.appendChild(storyboard);
}

TupStoryboardExportPackage::~TupStoryboardExportPackage()
{
}
