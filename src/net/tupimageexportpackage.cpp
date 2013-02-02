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

#include "tupimageexportpackage.h"

// <project_image version="0">
//     <image scene="0" frame="0" />
//         <title>Image Title</title>
//         <topics>Image Topics</topics>
//         <description>Image Description</description>          
//     </image>
// </project_image>

TupImageExportPackage::TupImageExportPackage(int frameIndex, int sceneIndex, const QString &title, const QString &topics, const QString &description): QDomDocument()
{
    QDomElement root = createElement("project_image");
    root.setAttribute("version", "0");
    appendChild(root);
    
    QDomElement image = createElement("image");
    image.setAttribute("scene", sceneIndex);
    image.setAttribute("frame", frameIndex);

    QDomText titleDom = createTextNode(title);
    QDomText topicDom = createTextNode(topics);
    QDomText descDom = createTextNode(description);

    image.appendChild(createElement("title")).appendChild(titleDom);
    image.appendChild(createElement("topics")).appendChild(topicDom);
    image.appendChild(createElement("description")).appendChild(descDom);
    
    root.appendChild(image);
}

TupImageExportPackage::~TupImageExportPackage()
{
}

