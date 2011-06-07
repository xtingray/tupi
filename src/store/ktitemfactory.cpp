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

#include "ktitemfactory.h"
#include <QGraphicsItem>

#include "tdebug.h"
#include <ktsvg2qt.h>

#include "ktpathitem.h"
#include "ktpixmapitem.h"
#include "kttextitem.h"
#include "ktbuttonitem.h"
#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"
#include "ktitemgroup.h"
#include "ktgraphiclibraryitem.h"
#include "ktlibrary.h"

#include "ktgraphicalgorithm.h"
#include "ktserializer.h"

struct KTItemFactory::Private
{
    QGraphicsItem *item;
    QGradient *gradient;
    QString loading; //brush or pen

    QStack<KTItemGroup *> groups;
    QStack<QGraphicsItem *> objects;

    bool addToGroup, isLoading;
    QString textReaded;

    const KTLibrary *library;

    KTItemFactory::Type type;
};

KTItemFactory::KTItemFactory() : KXmlParserBase(), k(new Private)
{
    k->item = 0;
    k->addToGroup = false;
    k->isLoading = false;
    k->library = 0;
}

KTItemFactory::~KTItemFactory()
{
    delete k;
}

void KTItemFactory::setLibrary(const KTLibrary *library)
{
    k->library = library;
}

QGraphicsItem* KTItemFactory::createItem(const QString &root)
{
    QGraphicsItem* item = 0;
    k->type = KTItemFactory::Vectorial;

    if (root == "path") {
        item = new KTPathItem;
    } else if (root == "rect") {
               item = new KTRectItem;
    } else if (root == "ellipse") {
               item = new KTEllipseItem;
    } else if (root == "button") {
               item = new KTButtonItem;
    } else if (root == "text") {
               item = new KTTextItem;
    } else if (root == "line") {
               item = new KTLineItem;
    } else if (root == "group") {
               item = new KTItemGroup;
    } else if(root == "symbol") {
               item = new KTGraphicLibraryItem;
               k->type = KTItemFactory::Library;
    }

    return item;
}

