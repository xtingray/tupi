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

#include "ktlibraryfolder.h"
#include "ktlibraryobject.h"

#include "kdebug.h"

#include "ktprojectloader.h"
#include "ktproject.h"

#include <QFileInfo>
#include <QFile>

struct KTLibraryFolder::Private
{
    QString id;
    Folders folders;
    LibraryObjects objects;
    
    KTProject *project;
};

KTLibraryFolder::KTLibraryFolder(const QString &id, KTProject *project, QObject *parent) : QObject(parent), k(new Private)
{
    k->id = id;
    k->project = project;
}

KTLibraryFolder::~KTLibraryFolder()
{
    delete k;
}

KTLibraryObject *KTLibraryFolder::createSymbol(KTLibraryObject::Type type, const QString &name, const QByteArray &data, bool loaded)
{
    KTLibraryObject *object = new KTLibraryObject(this);
    object->setSymbolName(name);
    object->setParent(this);
    object->setType(type);

    if (!object->loadRawData(data)) {
        delete object;
        return 0;
    }

    bool ret = addObject(object);
    object->saveData(k->project->dataDir());

    if (loaded && ret)
        KTProjectLoader::createSymbol(type, name, id(), data, k->project);
    
    return object;
}

bool KTLibraryFolder::addObject(KTLibraryObject *object)
{
    kFatal() << "KTLibraryFolder::addObject() - Adding object to root -> " << object->symbolName();

    if (!k->objects.contains(object->symbolName())) {
        k->objects.insert(object->symbolName(), object);
        return true;
    }

    return false;
}

bool KTLibraryFolder::addObject(const QString &folderName, KTLibraryObject *object)
{
    kFatal() << "KTLibraryFolder::addObject() - Adding object " << object->symbolName() << " to folder " << folderName;

    foreach (KTLibraryFolder *folder, k->folders) {
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

bool KTLibraryFolder::addFolder(KTLibraryFolder *folder)
{
    if (!k->folders.contains(folder->id())) {
        k->folders.insert(folder->id(), folder);
        return true;
    }

    return false;
}

bool KTLibraryFolder::removeObject(const QString &id, bool absolute)
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

    foreach (KTLibraryFolder *folder, k->folders) {
             KTLibraryObject *object = folder->findObject(id);
             if (object)
                 return folder->removeObject(id, absolute);
    }

    return false;
}

bool KTLibraryFolder::removeFolder(const QString &id)
{
    if (k->folders.contains(id)) {
        KTLibraryFolder *folder = findFolder(id);
        LibraryObjects objects = folder->objects();
        foreach (QString oid, objects.keys()) {
                 if (folder->removeObject(oid, true)) {
                     KTLibraryObject::Type extension = static_cast<KTLibraryObject::Type>(objects[oid]->type());
                     if (!k->project->removeSymbolFromProject(oid, extension))
                         return false;
                 }
        }
    
        return k->folders.remove(id);
    }

    return false;
}

bool KTLibraryFolder::moveObject(const QString &id, const QString &target)
{
    KTLibraryObject *object = findObject(id);
    if (object) {
        if (removeObject(id, false))
            foreach (KTLibraryFolder *folder, k->folders) {
                     if (folder->id().compare(target) == 0) {
                         folder->addObject(object);
                         return true;
                     }
            }

    }

    return false;
}

bool KTLibraryFolder::moveObjectToRoot(const QString &id)
{
    KTLibraryObject *object = findObject(id);
    if (object) {
        if (removeObject(id, false)) {
            addObject(object);
            return true;
        }

    }

    return false;
}

void KTLibraryFolder::setId(const QString &id)
{
    k->id = id;
}

QString KTLibraryFolder::id() const
{
    return k->id;
}

KTLibraryObject *KTLibraryFolder::findObject(const QString &id) const
{
    foreach (QString oid, k->objects.keys()) {
             kFatal() << "KTLibraryFolder::findObject() - oid: " << oid;
             if (oid.compare(id) == 0) 
                 return k->objects[oid];
    }
    
    foreach (KTLibraryFolder *folder, k->folders) {
             KTLibraryObject *object = folder->findObject(id);
             if (object)
                 return object;
    }
    
    #ifdef K_DEBUG
        kDebug() << "KTLibraryFolder::findObject(): Cannot find object with id: " << id;
    #endif
    
    return 0;
}

KTLibraryFolder *KTLibraryFolder::findFolder(const QString &id) const
{
    foreach (KTLibraryFolder *folder, k->folders) {
             if (folder->id().compare(id) == 0) 
                 return folder;
    }
   
    #ifdef K_DEBUG
        kDebug() << "KTLibraryFolder::findFolder(): Cannot find folder with id: " << id;
    #endif
   
    return 0;
}

bool KTLibraryFolder::folderExists(const QString &id) const
{
    foreach (KTLibraryFolder *folder, k->folders) {
             if (folder->id().compare(id) == 0)
                 return true;
    }
  
    #ifdef K_DEBUG
        kDebug() << "KTLibraryFolder::folderExists() - Cannot find folder with id: " << id;
    #endif
  
    return false;
}

bool KTLibraryFolder::renameObject(const QString &folder, const QString &oldId, const QString &newId)
{
    kFatal() << "KTLibraryFolder::renameObject() - Tracing method!";
    KTLibraryObject *object = findObject(oldId);

    if (object) {
        removeObject(oldId, false);
        object->setSymbolName(newId);

        if (folder.length() > 0)
            return addObject(folder, object);
        else
            return addObject(object);
       
    } else {
        #ifdef K_DEBUG
               kDebug() << "KTLibraryFolder::renameObject() - Object not found: " << oldId;
        #endif
    }

    return false;
}

bool KTLibraryFolder::renameFolder(const QString &oldId, const QString &newId)
{
    KTLibraryFolder *folder = findFolder(oldId);

    if (folder) {
        k->folders[oldId]->setId(newId);
        return true;
    } 

    return false;
}

int KTLibraryFolder::objectsCount() const
{
    return k->objects.count();
}

int KTLibraryFolder::foldersCount() const
{
    return k->folders.count();
}

KTProject *KTLibraryFolder::project() const
{
    return k->project;
}

Folders KTLibraryFolder::folders() const
{
    return k->folders;
}

LibraryObjects KTLibraryFolder::objects() const
{
    return k->objects;
}

void KTLibraryFolder::fromXml(const QString &xml)
{
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

                       KTLibraryFolder *folder = new KTLibraryFolder(e.attribute("id"), k->project, this);
                       addFolder(folder);

                       KTProjectLoader::createSymbol(KTLibraryObject::Folder,
                                                     e.attribute("id"), QString(), "FOLDER",  k->project);

                       // Loading the objects inside this folder
                       loadObjects(e.attribute("id"), folderDocument.toString(0));
               }

           }

           domNode = domNode.nextSibling();
    }
}

