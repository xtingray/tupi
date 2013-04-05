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

#include "tuptweenerstep.h"
#include "tdebug.h"

#include <QVector>

struct TupTweenerStep::Private
{
    QPointF position;
    double rotation;
    double opacity;
    QColor color;
    
    struct PairF {
        double x;
        double y;
    } shear, scale;
    
    int flags;
    int index;
};

TupTweenerStep::TupTweenerStep(int index)
 : TupAbstractSerializable(), k(new Private)
{
    k->index = index;
    k->flags = None;
}

TupTweenerStep::~TupTweenerStep()
{
    delete k;
}

void TupTweenerStep::setPosition(const QPointF &pos)
{
    k->position = pos;
    k->flags |= Position;
}

void TupTweenerStep::setRotation(double angle)
{
    k->rotation = angle;
    k->flags |= Rotation;
}

void TupTweenerStep::setScale(double sx, double sy)
{
    k->scale.x = sx;
    k->scale.y = sy;
    k->flags |= Scale;
}

void TupTweenerStep::setShear(double sh, double sv)
{
    tFatal() << "TupTweenerStep::setShear() - X: " << sh;
    tFatal() << "TupTweenerStep::setShear() - Y: " << sv;
    k->shear.x = sh;
    k->shear.y = sv;
    k->flags |= Shear;
}

void TupTweenerStep::setOpacity(double opacity)
{
    k->opacity = opacity;
    k->flags |= Opacity;
}

void TupTweenerStep::setColor(const QColor &color)
{
    k->color = color;
    k->flags |= Coloring;
}

bool TupTweenerStep::has(Type type) const
{
    bool test = k->flags & type;
    return k->flags & type;
}

int TupTweenerStep::index() const
{
    return k->index;
}

QPointF TupTweenerStep::position() const
{
    return k->position;
}

double TupTweenerStep::horizontalScale() const
{
    return k->scale.x;
}

double TupTweenerStep::verticalScale() const
{
    return k->scale.y;
}

double TupTweenerStep::horizontalShear() const
{
    return k->shear.x;
}

double TupTweenerStep::verticalShear() const
{
    return k->shear.y;
}

double TupTweenerStep::rotation() const
{
    return k->rotation;
}

double TupTweenerStep::opacity() const
{
    return k->opacity;
}

QColor TupTweenerStep::color() const
{
    return k->color;
}

QDomElement TupTweenerStep::toXml(QDomDocument& doc) const
{
    QDomElement step = doc.createElement("step");
    step.setAttribute("value", k->index);
    
    if (this->has(TupTweenerStep::Position)) {
        QDomElement e = doc.createElement("position");
        e.setAttribute("x", k->position.x());
        e.setAttribute("y", k->position.y());
        
        step.appendChild(e);
    }

    if (this->has(TupTweenerStep::Rotation)) {
        QDomElement e = doc.createElement("rotation");
        e.setAttribute("angle", k->rotation);
    
        step.appendChild(e);
    }
    
    if (this->has(TupTweenerStep::Scale)) {
        QDomElement e = doc.createElement("scale");
        e.setAttribute("sx", k->scale.x);
        e.setAttribute("sy", k->scale.y);
        
        step.appendChild(e);
    }
    
    if (this->has(TupTweenerStep::Shear)) {
        QDomElement e = doc.createElement("shear");
        e.setAttribute("sh", k->shear.x);
        e.setAttribute("sv", k->shear.y);
        
        step.appendChild(e);
    }
    
    if (this->has(TupTweenerStep::Opacity)) {
        QDomElement e = doc.createElement("opacity");
        e.setAttribute("opacity", k->opacity);

        step.appendChild(e);
    }

    if (this->has(TupTweenerStep::Coloring)) {
        QDomElement e = doc.createElement("color");
        QString red = QString::number(k->color.red());
        QString green = QString::number(k->color.green());
        QString blue = QString::number(k->color.blue());

        e.setAttribute("red", red);
        e.setAttribute("green", green);
        e.setAttribute("blue", blue);

        step.appendChild(e);
    }
    
    return step;
}

void TupTweenerStep::fromXml(const QString& xml)
{
    QDomDocument doc;
    
    if (doc.setContent(xml)) {
        QDomElement root = doc.documentElement();
        
        QDomNode node = root.firstChild();
        
        k->index = root.attribute("value").toInt();
        
        while (!node.isNull()) {
               QDomElement e = node.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "position") {
                       setPosition(QPointF(e.attribute("x").toDouble(), e.attribute("y").toDouble()));
                   } else if (e.tagName() == "rotation") {
                              setRotation(e.attribute("angle").toDouble());
                   } else if (e.tagName() == "scale") {
                              setScale(e.attribute("sx").toDouble(), e.attribute("sy").toDouble());
                   } else if (e.tagName() == "shear") {
                              setShear(e.attribute("sh").toDouble(), e.attribute("sv").toDouble());
                   } else if (e.tagName() == "opacity") {
                              setOpacity(e.attribute("opacity").toDouble());
                   } else if (e.tagName() == "color") {
                              int red = e.attribute("red").toInt();
                              int green = e.attribute("green").toInt();
                              int blue = e.attribute("blue").toInt();
                              setColor(QColor(red, green, blue));
                   }
               }
               node = node.nextSibling();
        }
    }
}
