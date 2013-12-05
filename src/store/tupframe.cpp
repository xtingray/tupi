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

#include "tupframe.h"
#include "tuplayer.h"
#include "tupsvg2qt.h"
#include "tupitemfactory.h"
#include "tupserializer.h"
#include "tupgraphicobject.h"
#include "tupgraphiclibraryitem.h"
#include "tuppixmapitem.h"
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
    FrameType type;
    bool isLocked;
    bool isVisible;

    QString direction;
    QString shift;

    GraphicObjects graphics;
    QList<QString> objectIndexes;
    SvgObjects svg;
    QList<QString> svgIndexes;
    int repeat;
    int zLevelIndex;
    int layerIndex;
};

TupFrame::TupFrame() : k(new Private)
{
}

TupFrame::TupFrame(TupLayer *parent) : QObject(parent), k(new Private)
{
    k->layerIndex = parent->layerIndex();
    k->name = "Frame";
    k->type = Regular;

    k->isLocked = false;
    k->isVisible = true;

    k->direction = "-1";
    k->shift = "0";

    k->repeat = 1;
    k->zLevelIndex = (k->layerIndex + 1)*10000; // Layers levels starts from 2
}

TupFrame::TupFrame(TupBackground *bg, const QString &label) : QObject(bg), k(new Private)
{
    k->layerIndex = 0;
    k->name = label;
    k->isLocked = false;
    k->isVisible = true;
    k->repeat = 1;

    k->direction = "-1";
    k->shift = "0";

    if (k->name.compare("landscape_dynamic") == 0) {
        k->zLevelIndex = 0;
        k->type = DynamicBg;
    } else {
        k->zLevelIndex = 10000;
        k->type = StaticBg;
    }
}

TupFrame::~TupFrame()
{
    k->objectIndexes.clear();
    k->svgIndexes.clear();

    k->graphics.clear();
    k->svg.clear();

    delete k;
}

void TupFrame::clear()
{
    k->objectIndexes.clear();
    k->svgIndexes.clear();

    k->graphics.clear();
    k->svg.clear();
}

void TupFrame::setFrameName(const QString &name)
{
    k->name = name;
}

void TupFrame::setDynamicDirection(const QString &direction)
{
    k->direction = direction;
}

void TupFrame::setDynamicShift(const QString &shift)
{
    k->shift = shift;
}

TupBackground::Direction TupFrame::dynamicDirection() const
{
    return TupBackground::Direction(k->direction.toInt());
}

int TupFrame::dynamicShift() const
{
    return k->shift.toInt();
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
    setFrameName(root.attribute("name", ""));

    if (k->type == DynamicBg) {
        setDynamicDirection(root.attribute("direction", "0"));
        setDynamicShift(root.attribute("shift", "0"));
    }

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
                              createItem(point, newDoc);
                              last = k->graphics.at(k->graphics.size()-1);
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
                                         svg->setZValue(k->zLevelIndex);
                                         k->zLevelIndex++;
                                         k->svgIndexes.append(symbol);
                                         k->svg.append(svg);
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
                                     tError() << "TupFrame::fromXml() - Fatal Error: Object id is NULL!";
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

    if (k->type == DynamicBg) {
        root.setAttribute("direction", k->direction);
        root.setAttribute("shift", k->shift);
    }

    doc.appendChild(root);

    for (int i=0; i < k->graphics.size(); i++) {
         TupGraphicObject *object = k->graphics.at(i);
         root.appendChild(object->toXml(doc));
    }

    for (int i=0; i < k->svg.size(); i++) {
         TupSvgItem *object = k->svg.at(i); 
         root.appendChild(object->toXml(doc));
    }

    return root;
}

void TupFrame::addItem(const QString &id, QGraphicsItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;
    TupGraphicObject *object = new TupGraphicObject(item, this);
    object->setObjectName(id);

    k->graphics.append(object);
    k->objectIndexes.append(id);
}

void TupFrame::removeImageItemFromFrame(const QString &id)
{
    for (int i=0; i<k->objectIndexes.size(); i++) {
             if (k->objectIndexes[i].compare(id) == 0)
                 removeGraphicAt(i);
    }
}

