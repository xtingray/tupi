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

#ifndef TUPFRAME_H
#define TUPFRAME_H

#include "tglobal.h"
#include "tupabstractserializable.h"
#include "tupsvgitem.h"
#include "tupbackground.h"
#include "tupgraphiclibraryitem.h"

#include <QGraphicsScene>
#include <QDomDocument>
#include <QDomElement>
#include <QList>

class TupFrame;
class TupLayer;
class TupBackground;
class TupGraphicObject;
class TupSvgItem;
class TupProject;
class TupScene;

typedef QList<TupGraphicObject *> GraphicObjects;
typedef QList<TupSvgItem *> SvgObjects;

/**
 * @brief Esta clase representa un marco o frame de la animacion
 * @author David Cuadrado
*/

class TUPI_EXPORT TupFrame : public QObject, public TupAbstractSerializable
{
    public:
       enum FrameType { DynamicBg = 0, StaticBg, Regular };
       enum MoveItemType { MoveBack, MoveToFront, MoveOneLevelBack, MoveOneLevelToFront };

       TupFrame();
       TupFrame(TupLayer *parent);
       TupFrame(TupBackground *bg, const QString &label);
       
       ~TupFrame();
       
       void setFrameName(const QString &name);
       QString frameName() const;

       void setDynamicDirection(const QString &direction);
       void setDynamicShift(const QString &shift);
       TupBackground::Direction dynamicDirection() const;
       int dynamicShift() const;

       void setLocked(bool isLocked);
       bool isLocked() const;
       
       void setVisible(bool isVisible);
       bool isVisible() const;

       void setOpacity(double opacity);
       double opacity(); 

       TupFrame::FrameType type();
      
       void addLibraryItem(const QString &id, TupGraphicLibraryItem *libraryItem);
       void addItem(const QString &id, QGraphicsItem *item);
       void removeImageItemFromFrame(const QString &id);
       void updateIdFromFrame(const QString &oldId, const QString &newId);

       void addSvgItem(const QString &id, TupSvgItem *item);
       void removeSvgItemFromFrame(const QString &id);
       void updateSvgIdFromFrame(const QString &oldId, const QString &newId);

       void replaceItem(int position, QGraphicsItem *item);
       bool moveItem(TupLibraryObject::Type type, int currentPosition, int action);
       
       bool removeGraphicAt(int position);
       bool removeSvgAt(int position);

       QGraphicsItem *createItem(QPointF coords, const QString &xml, bool loaded = false);
       TupSvgItem *createSvgItem(QPointF coords, const QString &xml, bool loaded = false);

       void setGraphics(GraphicObjects objects);       
       void setSvgObjects(SvgObjects objects);
       GraphicObjects graphics() const;
       SvgObjects svgItems() const; 
       
       TupGraphicObject *graphic(int position) const;
       TupSvgItem *svg(int position) const; 
       QGraphicsItem *item(int position) const;
       
       int createItemGroup(int position, QList<int> group);
       QList<QGraphicsItem *> splitGroup(int position);
             
       TupLayer *layer() const;
       TupScene *scene() const;
       TupProject *project() const;
       
       int indexOf(TupGraphicObject *object) const;
       int indexOf(QGraphicsItem *item) const;
       int indexOf(TupSvgItem *item) const;
       
       int index() const;
       
       // void setRepeat(int repeat);
       // int repeat() const;
       
       void clear();
       int graphicItemsCount();
       int svgItemsCount();
       int itemsTotalCount();

       int getTopZLevel();

       bool isEmpty();

       void reloadGraphicItem(const QString &id, const QString &path);
       void reloadSVGItem(const QString &id, TupLibraryObject *object);
       void updateZLevel(int zLevelIndex);
       
    public:
       virtual void fromXml(const QString &xml);
       virtual QDomElement toXml(QDomDocument &doc) const;
       
    private:
       void insertItem(int position, QGraphicsItem *item);
       struct Private;
       Private *const k;
};

#endif
