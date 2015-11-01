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

#ifndef TUPTWEENERSTEP_H
#define TUPTWEENERSTEP_H

#include "tglobal.h"
#include "tupabstractserializable.h"

#include <QPointF>
#include <QColor>
#include <QVector>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupTweenerStep : public TupAbstractSerializable
{
    public:
        enum Type {
             None = 0,
             Position = 1,
             Rotation = 2,
             Scale = 4,
             Shear = 8,
             Opacity = 16,
             Coloring = 32
        };
        
        TupTweenerStep(int index);
        ~TupTweenerStep();
        
        void setPosition(const QPointF &pos);
        void setRotation(double angle);
        void setScale(double sx, double sy);
        void setShear(double sh, double sv);
        void setOpacity(double opacity);
        void setColor(const QColor &color);
        
        QPointF position() const;
        double rotation() const;
        double horizontalScale() const;
        double verticalScale() const;
        double horizontalShear() const;
        double verticalShear() const;
        double opacity() const;
        QColor color() const;
        
        bool has(Type type) const;
        int index() const;
    
        virtual QDomElement toXml(QDomDocument& doc) const;
        virtual void fromXml(const QString& xml);
        
    private:
        struct Private;
        Private *const k;
};

#endif