void TupFrame::updateIdFromFrame(const QString &oldId, const QString &newId)
{
    for (int i=0; i<k->objectIndexes.size(); i++) {
             if (k->objectIndexes.at(i).compare(oldId) == 0) {
                 k->objectIndexes[i] = newId;

                 TupGraphicObject *object = k->graphics.at(i);
                 TupGraphicLibraryItem *libraryItem = static_cast<TupGraphicLibraryItem *>(object->item());
                 libraryItem->setSymbolName(newId);

                 object->setObjectName(newId);
                 object->setItem(libraryItem);

                 k->graphics[i] = object;
             }
    }
}

void TupFrame::addSvgItem(const QString &id, TupSvgItem *item)
{
    #ifdef K_DEBUG
           T_FUNCINFO << id;
    #endif

    k->svgIndexes.append(id);
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    k->svg.append(item);
}

void TupFrame::removeSvgItemFromFrame(const QString &id)
{
    for (int i = 0; i < k->svgIndexes.size(); ++i) {
         if (k->svgIndexes.at(i).compare(id) == 0)
             removeSvgAt(i); 
    }
}

void TupFrame::updateSvgIdFromFrame(const QString &oldId, const QString &newId)
{
    for (int i = 0; i < k->svgIndexes.size(); ++i) {
         if (k->svgIndexes.at(i).compare(oldId) == 0) {
             k->svgIndexes[i] = newId; 
             TupSvgItem *svgItem = k->svg.at(i);
             svgItem->setSymbolName(newId);
             k->svg[i] = svgItem;
         }
    }
}

// SQA: This method is really required?
/*
void TupFrame::insertItem(int position, QGraphicsItem *item)
{
    QGraphicsItem *oldItem = k->graphics.at(position)->item();
    int zLevel = oldItem->zValue();

    item->setZValue(zLevel);
    TupGraphicObject *object = new TupGraphicObject(item, this);
    k->graphics[position] = object;
}
*/

/*
void TupFrame::addSvgItem(int position, TupSvgItem *item)
{
    item->setZValue(k->zLevelIndex);
    k->zLevelIndex++;

    k->svg[position] = item;
}
*/

// SQA: This method requires tests + refactoring
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

    // QGraphicsItem *block = qgraphicsitem_cast<QGraphicsItem *>(itemGroup);
    // insertItem(position, block);

    return itemGroup;
}

