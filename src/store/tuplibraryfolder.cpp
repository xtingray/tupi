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

#include "tuplibraryfolder.h"
#include "tuplibraryobject.h"
#include "tupprojectloader.h"
#include "tupproject.h"

struct TupLibraryFolder::Private
{
    QString id;
    Folders folders;
    LibraryObjects objects;
    TupProject *project;
    bool loadingProject;
};

TupLibraryFolder::TupLibraryFolder(const QString &id, TupProject *project, QObject *parent) : QObject(parent), k(new Private)
{
    k->id = id;
    k->project = project;
    k->loadingProject = false;
}

TupLibraryFolder::~TupLibraryFolder()
{
    delete k;
}

TupLibraryObject *TupLibraryFolder::createSymbol(TupLibraryObject::Type type, const QString &name, const QByteArray &data, 
                                                 const QString &folder, bool loaded)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryFolder::createSymbol()]";
        #else
            T_FUNCINFO << " - Creating symbol -> " << name;
        #endif
    #endif

    if (data.isNull()) {
        #ifdef K_DEBUG
            QString msg = "TupLibraryFolder::createSymbol() - [ Fatal Error ] - Data is null!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return 0;
    }

    if (data.isEmpty()) {
        #ifdef K_DEBUG
            QString msg = "TupLibraryFolder::createSymbol() - [ Fatal Error ] - Data is empty!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return 0;
    }

    TupLibraryObject *object = new TupLibraryObject(this);
    object->setSymbolName(name);
    object->setParent(this);
    object->setType(type);

    if (!object->loadRawData(data)) {
        #ifdef K_DEBUG
            QString msg = "TupLibraryFolder::createSymbol() - [ Fatal Error ] - Object have no data raw!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        delete object;
        return 0;
    }

    bool ret;

    if (folder.length() == 0)
        ret = addObject(object);
    else
        ret = addObject(folder, object);

    object->saveData(k->project->dataDir());

    if (loaded && ret)
        TupProjectLoader::createSymbol(type, name, id(), data, k->project);

    return object;
}

bool TupLibraryFolder::addObject(TupLibraryObject *object)
{
    if (!k->objects.contains(object->symbolName())) {
        k->objects.insert(object->symbolName(), object);
        return true;
    }

    return false;
}

bool TupLibraryFolder::addObject(const QString &folderName, TupLibraryObject *object)
{
    foreach (TupLibraryFolder *folder, k->folders) {
             if (folder->id().compare(folderName) == 0) {
                 LibraryObjects bag = folder->objects();
                 if (!bag.contains(object->symbolName())) {
                     folder->addObject(object);
                     return true;
                 }
             }
    }

    return false;
}

