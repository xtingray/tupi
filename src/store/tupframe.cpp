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

#include "tupframe.h"
#include "tuplayer.h"
#include "tupsvg2qt.h"
#include "tupitemfactory.h"
#include "tupserializer.h"
#include "tupgraphicobject.h"
#include "tupgraphiclibraryitem.h"
#include "tuplibrary.h"
#include "tupitemgroup.h"
#include "tupitemtweener.h"
#include "tupprojectloader.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tdebug.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QCursor>
#include <QSvgRenderer>

struct TupFrame::Private
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

TupFrame::TupFrame(TupLayer *parent) : QObject(parent), k(new Private)
{
    k->layerIndex = parent->layerIndex();
    k->name = "Frame";
    k->isLocked = false;
    k->isVisible = true;
    k->repeat = 1;
    k->zLevelIndex = (k->layerIndex)*10000;
}

TupFrame::TupFrame(TupBackground *bg) : QObject(bg), k(new Private)
{
    k->layerIndex = 0;
    k->name = "Frame";
    k->isLocked = false;
    k->isVisible = true;
    k->repeat = 1;
    k->zLevelIndex = 0;
}

TupFrame::~TupFrame()
{
    k->objectIndexes.clear();
    k->svgIndexes.clear();
    k->graphics.clear(true);
    k->svg.clear(true);
    delete k;
}

void TupFrame::clear()
{
    k->graphics.clear(true);
    k->svg.clear(true);
}

void TupFrame::setFrameName(const QString &name)
{
    k->name = name;
}

void TupFrame::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

QString TupFrame::frameName() const
{
    return k->name;
}

bool TupFrame::isLocked() const
{
    return k->isLocked;
}

void TupFrame::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool TupFrame::isVisible() const
{
    return k->isVisible;
}

void TupFrame::fromXml(const QString &xml)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QDomDocument document;
	
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG
               tError() << "TupFrame::fromXml() - File corrupted!";
               tWarning() << "Content:";
               tWarning() << xml;
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

                   TupGraphicObject *last = 0; // This variable contains the object in case of tweening 

                   while (!n2.isNull()) {
                          QDomElement e2 = n2.toElement();

                          if (e2.tagName() == "tweening" && last) {
                              TupItemTweener *tweener = new TupItemTweener(); 
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
                                         TupSvg2Qt::parsePointF(e3.attribute("pos"), point);
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
                              TupLibraryObject *object = project()->library()->findObject(symbol);

                              QString path(object->dataPath());
                              QDomNode n2 = e.firstChild();
                              TupSvgItem *svg = new TupSvgItem();

                              while (!n2.isNull()) {
                                     QDomElement e2 = n2.toElement();

                                     if (e2.tagName() == "properties") {

                                         svg = new TupSvgItem(path, this);
                                         svg->setSymbolName(symbol);
                                         TupSerializer::loadProperties(svg, e2);
                                         k->svgIndexes[k->svg.count()] = symbol;
                                         insertSvgItem(k->svg.count(), svg);

                                     } else if (e2.tagName() == "tweening") {

                                         TupItemTweener *tweener = new TupItemTweener();
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
                                     tError() << "TupFrame::fromXml() - ERROR: Object id is null!";
                              #endif
                          }
               } 
           }
		
           n = n.nextSibling();
    }
}

QDomElement TupFrame::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("frame");
    root.setAttribute("name", k->name);
    doc.appendChild(root);

    foreach (TupGraphicObject *object, k->graphics.values())
             root.appendChild(object->toXml(doc));

    foreach (TupSvgItem *object, k->svg.values())
             root.appendChild(object->toXml(doc));

    return root;
}

void TupFrame::addItem(QGraphicsItem *item)
{
    insertItem(k->graphics.count(), item);
}

void TupFrame::addItem(const QString &key, QGraphicsItem *item)
{
    int index = k->graphics.count();
    insertItem(index, item);
    k->objectIndexes[index] = key;
}

void TupFrame::removeItemFromFrame(const QString &key)
{
    foreach (int i, k->objectIndexes.keys()) {
             if (k->objectIndexes[i].compare(key) == 0)
                 removeGraphicAt(i);
    }
}

void TupFrame::updateIdFromFrame(const QString &oldId, const QString &newId)
{
    foreach (int i, k->objectIndexes.keys()) {

             if (k->objectIndexes[i].compare(oldId) == 0) {
                 k->objectIndexes[i] = newId;

                 TupGraphicObject *object = k->graphics.value(i);
                 TupGraphicLibraryItem *libraryItem = static_cast<TupGraphicLibraryItem *>(object->item());
                 libraryItem->setSymbolName(newId);

                 object->setObjectName(newId);
                 object->setItem(libraryItem);

                 k->graphics.insert(i, object);
             }
    }
}

void TupFrame::addSvgItem(const QString &key, TupSvgItem *item)
{
    int index = k->svg.count();
    insertSvgItem(index, item);
    k->svgIndexes[index] = key;
}

void TupFrame::removeSvgItemFromFrame(const QString &key)
{
    foreach (int i, k->svgIndexes.keys()) {
             if (k->svgIndexes[i].compare(key) == 0)
                 removeSvgAt(i);
    }
}

void TupFrame::updateSvgIdFromFrame(const QString &oldId, const QString &newId)
{
    foreach (int i, k->svgIndexes.keys()) {

             if (k->svgIndexes[i].compare(oldId) == 0) {
                 k->svgIndexes[i] = newId;

                 TupSvgItem *svgItem = k->svg.value(i);
                 svgItem->setSymbolName(newId);

                 k->svg.insert(i, svgItem);
             }
    }
}

