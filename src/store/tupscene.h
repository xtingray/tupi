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

#ifndef TUPSCENE_H
#define TUPSCENE_H

#include "tglobal.h"
#include "tupabstractserializable.h"
#include "tupproject.h"
#include "tupstoryboard.h"
#include "tupbackground.h"
#include "tupitemtweener.h"
#include "tuplipsync.h"

#include <QDomDocument>
#include <QDomElement>
#include <QGraphicsScene>
#include <QMap>
#include <QList>
#include <QPainter>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QDir>
#include <QGraphicsView>

class TupLayer;
class TupSoundLayer;
class TupGraphicObject;
class TupSvgItem;
class TupBackground;
class TupItemTweener;

typedef QList<TupFrame *> Frames;
typedef QList<TupLayer *> Layers;
typedef QList<TupSoundLayer *> SoundLayers;
typedef QList<TupLipSync *> Mouths;

/**
 * @brief This class represents the scene data structure
 * @author David Cuadrado 
*/

class TUPI_EXPORT TupScene : public QObject, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        TupScene();
        TupScene(TupProject *parent, const QSize dimension, const QColor bgColor);
        ~TupScene();

        void setSceneName(const QString &name);
        QString sceneName() const;

        void setBgColor(const QColor bgColor);

        void setLocked(bool isLocked);
        bool isLocked() const;

        void setVisible(bool isVisible);
        bool isVisible() const;

        Layers layers() const;
        int layersCount() const;
        SoundLayers soundLayers() const;

        TupLayer *layer(int position) const;
        TupSoundLayer *soundLayer(int position) const;

        void setLayers(const Layers &);

        bool removeLayer(int index);

        TupLayer *createLayer(QString name, int position, bool loaded = false);

        TupSoundLayer *createSoundLayer(int position, bool loaded = false);

        bool moveLayer(int from, int to);

        int objectIndex() const;
        int visualIndexOf(TupLayer *layer) const;

        TupProject *project() const;
        void addTweenObject(TupGraphicObject *object);
        void addTweenObject(TupSvgItem *object);

        void updateTweenObject(int index, TupGraphicObject *object);
        void updateTweenObject(int index, TupSvgItem *object);

        void removeTweenObject(TupGraphicObject *object);
        void removeTweenObject(TupSvgItem *object);

        bool tweenExists(const QString &name, TupItemTweener::Type type);
        bool removeTween(const QString &name, TupItemTweener::Type type);

        TupItemTweener * tween(const QString &name, TupItemTweener::Type type);

        QList<QString> getTweenNames(TupItemTweener::Type type);
        QList<QGraphicsItem *> getItemsFromTween(const QString &name, TupItemTweener::Type type);

        int getTotalTweens();

        QList<TupGraphicObject *> tweeningGraphicObjects() const;
        QList<TupSvgItem *> tweeningSvgObjects() const;

        int framesCount();

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

        QList<QString> getLipSyncNames();
        bool lipSyncExists(const QString &name);
        int getLipSyncLayerIndex(const QString &name);
        TupLipSync * getLipSync(const QString &name);
        bool updateLipSync(TupLipSync *lipsync);
        bool removeLipSync(const QString &name);
        int lipSyncTotal();
        Mouths getLipSyncList();

    private:
        void removeTweensFromLayer(int layer);

        struct Private;
        Private *const k;
};

#endif
