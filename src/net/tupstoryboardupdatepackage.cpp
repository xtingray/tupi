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

#include "tupstoryboardupdatepackage.h"

// <project_storyboard_update version="0">
//   <storyboard>
//     <title>Storyboard title</title>
//     <author>Artist name</author>
//     <topics>#topic1 #topic2 #topicN</topics>
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

TupStoryboardUpdatePackage::TupStoryboardUpdatePackage(const QDomElement storyboard, int sceneIndex): QDomDocument()
{
    QDomElement root = createElement("project_storyboard_update");
    root.setAttribute("version", "0");
    appendChild(root);

    QDomText sceneDom = createTextNode(QString::number(sceneIndex));
    root.appendChild(createElement("sceneIndex")).appendChild(sceneDom);
    root.appendChild(storyboard);
}

TupStoryboardUpdatePackage::~TupStoryboardUpdatePackage()
{
}
