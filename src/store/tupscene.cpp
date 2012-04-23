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

#include "tupscene.h"
#include "tdebug.h"

#include "tupgraphicobject.h"
#include "tupsvgitem.h"
#include "tupsoundlayer.h"

#include "tupitemgroup.h"
#include "tupprojectloader.h"
#include "tupitemfactory.h"

#include <QDir>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

struct TupScene::Private
{
    TupBackground *background;
    Layers layers;
    SoundLayers soundLayers;
    QString name;
    bool isLocked;
    int layerCount;
    // int nameIndex;
    bool isVisible;

    QList<TupGraphicObject *> tweeningGraphicObjects;
    QList<TupSvgItem *> tweeningSvgObjects;

    struct Storyboard
    {
       QString title;
       QString duration;
       QString description;
    } storydata;
};

TupScene::TupScene(TupProject *parent) : QObject(parent), k(new Private)
{
    k->isLocked = false;
    k->layerCount = 0;
    // k->nameIndex = 0;
    k->isVisible = true;
    k->background = new TupBackground(this);

    k->storydata.title = "";
    k->storydata.duration = "";
    k->storydata.description = "";
}

TupScene::~TupScene()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    //k->layers.clear(true);
    delete k;
}

void TupScene::setSceneName(const QString &name)
{
    k->name = name;
}

void TupScene::setStoryTitle(const QString &title)
{
    k->storydata.title = title;
}

void TupScene::setStoryDuration(const QString &duration)
{
    k->storydata.duration = duration;
}

void TupScene::setStoryDescription(const QString &desc)
{
    k->storydata.description = desc;
}

QString TupScene::storyTitle() const
{
    return k->storydata.title;
}

QString TupScene::storyDuration() const
{
    return k->storydata.duration;
}

QString TupScene::storyDescription() const
{
    return k->storydata.description;
}

void TupScene::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

QString TupScene::sceneName() const
{
    return k->name;
}

bool TupScene::isLocked() const
{
    return k->isLocked;
}

void TupScene::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool TupScene::isVisible() const
{
    return k->isVisible;
}

Layers TupScene::layers() const
{
    return k->layers;
}

int TupScene::layersTotal() const
{
    return k->layerCount;
}

SoundLayers TupScene::soundLayers() const
{
    return k->soundLayers;
}

/**
 * Set the layers list, this function overwrites the old layers
 */
void TupScene::setLayers(const Layers &layers)
{
    k->layers = layers;
}

TupLayer *TupScene::createLayer(QString name, int position, bool loaded)
{
    // Q_CHECK_PTR(k->layers);

    if (position < 0 || position > k->layers.count()) {
        #ifdef K_DEBUG
               tDebug() << "TupScene::createLayer() - Invalid index";
        #endif
        return 0;
    }

    k->layerCount++;

    TupLayer *layer = new TupLayer(this, k->layerCount);
    layer->setLayerName(name);
    k->layers.insert(position, layer);

    if (loaded)
        TupProjectLoader::createLayer(project()->visualIndexOf(this), position, layer->layerName(), project());

    return layer;
}

TupSoundLayer *TupScene::createSoundLayer(int position, bool loaded)
{
    #ifdef K_DEBUG
           T_FUNCINFO << position;
    #endif

    if (position < 0 || position > k->soundLayers.count()) {
        tDebug() << "TupScene::createSoundLayer() - [ Fatal Error ] - Index incorrect!";
        return 0;
    }

    TupSoundLayer *layer = new TupSoundLayer(this);
    k->layerCount++;

    layer->setLayerName(tr("Sound layer %1").arg(k->layerCount));

    k->soundLayers.insert(position, layer);

    if (loaded)
        TupProjectLoader::createSoundLayer(objectIndex(), position, layer->layerName(), project());

    return layer;
}

