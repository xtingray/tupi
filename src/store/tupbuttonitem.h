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

#ifndef TUPBUTTONITEM_H
#define TUPBUTTONITEM_H

#include "tupabstractserializable.h"
#include "tupglobal_store.h"

#include <QGraphicsItem>
#include <QIcon>
#include <QFont>

/**
 * @author David Cuadrado
**/

class STORE_EXPORT TupButtonItem : public QObject, public QGraphicsItem, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        TupButtonItem(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);
        ~TupButtonItem();
        
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;
        
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w);
        QRectF boundingRect() const;
        
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
        
    public:
        void setIconSize(const QSize &size);
        void setIcon(const QIcon &icon);
        void setText(const QString &text);
        void setFont(const QFont &font);
        
    signals:
        void clicked();
        
    private:
        QSize m_iconSize;
        QIcon m_icon;
        QString m_text;
        QFont m_font;
};

#endif
