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

#ifndef KTSCENEMANAGER_H
#define KTSCENEMANAGER_H

#include "ktabstractserializable.h"
#include "ktproject.h"
#include "ktbackground.h"
#include "ktitemtweener.h"

#include <QDomDocument>
#include <QDomElement>
#include <QGraphicsScene>
#include <QMap>

#include "ktinthash.h"
#include "ktglobal_store.h"

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class KTLayer;
class KTSoundLayer;
class KTGraphicObject;
class KTSvgItem;
class KTBackground;
class KTItemTweener;

typedef KTIntHash<KTLayer *> Layers;
typedef KTIntHash<KTSoundLayer *> SoundLayers;

/**
 * @brief This class represents the scene data structure
 * @author David Cuadrado 
*/

class STORE_EXPORT KTScene : public QObject, public KTAbstractSerializable
{
    Q_OBJECT
    public:
        /**
          * Default Constructor
          */
        KTScene(KTProject *parent);

        /**
          * Destructor
          */
        ~KTScene();

        /**
          * Sets scene name
          */
        void setSceneName(const QString &name);

        /**
          * Locks the scene
          */
        void setLocked(bool isLocked);

        /**
          * Returns scene name
          */
        QString sceneName() const;

        /**
          * Returns true if scene is locked
          */
        bool isLocked() const;
        void setVisible(bool isVisible);
        bool isVisible() const;

        /**
          * Returns layers list
          */
        Layers layers() const;
        int layersTotal() const;
        SoundLayers soundLayers() const;

        KTLayer *layer(int position) const;
        KTSoundLayer *soundLayer(int position) const;

        void setLayers(const Layers &);

        /**
          * Removes the layer at index
          */
        bool removeLayer(int index);

        /**
          * Adds a layer, if addToEnd is true, the layer will be set at the end, otherwise next to the current layer
          */
        KTLayer *createLayer(QString name, int position, bool loaded = false);

        KTSoundLayer *createSoundLayer(int position, bool loaded = false);

        /**
          * Moves the layer to the index
          */
        bool moveLayer(int from, int to);

        int objectIndex() const;
        int visualIndexOf(KTLayer *layer) const;

        KTProject *project() const;
        void addTweenObject(KTGraphicObject *object);
        void addTweenObject(KTSvgItem *object);

        void insertTweenObject(int index, KTGraphicObject *object);
        void insertTweenObject(int index, KTSvgItem *object);

        int indexOfTweenObject(const QString &name, KTLibraryObject::Type type);

        void removeTweenObject(KTGraphicObject *object);
        void removeTweenObject(KTSvgItem *object);

        bool tweenExists(const QString &name);
        void removeTween(const QString &name);

        KTItemTweener *tween(const QString &name);

        QList<QString> getTweenNames(KTItemTweener::Type type);
        QList<QGraphicsItem *> getItemsFromTween(const QString &name);

        int getTotalTweens();

        QList<KTGraphicObject *> tweeningGraphicObjects() const;
        QList<KTSvgItem *> tweeningSvgObjects() const;

        int framesTotal();

        QList<int> layerIndexes();

        KTBackground *background();

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        struct Private;
        Private *const k;
};

#endif