bool TupScene::removeLayer(int position)
{
    #ifdef K_DEBUG
           T_FUNCINFO << position;
    #endif

    // Q_CHECK_PTR(layers);

    TupLayer *layer = this->layer(position);

    if (layer) {

        removeTweensFromLayer(position + 1);

        k->layers.remove(position);
        k->layerCount--;

        /*
        if (k->nameIndex == position + 1) {
            k->nameIndex--;
        } else {
            if (k->layerCount == 0) {
                k->nameIndex = 0;
            }
        }
        */

        QList<int> indexList = this->layers().indexes();
        int size = this->layersTotal();

        for (int i = 0; i < size; i++) 
             TupLayer *layer = this->layer(indexList.at(i));

        delete layer;

        return true;
    }

    return false;
}

/**
 * Retorna el layer que se encuentra en la posicion indicada
 * @param position 
 * @return 
 */
TupLayer *TupScene::layer(int position) const
{
    //if (position < 0 || position >= k->layers.count()) {

    if (position < 0) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: LAYERS TOTAL: " << k->layers.count();
               T_FUNCINFO << " FATAL ERROR: index out of bound -> Position: " << position;
               T_FUNCINFO << " FATAL ERROR: The layer requested doesn't exist anymore";
        #endif
        return 0;
    }

    return k->layers.value(position);
}

TupSoundLayer *TupScene::soundLayer(int position) const
{
    if (position < 0 || position >= k->soundLayers.count()) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound " << position;
        #endif
        return 0;
    }

    return k->soundLayers.value(position);
}

void TupScene::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();

    setSceneName(root.attribute("name", sceneName()));

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "layer") {
                   int pos = k->layers.count();
                   TupLayer *layer = createLayer(e.attribute("name"), pos, true);

                   if (layer) {
                       QString newDoc;
                       {
                         QTextStream ts(&newDoc);
                         ts << n;
                       }

                       layer->fromXml(newDoc);
                   }
               } else if (e.tagName() == "background") {

                   QString newDoc;
                   {
                     QTextStream ts(&newDoc);
                     ts << n;
                   }

                   k->background->fromXml(newDoc); 

               } else if (e.tagName() == "soundlayer") {
                          int pos = k->soundLayers.count();
                          TupSoundLayer *layer = createSoundLayer(pos, true);

                          if (layer) {
                              QString newDoc;
                               {
                                 QTextStream ts(&newDoc);
                                 ts << n;
                               }

                              layer->fromXml(newDoc);
                          }
               } else if (e.tagName() == "storyboard") {
                          QDomNode n2 = e.firstChild();
                          while (!n2.isNull()) {
                                 QDomElement e2 = n2.toElement();
                                 if (e2.tagName() == "title") {
                                     k->storydata.title = e2.text();
                                 } else if (e2.tagName() == "duration") {
                                            k->storydata.duration = e2.text();
                                 } else if (e2.tagName() == "description") {
                                            k->storydata.description = e2.text();
                                 }
                                 n2 = n2.nextSibling();
                          }
               }
           }

           n = n.nextSibling();

    } // end while
}

QDomElement TupScene::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("scene");
    root.setAttribute("name", k->name);

    if (k->storydata.title.length() > 0) {
        QDomElement storyboard = doc.createElement("storyboard");

        QDomText titleDom = doc.createTextNode(k->storydata.title);
        QDomText timeDom  = doc.createTextNode(k->storydata.duration);
        QDomText descDom  = doc.createTextNode(k->storydata.description);

        storyboard.appendChild(doc.createElement("title")).appendChild(titleDom);
        storyboard.appendChild(doc.createElement("duration")).appendChild(timeDom);
        storyboard.appendChild(doc.createElement("description")).appendChild(descDom);

        root.appendChild(storyboard);
    }

    root.appendChild(k->background->toXml(doc));

    foreach (TupLayer *layer, k->layers.values())
             root.appendChild(layer->toXml(doc));

    foreach (TupSoundLayer *sound, k->soundLayers.values())
             root.appendChild(sound->toXml(doc));

    return root;
}

bool TupScene::moveLayer(int from, int to)
{
    if (from < 0 || from >= k->layers.count() || to < 0 || to >= k->layers.count())
        return false;

    TupLayer *layer = k->layers[from];

    k->layers.insert(to, layer);
    k->layers.remove(from);

    return true;
}

