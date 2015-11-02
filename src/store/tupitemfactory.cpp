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

#include "tupitemfactory.h"
#include "tupsvg2qt.h"
#include "tuppathitem.h"
#include "tuppixmapitem.h"
#include "tuptextitem.h"
#include "tupbuttonitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tupitemgroup.h"
#include "tupgraphiclibraryitem.h"
#include "tuplibrary.h"
#include "tupgraphicalgorithm.h"
#include "tupserializer.h"

struct TupItemFactory::Private
{
    QGraphicsItem *item;
    QGradient *gradient;
    QString loading; // brush or pen

    QStack<TupItemGroup *> groups;
    QStack<QGraphicsItem *> objects;
    bool addToGroup;
    bool isLoading;
    QString textReaded;
    const TupLibrary *library;
    TupItemFactory::Type type;
};

TupItemFactory::TupItemFactory() : TXmlParserBase(), k(new Private)
{
    k->item = 0;
    k->addToGroup = false;
    k->isLoading = false;
    k->library = 0;
}

TupItemFactory::~TupItemFactory()
{
    delete k;
}

void TupItemFactory::setLibrary(const TupLibrary *library)
{
    k->library = library;
}

QGraphicsItem* TupItemFactory::createItem(const QString &root)
{
    QGraphicsItem* item = 0;
    k->type = TupItemFactory::Vectorial;

    if (root == "path") {
        item = new TupPathItem;
    } else if (root == "rect") {
               item = new TupRectItem;
    } else if (root == "ellipse") {
               item = new TupEllipseItem;
    } else if (root == "button") {
               item = new TupButtonItem;
    } else if (root == "text") {
               item = new TupTextItem;
    } else if (root == "line") {
               item = new TupLineItem;
    } else if (root == "group") {
               item = new TupItemGroup;
    } else if (root == "symbol") {
               item = new TupGraphicLibraryItem;
               k->type = TupItemFactory::Library;
    }

    return item;
}

bool TupItemFactory::startTag(const QString& qname, const QXmlAttributes& atts)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupItemFactory::startTag()] - qname: " << qname;
        #else
            T_FUNCINFOX("items") << qname;
        #endif
    #endif
    */

    if (qname == "path") {
        QPainterPath path;
        TupSvg2Qt::svgpath2qtpath(atts.value("coords"), path);

        if (k->addToGroup) {
            QGraphicsItem *item = createItem(qname);
            qgraphicsitem_cast<TupPathItem *>(item)->setPath(path);
            k->objects.push(item);
        } else {
            if (!k->item)
                k->item = createItem(qname);

            qgraphicsitem_cast<TupPathItem *>(k->item)->setPath(path);

            k->objects.push(k->item);
        }
    } else if (qname == "rect") {
               QRectF rect(atts.value("x").toDouble(), atts.value("y").toDouble(), atts.value("width").toDouble(), atts.value("height").toDouble());

               if (k->addToGroup) {
                   TupRectItem *item = qgraphicsitem_cast<TupRectItem *>(createItem(qname));
                   item->setRect(rect);
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);
                   qgraphicsitem_cast<TupRectItem *>(k->item)->setRect(rect);
                   k->objects.push(k->item);
               }
    } else if (qname == "ellipse") {
               QRectF rect(QPointF(0, 0), QSizeF(2 * atts.value("rx").toDouble(), 2 * atts.value("ry").toDouble()));

               if (k->addToGroup) {
                   TupEllipseItem *item = qgraphicsitem_cast<TupEllipseItem *>(createItem(qname));
                   item->setRect(rect);
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);

                   qgraphicsitem_cast<TupEllipseItem *>(k->item)->setRect(rect);
                   k->objects.push(k->item);
               }
    } else if (qname == "button") {
               if (!k->item) {
                   k->item = createItem(qname);
                   k->objects.push(k->item);
               }

               if (k->addToGroup) {
                   // k->groups.last()->addToGroup(createItem(qname));
               }
    } else if (qname == "text") {
               if (k->addToGroup) {
                   TupTextItem *item = qgraphicsitem_cast<TupTextItem *>(createItem(qname));
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);

                   k->objects.push(k->item);
               }
               setReadText(true);
               k->textReaded = "";
    } else if (qname == "line") {
               QLineF line(atts.value("x1").toDouble(), atts.value("y1").toDouble(), atts.value("x2").toDouble(), atts.value("y2").toDouble());
        
               if (k->addToGroup) {
                   TupLineItem *item = qgraphicsitem_cast<TupLineItem *>(createItem(qname));
                   item->setLine(line);
            
                   k->objects.push(item);
               } else {
               if (!k->item)
                   k->item = createItem(qname);
            
               qgraphicsitem_cast<TupLineItem *>(k->item)->setLine(line);
               k->objects.push(k->item);
        }
    } else if (qname == "group") {
               if (k->addToGroup) {
                   TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(createItem(qname));
                   k->groups.push(group);
                   k->objects.push(group);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);
                   k->groups.push(qgraphicsitem_cast<TupItemGroup *>(k->item));
                   k->objects.push(k->item);
               }
        
               k->addToGroup = true;
    } else if (qname == "symbol") {
               if (k->addToGroup) {
                   TupGraphicLibraryItem *item = qgraphicsitem_cast<TupGraphicLibraryItem *>(createItem(qname));
                   QString id = atts.value("id");
                   item->setSymbolName(id);
                   if (k->library)
                       item->setObject(k->library->getObject(id));

                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);

                   QString id = atts.value("id");

                   qgraphicsitem_cast<TupGraphicLibraryItem *>(k->item)->setSymbolName(id);

                   if (k->library)
                       qgraphicsitem_cast<TupGraphicLibraryItem *>(k->item)->setObject(k->library->getObject(id));

                   k->objects.push(k->item);
               }
    }

    //////////

    if (qname == "properties" && !k->objects.isEmpty()) {
        TupSerializer::loadProperties(k->objects.last(), atts);
    } else if (qname == "brush") {
               QBrush brush;
               TupSerializer::loadBrush(brush, atts);

               if (currentTag() == "pen") {
                   k->loading = "pen";
                   QPen pen = itemPen();
                   pen.setBrush(brush);
                   setItemPen(pen);
               } else {
                   k->loading = qname;
                   setItemBrush(brush);
               }
    } else if (qname == "pen") {
               QPen pen;
               k->loading = qname;
               TupSerializer::loadPen(pen, atts);
               setItemPen(pen);
    } else if (qname == "font") {
               QFont font;

               TupSerializer::loadFont(font, atts);

               if (TupTextItem *text = qgraphicsitem_cast<TupTextItem *>(k->objects.last()))
                   text->setFont(font);
    } else if (qname == "stop") {
               if (k->gradient) {
                   QColor c(atts.value("colorName"));
                   c.setAlpha(atts.value("alpha").toInt());
                   k->gradient->setColorAt(atts.value("value").toDouble(), c);
               }
    } else if (qname == "gradient") {
               k->gradient = TupSerializer::createGradient( atts);
    }

    return true;
}

