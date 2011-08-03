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

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QCursor>
#include <QSvgRenderer>
#include "tdebug.h"

#include "ktframe.h"
#include "ktlayer.h"

#include "ktsvg2qt.h"
#include "ktitemfactory.h"
#include "ktserializer.h"
#include "ktgraphicobject.h"
#include "ktgraphiclibraryitem.h"
#include "ktlibrary.h"

#include "ktitemgroup.h"
#include "ktitemtweener.h"

#include "ktprojectloader.h"

#include "ktscene.h"
#include "ktlayer.h"

struct KTFrame::Private
{
    QString name;
    bool isLocked;
    bool isVisible;
    GraphicObjects graphics;
    QHash<int, QString> objectIndexes;
    SvgObjects svg;
    QHash<int, QString> svgIndexes;
    int repeat;
    int zLevelIndex;
    int layerIndex;
};

KTFrame::KTFrame(KTLayer *parent) : QObject(parent), k(new Private)
{
    k->layerIndex = parent->layerIndex();
    k->name = "Frame";
    k->isLocked = false;
    k->isVisible = true;
    k->repeat = 1;
    k->zLevelIndex = (k->layerIndex)*10000;
}

KTFrame::KTFrame(KTBackground *bg) : QObject(bg), k(new Private)
{
    k->layerIndex = 0;
    k->name = "Frame";
    k->isLocked = false;
    k->isVisible = true;
    k->repeat = 1;
    k->zLevelIndex = 0;
}

KTFrame::~KTFrame()
{
    k->objectIndexes.clear();
    k->svgIndexes.clear();
    k->graphics.clear(true);
    k->svg.clear(true);
    delete k;
}

void KTFrame::clean()
{
    k->graphics.clear(true);
    k->svg.clear(true);
}

void KTFrame::setFrameName(const QString &name)
{
    k->name = name;
}

void KTFrame::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

QString KTFrame::frameName() const
{
    return k->name;
}

bool KTFrame::isLocked() const
{
    return k->isLocked;
}

void KTFrame::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool KTFrame::isVisible() const
{
    return k->isVisible;
}

void KTFrame::fromXml(const QString &xml)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    tFatal() << "KTFrame::fromXml() - Loading frame!";

    QDomDocument document;
	
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG
               tError() << "KTFrame::fromXml() - File corrupted!";
        #endif
        return;
    }

    QDomElement root = document.documentElement();
    setFrameName(root.attribute("name", frameName()));

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {

               if (e.tagName() == "object") {
                   QDomNode n2 = e.firstChild();

                   KTGraphicObject *last = 0; // This variable contains the object in case of tweening 

                   while (!n2.isNull()) {
                          QDomElement e2 = n2.toElement();

                          if (e2.tagName() == "tweening" && last) {
                              KTItemTweener *tweener = new KTItemTweener(); 
                              QString newDoc;

                              {
                                QTextStream ts(&newDoc);
                                ts << n2;
                              }

                              tweener->fromXml(newDoc);
                              last->setTween(tweener);
                              scene()->addTweenObject(last);
                          } else {
                              QString newDoc;

                              {
                                QTextStream ts(&newDoc);
                                ts << n2;
                              }

                              QPointF point = QPointF();
                              QDomNode n3 = n2.firstChild();
                              while (!n3.isNull()) {
                                     QDomElement e3 = n3.toElement();
                                     if (e3.tagName() == "properties") {
                                         KTSvg2Qt::parsePointF(e3.attribute("pos"), point);
                                         break;
                                     }
                                     n3 = n3.nextSibling();
                              }
                              createItem(k->graphics.count(), point, newDoc);
                              last = k->graphics.value(k->graphics.count()-1);
                          }

                          n2 = n2.nextSibling();
                   }
               } else if (e.tagName() == "svg") {

                          QString symbol = e.attribute("id");

                          if (symbol.length() > 0) {
                              KTLibraryObject *object = project()->library()->findObject(symbol);

                              QString path(object->dataPath());
                              QDomNode n2 = e.firstChild();
                              KTSvgItem *svg = new KTSvgItem();

                              while (!n2.isNull()) {
                                     QDomElement e2 = n2.toElement();

                                     if (e2.tagName() == "properties") {

                                         svg = new KTSvgItem(path, this);
                                         svg->setSymbolName(symbol);
                                         KTSerializer::loadProperties(svg, e2);
                                         k->svgIndexes[k->svg.count()] = symbol;
                                         insertSvgItem(k->svg.count(), svg);

                                     } else if (e2.tagName() == "tweening") {

                                         KTItemTweener *tweener = new KTItemTweener();
                                         QString newDoc;
                                         {
                                             QTextStream ts(&newDoc);
                                             ts << n2;
                                         }
                                         tweener->fromXml(newDoc);
                                         svg->setTween(tweener);
                                         scene()->addTweenObject(svg);
                                     }

                                     n2 = n2.nextSibling(); 
                              }
                          } else {
                              #ifdef K_DEBUG
                                     tError() << "KTFrame::fromXml() - ERROR: Object id is null!";
                              #endif
                          }
               }
           }
		
           n = n.nextSibling();
    }
}

