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
//     <storyboard scenes="0, 1, 2, N" />
//         <title>Storyboard Title</title>
//         <topics>Storyboard Topics</topics>
//         <description>Storyboard Description</description>          
//     </storyboard>
// </project_storyboard>

TupStoryboardExportPackage::TupStoryboardExportPackage(const QString &title, const QString &topics, const QString &description, const QList<int> sceneIndexes): QDomDocument()
{
    QDomElement root = createElement("project_storyboard");
    root.setAttribute("version", "0");
    appendChild(root);
    
    QString indexes = "";
    for (int i=0; i < sceneIndexes.size(); i++)
         indexes += QString::number(sceneIndexes.at(i)) + ","; 

    indexes.remove(indexes.length() - 1, 1);

    QDomElement story = createElement("storyboard");
    story.setAttribute("scenes", indexes);

    QDomText titleDom = createTextNode(title);
    QDomText topicDom = createTextNode(topics);
    QDomText descDom = createTextNode(description);

    story.appendChild(createElement("title")).appendChild(titleDom);
    story.appendChild(createElement("topics")).appendChild(topicDom);
    story.appendChild(createElement("description")).appendChild(descDom);
    
    root.appendChild(story);
}

TupStoryboardExportPackage::~TupStoryboardExportPackage()
{
}
