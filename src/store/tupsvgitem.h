/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#ifndef TUPSVGITEM_H
#define TUPSVGITEM_H

#include "tupabstractserializable.h"
#include "tupglobal_store.h"
#include "tupitemtweener.h"
#include "tupframe.h"

#include <QGraphicsSvgItem>
#include <QGraphicsSceneHoverEvent>

class TupFrame;

/**
 * @author David Cuadrado
*/

class STORE_EXPORT TupSvgItem : public QGraphicsSvgItem, public TupAbstractSerializable
{
    public:
        TupSvgItem(QGraphicsItem * parent = 0);
        TupSvgItem(QString &file, TupFrame *frame);
        ~TupSvgItem();

        void setSymbolName(const QString &symbolName);
        QString symbolName() const;
        QString itemPath() const;
        TupFrame *frame() const;
        TupItemTweener *tween() const;
        void removeTween();
        void rendering();
        void setTween(TupItemTweener *tween);
        bool hasTween();
        void setLastTweenPos(QPointF point);
        QPointF lastTweenPos();

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    protected:
        virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
        virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    // signals:
    //     void openInfoWidget();

    private:
        struct Private;
        Private *const k;
};

#endif