QDomElement KTFrame::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("frame");
    root.setAttribute("name", k->name);
    doc.appendChild(root);

    foreach (KTGraphicObject *object, k->graphics.values())
             root.appendChild(object->toXml(doc));

    foreach (KTSvgItem *object, k->svg.values())
             root.appendChild(object->toXml(doc));

    return root;
}

void KTFrame::addItem(QGraphicsItem *item)
{
    insertItem(k->graphics.count(), item);
}

void KTFrame::addItem(const QString &key, QGraphicsItem *item)
{
    int index = k->graphics.count();
    insertItem(index, item);
    k->objectIndexes[index] = key;
}

void KTFrame::removeItemFromFrame(const QString &key)
{
    foreach (int i, k->objectIndexes.keys()) {
             if (k->objectIndexes[i].compare(key) == 0)
                 removeGraphicAt(i);
    }
}

void KTFrame::updateIdFromFrame(const QString &oldId, const QString &newId)
{
    foreach (int i, k->objectIndexes.keys()) {

             if (k->objectIndexes[i].compare(oldId) == 0) {
                 k->objectIndexes[i] = newId;

                 KTGraphicObject *object = k->graphics.value(i);
                 KTGraphicLibraryItem *libraryItem = static_cast<KTGraphicLibraryItem *>(object->item());
                 libraryItem->setSymbolName(newId);

                 object->setObjectName(newId);
                 object->setItem(libraryItem);

                 k->graphics.insert(i, object);
             }
    }
}

void KTFrame::addSvgItem(const QString &key, KTSvgItem *item)
{
    int index = k->svg.count();
    insertSvgItem(index, item);
    k->svgIndexes[index] = key;
}

void KTFrame::removeSvgItemFromFrame(const QString &key)
{
    foreach (int i, k->svgIndexes.keys()) {
             if (k->svgIndexes[i].compare(key) == 0)
                 removeSvgAt(i);
    }
}

void KTFrame::updateSvgIdFromFrame(const QString &oldId, const QString &newId)
{
    foreach (int i, k->svgIndexes.keys()) {

             if (k->svgIndexes[i].compare(oldId) == 0) {
                 k->svgIndexes[i] = newId;

                 KTSvgItem *svgItem = k->svg.value(i);
                 svgItem->setSymbolName(newId);

                 k->svg.insert(i, svgItem);
             }
    }
}

void KTFrame::insertItem(int position, QGraphicsItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    KTGraphicObject *object = new KTGraphicObject(item, this);
    k->graphics.insert(position, object);
}

void KTFrame::insertSvgItem(int position, KTSvgItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    k->svg.insert(position, item);
}

QGraphicsItemGroup *KTFrame::createItemGroupAt(int position, QList<qreal> group)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    qSort(group.begin(), group.end());

    KTItemGroup *itemGroup = new KTItemGroup(0);
    int count = 0;

    foreach (qreal p, group) {
             int pos = (int)p - count;
             QGraphicsItem *item = this->item(pos);
             KTGraphicObject *object = this->graphic(pos);
             object->setItem(0);
             removeGraphicAt(pos);
             itemGroup->addToGroup(item);
             delete object;
             count++;
    }

    QGraphicsItem *block = qgraphicsitem_cast<QGraphicsItem *>(itemGroup);

    insertItem(position, block);

    return itemGroup;
}

QList<QGraphicsItem *> KTFrame::destroyItemGroup(int position)
{
    QList<QGraphicsItem *> items;

    if (KTItemGroup *group = qgraphicsitem_cast<KTItemGroup *>(item(position))) {
        removeGraphicAt(position);
        items = group->childs();
        foreach (QGraphicsItem *child, group->childs()) {
                 group->removeFromGroup(child);
                 addItem(child);
        }
    }

    return items;
}

void KTFrame::replaceItem(int position, QGraphicsItem *item)
{
    KTGraphicObject *toReplace = this->graphic(position);

    if (toReplace)
        toReplace->setItem(item);
}

bool KTFrame::moveItem(int currentPosition, int newPosition)
{
    #ifdef K_DEBUG
           T_FUNCINFO << "current "<< currentPosition << " new "  << newPosition;
    #endif

    if (currentPosition == newPosition || currentPosition < 0 
        || currentPosition >= k->graphics.count() || newPosition < 0 
        || newPosition >= k->graphics.count())
        return false;

    if (currentPosition < newPosition) {
        for (int i = currentPosition; i < newPosition; i++) {
             double tmp = k->graphics.value(i)->item()->zValue();
             k->graphics.value(i)->item()->setZValue(k->graphics.value(i+1)->item()->zValue());
             k->graphics.value(i+1)->item()->setZValue(tmp);
             k->graphics.copyObject(i, i+1);
        }
    } else {
             for (int i = currentPosition; i > newPosition; i--) {
                  double tmp = k->graphics.value(i)->item()->zValue();
                  k->graphics.value(i)->item()->setZValue(k->graphics.value(i-1)->item()->zValue());
                  k->graphics.value(i-1)->item()->setZValue(tmp);
                  k->graphics.copyObject(i, i-1);
             }
    }

    return true;
}