void TupFrame::insertItem(int position, QGraphicsItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    TupGraphicObject *object = new TupGraphicObject(item, this);
    k->graphics.insert(position, object);
}

void TupFrame::insertSvgItem(int position, TupSvgItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    k->svg.insert(position, item);
}

QGraphicsItemGroup *TupFrame::createItemGroupAt(int position, QList<qreal> group)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    qSort(group.begin(), group.end());

    TupItemGroup *itemGroup = new TupItemGroup(0);
    int count = 0;

    foreach (qreal p, group) {
             int pos = (int)p - count;
             QGraphicsItem *item = this->item(pos);
             TupGraphicObject *object = this->graphic(pos);
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

QList<QGraphicsItem *> TupFrame::destroyItemGroup(int position)
{
    QList<QGraphicsItem *> items;

    if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item(position))) {
        removeGraphicAt(position);
        items = group->childs();
        foreach (QGraphicsItem *child, group->childs()) {
                 group->removeFromGroup(child);
                 addItem(child);
        }
    }

    return items;
}

void TupFrame::replaceItem(int position, QGraphicsItem *item)
{
    TupGraphicObject *toReplace = this->graphic(position);

    if (toReplace)
        toReplace->setItem(item);
}

bool TupFrame::moveItem(int currentPosition, int newPosition)
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

bool TupFrame::removeGraphicAt(int position)
{
    if (position < 0)
        return false;

    // TupGraphicObject *object = k->graphics.takeObject(position);
    TupGraphicObject *object = k->graphics.value(position);
    k->objectIndexes.remove(position);

    if (object->hasTween())
        this->scene()->removeTweenObject(object);

    k->objectIndexes.remove(position);
    k->graphics.remove(position);

    return true;
}

bool TupFrame::removeSvgAt(int position)
{
    if (position < 0)
        return false;

    TupSvgItem *item = k->svg.takeObject(position);
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

QGraphicsItem *TupFrame::createItem(int position, QPointF coords, const QString &xml, bool loaded)
{
    TupItemFactory itemFactory;
    itemFactory.setLibrary(project()->library());
    QGraphicsItem *graphicItem = itemFactory.create(xml);
    graphicItem->setPos(coords);

    if (graphicItem) {
        insertItem(position, graphicItem);
        if (itemFactory.type() == TupItemFactory::Library) {
            QString tag = itemFactory.itemID(xml);
            k->objectIndexes[position] = tag;
        }
    }

    if (loaded)
        TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), position, 
                                    coords, TupLibraryObject::Item, xml, project());

    return graphicItem;
}

TupSvgItem *TupFrame::createSvgItem(int position, QPointF coords, const QString &xml, bool loaded)
{
    QDomDocument document;
    if (!document.setContent(xml))
        return 0;

    QDomElement root = document.documentElement();
    QString path = root.attribute("itemPath");

    TupSvgItem *item = new TupSvgItem(path, this);
    item->moveBy(coords.x(), coords.y()); 

    insertSvgItem(position, item);

    if (loaded)
        TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), 
                                    position, coords, TupLibraryObject::Svg, xml, project());

    return item;
}

void TupFrame::setGraphics(GraphicObjects objects)
{
    k->graphics = objects;
}

void TupFrame::setSvgObjects(SvgObjects items)
{
    k->svg = items;
}

GraphicObjects TupFrame::graphics() const
{
    return k->graphics;
}

SvgObjects TupFrame::svgItems() const
{
    return k->svg;
}

TupGraphicObject *TupFrame::graphic(int position) const
{
    if (position < 0) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound [TupFrame->graphic()] - index: " << position << " - total items: " << k->graphics.count();
        #endif

        return 0;
    } 

    return k->graphics.value(position);
}

TupSvgItem *TupFrame::svg(int position) const
{
    if (position < 0) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound [TupFrame->svg()] - index: " << position << " - total items: " << k->svg.count();
        #endif

        return 0;
    }

    return k->svg.value(position);
}


QGraphicsItem *TupFrame::item(int position) const
{
    TupGraphicObject *object = graphic(position);

    if (object)
        return object->item();

    return 0;
}

int TupFrame::indexOf(TupGraphicObject *object) const
{
    return k->graphics.objectIndex(object);
}

int TupFrame::indexOf(TupSvgItem *object) const
{
    return k->svg.objectIndex(object);
}

int TupFrame::indexOf(QGraphicsItem *item) const
{
    tError() << "TupFrame::indexOf() - Graphics Vector Size: " << k->graphics.count();

    foreach (TupGraphicObject *object, k->graphics.values()) {
             if (object->item() == item)
                 return k->graphics.objectIndex(object);
    }

    return -1;
}

void TupFrame::setRepeat(int repeat)
{
    k->repeat = repeat;
}

int TupFrame::repeat() const
{
    return k->repeat;
}

int TupFrame::index() const
{
    return layer()->visualIndexOf(const_cast<TupFrame *>(this));
}

TupLayer *TupFrame::layer() const
{
    return static_cast<TupLayer *>(parent());
}

TupScene *TupFrame::scene() const
{
    return layer()->scene();
}

TupProject *TupFrame::project() const
{
    return layer()->project();
}

int TupFrame::graphicItemsCount()
{
    return k->graphics.count();
}

int TupFrame::svgItemsCount()
{
    return k->svg.count();
}

int TupFrame::getTopZLevel()
{
    // return k->graphics.count();
    return k->zLevelIndex;
}

QList<int> TupFrame::itemIndexes()
{
    return k->graphics.indexes();
}

QList<int> TupFrame::svgIndexes()
{
    return k->svg.indexes();
}

bool TupFrame::isEmpty()
{
    if (graphicItemsCount() + svgItemsCount() > 0)
        return false;

    return true;
}