bool KTItemFactory::startTag(const QString& qname, const QXmlAttributes& atts)
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFOX("items") << qname;
    #endif
    */

    if (qname == "path") {

        QPainterPath path;
        KTSvg2Qt::svgpath2qtpath(atts.value("coords"), path);

        if (k->addToGroup) {
            QGraphicsItem *item = createItem(qname);
            qgraphicsitem_cast<KTPathItem *>(item)->setPath(path);
            k->objects.push(item);
        } else {
            if (!k->item)
                k->item = createItem(qname);

            qgraphicsitem_cast<KTPathItem *>(k->item)->setPath(path);

            k->objects.push(k->item);
        }
    } else if (qname == "rect") {
               QRectF rect(atts.value("x").toDouble(), atts.value("y").toDouble(), atts.value("width").toDouble(), atts.value("height").toDouble());

               if (k->addToGroup) {
                   KTRectItem *item = qgraphicsitem_cast<KTRectItem *>(createItem(qname));
                   item->setRect(rect);
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem( qname );
                   qgraphicsitem_cast<KTRectItem *>(k->item)->setRect(rect);
                   k->objects.push(k->item);
               }
    } else if (qname == "ellipse") {
               QRectF rect(QPointF(0, 0), QSizeF(2 * atts.value("rx").toDouble(), 2 * atts.value("ry").toDouble() ));

               if (k->addToGroup) {
                   KTEllipseItem *item = qgraphicsitem_cast<KTEllipseItem *>(createItem(qname));
                   item->setRect(rect);
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);

                   qgraphicsitem_cast<KTEllipseItem *>(k->item)->setRect(rect);
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
                   KTTextItem *item = qgraphicsitem_cast<KTTextItem *>(createItem( qname ));
                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem( qname );

                   k->objects.push(k->item);
               }
               setReadText(true);
               k->textReaded = "";
    } else if (qname == "line") {
        QLineF line(atts.value("x1").toDouble(), atts.value("y1").toDouble(), atts.value("x2").toDouble(), atts.value("y2").toDouble());
        
        if (k->addToGroup) {
            KTLineItem *item = qgraphicsitem_cast<KTLineItem *>(createItem(qname));
            item->setLine(line);
            
            k->objects.push(item);
        } else {
            if (!k->item)
                k->item = createItem(qname);
            
            qgraphicsitem_cast<KTLineItem *>(k->item)->setLine(line);
            k->objects.push(k->item);
        }
    } else if (qname == "group") {
        if (k->addToGroup) {
            KTItemGroup *group = qgraphicsitem_cast<KTItemGroup *>(createItem(qname));
            
            k->groups.push(group);
            k->objects.push(group);
        } else {
            if (!k->item)
                k->item = createItem(qname);
            k->groups.push(qgraphicsitem_cast<KTItemGroup *>(k->item));
            k->objects.push(k->item);
        }
        
        k->addToGroup = true;
    } else if (qname == "symbol") {
               if (k->addToGroup) {

                   KTGraphicLibraryItem *item = qgraphicsitem_cast<KTGraphicLibraryItem *>(createItem(qname));

                   QString id = atts.value("id");

                   item->setSymbolName(id);

                   if (k->library)
                       item->setObject(k->library->findObject(id));

                   k->objects.push(item);
               } else {
                   if (!k->item)
                       k->item = createItem(qname);

                   QString id = atts.value("id");

                   qgraphicsitem_cast<KTGraphicLibraryItem *>(k->item)->setSymbolName(id);

                   if (k->library)
                       qgraphicsitem_cast<KTGraphicLibraryItem *>(k->item)->setObject(k->library->findObject(id));

                   k->objects.push(k->item);
               }
    }

    //////////

    if (qname == "properties" && !k->objects.isEmpty()) {

        KTSerializer::loadProperties(k->objects.last(), atts);

    } else if (qname == "brush") {

               QBrush brush;
               KTSerializer::loadBrush(brush, atts);

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
               KTSerializer::loadPen(pen, atts);
               setItemPen(pen);

    } else if (qname == "font") {

               QFont font;

               KTSerializer::loadFont(font, atts);

               if (KTTextItem *text = qgraphicsitem_cast<KTTextItem *>(k->objects.last()))
                   text->setFont(font);

    } else if (qname == "stop") {

               if (k->gradient) {
                   QColor c(atts.value("colorName"));
                   c.setAlpha(atts.value("alpha").toInt());
                   k->gradient->setColorAt(atts.value("value").toDouble(), c);
               }

    } else if (qname == "gradient") {

               k->gradient = KTSerializer::createGradient( atts);

    }

    return true;
}

void KTItemFactory::text(const QString & ch)
{
    k->textReaded += ch;
}

bool KTItemFactory::endTag(const QString& qname)
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFOX("items") << qname;
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

               if (KTTextItem *text = qgraphicsitem_cast<KTTextItem *>(k->objects.last()))
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
                      tWarning("items") << "Unhandled: " << qname;
               #endif
    }

    return true;
}

void KTItemFactory::setItemPen(const QPen &pen)
{
    if (k->objects.isEmpty()) 
        return;

    if (QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem *>(k->objects.last())) {
        line->setPen(pen);
    } else if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last())) {
               shape->setPen(pen);
    }
}

void KTItemFactory::setItemBrush(const QBrush &brush)
{
    if (k->objects.isEmpty()) 
        return;

    if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last()))
        shape->setBrush(brush);
}

void  KTItemFactory::setItemGradient(const QGradient& gradient, bool brush)
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

QPen KTItemFactory::itemPen() const
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

QBrush KTItemFactory::itemBrush() const
{
    if (! k->objects.isEmpty()) {
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(k->objects.last()))
            return shape->brush();
    }

    return Qt::transparent;
}

bool KTItemFactory::loadItem(QGraphicsItem *item, const QString &xml)
{
    k->item = item;
    k->isLoading = true;
    bool ok = parse(xml);
    k->isLoading = false;

    return ok;
}

QGraphicsItem *KTItemFactory::create(const QString &xml)
{
    if (loadItem(0, xml))
        return k->item;

    return 0;
}

QString KTItemFactory::itemID(const QString &xml)
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

KTItemFactory::Type KTItemFactory::type() {
    return k->type;
}

