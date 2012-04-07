/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "inktool.h"
#include "ink.xpm"

#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicalgorithm.h"
#include "tupgraphicsscene.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"

#include "taction.h"
#include "talgorithm.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QBrush>
#include <cmath>

InkTool::InkTool() : m_configurator(0), m_item(0)
{
    setupActions();
}

InkTool::~InkTool()
{
}

void InkTool::init(TupGraphicsScene *scene)
{
    foreach (QGraphicsView * view, scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
             Q_CHECK_PTR(view->scene());
             if (QGraphicsScene *scene = qobject_cast<QGraphicsScene *>(view->scene())) {
                 foreach (QGraphicsItem *item, scene->items()) {
                          item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                          item->setFlag(QGraphicsItem::ItemIsMovable, false);
                 }
             }
    }
}

QStringList InkTool::keys() const
{
    return QStringList() << tr("Ink");
}

void InkTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    oldSlope = 0;
    penWidth = brushManager->pen().widthF()/2;

    dotsCounter = 1;
    m_firstPoint = input->pos();

    m_path = QPainterPath();
    m_path.moveTo(m_firstPoint);

    pathRight = QPainterPath();
    pathRight.moveTo(m_firstPoint);
    pathLeft = QPainterPath();
    pathLeft.moveTo(m_firstPoint);

    m_oldPos = input->pos();
    oldPosRight = input->pos();
    oldPosLeft = input->pos();
    previewPoint = input->pos();

    m_item = new TupPathItem();
    QPen pen(Qt::gray, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    m_item->setPen(pen);

    scene->includeObject(m_item);

    TupEllipseItem *m_ellipse = new TupEllipseItem(QRectF(input->pos(), QSize(1, 1)));
    QPen firstPointPen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_ellipse->setPen(firstPointPen);

    scene->includeObject(m_ellipse);

    itemRight = new TupPathItem();
    QPen rightPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    itemRight->setPen(rightPen);

    scene->includeObject(itemRight);

    itemLeft = new TupPathItem();
    QPen leftPen(Qt::blue, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    itemLeft->setPen(leftPen);

    scene->includeObject(itemLeft);
}

void InkTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    dotsCounter++;

    foreach (QGraphicsView * view, scene->views())
             view->setDragMode(QGraphicsView::NoDrag);

    QPointF currentPoint = input->pos();

    qreal my = currentPoint.y() - previewPoint.y(); 
    qreal mx = currentPoint.x() - previewPoint.x();
    qreal m;

    if (currentPoint != previewPoint) {
        if (mx != 0)
            m = my / mx;
        else
            m = 10;

        TupEllipseItem *m_ellipse = new TupEllipseItem(QRectF(input->pos(), QSize(1, 1)));
        QPen pen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_ellipse->setPen(pen);
        m_ellipse->setBrush(brushManager->brush());
        scene->includeObject(m_ellipse);

        m_path.moveTo(previewPoint);
        m_path.lineTo(currentPoint);

        m_item->setPath(m_path);

        qreal slopeVar = abs(oldSlope - m);
        qreal distance = sqrt(pow(abs(currentPoint.x() - m_oldPos.x()), 2) + pow(abs(currentPoint.y() - m_oldPos.y()), 2));

        if ((dotsCounter % 2 == 0 && distance > 10) || ((slopeVar >= 2) && (distance > 10))) {

            oldSlope = m;

            qreal pm;  
            qreal x0;
            qreal y0;
            qreal x1;
            qreal y1;
            bool isNAN = false;

            if (m != 0) {
                pm = (-1) * (1/m);

                #ifdef K_DEBUG
                       tDebug() << "InkTool::move() - M(inv): " << pm;
                #endif
     
                if (abs(pm) < 2) {
                    x0 = currentPoint.x() - penWidth;
                    y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y(); 

                    x1 = currentPoint.x() + penWidth;
                    y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                } else {
                    if (abs(pm) < 5) {
                        x0 = currentPoint.x() - 2;
                        y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y();

                        x1 = currentPoint.x() + 2;
                        y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                    } else {
                        x0 = currentPoint.x();
                        y0 = currentPoint.y() - penWidth;

                        x1 = currentPoint.x();
                        y1 = currentPoint.y() + penWidth;
                    }
                }

            } else {

                #ifdef K_DEBUG
                       tDebug() << "InkTool::move() - M(inv): NAN";
                #endif

                pm = 10;
                isNAN = true;

                x0 = currentPoint.x();
                y0 = currentPoint.y() - penWidth;

                x1 = currentPoint.x();
                y1 = currentPoint.y() + penWidth;
            }

            TupLineItem *perpendicularLine = new TupLineItem();
            QPen perPen(Qt::black, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            perpendicularLine->setPen(perPen);
            perpendicularLine->setLine(QLineF(x0, y0, x1, y1));
            scene->includeObject(perpendicularLine);

            TupLineItem *rightAuxLine = new TupLineItem();
            QPen redAuxPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            rightAuxLine->setPen(redAuxPen);

            TupLineItem *leftAuxLine = new TupLineItem();
            QPen leftAuxPen(Qt::blue, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            leftAuxLine->setPen(leftAuxPen);

            QPointF right;
            QPointF left;

            if (previewPoint.x() < currentPoint.x()) {
                if (previewPoint.y() < currentPoint.y()) {
                    #ifdef K_DEBUG
                           tDebug() << "InkTool::move() - Going down-right";
                    #endif
                    if (y0 > y1) {
                        left = QPointF(x0, y0);
                        right = QPointF(x1, y1);
                    } else {
                        left = QPointF(x1, y1);
                        right = QPointF(x0, y0);
                    }

                } else if (previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "InkTool::move() - Going up-right";
                           #endif
                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               left = QPointF(x1, y1);
                               right = QPointF(x0, y0);
                           }
                } else {
                     #ifdef K_DEBUG
                            tDebug() << "InkTool::move() - Going right";
                     #endif
                     if (y0 > y1) {
                         left = QPointF(x0, y0);
                         right = QPointF(x1, y1);
                     } else {
                         left = QPointF(x1, y1);
                         right = QPointF(x0, y0);
                     }
                }
            } else if (previewPoint.x() > currentPoint.x()) {
                if (previewPoint.y() < currentPoint.y()) {
                     #ifdef K_DEBUG
                            tDebug() << "InkTool::move() - Going down-left";
                     #endif

                    if (y0 > y1) {
                        right = QPointF(x0, y0);
                        left = QPointF(x1, y1);
                    } else {
                        right = QPointF(x1, y1);
                        left = QPointF(x0, y0);
                    }
                } else if (previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "InkTool::move() - Going up-left";
                           #endif

                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               left = QPointF(x1, y1);
                               right = QPointF(x0, y0);
                           }
                } else {
                     #ifdef K_DEBUG
                            tDebug() << "InkTool::move() - Going left";
                     #endif
                     if (y0 > y1) {
                         right = QPointF(x0, y0);
                         left = QPointF(x1, y1);
                     } else {
                         right = QPointF(x1, y1);
                         left = QPointF(x0, y0);
                     }
                }
            } else if (previewPoint.x() == currentPoint.x()) {
                       if (previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "InkTool::move() - Going up";
                           #endif
                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               left = QPointF(x1, y1);
                               right = QPointF(x0, y0);
                           }
                       } else {
                           #ifdef K_DEBUG
                                  tDebug() << "InkTool::move() - Going down";
                           #endif
                           if (x0 > x1) {
                               right = QPointF(x0, y0);
                               left = QPointF(x1, y1);
                           } else {
                               right = QPointF(x1, y1);
                               left = QPointF(x0, y0);
                           }
                       }
            }

            rightAuxLine->setLine(QLineF(previewPoint.x(), previewPoint.y(), right.x(), right.y()));
            leftAuxLine->setLine(QLineF(previewPoint.x(), previewPoint.y(), left.x(), left.y()));

            TupEllipseItem *blueEllipse = new TupEllipseItem(QRectF(left, QSize(1, 1)));
            QPen bluePen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            blueEllipse->setPen(bluePen);
            scene->includeObject(blueEllipse);

            TupEllipseItem *redEllipse = new TupEllipseItem(QRectF(right, QSize(1, 1)));
            QPen redPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            redEllipse->setPen(redPen);
            scene->includeObject(redEllipse);

            scene->includeObject(rightAuxLine);
            scene->includeObject(leftAuxLine);

            pathRight.moveTo(oldPosRight);
            pathRight.lineTo(right);
            itemRight->setPath(pathRight);
            oldPosRight = right;

            pathLeft.moveTo(oldPosLeft);
            pathLeft.lineTo(left);
            itemLeft->setPath(pathLeft);
            oldPosLeft = left;

            m_oldPos = currentPoint;
        }
    }

    previewPoint = currentPoint;
}

void InkTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(scene);

    double smoothness = m_configurator->exactness();

    if (m_firstPoint == input->pos() && pathRight.elementCount() == 1) {
        smoothness = 0;
        qreal radius = brushManager->pen().width();
        pathRight.addEllipse(input->pos().x(), input->pos().y(), radius, radius);
        pathLeft.addEllipse(input->pos().x(), input->pos().y(), radius, radius);
    } 

    m_item->setPath(m_path);

    QPointF currentPoint = input->pos();

    pathRight.moveTo(oldPosRight);
    pathRight.lineTo(currentPoint);
    pathLeft.moveTo(oldPosLeft);
    pathLeft.lineTo(currentPoint);

    itemRight->setPath(pathRight);
    itemLeft->setPath(pathLeft);
}

void InkTool::smoothPath(QPainterPath &path, double smoothness, int from, int to)
{
    QPolygonF pol;
    QList<QPolygonF> polygons = path.toSubpathPolygons();

    QList<QPolygonF>::iterator it = polygons.begin();

    QPolygonF::iterator pointIt;

    while (it != polygons.end()) {
           pointIt = (*it).begin();

           while (pointIt <= (*it).end()-2) {
                  pol << (*pointIt);
                  pointIt += 2;
           }
           ++it;
    }

    if (smoothness > 0) {
        path = TupGraphicalAlgorithm::bezierFit(pol, smoothness, from, to);
    } else {
        path = QPainterPath();
        path.addPolygon(pol);
    }
}

void InkTool::setupActions()
{
    TAction *inkpen = new TAction(QPixmap(THEME_DIR + "icons/ink.png"), tr("Ink"), this);
    inkpen->setShortcut(QKeySequence(tr("K")) );

    QPixmap pix(THEME_DIR + "cursors/pen.png");
    inkpen->setCursor(QCursor(pix, 0, pix.height()));

    m_actions.insert(tr("Ink"), inkpen);
}

QMap<QString, TAction *> InkTool::actions() const
{
    return m_actions;
}

int InkTool::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *InkTool::configurator() 
{
    if (! m_configurator)
        m_configurator = new Configurator;

    return m_configurator;
}

void InkTool::aboutToChangeTool() 
{
}

void InkTool::saveConfig()
{
    if (m_configurator) {
        TCONFIG->beginGroup("InkTool");
        TCONFIG->setValue("Smoothness", m_configurator->exactness());
    }
}

void InkTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape)
        emit closeHugeCanvas();
}

Q_EXPORT_PLUGIN2(kt_brush, InkTool);
