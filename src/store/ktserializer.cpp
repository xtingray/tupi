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

#include "ktserializer.h"

#include <QGraphicsItem>
#include <QFont>

#include "ktsvg2qt.h"
#include "tdebug.h"

KTSerializer::KTSerializer()
{
}

KTSerializer::~KTSerializer()
{
}

QDomElement KTSerializer::properties(const QGraphicsItem *item, QDomDocument &doc)
{
    QDomElement properties = doc.createElement("properties");
    
    QString strMatrix = "matrix(";
    QTransform m = item->transform();
    qreal a = m.m11();
    qreal b = m.m12();
    qreal c = m.m21();
    qreal d = m.m22();
    qreal e = m.dx();
    qreal f = m.dy();
    
    strMatrix += QString::number(a) + "," + QString::number(b) + "," + QString::number(c) + "," + QString::number(d) + "," + QString::number(e) + "," + QString::number(f) + ")" ; 
    
    properties.setAttribute("transform", strMatrix);
    properties.setAttribute("pos", "(" + QString::number(item->pos().x()) + "," + QString::number(item->pos().y()) + ")");
    properties.setAttribute("enabled", item->isEnabled());
    properties.setAttribute("flags", item->flags());

    return properties;
}

void KTSerializer::loadProperties(QGraphicsItem *item, const QXmlAttributes &atts)
{
    QMatrix matrix;
    KTSvg2Qt::svgmatrix2qtmatrix(atts.value("transform"), matrix);
    QTransform transform(matrix);
    item->setTransform(transform);
        
    QPointF pos;
    KTSvg2Qt::parsePointF(atts.value("pos"), pos);
    item->setPos(pos);
        
    item->setEnabled(atts.value("pos") != "0"); // default true
    item->setFlags(QGraphicsItem::GraphicsItemFlags(atts.value("flags").toInt()));
}

void KTSerializer::loadProperties(QGraphicsItem *item, const QDomElement &e)
{
    if (e.tagName() == "properties") {

        QMatrix matrix;
        KTSvg2Qt::svgmatrix2qtmatrix(e.attribute("transform"), matrix);
        QTransform transform(matrix);
        item->setTransform(transform); 

        QPointF pos;
        KTSvg2Qt::parsePointF(e.attribute("pos"), pos);
        item->setPos(pos);
        
        item->setEnabled(e.attribute("pos") != "0");
        item->setFlags(QGraphicsItem::GraphicsItemFlags(e.attribute("flags").toInt()));
    }
}

QDomElement KTSerializer::gradient(const QGradient *gradient, QDomDocument &doc)
{
    QDomElement element = doc.createElement("gradient");
    element.setAttribute("type", gradient->type());
    element.setAttribute("spread", gradient->spread());
    
    switch (gradient->type()) {
        case QGradient::LinearGradient:
        {
            element.setAttribute("startX", static_cast<const QLinearGradient *>(gradient)->start().x());
            element.setAttribute("startY", static_cast<const QLinearGradient *>(gradient)->start().y());

            element.setAttribute("finalX", static_cast<const QLinearGradient *>(gradient)->finalStop().x());
            element.setAttribute("finalY", static_cast<const QLinearGradient *>(gradient)->finalStop().y());
        }
        break;
        case QGradient::RadialGradient:
        {
            element.setAttribute("centerX", static_cast<const QRadialGradient *>(gradient)->center().x());
            element.setAttribute("centerY", static_cast<const QRadialGradient *>(gradient)->center().y());

            element.setAttribute("focalX", static_cast<const QRadialGradient *>(gradient)->focalPoint().x());
            element.setAttribute("focalY", static_cast<const QRadialGradient *>(gradient)->focalPoint().y());

            element.setAttribute("radius", static_cast<const QRadialGradient *>(gradient)->radius());
        }
        break;
        case QGradient::ConicalGradient:
        {
            element.setAttribute("centerX", static_cast<const QRadialGradient *>(gradient)->center().x());
            element.setAttribute("centerY", static_cast<const QRadialGradient *>(gradient)->center().y());

            element.setAttribute("angle", static_cast<const QConicalGradient *>(gradient)->angle());
        }
        break;
        case QGradient::NoGradient:
        {
        }
        break;
    }

    QGradientStops stops = gradient->stops();

    foreach (QGradientStop stop, stops) {
             QDomElement stopElement = doc.createElement("stop");
             stopElement.setAttribute("value", stop.first);
             stopElement.setAttribute("colorName", stop.second.name());
             stopElement.setAttribute("alpha", stop.second.alpha());
             element.appendChild(stopElement);
    }
    
    return element;
}

QGradient * KTSerializer::createGradient(const QXmlAttributes &atts)
{
    QGradient *result;

    switch (atts.value("type").toInt()) {
        case QGradient::LinearGradient:
        {
            result = new QLinearGradient(QPointF(atts.value("startX").toDouble(),
                     atts.value("startY").toDouble()),
                     QPointF(atts.value("finalX").toDouble(),
                     atts.value("finalY").toDouble()));
        }
        break;
        case QGradient::RadialGradient:
        {
            result = new QRadialGradient(
                     QPointF(atts.value("centerX").toDouble(),atts.value("centerY").toDouble()),
                     atts.value("radius").toDouble(),
                     QPointF(atts.value("focalX").toDouble(), atts.value("focalY").toDouble()));
        }
        break;
        case QGradient::ConicalGradient:
        {
            result = new QConicalGradient(QPointF(atts.value("centerX").toDouble(),atts.value("centerY").toDouble()), atts.value("angle").toDouble());
        }
        break;
        case QGradient::NoGradient:
        {
            result = 0;
        }
        break;
    }
    
    if (!result)
        return 0;

    result->setSpread(QGradient::Spread(atts.value("spread").toInt()));

    return result;
}

