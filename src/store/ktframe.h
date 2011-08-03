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

#ifndef KTFRAME_H
#define KTFRAME_H

#include "ktabstractserializable.h"
#include "ktinthash.h"
#include "ktsvgitem.h"
#include "ktbackground.h"
#include "ktglobal_store.h"

#include <QGraphicsScene>
#include <QDomDocument>
#include <QDomElement>

class KTFrame;
class KTLayer;
class KTBackground;
class KTGraphicObject;
class KTSvgItem;
class KTProject;
class KTScene;

typedef KTIntHash<KTGraphicObject *> GraphicObjects;
typedef KTIntHash<KTSvgItem *> SvgObjects;

/**
 * @brief Esta clase representa un marco o frame de la animacion
 * @author David Cuadrado
*/

class STORE_EXPORT KTFrame : public QObject, public KTAbstractSerializable
{
    public:
       /**
        * Constructor por defecto
        */
       KTFrame(KTLayer *parent);
       KTFrame(KTBackground *bg);
       
       /**
        * Destructor
        */
       ~KTFrame();
       
       /**
        * Pone el nombre del frame
        */
       void setFrameName(const QString &name);
       
       /**
        * Bloquea el frame
        */
       void setLocked(bool isLocked);
       
       /**
        * Retorna el nombre del frame
        */
       QString frameName() const;
       
       /**
        * Returna verdadero cuando el frame esta bloqueado
        */
       bool isLocked() const;
       
       void setVisible(bool isVisible);
       bool isVisible() const;
       
       void addItem(QGraphicsItem *item);
       void addItem(const QString &key, QGraphicsItem *item);
       void removeItemFromFrame(const QString &key);
       void updateIdFromFrame(const QString &oldId, const QString &newId);

       void addSvgItem(const QString &key, KTSvgItem *item);
       void removeSvgItemFromFrame(const QString &key);
       void updateSvgIdFromFrame(const QString &oldId, const QString &newId);

       void insertItem(int position, QGraphicsItem *item);
       void insertSvgItem(int position, KTSvgItem *item);
       
       void replaceItem(int position, QGraphicsItem *item);
       bool moveItem(int currentPosition, int newPosition);
       
       bool removeGraphicAt(int position);
       bool removeSvgAt(int position);

       QGraphicsItem *createItem(int position, QPointF coords, const QString &xml, bool loaded = false);
       KTSvgItem *createSvgItem(int position, QPointF coords, const QString &xml, bool loaded = false);

       void setGraphics(GraphicObjects objects);       
       void setSvgObjects(SvgObjects objects);
       GraphicObjects graphics() const;
       SvgObjects svgItems() const; 
       
       KTGraphicObject *graphic(int position) const;
       KTSvgItem *svg(int position) const; 
       QGraphicsItem *item(int position) const;
       
       QGraphicsItemGroup *createItemGroupAt(int position, QList<qreal> group);
       QList<QGraphicsItem *> destroyItemGroup(int position);
             
       KTLayer *layer() const;
       KTScene *scene() const;
       KTProject *project() const;
       
       int indexOf(KTGraphicObject *object) const;
       int indexOf(QGraphicsItem *item) const;
       int indexOf(KTSvgItem *item) const;
       
       int index() const;
       
       void setRepeat(int repeat);
       int repeat() const;
       
       void clean();
       int graphicItemsCount();
       int svgItemsCount();

       int getTopZLevel();
       QList<int> itemIndexes();
       QList<int> svgIndexes();

       bool isEmpty();
       
    public:
       virtual void fromXml(const QString &xml);
       virtual QDomElement toXml(QDomDocument &doc) const;
       
    private:
       struct Private;
       Private *const k;
};

#endif