// SQA: This method requires tests + refactoing
QList<QGraphicsItem *> TupFrame::destroyItemGroup(int position)
{
    QList<QGraphicsItem *> items;

    if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item(position))) {
        removeGraphicAt(position);
        items = group->childs();
        foreach (QGraphicsItem *child, group->childs()) {
                 group->removeFromGroup(child);
                 // addItem(child);
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

bool TupFrame::moveItem(TupLibraryObject::Type type, int currentIndex, int action)
{
    MoveItemType move = MoveItemType(action); 
    switch(move) {
           case MoveBack :
             {
                int zMin = (k->layerIndex + 1)*10000;

                if (type == TupLibraryObject::Svg) {
                    int zLimit = k->svg.at(currentIndex)->zValue();
                    if (zLimit == zMin)
                        return true;

                    if (k->svg.size() > 1) {
                        TupSvgItem *object = k->svg.takeAt(currentIndex);
                        QString id = k->svgIndexes.takeAt(currentIndex);

                        object->setZValue(zMin);
                        k->svg.insert(0, object);
                        k->svgIndexes.insert(0, id);

                        for (int i=1; i <= currentIndex; ++i) {
                             int zLevel = k->svg.at(i)->zValue();
                             if (zLevel < zLimit)
                                 k->svg.at(i)->setZValue(zLevel + 1);
                        } 
                    } else {
                        k->svg.at(currentIndex)->setZValue(zMin);
                    }

                    for (int i=0; i < k->graphics.size(); ++i) {
                         int zLevel = k->graphics.at(i)->itemZValue();
                         if (zLevel < zLimit)
                             k->graphics.at(i)->setItemZValue(zLevel + 1);
                    }

                    return true;
                } else {
                    int zLimit = k->graphics.at(currentIndex)->itemZValue();
                    if (zLimit == zMin)
                        return true;

                    if (k->graphics.size() > 1) {
                        TupGraphicObject *object = k->graphics.takeAt(currentIndex);
                        QString id = k->objectIndexes.takeAt(currentIndex);

                        object->setItemZValue(zMin);
                        k->graphics.insert(0, object);
                        k->objectIndexes.insert(0, id);

                        for (int i=1; i < k->graphics.size(); ++i) {
                             int zLevel = k->graphics.at(i)->itemZValue();
                             if (zLevel < zLimit)
                                 k->graphics.at(i)->setItemZValue(zLevel + 1);
                        }

                    } else {
                        k->graphics.at(currentIndex)->setItemZValue(zMin);
                    }
            
                    for (int i=0; i < k->svg.size(); ++i) {
                         int zLevel = k->svg.at(i)->zValue();
                         if (zLevel < zLimit)
                             k->svg.at(i)->setZValue(zLevel + 1);
                    }

                    return true;
                }
             }
           break;
           case MoveToFront :
             {
                int zMax = k->zLevelIndex - 1;
                if (type == TupLibraryObject::Svg) {
                    int zLimit = k->svg.at(currentIndex)->zValue();
                    if (zLimit == zMax)
                        return true;

                    if (k->svg.size() > 1) {
                        TupSvgItem *object = k->svg.takeAt(currentIndex);
                        QString id = k->svgIndexes.takeAt(currentIndex);

                        for (int i=currentIndex; i < k->svg.size(); ++i) {
                             int zLevel = k->svg.at(i)->zValue();
                             k->svg.at(i)->setZValue(zLevel - 1);
                        }

                        object->setZValue(zMax);
                        k->svg.append(object);
                        k->svgIndexes.append(id);
                    } else {
                        k->svg.at(currentIndex)->setZValue(zMax);
                    }

                    for (int i=0; i < k->graphics.size(); ++i) {
                         int zLevel = k->graphics.at(i)->itemZValue();
                         if (zLevel > zLimit)
                             k->graphics.at(i)->setItemZValue(zLevel - 1);
                    }

                    return true;
                } else {
                    int zLimit = k->graphics.at(currentIndex)->itemZValue();
                    if (zLimit == zMax)
                        return true;

                    if (k->graphics.size() > 1) {
                        TupGraphicObject *object = k->graphics.takeAt(currentIndex);
                        QString id = k->objectIndexes.takeAt(currentIndex);

                        for (int i=currentIndex; i < k->graphics.size(); ++i) {
                             int zLevel = k->graphics.at(i)->itemZValue();
                             k->graphics.at(i)->setItemZValue(zLevel - 1);
                        }

                        object->setItemZValue(zMax);
                        k->graphics.append(object);
                        k->objectIndexes.append(id);
                    } else {
                        k->graphics.at(currentIndex)->setItemZValue(zMax); 
                    }

                    for (int i=0; i < k->svg.size(); ++i) {
                         int zLevel = k->svg.at(i)->zValue();
                         if (zLevel > zLimit)
                             k->svg.at(i)->setZValue(zLevel - 1);
                    }

                    return true;
                }
             }
           break;
           case MoveOneLevelBack :
             {
                int zMin = (k->layerIndex + 1)*10000;

                if (type == TupLibraryObject::Svg) {
                    if (k->svg.at(currentIndex)->zValue() == zMin)
                        return true;

                    TupSvgItem *object = k->svg.at(currentIndex);
                    int zLimit = object->zValue();

                    if ((k->svg.size() > 1) && (currentIndex > 0)) { 
                        object = k->svg.at(currentIndex - 1);
                        int downzValue = object->zValue();
                        if (downzValue == (zLimit - 1)) {
                            k->svg.at(currentIndex)->setZValue(downzValue);
                            k->svg.at(currentIndex - 1)->setZValue(zLimit);
                            k->svg.swap(currentIndex, currentIndex -1);
                            k->svgIndexes.swap(currentIndex, currentIndex -1);
                            return true;
                        } else {
                            for (int i=0; i < k->graphics.size(); ++i) {
                                 int zLevel = k->graphics.at(i)->itemZValue();
                                 if (zLevel == (zLimit - 1)) {
                                     k->svg.at(currentIndex)->setZValue(zLevel);
                                     k->graphics.at(i)->setItemZValue(zLimit); 
                                     return true;
                                 }
                            }
                        } 
                    } else {
                        if (!k->graphics.isEmpty()) {
                            for (int i=0; i < k->graphics.size(); ++i) {
                                 int zLevel = k->graphics.at(i)->itemZValue();
                                 if (zLevel == (zLimit - 1)) {
                                     k->svg.at(currentIndex)->setZValue(zLevel);
                                     k->graphics.at(i)->setItemZValue(zLimit);
                                     return true;
                                 }
                            }
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelBack/Svg ]";
                            #endif
                            return false;
                        }
                    }
                } else {
                    if (k->graphics.at(currentIndex)->itemZValue() == zMin)
                        return true;

                    TupGraphicObject *object = k->graphics.at(currentIndex);
                    int zLimit = object->itemZValue();

                    if ((k->graphics.size() > 1) && (currentIndex > 0)) {
                        object = k->graphics.at(currentIndex - 1);
                        int downzValue = object->itemZValue();
                        if (downzValue == (zLimit - 1)) {
                            k->graphics.at(currentIndex)->setItemZValue(downzValue);
                            k->graphics.at(currentIndex - 1)->setItemZValue(zLimit);
                            k->graphics.swap(currentIndex, currentIndex - 1);
                            k->objectIndexes.swap(currentIndex, currentIndex - 1);
                            return true;
                        } else {
                            for (int i=0; i < k->svg.size(); ++i) {
                                 int zLevel = k->svg.at(i)->zValue();
                                 if (zLevel == (zLimit - 1)) {
                                     k->graphics.at(currentIndex)->setItemZValue(zLevel);
                                     k->svg.at(i)->setZValue(zLimit);
                                     return true;
                                 }
                            }
                        }
                    } else {
                        if (!k->svg.isEmpty()) {
                            for (int i=0; i < k->svg.size(); ++i) {
                                 int zLevel = k->svg.at(i)->zValue();
                                 if (zLevel == (zLimit - 1)) {
                                     k->graphics.at(currentIndex)->setItemZValue(zLevel);
                                     k->svg.at(i)->setZValue(zLimit);
                                     return true;
                                 }
                            }
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelBack/Items ]";
                            #endif
                            return false;
                        }
                    }
                }
             }
           break;
           case MoveOneLevelToFront :
             {
                int zMax = k->zLevelIndex - 1;

                if (type == TupLibraryObject::Svg) {
                    if (k->svg.at(currentIndex)->zValue() == zMax)
                        return true;

                    TupSvgItem *object = k->svg.at(currentIndex);
                    int zLimit = object->zValue();

                    if (currentIndex < (k->svg.size() - 1)) {
                        object = k->svg.at(currentIndex + 1);
                        int upZValue = object->zValue();
                        if (upZValue == (zLimit + 1)) {
                            k->svg.at(currentIndex)->setZValue(upZValue);
                            k->svg.at(currentIndex + 1)->setZValue(zLimit);
                            k->svg.swap(currentIndex, currentIndex + 1);
                            k->svgIndexes.swap(currentIndex, currentIndex + 1);
                            return true;
                        } else {
                            for (int i=0; i < k->graphics.size(); ++i) {
                                 int zLevel = k->graphics.at(i)->itemZValue();
                                 if (zLevel == (zLimit + 1)) {
                                     k->svg.at(currentIndex)->setZValue(zLevel);
                                     k->graphics.at(i)->setItemZValue(zLimit);
                                     return true;
                                 }
                            }
                        }
                    } else {
                        if (!k->graphics.isEmpty()) {
                            for (int i=0; i < k->graphics.size(); ++i) {
                                 int zLevel = k->graphics.at(i)->itemZValue();
                                 if (zLevel == (zLimit + 1)) {
                                     k->svg.at(currentIndex)->setZValue(zLevel);
                                     k->graphics.at(i)->setItemZValue(zLimit);
                                     return true;
                                 }
                            }
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelToFront/Svg ]";
                            #endif
                            return false;
                        }
                    }
                } else {
                    if (k->graphics.at(currentIndex)->itemZValue() == zMax)
                        return true;

                    TupGraphicObject *object = k->graphics.at(currentIndex);
                    int zLimit = object->itemZValue();

                    if (currentIndex < (k->graphics.size() - 1)) {
                        object = k->graphics.at(currentIndex + 1);
                        int upZValue = object->itemZValue();
                        if (upZValue == (zLimit + 1)) {
                            k->graphics.at(currentIndex)->setItemZValue(upZValue);
                            k->graphics.at(currentIndex + 1)->setItemZValue(zLimit);
                            k->graphics.swap(currentIndex, currentIndex + 1);
                            k->objectIndexes.swap(currentIndex, currentIndex + 1);
                            return true;
                        } else {
                            for (int i=0; i < k->svg.size(); ++i) {
                                 int zLevel = k->svg.at(i)->zValue();
                                 if (zLevel == (zLimit + 1)) {
                                     k->graphics.at(currentIndex)->setItemZValue(zLevel);
                                     k->svg.at(i)->setZValue(zLimit);
                                     return true;
                                 }
                            }
                        }
                    } else {
                        if (!k->svg.isEmpty()) {
                            for (int i=0; i < k->svg.size(); ++i) {
                                 int zLevel = k->svg.at(i)->zValue();
                                 if (zLevel == (zLimit + 1)) {
                                     k->graphics.at(currentIndex)->setItemZValue(zLevel);
                                     k->svg.at(i)->setZValue(zLimit);
                                     return true;
                                 }
                            }
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelToFront/Items ]";
                            #endif
                            return false;
                        }
                    }
                }
             }
           break;
    }

    #ifdef K_DEBUG
           tError() << "TupFrame::moveItem() - Fatal Error: Something went wrong!";
    #endif

    return false;
}