bool KTFrame::removeGraphicAt(int position)
{
    if (position < 0)
        return false;

    // KTGraphicObject *object = k->graphics.takeObject(position);
    KTGraphicObject *object = k->graphics.value(position);
    k->objectIndexes.remove(position);

    if (object->hasTween())
        this->scene()->removeTweenObject(object);

    k->objectIndexes.remove(position);
    k->graphics.remove(position);

    return true;
}

bool KTFrame::removeSvgAt(int position)
{
    if (position < 0)
        return false;

    KTSvgItem *item = k->svg.takeObject(position);
    k->svgIndexes.remove(position); 

    // SQA: Delete indexes here

    if (item) {
        // SQA: Verify if this is necessary
        QGraphicsScene *scene = item->scene();
        if (scene)
            scene->removeItem(item);
 
        // SQA: Pending to check
        //this->scene()->removeTweenObject(item);

        return true;
    } 

    return false;
}

QGraphicsItem *KTFrame::createItem(int position, QPointF coords, const QString &xml, bool loaded)
{
    KTItemFactory itemFactory;
    itemFactory.setLibrary(project()->library());
    QGraphicsItem *graphicItem = itemFactory.create(xml);
    graphicItem->setPos(coords);

    if (graphicItem) {
        insertItem(position, graphicItem);
        if (itemFactory.type() == KTItemFactory::Library) {
            QString tag = itemFactory.itemID(xml);
            k->objectIndexes[position] = tag;
        }
    }

    if (loaded)
        KTProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), position, 
                                    coords, KTLibraryObject::Item, xml, project());

    return graphicItem;
}

KTSvgItem *KTFrame::createSvgItem(int position, QPointF coords, const QString &xml, bool loaded)
{
    QDomDocument document;
    if (!document.setContent(xml))
        return 0;

    QDomElement root = document.documentElement();
    QString path = root.attribute("itemPath");

    KTSvgItem *item = new KTSvgItem(path, this);
    item->moveBy(coords.x(), coords.y()); 

    insertSvgItem(position, item);

    if (loaded)
        KTProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), 
                                    position, coords, KTLibraryObject::Svg, xml, project());

    return item;
}

void KTFrame::setGraphics(GraphicObjects objects)
{
    k->graphics = objects;
}

void KTFrame::setSvgObjects(SvgObjects items)
{
    k->svg = items;
}

GraphicObjects KTFrame::graphics() const
{
    return k->graphics;
}

SvgObjects KTFrame::svgItems() const
{
    return k->svg;
}

KTGraphicObject *KTFrame::graphic(int position) const
{
    if (position < 0) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound [KTFrame->graphic()] - index: " << position << " - total items: " << k->graphics.count();
        #endif

        return 0;
    } 

    return k->graphics.value(position);
}

KTSvgItem *KTFrame::svg(int position) const
{
    if (position < 0) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound [KTFrame->svg()] - index: " << position << " - total items: " << k->svg.count();
        #endif

        return 0;
    }

    return k->svg.value(position);
}


QGraphicsItem *KTFrame::item(int position) const
{
    KTGraphicObject *object = graphic(position);

    if (object)
        return object->item();

    return 0;
}

int KTFrame::indexOf(KTGraphicObject *object) const
{
    return k->graphics.objectIndex(object);
}

int KTFrame::indexOf(KTSvgItem *object) const
{
    return k->svg.objectIndex(object);
}

int KTFrame::indexOf(QGraphicsItem *item) const
{
    foreach (KTGraphicObject *object, k->graphics.values()) {
             if (object->item() == item)
                 return k->graphics.objectIndex(object);
    }

    return -1;
}

void KTFrame::setRepeat(int repeat)
{
    k->repeat = repeat;
}

int KTFrame::repeat() const
{
    return k->repeat;
}

int KTFrame::index() const
{
    return layer()->visualIndexOf(const_cast<KTFrame *>(this));
}

KTLayer *KTFrame::layer() const
{
    return static_cast<KTLayer *>(parent());
}

KTScene *KTFrame::scene() const
{
    return layer()->scene();
}

KTProject *KTFrame::project() const
{
    return layer()->project();
}

int KTFrame::graphicItemsCount()
{
    return k->graphics.count();
}

int KTFrame::svgItemsCount()
{
    return k->svg.count();
}

int KTFrame::getTopZLevel()
{
    // return k->graphics.count();
    return k->zLevelIndex;
}

QList<int> KTFrame::itemIndexes()
{
    return k->graphics.indexes();
}

QList<int> KTFrame::svgIndexes()
{
    return k->svg.indexes();
}

bool KTFrame::isEmpty()
{
    int total = graphicItemsCount() + svgItemsCount();
    tError() << "KTFrame::isEmpty() - ItemsCount: " << total;
    if (graphicItemsCount() + svgItemsCount() > 0)
        return false;

    return true;
}
