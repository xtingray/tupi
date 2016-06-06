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

#ifndef TUPITEMTWEENER_H
#define TUPITEMTWEENER_H

#include "tglobal.h"
#include "tuptweenerstep.h"

#include <QObject>
#include <QMatrix>
#include <QPointF>
#include <QColor>
#include <QGraphicsItem>
#include <QHash>

class QGraphicsItem;
class QGraphicsPathItem;

/**
 * @TODO: - setColorAt, setZAt
 * @author David Cuadrado
*/

class TUPI_EXPORT TupItemTweener : public QObject, public TupAbstractSerializable
{
    Q_OBJECT
 
    public:
        enum Type {
             Position = 0,
             Rotation = 1,
             Scale = 2,
             Shear = 3,
             Opacity = 4,
             Coloring = 5,
             Composed = 6, 
             Papagayo = 7
        };

        enum RotationType { Continuos = 0, Partial };
        enum RotateDirection { Clockwise = 0, Counterclockwise };
        enum TransformAxes { XY = 0, X, Y };
        enum FillType { Line = 0, Internal, FillAll};

        TupItemTweener();
        ~TupItemTweener();

        QString name();
        TupItemTweener::Type type();
        
        void setPosAt(int step, const QPointF & point);
        void setRotationAt(int step, double angle);
        void setScaleAt(int step, double sx, double sy);
        void setShearAt(int step, double sh, double sv);
        void setOpacityAt(int step, double opacity);
        void setColorAt(int step, const QColor &color);
        
        void addStep(const TupTweenerStep &step);
        TupTweenerStep *stepAt(int index);
        
        void setFrames(int frames);

        int frames() const;
        int initFrame();
        int initLayer();
        int initScene();
        QPointF transformOriginPoint();
        
        void setStep(int step);
        
        void fromXml(const QString &xml);
        QDomElement toXml(QDomDocument &doc) const;

        QGraphicsPathItem *graphicsPath() const;
        QList<int> intervals();
        QString tweenType();

        TupItemTweener::RotationType tweenRotationType();
        int tweenRotateSpeed();
        bool tweenRotateLoop();
        TupItemTweener::RotateDirection tweenRotateDirection();
        int tweenRotateStartDegree();
        int tweenRotateEndDegree();
        bool tweenRotateReverseLoop();

        TupItemTweener::TransformAxes tweenScaleAxes();
        double tweenScaleFactor();
        int tweenScaleIterations();
        int tweenScaleLoop();
        int tweenScaleReverseLoop();

        TupItemTweener::TransformAxes tweenShearAxes();
        double tweenShearFactor();
        int tweenShearIterations();
        int tweenShearLoop();
        int tweenShearReverseLoop();

        double tweenOpacityInitialFactor();
        double tweenOpacityEndingFactor();
        int tweenOpacityIterations();
        int tweenOpacityLoop();       
        int tweenOpacityReverseLoop();

        TupItemTweener::FillType tweenColorFillType();
        QColor tweenInitialColor();
        QColor tweenEndingColor();
        int tweenColorIterations();
        int tweenColorLoop();
        int tweenColorReverseLoop();

        bool contains(TupItemTweener::Type type);
        
    private:
        struct Private;
        Private *const k;
};

#endif
