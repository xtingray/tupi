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
#include "tuppixmapitem.h"
#include "tuplibrary.h"
#include "tupitemgroup.h"
#include "tupitemtweener.h"
#include "tupprojectloader.h"
#include "tupscene.h"
#include "tuplayer.h"

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QCursor>
#include <QSvgRenderer>

struct TupFrame::Private
{
    TupLayer *layer;
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
    // int repeat;
    int zLevelIndex;
    double opacity;
};

TupFrame::TupFrame() : k(new Private)
{
    k->type = Regular;
    k->isLocked = false;
    k->isVisible = true;
}

TupFrame::TupFrame(TupLayer *parent) : QObject(parent), k(new Private)
{
    k->layer = parent;
    k->name = "Frame";
    k->type = Regular;

    k->isLocked = false;
    k->isVisible = true;

    k->direction = "-1";
    k->shift = "0";

    k->zLevelIndex = (k->layer->layerIndex() + 2)*ZLAYER_LIMIT; // Layers levels starts from 2
    // k->zLevelIndex = 0;
}

TupFrame::TupFrame(TupBackground *bg, const QString &label) : QObject(bg), k(new Private)
{
    k->name = label;
    k->isLocked = false;
    k->isVisible = true;
    // k->repeat = 1;
    k->opacity = 1.0;

    k->direction = "-1";
    k->shift = "0";

    if (k->name.compare("landscape_dynamic") == 0) {
        k->zLevelIndex = 0;
        k->type = DynamicBg;
    } else {
        k->zLevelIndex = ZLAYER_LIMIT;
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

QString TupFrame::frameName() const
{
    return k->name;
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

bool TupFrame::isLocked() const
{
    return k->isLocked;
}

TupFrame::FrameType TupFrame::type()
{
   return k->type;
}

void TupFrame::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool TupFrame::isVisible() const
{
    return k->isVisible;
}

void TupFrame::setOpacity(double opacity)
{
    k->opacity = opacity;
}

double TupFrame::opacity() 
{
    return k->opacity;
}

void TupFrame::fromXml(const QString &xml)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupFrame::fromXml()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QDomDocument document;
    if (!document.setContent(xml)) {    
        #ifdef K_DEBUG
            QString msg = "TupFrame::fromXml() - File corrupted!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
                qWarning() << "Content:";
                qWarning() << xml;
            #else
                tError() << msg;
                tWarning() << "Content:";
                tWarning() << xml;            
            #endif
        #endif
        
        return;
    }

    QDomElement root = document.documentElement();
    setFrameName(root.attribute("name", tr("Frame")));

    if (k->type == DynamicBg) {
        setDynamicDirection(root.attribute("direction", "0"));
        setDynamicShift(root.attribute("shift", "0"));
        setOpacity(root.attribute("opacity", "1.0").toDouble());
    }

    if (k->type == StaticBg)
        setOpacity(root.attribute("opacity", "1.0").toDouble());

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
                              scene()->addTweenObject(k->layer->layerIndex(), last);
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
                              TupLibraryObject *object = project()->library()->getObject(symbol);

                              QString path(object->dataPath());
                              QDomNode n2 = e.firstChild();
                              TupSvgItem *svg = new TupSvgItem();

                              while (!n2.isNull()) {
                                     QDomElement e2 = n2.toElement();

                                     if (e2.tagName() == "properties") {
                                         svg = new TupSvgItem(path, this);
                                         svg->setSymbolName(symbol);
                                         TupSerializer::loadProperties(svg, e2);

                                         addSvgItem(symbol, svg);
                                     } else if (e2.tagName() == "tweening") {
                                         TupItemTweener *tweener = new TupItemTweener();
                                         QString newDoc;
                                         {
                                             QTextStream ts(&newDoc);
                                             ts << n2;
                                         }
                                         tweener->fromXml(newDoc);
                                         svg->setTween(tweener);
                                         scene()->addTweenObject(k->layer->layerIndex(), svg);
                                     }
                                     n2 = n2.nextSibling(); 
                              }
                          } else {                              
                              #ifdef K_DEBUG
                                  QString msg = "TupFrame::fromXml() - Fatal Error: Object id is NULL!";
                                  #ifdef Q_OS_WIN
                                      qDebug() << msg;
                                  #else
                                      tError() << msg;
                                  #endif
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
        root.setAttribute("opacity", QString::number(k->opacity));
    }

    if (k->type == StaticBg)
        root.setAttribute("opacity", QString::number(k->opacity));

    doc.appendChild(root);

    int objectsCount = k->graphics.count();
    int svgCount = k->svg.count();

    if (objectsCount == 0 && svgCount == 0)
        return root;

    if (objectsCount > 0 && svgCount == 0) {
        foreach (TupGraphicObject *object, k->graphics)
                 root.appendChild(object->toXml(doc));
        return root;
    }

    if (svgCount > 0 && objectsCount == 0) {
        foreach (TupSvgItem *svg, k->svg) 
                 root.appendChild(svg->toXml(doc));
        return root;
    }

    do {
           int objectZValue = k->graphics.at(0)->itemZValue();
           int svgZValue = k->svg.at(0)->zValue();

           if (objectZValue < svgZValue) {
               TupGraphicObject *object = k->graphics.takeFirst();
               root.appendChild(object->toXml(doc));
           } else { 
               TupSvgItem *svg = k->svg.takeFirst();
               root.appendChild(svg->toXml(doc));
           }

           if (k->graphics.isEmpty()) {
               foreach (TupSvgItem *svg, k->svg) 
                        root.appendChild(svg->toXml(doc));
               break;
           } else {
               if (k->svg.isEmpty()) {
                   foreach (TupGraphicObject *object, k->graphics)
                            root.appendChild(object->toXml(doc));
                   break;
               }
           }
    } while (true);

    return root;
}

void TupFrame::addLibraryItem(const QString &id, TupGraphicLibraryItem *libraryItem)
{
    QGraphicsItem *item = libraryItem->item();
    QDomDocument dom;
    TupItemFactory itemFactory;

    if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item)) {
        dom.appendChild(dynamic_cast<TupAbstractSerializable *>(group)->toXml(dom));
        item = itemFactory.create(dom.toString());
    } else {
        if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(item)) {
            dom.appendChild(dynamic_cast<TupAbstractSerializable *>(path)->toXml(dom));
            item = itemFactory.create(dom.toString());
        }
    } 

    addItem(id, item);
}