bool TupFrame::removeGraphicAt(int position)
{
    if ((position < 0) || (position >= k->graphics.size())) {
        #ifdef K_DEBUG
               tError() << "TupFrame::removeGraphicAt() - Fatal Error: invalid object index! [ " << position << " ]";
        #endif
        return false;
    }

    TupGraphicObject *object = k->graphics.at(position);
    if (object) {
        if (object->hasTween())
            this->scene()->removeTweenObject(object);

        int zLimit = k->graphics.at(position)->itemZValue();
        k->objectIndexes.removeAt(position);
        k->graphics.removeAt(position);

        for (int i=position; i < k->graphics.size(); ++i) {
             int zLevel = k->graphics.at(i)->itemZValue();
             k->graphics.at(i)->setItemZValue(zLevel - 1);
        }
        for (int i=0; i < k->svg.size(); ++i) {
             int zLevel = k->svg.at(i)->zValue();
             if (zLevel > zLimit)
                 k->svg.at(i)->setZValue(zLevel-1);
        }
        k->zLevelIndex--;

        return true;
    } 

    #ifdef K_DEBUG
           tError() << "TupFrame::removeGraphicAt() - Error: Object at position " << position << " is NULL!";
    #endif

    return false;
}

bool TupFrame::removeSvgAt(int position)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if ((position < 0) || (position >= k->svg.size())) {
        #ifdef K_DEBUG
               tError() << "TupFrame::removeSvgAt() - Fatal Error: invalid object index! [ " << position << " ]";
        #endif
        return false;
    }

    TupSvgItem *item = k->svg.at(position);

    if (item) {
        QGraphicsScene *scene = item->scene();
        if (scene)
            scene->removeItem(item);

        int zLimit = k->svg.at(position)->zValue();
        k->svgIndexes.removeAt(position);
        k->svg.removeAt(position);

        for (int i=position; i < k->svg.size(); ++i) {
             int zLevel = k->svg.at(i)->zValue();
             k->svg.at(i)->setZValue(zLevel-1);
        }
        for (int i=0; i < k->graphics.size(); ++i) {
             int zLevel = k->graphics.at(i)->itemZValue();
             if (zLevel > zLimit)
                 k->graphics.at(i)->setItemZValue(zLevel - 1);
        }
        k->zLevelIndex--;

        #ifdef K_DEBUG
               tFatal() << "TupFrame::removeSvgAt() - SVG object has been removed (" << position << ")";
        #endif

        return true;
    }

    #ifdef K_DEBUG
           tError() << "TupFrame::removeSvgAt() - Error: Couldn't find SVG object (" << position << ")";
    #endif

    return false;
}

