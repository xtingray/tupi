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

#ifndef KTITEMTWEENER_H
#define KTITEMTWEENER_H

#include <QObject>
#include <QMatrix>
#include <QPointF>

#include "kttweenerstep.h"
#include "ktglobal_store.h"

class QGraphicsItem;
class QGraphicsPathItem;

/**
 * @TODO: - setColorAt, setZAt
 * @author David Cuadrado
*/

class STORE_EXPORT KTItemTweener : public QObject, public KTAbstractSerializable
{
    Q_OBJECT
 
    public:
        enum Type { Position = 0, Rotation, Scale, Opacity, Colouring, All };
        enum RotationType { Continuos = 0, Partial };
        enum RotateDirection { Clockwise = 0, Counterclockwise };

        KTItemTweener();
        ~KTItemTweener();

        QString name();
        KTItemTweener::Type type();
        
        void setPosAt(int step, const QPointF & point);
        void setRotationAt(int step, double angle);
        void setScaleAt(int step, double sx, double sy);
        void setShearAt(int step, double sh, double sv);
        void setTranslationAt(int step, double dx, double dy);
        
        void addStep(const KTTweenerStep &step);
        KTTweenerStep * stepAt(int index);
        
        void setFrames(int frames);

        int frames() const;
        int startFrame();
        QPointF transformOriginPoint();
        
        void setStep(int step);
        
        void fromXml(const QString &xml);
        QDomElement toXml(QDomDocument &doc) const;

        QGraphicsPathItem *graphicsPath() const;
        QString tweenType();

        KTItemTweener::RotationType tweenRotationType();
        
    private:
        struct Private;
        Private *const k;
};

#endif