void TupFrame::addItem(const QString &id, QGraphicsItem *item)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupFrame::addItem()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

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
        #ifdef Q_OS_WIN
            qDebug() << "[TupFrame::addSvgItem()] - id: " << id;
        #else
            T_FUNCINFO << id;
        #endif
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

void TupFrame::insertItem(int position, QGraphicsItem *item)
{
    TupGraphicObject *object = new TupGraphicObject(item, this);

    k->graphics.insert(position, object);
    k->objectIndexes.insert(position, "path");

    for (int i=position+1; i < k->graphics.size(); ++i) {
         int zLevel = k->graphics.at(i)->itemZValue();
         k->graphics.at(i)->setItemZValue(zLevel + 1);
    }
    for (int i=0; i < k->svg.size(); ++i) {
         int zLevel = k->svg.at(i)->zValue();
         if (zLevel < item->zValue())
             k->svg.at(i)->setZValue(zLevel + 1);
    }

    k->zLevelIndex++;
}

int TupFrame::createItemGroup(int position, QList<int> group)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupFrame::createItemGroup()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int zBase = this->item(position)->zValue();
    TupItemGroup *itemGroup = new TupItemGroup;

    foreach (int index, group) {
             QGraphicsItem *item = this->item(index);
             item->setOpacity(1.0);
             itemGroup->addToGroup(item);
    }

    int size = group.size()-1;
    for (int i=size;i>=0;i--)
         removeGraphicAt(group.at(i));

    QGraphicsItem *block = qgraphicsitem_cast<QGraphicsItem *>(itemGroup);
    block->setZValue(zBase);
    insertItem(position, block);

    return position;
}

QList<QGraphicsItem *> TupFrame::splitGroup(int position)
{
    QList<QGraphicsItem *> items;
    QGraphicsItem *object = qgraphicsitem_cast<TupItemGroup *>(item(position));

    if (object) {
        if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item(position))) {
            removeGraphicAt(position);
            items = group->childItems();
            foreach (QGraphicsItem *child, group->childItems()) {
                     group->removeFromGroup(child);
                     addItem("path", child);
            }
        }
    }

    return items;
}

void TupFrame::replaceItem(int position, QGraphicsItem *item)
{
    TupGraphicObject *toReplace = this->graphicAt(position);

    if (toReplace)
        toReplace->setItem(item);
}