QGraphicsItem *TupFrame::createItem(QPointF coords, const QString &xml, bool loaded)
{
    TupItemFactory itemFactory;
    itemFactory.setLibrary(project()->library());
    QGraphicsItem *graphicItem = itemFactory.create(xml);
    graphicItem->setPos(coords);

    if (graphicItem) {
        QString id = "path";
        if (itemFactory.type() == TupItemFactory::Library)
            id = itemFactory.itemID(xml);

        addItem(id, graphicItem);
        k->objectIndexes.append(id);

        if (loaded)
            TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), k->graphics.size() - 1, 
                                         coords, TupLibraryObject::Item, xml, project());
        return graphicItem;
    }

    #ifdef K_DEBUG
           tError() << "TupFrame::createItem() - Error: Couldn't create QGraphicsItem object";
           tError() << "TupFrame::createItem() - xml: ";
           tError() << xml;
    #endif

    return 0;
}

// SQA: Check if this method really is being used
TupSvgItem *TupFrame::createSvgItem(QPointF coords, const QString &xml, bool loaded)
{
    QDomDocument document;
    if (!document.setContent(xml))
        return 0;

    QDomElement root = document.documentElement();
    QString id = root.attribute("id");
    QString path = root.attribute("itemPath");

    TupSvgItem *item = new TupSvgItem(path, this);
    item->moveBy(coords.x(), coords.y()); 

    addSvgItem(id, item);

    if (loaded)
        TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), 
                                     k->svg.size() - 1, coords, TupLibraryObject::Svg, xml, project());

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
    if ((position < 0) || (position >= k->graphics.count())) {
        #ifdef K_DEBUG
               tError() << "TupFrame::graphic() - Fatal Error: index out of bound [ " << position << " ] /  Total items: " << k->graphics.count();
        #endif

        return 0;
    } 

    return k->graphics.at(position);
}

