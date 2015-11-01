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

#include "tuplocalprojectmanagerhandler.h"
#include "tupprojectrequest.h"
#include "tupprojectcommand.h"
#include "tupfilemanager.h"

/**
 * This class handles all the procedures related to the projects local format for Tupi.
 *
 * @author David Cuadrado
*/

TupLocalProjectManagerHandler::TupLocalProjectManagerHandler(QObject *parent) : TupAbstractProjectHandler(parent)
{
}

TupLocalProjectManagerHandler::~TupLocalProjectManagerHandler()
{
}

// SQA: Verify the way to enhance the undo/redo system
//      These method is too heavy to do the job :/ 

bool TupLocalProjectManagerHandler::isUndoCommand(const QString &xml)
{
    bool isAdd = false;
    bool firstScene = false;
    bool firstLayer = false;

    QDomDocument doc;
    doc.setContent(xml);
    QDomElement root = doc.documentElement();

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "action") {
                   if (e.attribute("id").toInt() == 1)
                       isAdd = true;
                   else 
                       return true;
               } else if (e.tagName() == "scene") {
                          if (e.attribute("index").toInt() == 0)
                              firstScene = true;
                          QDomNode n2 = e.firstChild();
                          if (n2.isNull() && isAdd && firstScene)
                              return false;
                          while (!n2.isNull()) {
                                 QDomElement e2 = n2.toElement();
                                 if (!e2.isNull()) {
                                     if (e2.tagName() == "layer") {
                                         if (e2.attribute("index").toInt() == 0)
                                             firstLayer = true;
                                         QDomNode n3 = e2.firstChild();
                                         if (n3.isNull() && isAdd && firstScene && firstLayer)
                                             return false;
                                          while (!n3.isNull()) {
                                                 QDomElement e3 = n3.toElement();
                                                if (!e3.isNull()) {
                                                    if (e3.tagName() == "frame") {
                                                        QDomNode n4 = e3.firstChild();
                                                        if (!n4.isNull()) {
                                                            return true; 
                                                        } else if ((e3.attribute("index").toInt() == 0) && isAdd && firstScene && firstLayer) {
                                                                  return false; 
                                                        }
                                                    }
                                                } 
                                                n3 = n3.nextSibling();
                                          } 
                                     } // end if e2 layer
                                 } else if (isAdd && firstScene) {
                                            return false;
                                 }
                                 n2 = n2.nextSibling();
                          } // end while n2
               } // end if scene
           }
           n = n.nextSibling();
    }

    return true;
}

void TupLocalProjectManagerHandler::handleProjectRequest(const TupProjectRequest *request)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupLocalProjectManagerHandler::handleProjectRequest()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (request->isValid()) {
        emit sendCommand(request, true);
        //emit sendCommand(request, isUndoCommand(request->xml()));
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLocalProjectManagerHandler::handleProjectRequest() - INVALID REQUEST! ID: " + QString::number(request->id());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

bool TupLocalProjectManagerHandler::saveProject(const QString &fileName, TupProject *project)
{
    bool result = false;
    QString file = fileName;

    if (!fileName.endsWith(".tup"))
        file += ".tup";

    TupFileManager *manager = new TupFileManager;
    result = manager->save(file, project);
    delete manager;

    return result;
}

bool TupLocalProjectManagerHandler::loadProject(const QString &fileName, TupProject *project)
{
    bool result = false;

    TupFileManager *manager = new TupFileManager;
    result = manager->load(fileName, project);
    delete manager;

    return result;
}

void TupLocalProjectManagerHandler::setProject(TupProject *)
{

}
