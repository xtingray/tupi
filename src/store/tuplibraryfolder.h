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

#ifndef TUPLIBRARYFOLDER_H
#define TUPLIBRARYFOLDER_H

#include "tupabstractserializable.h"
#include "tuplibraryobject.h"
#include "tupglobal_store.h"

#include <QObject>
#include <QHash>
#include <QMap>
#include <QByteArray>

class TupProject;
class TupLibraryFolder;
class TupLibraryObject;

typedef QMap<QString, TupLibraryFolder *> Folders;
typedef QMap<QString, TupLibraryObject *> LibraryObjects;

/**
 * @author David Cuadrado
**/

class STORE_EXPORT TupLibraryFolder : public QObject, public TupAbstractSerializable
{
    Q_OBJECT
    
    public:
        TupLibraryFolder(const QString &id, TupProject *project, QObject *parent = 0);
        ~TupLibraryFolder();
        
        void setId(const QString &id);
        QString id() const;
        
        TupLibraryObject *createSymbol(TupLibraryObject::Type type, const QString &name, const QByteArray &data, const QString &folder = QString(), bool loaded = false);
        
        bool addObject(TupLibraryObject *object); 
        bool addObject(const QString &folderName, TupLibraryObject *object);

        bool reloadObject(const QString &id);

        bool addFolder(TupLibraryFolder *folder);

        bool removeObject(const QString &id, bool absolute);

        bool removeFolder(const QString &id);

        bool renameObject(const QString &folder, const QString &oldId, const QString &newId);
        bool renameFolder(const QString &oldId, const QString &newId);
        
        bool moveObject(const QString &id, const QString &folder);
        bool moveObjectToRoot(const QString &id);

        bool exists(const QString &id);
        
        TupLibraryObject *getObject(const QString &id) const;
        
        Folders folders() const;
        LibraryObjects objects() const;
        
        int objectsCount() const;
        int foldersCount() const;
        
        TupProject *project() const;
        void reset();

        TupLibraryFolder *getFolder(const QString &id) const;
        bool folderExists(const QString &id) const;

        void updatePaths(const QString &newPath);

        bool loadingProject();
        
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
