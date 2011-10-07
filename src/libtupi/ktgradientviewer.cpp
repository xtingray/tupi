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

#include "ktgradientviewer.h"
#include "tdebug.h"

#include <QPainter>
#include <QMouseEvent>
#include <QRectF>

class KTGradientViewer::ControlPoint
{
    public:
        ControlPoint()
        {
            currentIndex = 0;
            points << QPointF(10,50) << QPointF(60,50);
        }

        /*
        ~ControlPoint() 
        {
        }
        */

        QVector<QPointF> points;
        int currentIndex;
        
        void selectPoint(const QPointF &point)
        {
            int rate = 5;
            QRectF rect(point - QPointF(rate/2, rate/2) , QSizeF(rate, rate));
            
            QVector<QPointF>::const_iterator it;
            for (it = points.begin(); it != points.end(); ++it) {
                 if (rect.contains(*it)) {
                     currentIndex = points.indexOf(*it);
                     break;
                 }
            }
        }

        void drawPoints(QPainter *p)
        {
            foreach (QPointF point, points) {
                     if (point == points[currentIndex]) {
                         p->save();
                         p->setPen(QPen(Qt::red, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                     } else {
                         p->save();
                         p->setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                     }
                     p->drawPoint(point);
                     p->restore();
            }
        }
};

KTGradientViewer::KTGradientViewer(QWidget *parent) : QFrame(parent), m_angle(0), m_radius(50)
{
    m_controlPoint = new ControlPoint();
    m_type = QGradient::LinearGradient;
    m_spread = QGradient::PadSpread;

    setMidLineWidth(2);
    setLineWidth(2);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    createGradient();
    repaint();
}

KTGradientViewer::~KTGradientViewer()
{
    delete m_controlPoint;
}

void KTGradientViewer::paintEvent(QPaintEvent *event)
{
    createGradient();
    QPainter p;
    p.begin(this);
    p.setBrush(m_gradient);
    p.drawRect(rect());
    p.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    m_controlPoint->drawPoints(&p);
    
    p.end();
    QFrame::paintEvent(event);
}

QSize KTGradientViewer::sizeHint() const
{
    return QSize(100, 100);
}

void KTGradientViewer::resizeEvent(QResizeEvent *event)
{
    int s = qMin(event->size().width(), event->size().height());
    resize(QSize(s, s));
}

void KTGradientViewer::createGradient()
{
    switch (m_type) {
            case  QGradient::LinearGradient:
            {
                m_gradient = QLinearGradient(m_controlPoint->points[0], m_controlPoint->points[1]);
                break;
            }
            case QGradient::RadialGradient:
            {
                m_gradient = QRadialGradient(m_controlPoint->points[0], m_radius, m_controlPoint->points[1]);
                break;
            }
            case QGradient::ConicalGradient:
            {
                m_gradient = QConicalGradient(m_controlPoint->points[0], m_angle);
                break;
            }
            default:
            {
                #ifdef K_DEBUG
                       tError() << "KTGradientViewer::createGradient() - Fatal Error: the gradient type doesn't exists!";
                #endif
            }
    }

    m_gradient.setStops(m_gradientStops);
    m_gradient.setSpread(m_spread);
}

void KTGradientViewer::changeGradientStops(const QGradientStops &stops)
{
    m_gradientStops = stops;
    
    repaint();
}

void KTGradientViewer::changeType(int type)
{
    m_type = QGradient::Type(type);

    repaint();
}

void KTGradientViewer::setSpread(int spread)
{
    m_spread = QGradient::Spread(spread);

    repaint();
}

QGradient KTGradientViewer::gradient()
{
    QGradient gradientNormalized;

    switch (m_gradient.type()) {
            case QGradient::LinearGradient:
            {
                gradientNormalized = QLinearGradient(normalizePoint(m_controlPoint->points[0]), normalizePoint(m_controlPoint->points[1]));
                break;
            }
            case QGradient::RadialGradient:
            {
                gradientNormalized = QRadialGradient(normalizePoint(m_controlPoint->points[0]), m_radius, normalizePoint(m_controlPoint->points[1]));
                break;
            }
            case QGradient::ConicalGradient:
            {
                gradientNormalized = QConicalGradient(normalizePoint(m_controlPoint->points[0]), m_angle);
                break;
            }
            default:
            {
                #ifdef K_DEBUG
                       tError() << "KTGradientViewer::gradient() - Fatal error: the gradient type doesn't exists!";
                #endif
            }
    }

    gradientNormalized.setStops(m_gradientStops);
    gradientNormalized.setSpread(m_spread);

    return gradientNormalized;
}

void KTGradientViewer::mousePressEvent(QMouseEvent *event)
{
    m_controlPoint->selectPoint(event->pos());
    update();
}

void KTGradientViewer::mouseMoveEvent(QMouseEvent *event)
{
    m_controlPoint->points[m_controlPoint->currentIndex] = event->pos();
    update();

    emit gradientChanged();
}

void KTGradientViewer::changeAngle(int angle)
{
    m_angle = angle;
    update();

    emit gradientChanged();
}

void KTGradientViewer::changeRadius(int radius)
{
    m_radius = radius;
    update();

    emit gradientChanged();
}

void KTGradientViewer::setGradient(const QGradient* gradient)
{
    m_gradientStops = gradient->stops();
    m_spread = gradient->spread();
    m_type = gradient->type();

    switch (m_type) {
            case QGradient::LinearGradient:
            {
                 m_controlPoint->points[0] = static_cast<const QLinearGradient*>(gradient)->start();
                 m_controlPoint->points[1] = static_cast<const QLinearGradient*>(gradient)->finalStop();
                 break;
            }
            case QGradient::RadialGradient:
            {
                 m_controlPoint->points[0] = static_cast<const QRadialGradient*>(gradient)->center();
                 m_controlPoint->points[1] = static_cast<const QRadialGradient*>(gradient)->focalPoint();
                 m_radius = static_cast<const QRadialGradient*>(gradient)->radius();
                 break;
            }
            case QGradient::ConicalGradient:
            {
                 m_controlPoint->points[0] = static_cast<const QConicalGradient*>(gradient)->center();
                 m_controlPoint->points[1] = static_cast<const QConicalGradient*>(gradient)->center();
                 m_radius = static_cast<const QConicalGradient*>(gradient)->angle();
                 break;
            }
            default:
            {
                 #ifdef K_DEBUG
                        tError() << "KTGradientViewer::setGradient() - Fatal Error: the gradient type doesn't exists!";
                 #endif
            }
    }

    repaint();
}

QPointF KTGradientViewer::normalizePoint(const QPointF & point)
{
    return QPointF(point.x()*(100/width()), point.y()*(100/height()));
}
