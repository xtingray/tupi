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

#ifndef TUPSCENEMANAGER_H
#define TUPSCENEMANAGER_H

#include "tupabstractserializable.h"
#include "tupproject.h"
#include "tupstoryboard.h"
#include "tupbackground.h"
#include "tupitemtweener.h"
#include "tupinthash.h"
#include "tupglobal_store.h"

#include <QDomDocument>
#include <QDomElement>
#include <QGraphicsScene>
#include <QMap>

class QGraphicsItem;
class QPainter;
class QStyleOptionGraphicsItem;
class TupLayer;
class TupSoundLayer;
class TupGraphicObject;
class TupSvgItem;
class TupBackground;
class TupItemTweener;

typedef TupIntHash<TupLayer *> Layers;
typedef TupIntHash<TupSoundLayer *> SoundLayers;

/**
 * @brief This class represents the scene data structure
 * @author David Cuadrado 
*/

class STORE_EXPORT TupScene : public QObject, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        /**
          * Default Constructor
          */
        TupScene(TupProject *parent, const QSize dimension, const QColor bgColor);

        /**
          * Destructor
          */
        ~TupScene();

        /**
          * Sets scene name
          */
        void setSceneName(const QString &name);

        /**
          * Sets background color 
          */
        void setBgColor(const QColor bgColor);

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

        TupLayer *layer(int position) const;
        TupSoundLayer *soundLayer(int position) const;

        void setLayers(const Layers &);

        /**
          * Removes the layer at index
          */
        bool removeLayer(int index);

        /**
          * Adds a layer, if addToEnd is true, the layer will be set at the end, otherwise next to the current layer
          */
        TupLayer *createLayer(QString name, int position, bool loaded = false);

        TupSoundLayer *createSoundLayer(int position, bool loaded = false);

        /**
          * Moves the layer to the index
          */
        bool moveLayer(int from, int to);

        int objectIndex() const;
        int visualIndexOf(TupLayer *layer) const;

        TupProject *project() const;
        void addTweenObject(TupGraphicObject *object);
        void addTweenObject(TupSvgItem *object);

        void updateTweenObject(int index, TupGraphicObject *object);
        void updateTweenObject(int index, TupSvgItem *object);

        // int indexOfTweenObject(const QString &name, TupLibraryObject::Type itemType, TupItemTweener::Type tweenType);

        void removeTweenObject(TupGraphicObject *object);
        void removeTweenObject(TupSvgItem *object);

        bool tweenExists(const QString &name, TupItemTweener::Type type);
        void removeTween(const QString &name, TupItemTweener::Type type);

        TupItemTweener * tween(const QString &name, TupItemTweener::Type type);

        QList<QString> getTweenNames(TupItemTweener::Type type);
        QList<QGraphicsItem *> getItemsFromTween(const QString &name, TupItemTweener::Type type);

        int getTotalTweens();

        QList<TupGraphicObject *> tweeningGraphicObjects() const;
        QList<TupSvgItem *> tweeningSvgObjects() const;

        int framesTotal();

        QList<int> layerIndexes();

        TupBackground *background();

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

        void removeTweensFromFrame(int frame);
        void reset(QString &name);

        void setStoryboard(TupStoryboard *storyboard);
        TupStoryboard * storyboard();

        void insertStoryBoardScene(int index);
        void appendStoryBoardScene();
        void moveStoryBoardScene(int oldIndex, int newIndex);
        void resetStoryBoardScene(int index);
        void removeStoryBoardScene(int index);

    private:
        void removeTweensFromLayer(int layer);

        struct Private;
        Private *const k;
};

#endif