QDomElement KTSerializer::brush(const QBrush *brush, QDomDocument &doc)
{
    QDomElement brushElement = doc.createElement("brush");
    
    brushElement.setAttribute("style", brush->style());
    
    if (brush->gradient()) {
        brushElement.appendChild(gradient(brush->gradient() , doc));
    } else if(brush->color().isValid()) {
        brushElement.setAttribute("color", brush->color().name());
        brushElement.setAttribute("alpha", brush->color().alpha());
    }
    
    QString strMatrix = "matrix(";
    QMatrix m = brush->matrix();
    qreal a = m.m11();
    qreal b = m.m12();
    qreal c = m.m21();
    qreal d = m.m22();
    qreal e = m.dx();
    qreal f = m.dy();
    
    strMatrix += QString::number(a) + "," +QString::number(b) + "," + QString::number(c) + "," + QString::number(d) + "," + QString::number(e) + "," + QString::number(f) + ")"; 
    
    brushElement.setAttribute("transform", strMatrix);

    return brushElement;
}

void KTSerializer::loadBrush(QBrush &brush, const QXmlAttributes &atts)
{
    brush.setStyle(Qt::BrushStyle(atts.value("style").toInt()));
    
    if (!atts.value("color").isEmpty()) {
        QColor color(atts.value("color"));
        color.setAlpha(atts.value("alpha").toInt());
        brush.setColor(color);
    }
    
    QMatrix matrix;
    KTSvg2Qt::svgmatrix2qtmatrix(atts.value("transform"), matrix);
    brush.setMatrix(matrix);
}

void KTSerializer::loadBrush(QBrush &brush, const QDomElement &e)
{
    brush.setStyle(Qt::BrushStyle(e.attribute("style").toInt()));

    if (!e.attribute("color").isEmpty()) {
        brush.setColor(QColor(e.attribute("color")));
    } else {
        
    }

    QMatrix matrix;
    KTSvg2Qt::svgmatrix2qtmatrix(e.attribute("transform"), matrix);
    brush.setMatrix(matrix);
}

QDomElement KTSerializer::pen(const QPen *pen, QDomDocument &doc)
{
    QDomElement penElement = doc.createElement("pen");
    
    penElement.setAttribute("style", pen->style());
    penElement.setAttribute("color", pen->color().name());
    penElement.setAttribute("alpha", pen->color().alpha());
    penElement.setAttribute("capStyle", pen->capStyle());
    penElement.setAttribute("joinStyle", pen->joinStyle());
    penElement.setAttribute("width", pen->widthF());
    penElement.setAttribute("miterLimit", pen->miterLimit());
    
    QBrush brush = pen->brush();
    penElement.appendChild(KTSerializer::brush(&brush, doc));
    
    return penElement;
}

void KTSerializer::loadPen(QPen &pen, const QXmlAttributes &atts)
{
    pen.setCapStyle(Qt::PenCapStyle(atts.value("capStyle").toInt()));
    pen.setStyle(Qt::PenStyle(atts.value("style").toInt()));
    pen.setJoinStyle(Qt::PenJoinStyle(atts.value("joinStyle").toInt()));
    pen.setWidthF(atts.value("width").toDouble());
    pen.setMiterLimit(atts.value("miterLimit").toInt());
    
    if (!atts.value("color").isEmpty()) {
        QColor color(atts.value("color"));
        color.setAlpha(atts.value("alpha").toInt());
    }
}

void KTSerializer::loadPen(QPen &pen, const QDomElement &e)
{
    
}

QDomElement KTSerializer::font(const QFont *font, QDomDocument &doc)
{
    QDomElement fontElement = doc.createElement("font");
    
    fontElement.setAttribute("family", font->family());
    fontElement.setAttribute("pointSize", font->pointSize());
    fontElement.setAttribute("weight", font->weight());
    fontElement.setAttribute("italic", font->italic());
    fontElement.setAttribute("bold", font->bold());
    fontElement.setAttribute("style", font->style());
    
    fontElement.setAttribute("underline", font->underline());
    fontElement.setAttribute("overline", font->overline());
    
    return fontElement;
}

void KTSerializer::loadFont(QFont &font, const QDomElement &e)
{
    font = QFont(e.attribute("family"), e.attribute("pointSize", "-1").toInt(), e.attribute("weight", "-1").toInt(), e.attribute( "italic", "0").toInt());
    
    font.setBold(e.attribute("bold", "0").toInt());
    font.setStyle(QFont::Style(e.attribute("style").toInt()));
    font.setUnderline(e.attribute("underline", "0").toInt());
    font.setOverline(e.attribute("overline", "0").toInt());
}

void KTSerializer::loadFont(QFont &font, const QXmlAttributes &atts)
{
    font = QFont(atts.value("family"), atts.value("pointSize", "-1").toInt(), atts.value("weight", "-1").toInt(), atts.value( "italic", "0").toInt());
    
    font.setBold(atts.value("bold", "0").toInt());
    font.setStyle(QFont::Style(atts.value("style").toInt()));
    font.setUnderline(atts.value("underline", "0").toInt());
    font.setOverline(atts.value("overline", "0").toInt());
}