TupSvgItem *TupFrame::svg(int position) const
{
    if ((position < 0) || (position >= k->svg.count())) {
        #ifdef K_DEBUG
               tError() << "TupFrame::svg() -  Fatal Error: index out of bound [ " << position << " ] / Total items: " << k->svg.count();
        #endif

        return 0;
    }

    return k->svg.at(position);
}

QGraphicsItem *TupFrame::item(int position) const
{
    if ((position < 0) || (position >= k->graphics.count())) {
        #ifdef K_DEBUG
               tError() << "TupFrame::item() -  Fatal Error: index out of bound [ " << position << " ] / Total items: " << k->graphics.count();
        #endif

        return 0;
    }

    TupGraphicObject *object = k->graphics.at(position);
    if (object) {
        QGraphicsItem *item = object->item();
        if (item) {
            return item;
        } else {
            #ifdef K_DEBUG
                   tError() << "TupFrame::item() -  Fatal Error: QGraphicsItem object is NULL!";
            #endif
            return 0;
        }
    }

    #ifdef K_DEBUG
           tError() << "TupFrame::item() -  Fatal Error: TupGraphicObject is NULL!";
    #endif
    return 0;
}

int TupFrame::indexOf(TupGraphicObject *object) const
{
    return k->graphics.indexOf(object);
}

int TupFrame::indexOf(TupSvgItem *object) const
{
    return k->svg.indexOf(object); 
}

int TupFrame::indexOf(QGraphicsItem *item) const
{
    if (item) {
        for (int i = 0; i < k->graphics.size(); i++) {
             TupGraphicObject *object = k->graphics.at(i);
             if (object->item()->zValue() == item->zValue())
                 return k->graphics.indexOf(object);
        }
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
    return k->zLevelIndex;
}

bool TupFrame::isEmpty()
{
    if (graphicItemsCount() + svgItemsCount() > 0)
        return false;

    return true;
}

void TupFrame::reloadGraphicItem(const QString &id, const QString &path)
{
    for (int i = 0; i < k->objectIndexes.size(); ++i) {
         if (k->objectIndexes.at(i).compare(id) == 0) {
             TupGraphicObject *old = k->graphics.at(i);
             QGraphicsItem *oldItem = old->item();

             QPixmap pixmap(path);
             TupPixmapItem *image = new TupPixmapItem;
             image->setPixmap(pixmap);

             TupGraphicLibraryItem *item = new TupGraphicLibraryItem;
             item->setSymbolName(id);
             item->setItem(image);
             item->setTransform(oldItem->transform());
             item->setPos(oldItem->pos());
             item->setEnabled(true);
             item->setFlags(oldItem->flags());
             item->setZValue(oldItem->zValue());

             TupGraphicObject *object = new TupGraphicObject(item, this);
             k->graphics[i] = object;
         }
    }
}

void TupFrame::reloadSVGItem(const QString &id, TupLibraryObject *object)
{
    for (int i = 0; i < k->svgIndexes.size(); ++i) {
         if (k->svgIndexes.at(i).compare(id) == 0) {
                 TupSvgItem *oldItem = k->svg.value(i);

                 QString path = object->dataPath();
                 TupSvgItem *item = new TupSvgItem(path, this);
                 item->setSymbolName(object->symbolName());
                 item->setTransform(oldItem->transform());
                 item->setPos(oldItem->pos());
                 item->setEnabled(true);
                 item->setFlags(oldItem->flags());
                 item->setZValue(oldItem->zValue());

                 k->svg.insert(i, item);


         }
    }
}