void TupItemFactory::text(const QString & ch)
{
    k->textReaded += ch;
}

bool TupItemFactory::endTag(const QString& qname)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupItemFactory::endTag()] - qname: " << qname;
        #else
            T_FUNCINFOX("items") << qname;
        #endif
    #endif
    */

    if (qname == "path") {
        if (k->addToGroup)
            k->groups.last()->addToGroup(k->objects.last());
        k->objects.pop();
    } else if (qname == "rect") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());
               k->objects.pop();
    } else if (qname == "ellipse") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());
               k->objects.pop();
    } else if (qname == "symbol") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());
               k->objects.pop();
    } else if (qname == "line") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());
               k->objects.pop();
    } else if (qname == "button") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());
               k->objects.pop();
    } else if (qname == "text") {
               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());

               if (TupTextItem *text = qgraphicsitem_cast<TupTextItem *>(k->objects.last()))
                   text->setHtml(k->textReaded);
               k->objects.pop();
    } else if (qname == "group") {
               k->groups.pop();
               k->addToGroup = !k->groups.isEmpty();

               if (k->addToGroup)
                   k->groups.last()->addToGroup(k->objects.last());

               k->objects.pop();
    } else if (qname == "gradient") {
               if (k->loading == "brush")
                   setItemGradient(*k->gradient, true);
               else
                   setItemGradient(*k->gradient, false);
    } else {
               #ifdef K_DEBUG
                   QString msg = "TupItemFactory::endTag() - Unknown tag: " + qname;
                   #ifdef Q_OS_WIN
                       qWarning() << msg;
                   #else
                       tWarning("items") << msg;
                   #endif
               #endif
    }

    return true;
}

void TupItemFactory::setItemPen(const QPen &pen)
{
    if (k->objects.isEmpty()) 
        return;

    if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(k->objects.last())) {
        line->setPen(pen);
    } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last())) {
               shape->setPen(pen);
    }
}

void TupItemFactory::setItemBrush(const QBrush &brush)
{
    if (k->objects.isEmpty()) 
        return;

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last()))
        shape->setBrush(brush);
}

void  TupItemFactory::setItemGradient(const QGradient& gradient, bool brush)
{
    if (k->objects.isEmpty()) 
        return;

    QBrush gBrush(gradient);

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last())) {
        if (brush) {
            gBrush.setMatrix(shape->brush().matrix());
            shape->setBrush(gBrush);
        } else {
            gBrush.setMatrix(shape->pen().brush().matrix());
            QPen pen = shape->pen();
            pen.setBrush(gBrush);
            shape->setPen(pen);
        }
    } else if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(k->objects.last())) {
               gBrush.setMatrix(line->pen().brush().matrix());
               QPen pen = line->pen();
               pen.setBrush(gBrush);
               line->setPen(pen);
    }
}

QPen TupItemFactory::itemPen() const
{
    if (! k->objects.isEmpty()) {
        if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(k->objects.last())) {
            return line->pen();
        } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last())) {
                   return shape->pen();
        }
    }

    return QPen(Qt::transparent, 1);
}

QBrush TupItemFactory::itemBrush() const
{
    if (! k->objects.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last()))
            return shape->brush();
    }

    return Qt::transparent;
}

bool TupItemFactory::loadItem(QGraphicsItem *item, const QString &xml)
{
    k->item = item;
    k->isLoading = true;
    bool ok = parse(xml);
    k->isLoading = false;

    return ok;
}

QGraphicsItem *TupItemFactory::create(const QString &xml)
{
    if (loadItem(0, xml))
        return k->item;

    return 0;
}

QString TupItemFactory::itemID(const QString &xml)
{
    QDomDocument document;

    if (!document.setContent(xml))
        return "item";

    QDomElement root = document.documentElement();
    QString id = root.attribute("id");

    if (id.length() > 0)
        return id;

    return "item";
}

TupItemFactory::Type TupItemFactory::type() {
    return k->type;
}

