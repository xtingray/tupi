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

#include "ktscene.h"
#include "kdebug.h"

#include <QDir>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

#include "ktgraphicobject.h"
#include "ktsvgitem.h"
#include "ktsoundlayer.h"

#include "ktitemgroup.h"
#include "ktprojectloader.h"
#include "ktitemfactory.h"

struct KTScene::Private
{
    KTBackground *background;
    Layers layers;
    SoundLayers soundLayers;
    QString name;
    bool isLocked;
    int layerCount;
    int nameIndex;
    bool isVisible;

    QList<KTGraphicObject *> tweeningGraphicObjects;
    QList<KTSvgItem *> tweeningSvgObjects;
};

KTScene::KTScene(KTProject *parent) : QObject(parent), k(new Private)
{
    k->isLocked = false;
    k->layerCount = 0;
    k->nameIndex = 0;
    k->isVisible = true;
    k->background = new KTBackground(this);
}

KTScene::~KTScene()
{
    #ifdef K_DEBUG
           KEND;
    #endif

    //k->layers.clear(true);
    delete k;
}

void KTScene::setSceneName(const QString &name)
{
    k->name = name;
}

void KTScene::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

QString KTScene::sceneName() const
{
    return k->name;
}

bool KTScene::isLocked() const
{
    return k->isLocked;
}

void KTScene::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool KTScene::isVisible() const
{
    return k->isVisible;
}

Layers KTScene::layers() const
{
    return k->layers;
}

int KTScene::layersTotal() const
{
    return k->layerCount;
}

SoundLayers KTScene::soundLayers() const
{
    return k->soundLayers;
}

/**
 * Set the layers list, this function overwrites the old layers
 */
void KTScene::setLayers(const Layers &layers)
{
    k->layers = layers;
}

KTLayer *KTScene::createLayer(int position, bool loaded)
{
    // Q_CHECK_PTR(k->layers);

    if (position < 0 || position > k->layers.count()) {
        #ifdef K_DEBUG
               kDebug() << "KTScene::createLayer() - Invalid index";
        #endif
        return 0;
    }

    k->layerCount++;
    k->nameIndex++;

    KTLayer *layer = new KTLayer(this, k->layerCount);

    layer->setLayerName(tr("Layer %1").arg(k->nameIndex));

    k->layers.insert(position, layer);

    if (loaded)
        KTProjectLoader::createLayer(project()->visualIndexOf(this), position, layer->layerName(), project());

    return layer;
}

KTSoundLayer *KTScene::createSoundLayer(int position, bool loaded)
{
    #ifdef K_DEBUG
           K_FUNCINFO << position;
    #endif

    if (position < 0 || position > k->soundLayers.count()) {
        kDebug() << "Error in createSoundLayer";
        return 0;
    }

    KTSoundLayer *layer = new KTSoundLayer(this);
    k->layerCount++;

    layer->setLayerName(tr("Sound layer %1").arg(k->nameIndex));

    k->soundLayers.insert(position, layer);

    if (loaded)
        KTProjectLoader::createSoundLayer(objectIndex(), position, layer->layerName(), project());

    return layer;
}

bool KTScene::removeLayer(int position)
{
    // Q_CHECK_PTR(layers);

    KTLayer *layer = this->layer(position);

    if (layer) {
        k->layers.remove(position);

        k->layerCount--;
        if (k->nameIndex == position + 1) {
            k->nameIndex--;
        } else {
            if (k->layerCount == 0) {
                k->nameIndex = 0;
            }
        }

        QList<int> indexList = this->layers().indexes();
        int size = this->layersTotal();

        for (int i = 0; i < size; i++) 
             KTLayer *layer = this->layer(indexList.at(i));

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
KTLayer *KTScene::layer(int position) const
{
    //if (position < 0 || position >= k->layers.count()) {

    if (position < 0) {
        #ifdef K_DEBUG
               K_FUNCINFO << " FATAL ERROR: LAYERS TOTAL: " << k->layers.count();
               K_FUNCINFO << " FATAL ERROR: index out of bound -> Position: " << position;
               K_FUNCINFO << " FATAL ERROR: The layer requested doesn't exist anymore";
        #endif
        return 0;
    }

    return k->layers.value(position);
}

KTSoundLayer *KTScene::soundLayer(int position) const
{
    if (position < 0 || position >= k->soundLayers.count()) {
        #ifdef K_DEBUG
               K_FUNCINFO << " FATAL ERROR: index out of bound " << position;
        #endif
        return 0;
    }

    return k->soundLayers.value(position);
}

void KTScene::fromXml(const QString &xml)
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
                   KTLayer *layer = createLayer(pos, true);

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
                          KTSoundLayer *layer = createSoundLayer(pos, true);

                          if (layer) {
                              QString newDoc;
                               {
                                 QTextStream ts(&newDoc);
                                 ts << n;
                               }

                              layer->fromXml(newDoc);
                          }
               }
           }

           n = n.nextSibling();

    } // end while
}

