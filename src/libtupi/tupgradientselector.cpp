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

#include "tupgradientselector.h"
#include "tdebug.h"

#include <qdrawutil.h>
#include <QMatrix>
#include <QPolygon>

TupGradientSelector::TupGradientSelector(QWidget *parent) 
    : QAbstractSlider(parent), m_currentArrowIndex(0), m_gradient(0,0,1,1), m_update(true), m_maxArrows(10), m_currentColor(Qt::black)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    _orientation = Qt::Horizontal;
    init();
}

TupGradientSelector::TupGradientSelector(Qt::Orientation o, QWidget *parent)
    : QAbstractSlider(parent), m_currentArrowIndex(0), m_gradient(0,0,1,1), m_currentColor(Qt::black)
{
    _orientation = o;
    init();
}

void TupGradientSelector::init()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    
    setMaximumWidth(100);
    setMinimumWidth(100);
    
    setStops(m_gradient.stops());
    
    createGradient();
    emit gradientChanged(m_gradient.stops());;
}

TupGradientSelector::~TupGradientSelector()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

QRect TupGradientSelector::contentsRect() const
{
    if (orientation() == Qt::Vertical)
        return QRect(2, 2, width()-14, height());
    else
        return QRect(2, 2, width(), height()-14);
}

void  TupGradientSelector::setMaxArrows(int value)
{
    m_maxArrows = value;
    while (m_maxArrows < m_arrows.count())
           m_arrows.removeLast();

    update();
}

void TupGradientSelector::paintEvent(QPaintEvent *)
{
    QPainter painter;
    QBrush brush;
    
    painter.begin(this);
    drawContents(&painter);

    for (int i = 0; i < m_arrows.count(); i++) {

         painter.setBrush(m_arrows[i]->color());

         if (i == m_currentArrowIndex)
             painter.setPen(QPen(palette().highlight(), 1));
         else
             painter.setPen(Qt::gray);

         painter.drawPath(m_arrows[i]->form());
    }
    
    painter.end();
}

void TupGradientSelector::mousePressEvent(QMouseEvent *event)
{
    bool select = false;
    for (int i = 0; i < m_arrows.count(); i++) {
         TupGradientArrow *aArrow = m_arrows[i];
         if (aArrow->contains(event->pos())) {
             m_currentArrowIndex = i;
             select = true;
             break;
         }
    }

    if (m_arrows.count() > 2 && event->button() == Qt::RightButton) {
        m_arrows.removeAt(m_currentArrowIndex);
        m_currentArrowIndex = 0;
        repaint();
    }
    else if (!select) {
             int val;
             if (orientation() == Qt::Vertical)
                 val = (maximum() - minimum()) * (height() - event->y()) / (height()) + minimum();
             else
                 val = (maximum() - minimum()) * (width() - event->x()) / width() + minimum();

             if (!m_arrows.isEmpty())
                 addArrow(calcArrowPos(val), m_arrows[m_currentArrowIndex]->color());
             else
                 addArrow(calcArrowPos(val), m_currentColor);
    }
}

void TupGradientSelector::mouseMoveEvent(QMouseEvent *event) {
    moveArrow(event->pos());
}

void TupGradientSelector::wheelEvent(QWheelEvent *event)
{
    int val = value() + event->delta()/120;
    setValue(val);
}

void  TupGradientSelector::resizeEvent(QResizeEvent *event)
{
    QAbstractSlider::setRange(0,width());
    QAbstractSlider::setMaximum(width());

    m_update = true;

    for (int i =0; i < m_arrows.count(); i++)
         m_arrows[i]->moveVertical(calcArrowPos((int)m_arrows[i]->position()));

    QWidget::resizeEvent(event);
}

void TupGradientSelector::valueChange(int newV)
{
    update();
    
    emit newValue(newV);
}

void TupGradientSelector::moveArrow(const QPoint &pos)
{
    if (orientation() == Qt::Vertical && (pos.y() <= minimum() || pos.y() >= maximum()))
        return;

    if (orientation() == Qt::Horizontal &&  (pos.x() <= minimum() || pos.x() >= maximum()))
        return;

    int val;

    if (orientation() == Qt::Vertical)
        val = (maximum() - minimum()) * (height()-pos.y()) / (height()) + minimum();
    else
        val = (maximum() - minimum()) * (width()-pos.x()) / (width()) + minimum();

    setValue(val);
    
    m_arrows[m_currentArrowIndex]->moveArrow(pos);
    m_update = true;
    
    emit gradientChanged(m_gradient.stops());
}

QPoint TupGradientSelector::calcArrowPos(int val)
{
    QPoint p;

    if (orientation() == Qt::Vertical) {
        p.setY(height() - ((height()-10) * val / (maximum() - minimum()) + 5));
        p.setX(width() - 10);
    } else {
        p.setX(width() - ((width()) * val / (maximum() - minimum())));
        p.setY(height() - 10);
    }

    return p;
}

void TupGradientSelector::drawContents(QPainter *painter)
{
    createGradient();
    painter->setBrush(m_gradient);
    painter->drawRect(contentsRect());
}

void  TupGradientSelector::createGradient()
{
    m_gradient = QLinearGradient(contentsRect().topLeft(), contentsRect().topRight());

    for (int i = 0; i < m_arrows.count(); i++)
         m_gradient.setColorAt(valueToGradient((int) m_arrows[i]->position()), m_arrows[i]->color());
}

double TupGradientSelector::valueToGradient(int value) const
{
    float factor = static_cast<float>((value))/maximum();

    if (factor > 1.0)
        factor = 1.0;
    
    if (factor < 0)
        factor = 0;
    
    return factor;
}

void TupGradientSelector::addArrow(QPoint position, QColor color)
{
    if (m_arrows.count() < m_maxArrows) {
        TupGradientArrow *arrow = new TupGradientArrow(position, color);
        m_arrows << arrow;
        m_currentArrowIndex = m_arrows.count()-1;
        update();
        
        emit arrowAdded();
    }
}

void TupGradientSelector::setCurrentColor(const QColor& color)
{
    if (m_arrows.count() > 0) {
        TupGradientArrow *arrow  = m_arrows[m_currentArrowIndex];
        if (arrow)
            m_arrows[m_currentArrowIndex]->setColor(color);
    }
    
    createGradient();
    emit gradientChanged(m_gradient.stops());
    repaint();
    m_currentColor = color;
}

void TupGradientSelector::setStops(const QGradientStops &stops)
{
    m_gradient.setStops(stops);
    m_arrows.clear();

    for (int i = 0; i < stops.count(); i++)
         addArrow(calcArrowPos((int) (100 - (stops[i].first * 100)) ), stops[i].second);

    update();
}
