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

#ifndef KTLIBRARYFOLDER_H
#define KTLIBRARYFOLDER_H

#include "ktabstractserializable.h"
#include "ktlibraryobject.h"
#include "ktglobal_store.h"

#include <QObject>
#include <QHash>
#include <QMap>
#include <QByteArray>

class KTProject;
class KTLibraryFolder;
class KTLibraryObject;

typedef QMap<QString, KTLibraryFolder *> Folders;
typedef QMap<QString, KTLibraryObject *> LibraryObjects;

//typedef QHash<QString, KTLibraryObject *> LibraryObjects;
//typedef QMap<QString, KTLibraryFolder *> Folders;

/**
 * @author David Cuadrado
**/

class STORE_EXPORT KTLibraryFolder : public QObject, public KTAbstractSerializable
{
    Q_OBJECT
    
    public:
        KTLibraryFolder(const QString &id, KTProject *project, QObject *parent = 0);
        ~KTLibraryFolder();
        
        void setId(const QString &id);
        QString id() const;
        
        KTLibraryObject *createSymbol(KTLibraryObject::Type type, const QString &name, const QByteArray &data, const QString &folder = QString(), bool loaded = false);
        
        bool addObject(KTLibraryObject *object); 
        bool addObject(const QString &folderName, KTLibraryObject *object);

        bool addFolder(KTLibraryFolder *folder);

        bool removeObject(const QString &id, bool absolute);

        bool removeFolder(const QString &id);

        bool renameObject(const QString &folder, const QString &oldId, const QString &newId);
        bool renameFolder(const QString &oldId, const QString &newId);
        
        bool moveObject(const QString &id, const QString &folder);
        bool moveObjectToRoot(const QString &id);
        
        KTLibraryObject *findObject(const QString &id) const;
        
        Folders folders() const;
        LibraryObjects objects() const;
        
        int objectsCount() const;
        int foldersCount() const;
        
        KTProject *project() const;
        void reset();

        KTLibraryFolder *findFolder(const QString &id) const;
        bool folderExists(const QString &id) const;

        void updatePaths(const QString &newPath);
        
    public:
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;
        
    private:
        void loadObjects(const QString &folder, const QString &xml);
        void loadItem(const QString &folder, QDomNode xml);

        struct Private;
        Private *const k;
};

#endif
