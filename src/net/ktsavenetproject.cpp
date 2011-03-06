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

#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

#include "ktsavenetproject.h"
#include "ktproject.h"
#include "ktnetprojectmanagerparams.h"

KTSaveNetProject::KTSaveNetProject() : KTSaveProject()
{
}

KTSaveNetProject::KTSaveNetProject(const QString &server, int port)
 : KTSaveProject(), m_server(server), m_port(port)
{
}

KTSaveNetProject::~KTSaveNetProject()
{
}

bool KTSaveNetProject::save(const QString &_filename, const KTProject *project)
{
    QString filename = _filename;
    
    if (!filename.endsWith(".ntup"))
        filename += ".ntup";
    
    QDomDocument doc;
    QDomElement root = doc.createElement("NetProject");
    
    QDomElement projectElement = doc.createElement("project");
    projectElement.setAttribute("name", project->projectName());
    
    root.appendChild(projectElement);
    
    QDomElement connection = doc.createElement("connection");
    
    QDomElement server = doc.createElement("server");
    QDomText serverValue = doc.createTextNode(m_server);
    
    server.appendChild(serverValue);
    
    QDomElement port = doc.createElement("port");
    QDomText portValue = doc.createTextNode(QString::number(m_port));
    
    port.appendChild(portValue);
    
    connection.appendChild(server);
    connection.appendChild(port);
    
    root.appendChild(connection);
    
    doc.appendChild(root);
    
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    QTextStream ts(&file);
    ts << doc.toString();
    
    file.close();
    
    return true;
}

bool KTSaveNetProject::load(const QString &, KTProject *)
{
    return false;
}

KTNetProjectManagerParams *KTSaveNetProject::params(const QString &filename)
{
    KTNetProjectManagerParams *params = new KTNetProjectManagerParams;
    
    QFile file(filename);
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QDomDocument doc;
        if (doc.setContent(file.readAll())) {
            QDomElement docElem = doc.documentElement();
            
            QDomNode n = docElem.firstChild();
            while (!n.isNull()) {
                   QDomElement e = n.toElement();
                   if (!e.isNull()) {
                       if (e.tagName() == "project") {
                           params->setProjectName(e.attributeNode("name").value());
                       } else if (e.tagName() == "connection") {
                                  QDomNode n1 = e.firstChild();
                                  while (! n1.isNull()) {
                                         QDomElement e1 = n1.toElement();
                            
                                  if (!e1.isNull()) {
                                      if (e.tagName() == "server") {
                                          params->setServer(e.text());
                                      } else if (e.tagName() == "port") {
                                                 params->setPort(e.text().toInt());
                                      }
                                  }
                            
                                  n1 = n1.nextSibling();
                       }
                   }
                }
                n = n.nextSibling();
            }
        }
        
        file.close();
    }
    
    return params;
}