/*
int TupScene::logicalIndex() const
{
    if (TupProject *project = dynamic_cast<TupProject *>(parent()))
        return project->logicalIndexOf(const_cast<TupScene *>(this));
	
    return -1;
}
*/

int TupScene::objectIndex() const
{
    if (TupProject *project = dynamic_cast<TupProject *>(parent()))
        return project->visualIndexOf(const_cast<TupScene *>(this));

    return -1;
}

int TupScene::visualIndexOf(TupLayer *layer) const
{
    return k->layers.objectIndex(layer);
}

/*
int TupScene::logicalIndexOf(TupLayer *layer) const
{
    return k->layers.logicalIndex(layer);
}
*/

TupProject *TupScene::project() const
{
    return static_cast<TupProject *>(parent());
}

void TupScene::addTweenObject(TupGraphicObject *object)
{
    k->tweeningGraphicObjects << object;
}

void TupScene::addTweenObject(TupSvgItem *object)
{
    k->tweeningSvgObjects << object;
}

void TupScene::updateTweenObject(int index, TupGraphicObject *object)
{
    k->tweeningGraphicObjects.replace(index, object); 
}

void TupScene::updateTweenObject(int index, TupSvgItem *object)
{
    k->tweeningSvgObjects.replace(index, object);
}

void TupScene::removeTweenObject(TupGraphicObject *object)
{
    if (k->tweeningGraphicObjects.size() > 0)
        k->tweeningGraphicObjects.removeAll(object);
}

void TupScene::removeTweenObject(TupSvgItem *object)
{
    if (k->tweeningSvgObjects.size() > 0)
        k->tweeningSvgObjects.removeAll(object);
}

QList<TupGraphicObject *> TupScene::tweeningGraphicObjects() const
{
    return k->tweeningGraphicObjects;
}

QList<TupSvgItem *> TupScene::tweeningSvgObjects() const
{
    return k->tweeningSvgObjects;
}

bool TupScene::tweenExists(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    return false;
}

void TupScene::removeTween(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                 }
             }
    }
}

void TupScene::removeTweensFromLayer(int layer)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->frame()->layer()->layerIndex() == layer) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->frame()->layer()->layerIndex() == layer) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }
}

void TupScene::removeTweensFromFrame(int frame)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->frame()->index() == frame) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->frame()->index() == frame) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }
}

TupItemTweener *TupScene::tween(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     return tween;
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     return tween;
                 }
             }
    }

    return 0;
}

QList<QString> TupScene::getTweenNames(TupItemTweener::Type type)
{
    QList<QString> names;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if (tween->type() == type && !names.contains(tween->name()))
                     names.append(tween->name());
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if (tween->type() == type && !names.contains(tween->name()))
                     names.append(tween->name());
             } 
    }

    return names;
}

QList<QGraphicsItem *> TupScene::getItemsFromTween(const QString &name, TupItemTweener::Type type)
{
    QList<QGraphicsItem *> items;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     items.append(object->item());
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     items.append(object);
                 }
             }
    }

    return items;
}

int TupScene::getTotalTweens()
{
    int total = 0;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->hasTween()) 
                 total++;
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->hasTween())                    
                 total++;
    }

    return total;
}

int TupScene::framesTotal()
{
    int total = 0;
    foreach (TupLayer *layer, k->layers.values()) {
             int frames =layer->framesTotal();
             if (frames > total)
                 total = frames;
    }

    return total;
}

QList<int> TupScene::layerIndexes()
{
    return this->layers().indexes();
}

TupBackground* TupScene::background()
{
    return k->background;
}

void TupScene::reset(QString &name)
{
    k->name = name;

    k->background = new TupBackground(this);
    k->layers.clear();
    k->tweeningGraphicObjects.clear();
    k->tweeningSvgObjects.clear();

    k->layerCount = 1;
    TupLayer *layer = new TupLayer(this, k->layerCount);
    layer->setLayerName(tr("Layer %1").arg(1));
    layer->createFrame(tr("Frame %1").arg(1), 0, false);

    k->layers.insert(0, layer);
}