bool TupFrame::moveItem(TupLibraryObject::Type type, int currentIndex, int action)
{
    if ((k->svg.size() + k->graphics.size()) == 1)  
        return true;

    int layerIndex = 0;
    if (k->type == Regular)
        layerIndex = k->layer->layerIndex();

    MoveItemType move = MoveItemType(action); 
    switch(move) {
           case MoveBack :
             {
                int zMin = (layerIndex + 1)*ZLAYER_LIMIT;

                if (type == TupLibraryObject::Svg) {
                    int zLimit = k->svg.at(currentIndex)->zValue();
                    if (zLimit == zMin) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveBack: Minimum level has been reached! (SVG)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                    if (zLimit == zMin) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveBack: Minimum level has been reached! (VECTOR/RASTER)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg; 
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                    if (zLimit == zMax) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveToFront: Maximum level has been reached! (SVG)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                    if (zLimit == zMax) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveToFront: Maximum level has been reached! (VECTOR/RASTER)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                // int zMin = (k->layerIndex + 1)*ZLAYER_LIMIT;
                int zMin = (layerIndex + 1)*ZLAYER_LIMIT;

                if (type == TupLibraryObject::Svg) {
                    if (k->svg.at(currentIndex)->zValue() == zMin) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveOneLevelBack: Minimum level has been reached! (SVG)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                                QString msg = "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelBack/Svg ]";
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif

                            return false;
                        }
                    }
                } else {
                    if (k->graphics.at(currentIndex)->itemZValue() == zMin) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveOneLevelBack: Minimum level has been reached! (VECTOR/RASTER)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                                QString msg = "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelBack/Items ]";
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
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
                    if (k->svg.at(currentIndex)->zValue() == zMax) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveOneLevelToFront: Maximum level has been reached! (SVG)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                                QString msg = "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelToFront/Svg ]";
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif
                            
                            return false;
                        }
                    }
                } else {
                    if (k->graphics.at(currentIndex)->itemZValue() == zMax) {
                        #ifdef K_DEBUG
                            QString msg = "TupFrame::moveItem() - MoveOneLevelToFront: Maximum level has been reached! (VECTOR/RASTER)";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return true;
                    }

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
                                QString msg = "TupFrame::moveItem() - Fatal Error: Something went wrong [ case MoveOneLevelToFront/Items ]";
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif
                            return false;
                        }
                    }
                }
             }
           break;
    }

    #ifdef K_DEBUG
        QString msg = "TupFrame::moveItem() - Fatal Error: Something went wrong!";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif
    
    return false;
}

bool TupFrame::removeGraphicAt(int position)
{
    if ((position < 0) || (position >= k->graphics.size())) {
        #ifdef K_DEBUG
            QString msg = "TupFrame::removeGraphicAt() - Fatal Error: invalid object index! [ " + QString::number(position) + " ]";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }

    TupGraphicObject *object = k->graphics.at(position);
    if (object) {
        if (object->hasTween())
            this->scene()->removeTweenObject(k->layer->layerIndex(), object);

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
        QString msg = "TupFrame::removeGraphicAt() - Error: Object at position " + QString::number(position) + " is NULL!";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

bool TupFrame::removeSvgAt(int position)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupFrame::removeSvgAt()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    
    if ((position < 0) || (position >= k->svg.size())) {    
        #ifdef K_DEBUG
            QString msg = "TupFrame::removeSvgAt() - Fatal Error: invalid object index! [ " + QString::number(position) + " ]";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
            QString msg = "TupFrame::removeSvgAt() - SVG object has been removed (" + QString::number(position) + ")";
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

        return true;
    }

    #ifdef K_DEBUG
        QString msg = "TupFrame::removeSvgAt() - Error: Couldn't find SVG object (" + QString::number(position) + ")";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return false;
}

QGraphicsItem *TupFrame::createItem(QPointF coords, const QString &xml, bool loaded)
{
    TupItemFactory itemFactory;
    // SQA: Refactor the code related to the library variable within this class

    TupLibrary *library = project()->library();
    if (library)
        itemFactory.setLibrary(library);

    QGraphicsItem *graphicItem = itemFactory.create(xml);
    if (graphicItem) {
        graphicItem->setPos(coords);
        QString id = "path";
        if (library) {
            if (itemFactory.type() == TupItemFactory::Library)
                id = itemFactory.itemID(xml);
        }

        addItem(id, graphicItem);

        if (k->type == Regular) {
            if (loaded)
                TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), k->graphics.size() - 1, 
                                             coords, TupLibraryObject::Item, xml, project());
        }

        return graphicItem;
    }

    #ifdef K_DEBUG
        QString msg1 = "TupFrame::createItem() - Fatal Error: Couldn't create QGraphicsItem object";
        QString msg2 = "TupFrame::createItem() - xml: ";
        #ifdef Q_OS_WIN
            qDebug() << msg1;
            qDebug() << msg2;
            qDebug() << xml;
        #else
            tError() << msg1;
            tError() << msg2;    
            tError() << xml;
        #endif
    #endif

    return 0;
}