bool TupLibraryFolder::reloadObject(const QString &id)
{
    foreach (QString oid, k->objects.keys()) {
             if (oid.compare(id) == 0) {
                 QString path = k->objects[id]->dataPath();
                 if (QFile::exists(path)) {  
                     return k->objects[id]->loadData(path);
                 }
             }
    }

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::reloadObject() - [ Fatal Error ] - Object ID wasn't found -> " + id;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

bool TupLibraryFolder::addFolder(TupLibraryFolder *folder)
{
    if (!k->folders.contains(folder->id())) {
        k->folders.insert(folder->id(), folder);
        return true;
    }

    return false;
}

bool TupLibraryFolder::removeObject(const QString &id, bool absolute)
{
    foreach (QString oid, k->objects.keys()) {
             if (oid.compare(id) == 0) {
                 QString path = k->objects[id]->dataPath();
                 if (absolute) {
                     QFileInfo finfo(path);
                     if (finfo.isFile())
                         QFile::remove(path);
                 }
                 return k->objects.remove(id);
             }
    }

    foreach (TupLibraryFolder *folder, k->folders) {
             TupLibraryObject *object = folder->getObject(id);
             if (object)
                 return folder->removeObject(id, absolute);
    }

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::removeObject() - [ Fatal Error ] - Object ID wasn't found -> " + id;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

bool TupLibraryFolder::removeFolder(const QString &id)
{
    if (k->folders.contains(id)) {
        TupLibraryFolder *folder = getFolder(id);
        LibraryObjects objects = folder->objects();
        foreach (QString oid, objects.keys()) {
                 if (folder->removeObject(oid, true)) {
                     TupLibraryObject::Type extension = static_cast<TupLibraryObject::Type>(objects[oid]->type());
                     if (extension != TupLibraryObject::Item) {
                         if (!k->project->removeSymbolFromFrame(oid, extension))
                             return false;
                     }
                 }
        }

        bool result = k->folders.remove(id); 
        return result;
    }

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::removeFolder() - [ Fatal Error ] - Folder wasn't found -> " + id;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

bool TupLibraryFolder::moveObject(const QString &id, const QString &target)
{
    TupLibraryObject *object = getObject(id);
    if (object) {
        if (removeObject(id, false))
            foreach (TupLibraryFolder *folder, k->folders) {
                     if (folder->id().compare(target) == 0) {
                         folder->addObject(object);
                         return true;
                     }
            }

    }

    return false;
}

bool TupLibraryFolder::moveObjectToRoot(const QString &id)
{
    TupLibraryObject *object = getObject(id);
    if (object) {
        if (removeObject(id, false)) {
            addObject(object);
            return true;
        }

    }

    return false;
}

void TupLibraryFolder::setId(const QString &id)
{
    k->id = id;
}

QString TupLibraryFolder::id() const
{
    return k->id;
}

bool TupLibraryFolder::exists(const QString &id)
{
    foreach (QString oid, k->objects.keys()) {
             if (oid.compare(id) == 0)
                 return true;
    }

    foreach (TupLibraryFolder *folder, k->folders) {
             if (folder->exists(id))
                 return true;
    }

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::exists() - [ Fatal Error ] - Object doesn't exist -> " + id;
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    return false;
}

TupLibraryObject *TupLibraryFolder::getObject(const QString &id) const
{
    foreach (QString oid, k->objects.keys()) {
             if (oid.compare(id) == 0) 
                 return k->objects[oid];
    }

    foreach (TupLibraryFolder *folder, k->folders) {
             TupLibraryObject *object = folder->getObject(id);
             if (object)
                 return object;
    }
    
    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::getObject() - [ Fatal Error ] - Can't get object with id -> " + id;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif
    
    return 0;
}

TupLibraryFolder *TupLibraryFolder::getFolder(const QString &id) const
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryFolder::getFolder()]";
        #else
            T_FUNCINFO << "folder id -> " << id;
        #endif
    #endif

    foreach (TupLibraryFolder *folder, k->folders) {
             if (folder->id().compare(id) == 0)
                 return folder;
    }

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::getFolder() - [ Fatal Error ] - Can't find folder with id -> " + id;
        #ifdef Q_OS_WIN
           qDebug() << msg;
        #else
           tError() << msg;
        #endif
    #endif
   
    return 0;
}

bool TupLibraryFolder::folderExists(const QString &id) const
{
    foreach (TupLibraryFolder *folder, k->folders) {
             if (folder->id().compare(id) == 0)
                 return true;
    }
  
    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::getFolder() - [ Fatal Error ] - Can't find folder with id -> " + id;
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif
  
    return false;
}

bool TupLibraryFolder::renameObject(const QString &folder, const QString &oldId, const QString &newId)
{
    TupLibraryObject *object = getObject(oldId);

    if (object) {
        removeObject(oldId, false);
        object->setSymbolName(newId);

        if (folder.length() > 0)
            return addObject(folder, object);
        else
            return addObject(object);
       
    } 

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::renameObject() - [ Fatal Error ] - Object not found -> " + oldId;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif


    return false;
}

bool TupLibraryFolder::renameFolder(const QString &oldId, const QString &newId)
{
    TupLibraryFolder *folder = getFolder(oldId);

    if (folder) {
        k->folders[oldId]->setId(newId);
        return true;
    } 

    #ifdef K_DEBUG
        QString msg = "TupLibraryFolder::renameFolder() - [ Fatal Error ] - Folder not found -> " + oldId;
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

int TupLibraryFolder::objectsCount() const
{
    return k->objects.count();
}

int TupLibraryFolder::foldersCount() const
{
    return k->folders.count();
}

TupProject *TupLibraryFolder::project() const
{
    return k->project;
}

Folders TupLibraryFolder::folders() const
{
    return k->folders;
}

LibraryObjects TupLibraryFolder::objects() const
{
    return k->objects;
}

void TupLibraryFolder::fromXml(const QString &xml)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryFolder::fromXml()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->loadingProject = true;

    QDomDocument document;
    if (! document.setContent(xml))
        return;
    
    QDomElement root = document.documentElement();
    QDomNode domNode = root.firstChild();
    
    while (!domNode.isNull()) {
           QDomElement e = domNode.toElement();
        
           if (!e.isNull()) {

               if (e.tagName() == "object") {
                   loadItem(id(), domNode);
               } else if (e.tagName() == "folder") {
                       QDomDocument folderDocument;
                       folderDocument.appendChild(folderDocument.importNode(domNode, true));

                       TupLibraryFolder *folder = new TupLibraryFolder(e.attribute("id"), k->project, this);
                       addFolder(folder);

                       TupProjectLoader::createSymbol(TupLibraryObject::Folder, e.attribute("id"), QString(), 
                                                      "FOLDER",  k->project);

                       // Loading the objects inside this folder
                       loadObjects(e.attribute("id"), folderDocument.toString(0));
               }

           }

           domNode = domNode.nextSibling();
    }

    k->loadingProject = false;
}

void TupLibraryFolder::loadObjects(const QString &folder, const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();
    QDomNode domNode = root.firstChild();

    while (!domNode.isNull()) {
           QDomElement e = domNode.toElement();
    
           if (!e.isNull()) {
               if (e.tagName() == "object")
                   loadItem(folder, domNode);
           }
           domNode = domNode.nextSibling();
    }
}

void TupLibraryFolder::loadItem(const QString &folder, QDomNode xml)
{
    QDomDocument objectDocument;
    objectDocument.appendChild(objectDocument.importNode(xml, true));

    TupLibraryObject *object = new TupLibraryObject(this);
    object->fromXml(objectDocument.toString(0));

    switch (object->type()) {
            case TupLibraryObject::Image:
            case TupLibraryObject::Svg:
            case TupLibraryObject::Sound:
            {
                 object->loadDataFromPath(k->project->dataDir());
            }
            break;
            default:
            break;
    }

    if (folder.compare("library") == 0)
        addObject(object);
    else
        addObject(folder, object);

    QDomElement objectData = objectDocument.documentElement().firstChild().toElement();
    QString data;

    if (!objectData.isNull()) {
        QTextStream ts(&data);
        ts << objectData;
    }

    TupProjectLoader::createSymbol(TupLibraryObject::Type(object->type()),
                                   object->symbolName(), folder, data.toLocal8Bit(), k->project);
}

QDomElement TupLibraryFolder::toXml(QDomDocument &doc) const
{
    QDomElement folder = doc.createElement("folder");
    folder.setAttribute("id", k->id);

    foreach (TupLibraryFolder *folderObject, k->folders)
             folder.appendChild(folderObject->toXml(doc));
    
    foreach (TupLibraryObject *object, k->objects.values())
             folder.appendChild(object->toXml(doc));
    
    return folder;
}

void TupLibraryFolder::reset()
{
    k->objects.clear();
    k->folders.clear();
}

void TupLibraryFolder::updatePaths(const QString &newPath)
{
    foreach (QString oid, k->objects.keys()) {
             QString oldPath = k->objects[oid]->dataPath();
             QFileInfo logicalPath(oldPath);
             QString filename = logicalPath.fileName();
             QString path = "";

             if (k->objects[oid]->type() == TupLibraryObject::Image)
                 path = newPath + "/images/" + filename; 

             if (k->objects[oid]->type() == TupLibraryObject::Svg)
                 path = newPath + "/svg/" + filename;

             if (k->objects[oid]->type() == TupLibraryObject::Sound)
                 path = newPath + "/audio/" + filename;

             k->objects[oid]->setDataPath(path);

    }

    foreach (TupLibraryFolder *folder, k->folders)
             folder->updatePaths(newPath);    
}

bool TupLibraryFolder::loadingProject()
{
    return k->loadingProject;
}
