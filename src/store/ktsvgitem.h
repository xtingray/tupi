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

#ifndef KTSVGITEM_H
#define KTSVGITEM_H

#include <QGraphicsSvgItem>
#include "ktabstractserializable.h"
#include "ktglobal_store.h"
#include "ktitemtweener.h"
#include "ktframe.h"

class KTFrame;

/**
 * @author David Cuadrado
*/

class STORE_EXPORT KTSvgItem : public QGraphicsSvgItem, public KTAbstractSerializable
{
    public:
        KTSvgItem(QGraphicsItem * parent = 0);
        KTSvgItem(QString &file, KTFrame *frame);
        ~KTSvgItem();

        void setSymbolName(const QString &symbolName);
        QString symbolName() const;
        QString itemPath() const;
        KTFrame *frame() const;
        KTItemTweener *tween() const;
        void removeTween();
        void rendering();
        void setTween(KTItemTweener *tween);
        bool hasTween();
        void setLastTweenPos(QPointF point);
        QPointF lastTweenPos();

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        struct Private;
        Private *const k;
};

#endif
