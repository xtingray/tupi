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

#ifndef KTPROJECT_H
#define KTPROJECT_H

#include "ktabstractserializable.h"
#include "ktglobal.h"

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QSize>

#include "ktinthash.h"
#include "ktglobal_store.h"
#include "ktlibraryobject.h"

class KTScene;
class KTLayer;
class KTFrame;
class KTProjectRequest;
class KTProjectResponse;
class QGraphicsItem;
class KTLibrary;

typedef KTIntHash<KTScene *> Scenes;

/**
 * This class contains the project data structure including scenes.
 * 
 * @author David Cuadrado \<krawek@toonka.com\>
*/

class STORE_EXPORT KTProject : public QObject, public KTAbstractSerializable
{
    Q_OBJECT;
    public:
        KTProject(QObject *parent = 0);
        ~KTProject();

        void setProjectName(const QString &name);
        void setAuthor(const QString &author);
        void setDescription(const QString &description);
        void setDimension(const QSize dimension);
        void setFPS(const int fps);	

        QString projectName() const;
        QString author() const;
        QString description() const;
        QSize dimension() const;
        int fps() const;

        bool deleteDataDir();
        QString dataDir() const;

        KTScene *scene(int position) const;

        int visualIndexOf(KTScene *scene) const;
        //int logicalIndexOf(KTScene *scene) const;

        Scenes scenes() const;

        KTScene *createScene(int position, bool loaded = false);
        void updateScene(int position, KTScene *scene);
        bool removeScene(int position);
        bool moveScene(int position, int newPosition);

        bool createSymbol(int type, const QString &name, const QByteArray &data);
        bool removeSymbol(const QString &name, KTLibraryObject::Type symbolType, int sceneIndex, int layerIndex, int frameIndex);
        bool removeSymbol(const QString &name);

        bool addSymbolToProject(const QString &name, int scene, int layer, int frame);
        bool removeSymbolFromProject(const QString &name, KTLibraryObject::Type type);

        bool updateSymbolId(KTLibraryObject::Type type, const QString &oldId, const QString &newId);

        void clear();
        void loadLibrary(const QString &filename);

        //KTLibrary *library() const;
        KTLibrary *library();
        void emitResponse(KTProjectResponse *response);

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

        void setOpen(bool open);
        bool isOpen();
        int scenesTotal() const;

    signals:
        void responsed(KTProjectResponse *response);

    private:
        struct Private;
        Private *const k;
};

#endif