QDomElement KTScene::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("scene");
    root.setAttribute("name", k->name);

    root.appendChild(k->background->toXml(doc));

    foreach (KTLayer *layer, k->layers.values())
             root.appendChild(layer->toXml(doc));

    foreach (KTSoundLayer *sound, k->soundLayers.values())
             root.appendChild(sound->toXml(doc));

    return root;
}

bool KTScene::moveLayer(int from, int to)
{
    if (from < 0 || from >= k->layers.count() || to < 0 || to >= k->layers.count())
        return false;

    KTLayer *layer = k->layers[from];

    k->layers.insert(to, layer);
    k->layers.remove(from);

    return true;
}

/*
int KTScene::logicalIndex() const
{
    if (KTProject *project = dynamic_cast<KTProject *>(parent()))
        return project->logicalIndexOf(const_cast<KTScene *>(this));
	
    return -1;
}
*/

int KTScene::objectIndex() const
{
    if (KTProject *project = dynamic_cast<KTProject *>(parent()))
        return project->visualIndexOf(const_cast<KTScene *>(this));

    return -1;
}

int KTScene::visualIndexOf(KTLayer *layer) const
{
    return k->layers.objectIndex(layer);
}

/*
int KTScene::logicalIndexOf(KTLayer *layer) const
{
    return k->layers.logicalIndex(layer);
}
*/

KTProject *KTScene::project() const
{
    return static_cast<KTProject *>(parent());
}

void KTScene::addTweenObject(KTGraphicObject *object)
{
    k->tweeningGraphicObjects << object;
}

void KTScene::addTweenObject(KTSvgItem *object)
{
    k->tweeningSvgObjects << object;
}

void KTScene::removeTweenObject(KTGraphicObject *object)
{
    if (k->tweeningGraphicObjects.size() > 0)
        k->tweeningGraphicObjects.removeAll(object);
}

void KTScene::removeTweenObject(KTSvgItem *object)
{
    if (k->tweeningSvgObjects.size() > 0)
        k->tweeningSvgObjects.removeAll(object);
}

QList<KTGraphicObject *> KTScene::tweeningGraphicObjects() const
{
    return k->tweeningGraphicObjects;
}

QList<KTSvgItem *> KTScene::tweeningSvgObjects() const
{
    return k->tweeningSvgObjects;
}

bool KTScene::tweenExists(const QString &name)
{
    foreach (KTGraphicObject *object, k->tweeningGraphicObjects) {
             if (KTItemTweener *tween = object->tween()) {
                 if (tween->name().compare(name) == 0)
                     return true;
             }
    }

    foreach (KTSvgItem *object, k->tweeningSvgObjects) {
             if (KTItemTweener *tween = object->tween()) {
                 if (tween->name().compare(name) == 0)
                     return true;
             }
    }

    return false;
}

void KTScene::removeTween(const QString &name)
{
    foreach (KTGraphicObject *object, k->tweeningGraphicObjects) {
             if (KTItemTweener *tween = object->tween()) {
                 if (tween->name().compare(name) == 0) {
                     object->removeTween();
                     removeTweenObject(object);
                     return;
                 }
             }
    }

    foreach (KTSvgItem *object, k->tweeningSvgObjects) {
             if (KTItemTweener *tween = object->tween()) {
                 if (tween->name().compare(name) == 0) {
                     object->removeTween();
                     removeTweenObject(object);
                     return;
                 }
             }
    }
}

QList<QString> KTScene::getTweenNames()
{
    QList<QString> names;

    foreach (KTGraphicObject *object, k->tweeningGraphicObjects) {
             if (KTItemTweener *tween = object->tween())
                 names.append(tween->name());
    }

    foreach (KTSvgItem *object, k->tweeningSvgObjects) {
             if (KTItemTweener *tween = object->tween()) 
                 names.append(tween->name());
    }

    return names;
}

int KTScene::getTotalTweens()
{
    int total = 0;

    foreach (KTGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->hasTween()) 
                 total++;
    }

    foreach (KTSvgItem *object, k->tweeningSvgObjects) {
             if (object->hasTween())                    
                 total++;
    }

    return total;
}

int KTScene::framesTotal()
{
    int total = 0;
    foreach (KTLayer *layer, k->layers.values()) {
             int frames =layer->framesNumber();
             if (frames > total)
                 total = frames;
    }

    return total;
}

QList<int> KTScene::layerIndexes()
{
    return this->layers().indexes();
}

KTBackground* KTScene::background()
{
    return k->background;
}