TupSvgItem *TupFrame::createSvgItem(QPointF coords, const QString &xml, bool loaded)
{
    QDomDocument document;
    if (!document.setContent(xml)) {
        #ifdef K_DEBUG
            QString msg1 = "TupFrame::createSvgItem() - Fatal Error: Svg xml content is invalid!";
            QString msg2 = "TupFrame::createSvgItem() - xml: ";
            #ifdef Q_OS_WIN
                qDebug() << msg1;
                qDebug() << msg2;
                qDebug() << xml;
            #else
                tError() << msg1;
                tError() << msg2;    
                tError() << xml;
            #endif
        #endif
        return 0;
    }

    QDomElement root = document.documentElement();
    QString id = root.attribute("id");
    TupLibraryObject *object = project()->library()->getObject(id);
    if (object) {
        QString path = object->dataPath();
        TupSvgItem *item = new TupSvgItem(path, this);
        if (item) {
            item->setSymbolName(id);
            item->moveBy(coords.x(), coords.y()); 
            addSvgItem(id, item);
            if (loaded)
                TupProjectLoader::createItem(scene()->objectIndex(), layer()->objectIndex(), index(), 
                                             k->svg.size() - 1, coords, TupLibraryObject::Svg, xml, project());
            return item;
        } else {        
            #ifdef K_DEBUG
                QString msg = "TupFrame::createSvgItem() - Fatal Error: Svg object is invalid!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return 0;
        }
    }

    #ifdef K_DEBUG
        QString msg = "TupFrame::createSvgItem() - Fatal Error: TupLibraryObject variable is NULL!";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif
    
    return 0;
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

TupGraphicObject *TupFrame::graphicAt(int position) const
{
    if ((position < 0) || (position >= k->graphics.count())) {
        #ifdef K_DEBUG
            QString msg = "TupFrame::graphicAt() - Fatal Error: index out of bound [ " + QString::number(position) + " ] /  Total items: " + QString::number(k->graphics.count());
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        
        return 0;
    } 

    return k->graphics.at(position);
}

TupSvgItem *TupFrame::svgAt(int position) const
{
    if ((position < 0) || (position >= k->svg.count())) {
        #ifdef K_DEBUG
            QString msg = "TupFrame::svgAt() -  Fatal Error: index out of bound [ " + QString::number(position) + " ] / Total items: " + QString::number(k->svg.count());
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        
        return 0;
    }

    return k->svg.at(position);
}

QGraphicsItem *TupFrame::item(int position) const
{
    if ((position < 0) || (position >= k->graphics.count())) {
        #ifdef K_DEBUG
            QString msg = "TupFrame::item() -  Fatal Error: index out of bound [ " + QString::number(position) + " ] / Total items: " + QString::number(k->graphics.count());
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
                QString msg = "TupFrame::item() -  Fatal Error: QGraphicsItem object is NULL!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return 0;        
        }
    }

    #ifdef K_DEBUG
        QString msg = "TupFrame::item() -  Fatal Error: TupGraphicObject is NULL!";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return 0;
}

// SQA: Verify if this method is used by something... anything! 

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

/*
void TupFrame::setRepeat(int repeat)
{
    k->repeat = repeat;
}

int TupFrame::repeat() const
{
    return k->repeat;
}
*/

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
    if (k->type == Regular)
        return layer()->project();

    TupBackground *bg = static_cast<TupBackground *>(parent());
    return bg->project();
}

int TupFrame::graphicItemsCount()
{
    return k->graphics.count();
}

int TupFrame::svgItemsCount()
{
    return k->svg.count();
}

int TupFrame::itemsTotalCount()
{
    return k->graphics.count() + k->svg.count();
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

             k->svg[i] = item;
         }
    }
}

void TupFrame::updateZLevel(int zLevelIndex)
{
    int max = 0;
    int graphicsSize = k->graphics.size();
    for (int i = 0; i < graphicsSize; ++i) {
         TupGraphicObject *object = k->graphics.at(i); 
         int currentZValue = object->itemZValue();
         int zLevel = zLevelIndex + (currentZValue % ZLAYER_LIMIT);
         object->setItemZValue(zLevel);
         if (i == (graphicsSize-1)) {
             if (zLevel > max)
                 max = zLevel;
         }
    }

    graphicsSize = k->svgIndexes.size(); 
    for (int i = 0; i < graphicsSize; ++i) {
         TupSvgItem *item = k->svg.value(i);
         int currentZValue = item->zValue();
         int zLevel = zLevelIndex + (currentZValue % ZLAYER_LIMIT);
         item->setZValue(zLevel);
         if (i == (graphicsSize-1)) {
             if (zLevel > max)
                 max = zLevel;
         }
    }

    if (max > 0) {
        k->zLevelIndex = max;
        k->zLevelIndex++;
    } else {
        k->zLevelIndex = (k->layer->layerIndex() + 1)*ZLAYER_LIMIT;
    }
}