void KTLibraryFolder::loadObjects(const QString &folder, const QString &xml)
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

void KTLibraryFolder::loadItem(const QString &folder, QDomNode xml)
{
    QDomDocument objectDocument;
    objectDocument.appendChild(objectDocument.importNode(xml, true));

    KTLibraryObject *object = new KTLibraryObject(this);
    object->fromXml(objectDocument.toString(0));

    switch (object->type()) {
            case KTLibraryObject::Image:
            case KTLibraryObject::Svg:
            case KTLibraryObject::Sound:
            {
                 object->loadDataFromPath(k->project->dataDir());
            }
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
  
    KTProjectLoader::createSymbol(KTLibraryObject::Type(object->type()),
                                  object->symbolName(), folder, data.toLocal8Bit(), k->project);
}

QDomElement KTLibraryFolder::toXml(QDomDocument &doc) const
{
    kFatal() << "KTLibraryFolder::toXml() - Saving a folder...";

    QDomElement folder = doc.createElement("folder");
    folder.setAttribute("id", k->id);

    foreach (KTLibraryFolder *folderObject, k->folders) {
             kFatal() << "KTLibraryFolder::toXml() - Saving an inner folder...";
             folder.appendChild(folderObject->toXml(doc));
    }
    
    foreach (KTLibraryObject *object, k->objects.values()) {
             kFatal() << "KTLibraryFolder::toXml() - Saving an object!";
             folder.appendChild(object->toXml(doc));
    }
    
    return folder;
}

void KTLibraryFolder::reset()
{
    k->objects.clear();
    k->folders.clear();
}

void KTLibraryFolder::updatePaths(const QString &newPath)
{
    foreach (QString oid, k->objects.keys()) {
             QString oldPath = k->objects[oid]->dataPath();
             QFileInfo logicalPath(oldPath);
             QString filename = logicalPath.fileName();
             QString path = "";

             if (k->objects[oid]->type() == KTLibraryObject::Image)
                 path = newPath + "/images/" + filename; 

             if (k->objects[oid]->type() == KTLibraryObject::Svg)
                 path = newPath + "/svg/" + filename;

             if (k->objects[oid]->type() == KTLibraryObject::Sound)
                 path = newPath + "/audio/" + filename;

             k->objects[oid]->setDataPath(path);

             kFatal() << "KTLibraryFolder::updatePaths() - Updating from: " << oldPath;
             kFatal() << "KTLibraryFolder::updatePaths() - to: " << path;
    }

    foreach (KTLibraryFolder *folder, k->folders)
             folder->updatePaths(newPath);    
}
