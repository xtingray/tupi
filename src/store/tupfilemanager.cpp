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

#include "tupfilemanager.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuplibrary.h"
#include "tuppackagehandler.h"
#include "talgorithm.h"
#include "tdebug.h"

#include <QDir>

TupFileManager::TupFileManager() : QObject()
{
}

TupFileManager::~TupFileManager()
{
}

bool TupFileManager::save(const QString &fileName, TupProject *project)
{
    #ifdef K_DEBUG
           tDebug() << "TupFileManager::save() - Saving file " << fileName;
    #endif

    int indexPath = fileName.lastIndexOf(QDir::separator());
    int indexFile = fileName.length() - indexPath;
    QString name = fileName.right(indexFile - 1);
    int indexDot = name.lastIndexOf(".");
    name = name.left(indexDot);

    QString oldDirName = CACHE_DIR + project->projectName();
    QDir projectDir(oldDirName);

    if (name.compare(project->projectName()) != 0) {
        project->setProjectName(name);
        projectDir.setPath(CACHE_DIR + name);    
        project->library()->updatePaths(CACHE_DIR + name);
        if (!projectDir.exists()) {
            if (projectDir.rename(oldDirName, projectDir.path())) {
                #ifdef K_DEBUG
                       tDebug() << "TupFileManager::save() - Directory renamed to " << projectDir.path();
                #endif
            } else {
                // SQA: Check if these lines are really needed
                if (! projectDir.mkdir(projectDir.path())) {
                    #ifdef K_DEBUG
                           tError() << "TupFileManager::save() - Error: Can't create path " << projectDir.path();
                    #endif
                    return false;
                } else {
                    #ifdef K_DEBUG
                           tDebug() << "TupFileManager::save() - Directory " << projectDir.path() << " created successfully";
                    #endif
                }
            }
        }
    } else {
        if (!projectDir.exists()) {
            if (! projectDir.mkdir(projectDir.path())) {
                #ifdef K_DEBUG
                       tError() << "TupFileManager::save() - Error: Can't create path " << projectDir.path();
                #endif
                return false;
            } else {
                #ifdef K_DEBUG
                       tDebug() << "TupFileManager::save() - Directory " << projectDir.path() << " created successfully";
                #endif
            }
        }
    }

    {
     // Save project
     QFile projectFile(projectDir.path() + QDir::separator() + "project.tpp");

     if (projectFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream ts(&projectFile);
         QDomDocument doc;
         project->setProjectName(name);
         doc.appendChild(project->toXml(doc));
         ts << doc.toString();
         projectFile.close();
     } else {
         #ifdef K_DEBUG
                tError() << "TupFileManager::save() - Error: Can't create file " << projectDir.path() + QDir::separator() + "project.tpp";
         #endif
     }
    }

    // Save scenes
    {
     int index = 0;
     foreach (TupScene *scene, project->scenes().values()) {
              QDomDocument doc;
              doc.appendChild(scene->toXml(doc));

              QString scenePath = projectDir.path() + QDir::separator() + "scene" + QString::number(index) + ".tps";
              QFile sceneFile(scenePath);

              if (sceneFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                  QTextStream st(&sceneFile);
                  st << doc.toString();
                  index += 1;
                  sceneFile.close();
              } else {
                  #ifdef K_DEBUG
                         tError() << "TupFileManager::save() - Error: Can't create file " << scenePath;
                  #endif
              }
     }
    }

    {
     // Save library
     QFile lbr(projectDir.path() + QDir::separator() + "library.tpl");

     if (lbr.open(QIODevice::WriteOnly | QIODevice::Text)) {
         QTextStream ts(&lbr);

         QDomDocument doc;
         doc.appendChild(project->library()->toXml(doc));

         ts << doc.toString();
         lbr.close();
     } else {
         #ifdef K_DEBUG
                tError() << "TupFileManager::save() - Error: Can't create file " << projectDir.path() + QDir::separator() + "library.tpl";
         #endif
     }
    }

    TupPackageHandler packageHandler;
    bool ok = packageHandler.makePackage(projectDir.path(), fileName);

    if (ok) {
        #ifdef K_DEBUG
               tWarning() << "TupFileManager::save() - Project saved in " << fileName;
        #endif
    } else {
        #ifdef K_DEBUG
               tError() << "TupFileManager::save() - Error: Project couldn't be saved in " << fileName;
        #endif
    }

    return ok;
}

bool TupFileManager::load(const QString &fileName, TupProject *project)
{
    #ifdef K_DEBUG
           T_FUNCINFO << fileName;
    #endif

    TupPackageHandler packageHandler;

    if (packageHandler.importPackage(fileName)) {

        QDir projectDir(packageHandler.importedProjectPath());
        QFile pfile(projectDir.path() + QDir::separator() + "project.tpp");

        if (pfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            project->fromXml(QString::fromLocal8Bit(pfile.readAll()));
            pfile.close();
        } else {
            #ifdef K_DEBUG
                   tError() << "TupFileManager::load() - Error while open .tpp file. Name: " << pfile.fileName();
                   tError() << "TupFileManager::load() - Path: " << projectDir.path();
                   tError() << "TupFileManager::load() - Error Description: " << pfile.errorString();
                   tError() << "TupFileManager::load() - Error type: " << pfile.error();
            #endif
            return false;
        }

        project->setDataDir(packageHandler.importedProjectPath());
        project->loadLibrary(projectDir.path() + QDir::separator() + "library.tpl");

        QStringList scenes = projectDir.entryList(QStringList() << "*.tps", QDir::Readable | QDir::Files);

        if (scenes.count() > 0) {
            int index = 0;
            foreach (QString scenePath, scenes) {
                     scenePath = projectDir.path() + QDir::separator() + scenePath;
                     QFile file(scenePath);

                     if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                         QString xml = QString::fromLocal8Bit(file.readAll());
                         QDomDocument document;
                         if (! document.setContent(xml))
                             return false;
                         QDomElement root = document.documentElement();

                         TupScene *scene = project->createScene(root.attribute("name"), index, true);
                         scene->fromXml(xml);

                         index += 1;
                         file.close();
                     } else {
                         #ifdef K_DEBUG
                                tError() << "TupFileManager::load() - Error: Can't open file " << scenePath;
                         #endif
                         return false;
                     }
            }

            project->setOpen(true);

            return true;

        } else {
            #ifdef K_DEBUG
                   tError() << "TupFileManager::load() - Error: No scene files found (*.tps)";
            #endif
            return false;
        }
    }

    #ifdef K_DEBUG
           tError() << "TupFileManager::load() - Error: Can't import package (" << fileName << ")";
    #endif

    return false;
}
