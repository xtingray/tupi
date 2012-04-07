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

#include "schemetool.h"
// #include "ink.xpm"

#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicalgorithm.h"
#include "tupgraphicsscene.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tuptextitem.h"

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

struct SchemeTool::Private
{
    QPointF firstPoint;
    QPointF oldPos;
    QPointF previewPoint;
    QPointF oldPosRight;
    QPointF oldPosLeft;
    QPointF connector;
    QPainterPath path;
    QPainterPath pathRight;
    QPainterPath pathLeft;
    Configurator *configurator;
    QMap<QString, TAction *> actions;
    TupPathItem *item;
    TupPathItem *itemRight;
    TupPathItem *itemLeft;
    int dotsCounter;
    qreal penWidth;
    qreal oldSlope;
    int spacing;
    qreal tolerance;
    qreal widthVar;
    int arrowSize;
    int firstArrow;
    QCursor cursor;
};

SchemeTool::SchemeTool() : k(new Private)
{
    k->configurator = 0;
    k->item = 0;
    k->cursor = QCursor(THEME_DIR + "cursors/contour.png", 0, 15);

    setupActions();
}

SchemeTool::~SchemeTool()
{
}

void SchemeTool::init(TupGraphicsScene *scene)
{
    k->spacing = k->configurator->spacingValue();

    TCONFIG->beginGroup("PenParameters");
    int thickness = TCONFIG->value("Thickness", -1).toInt();
    k->tolerance = k->configurator->sizeToleranceValue()/(qreal)100;

    k->widthVar = k->tolerance*thickness;

    if (k->widthVar < 1)
        k->widthVar = 1; 

    tError() << "SchemeTool::init() - thickness: " << thickness;
    tError() << "SchemeTool::init() - k->tolerance: " << k->tolerance;
    tError() << "SchemeTool::init() - k->widthVar: " << k->widthVar;

    TupTextItem *textItem = new TupTextItem;
    textItem->setPos(QPointF(5, 0));
    textItem->setDefaultTextColor(QColor(0, 0, 0, 80));
    textItem->setFont(QFont("Times", 10, QFont::Bold));
    textItem->setPlainText("Note: Remember, this is just an experimental plugin for R&D issues");
    scene->includeObject(textItem);

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

QStringList SchemeTool::keys() const
{
    return QStringList() << tr("Scheme");
}

void SchemeTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    k->oldSlope = 0;
    k->penWidth = brushManager->pen().widthF()/2;

    k->dotsCounter = 1;
    k->firstPoint = input->pos();
    k->connector = k->firstPoint;

    k->path = QPainterPath();
    k->path.moveTo(k->firstPoint);

    k->pathRight = QPainterPath();
    k->pathRight.moveTo(k->firstPoint);
    k->pathLeft = QPainterPath();
    k->pathLeft.moveTo(k->firstPoint);

    k->oldPos = input->pos();
    k->oldPosRight = input->pos();
    k->oldPosLeft = input->pos();
    k->previewPoint = input->pos();

    k->item = new TupPathItem();
    QPen pen(Qt::lightGray, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    k->item->setPen(pen);

    scene->includeObject(k->item);

    QPointF inception = input->pos() - QPointF(0.5, 0.5);
    TupEllipseItem *m_ellipse = new TupEllipseItem(QRectF(inception, QSize(1, 1)));
    QPen firstPointPen(QColor(255, 106, 255), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_ellipse->setPen(firstPointPen);
    scene->includeObject(m_ellipse);

    if (k->configurator->structureType() == Configurator::Organic) {
        k->itemRight = new TupPathItem();
        QPen rightPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        k->itemRight->setPen(rightPen);

        scene->includeObject(k->itemRight);

        k->itemLeft = new TupPathItem();
        QPen leftPen(Qt::blue, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        k->itemLeft->setPen(leftPen);

        scene->includeObject(k->itemLeft);
    }

    k->firstArrow = rand() % 10 + 1;
    k->arrowSize = -1;
}

void SchemeTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    k->dotsCounter++;

    foreach (QGraphicsView * view, scene->views())
             view->setDragMode(QGraphicsView::NoDrag);

    QPointF currentPoint = input->pos();

    qreal my = currentPoint.y() - k->previewPoint.y(); 
    qreal mx = currentPoint.x() - k->previewPoint.x();
    qreal m;

    if (currentPoint != k->previewPoint) {
        if (mx != 0)
            m = my / mx;
        else
            m = 100;

        k->path.moveTo(k->previewPoint);
        k->path.lineTo(currentPoint);

        k->item->setPath(k->path);

        tError() << "SchemeTool::move() - Flag 1";

        qreal slopeVar = abs(k->oldSlope - m);
        qreal distance = sqrt(pow(abs(currentPoint.x() - k->oldPos.x()), 2) + pow(abs(currentPoint.y() - k->oldPos.y()), 2));

        tError() << "SchemeTool::move() - k->firstArrow: " << k->firstArrow;
        tError() << "SchemeTool::move() - k->dotsCounter: " << k->dotsCounter;

        if ((k->dotsCounter > k->firstArrow) && ((k->dotsCounter % k->spacing == 0) || ((slopeVar >= 1) && (distance > 10)))) {

            if (k->arrowSize == -1) {
                tError() << "SchemeTool::move() - Flag 3";
                qreal pow1 = pow(currentPoint.x() - k->firstPoint.x(), 2);
                qreal pow2 = pow(currentPoint.y() - k->firstPoint.y(), 2); 
                k->arrowSize = sqrt(pow1 + pow2);
                tError() << "SchemeTool::move() - Flag 3A - pow1: " << pow1;
                tError() << "SchemeTool::move() - Flag 3A - pow2: " << pow2;
                tError() << "SchemeTool::move() - Flag 3A - k->arrowSize: " << k->arrowSize;
                if (k->arrowSize > 0)
                    k->arrowSize = (rand() % k->arrowSize) + 1; 
                else
                    k->arrowSize = 5;
            }

            tError() << "SchemeTool::move() - Arrow Size: " << k->arrowSize;

            k->oldSlope = m;

            qreal pm;  
            qreal x0;
            qreal y0;
            qreal x1;
            qreal y1;
            bool isNAN = false;

            if (m == 0) {
                isNAN = true;
                pm = 100; 
            } else {
                pm = (-1) * (1/m);
            } 

            #ifdef K_DEBUG
                   if (m == 100)
                       tError() << "SchemeTool::move() - M: NAN";
                   else
                       tError() << "SchemeTool::move() - M: " << m;

                   if (isNAN)
                       tError() << "SchemeTool::move() - M(inv): NAN";
                   else
                       tError() << "SchemeTool::move() - M(inv): " << pm;
            #endif

            qreal hypotenuse;

            if (fabs(pm) < 5) { // Line's slope is close to 0

                int cutter = k->penWidth;
                bool found = false;
                qreal limit = 0;
                int iterations = 0;

                if (k->tolerance < 1) {
                    while (!found) {
                       iterations++;
                       x0 = currentPoint.x() - cutter;
                       y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y();

                       x1 = currentPoint.x() + cutter;
                       y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                       hypotenuse = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

                       limit = hypotenuse - brushManager->pen().widthF();

                       if (fabs(limit) > k->widthVar) {
                           if (limit > 0) {
                                   cutter -= 0.2;
                                   if (cutter == 0)
                                       found = true;
                           } else {
                                   cutter += 0.2;
                           }
                       } else {
                               found = true;
                       }

                       if (iterations >10)
                           found = true;
                    }
                } else {
                       int random = rand() % 101;

                       qreal plus = (qreal)random/(qreal)100 * (k->penWidth*k->tolerance);

                       x0 = currentPoint.x() - plus;
                       y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y();

                       x1 = currentPoint.x() + plus;
                       y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                       hypotenuse = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
                }

                tError() << "SchemeTool::move() - H1: " << hypotenuse;
                tError() << "SchemeTool::move() - Width: " << brushManager->pen().width();
                tError() << "SchemeTool::move() - limit: " << limit;
                tError() << "SchemeTool::move() - k->tolerance: " << k->tolerance;
                tError() << "SchemeTool::move() - iterations: " << iterations;
                    
            } else { // Line's slope is 0 or very very close to

                    qreal delta;
                    qreal plus;
                    int random = rand() % 101;

                    if (k->tolerance == 0) {
                        plus = 0; 
                    } else if (k->tolerance < 1) {
                               if (k->widthVar > 0)
                                   plus = rand() % (int) k->widthVar;
                               else
                                   plus = rand() % 5;
                    } else {
                        plus = (qreal)random/(qreal)100 * (k->penWidth*k->tolerance);
                    }

                    delta = k->penWidth + plus;

                    x0 = currentPoint.x();
                    y0 = currentPoint.y() - delta;

                    x1 = currentPoint.x();
                    y1 = currentPoint.y() + delta;

                    hypotenuse = fabs(y1 - y0);

                    tError() << "SchemeTool::move() - Line's slope is 0!";
                    tError() << "SchemeTool::move() - k->widthVar: " << k->widthVar;
                    tError() << "SchemeTool::move() - k->tolerance: " << k->tolerance;
                    tError() << "SchemeTool::move() - H2: " << hypotenuse;
                    tError() << "SchemeTool::move() - Width: " << brushManager->pen().width();
            }


            if (k->configurator->structureType() != Configurator::Basic) {

                QPen perPen;

                int h = hypotenuse;
                if (h == brushManager->pen().width()) {
                    tError() << "SchemeTool::move() - VioLeT!: " << h;
                    perPen = QPen(QColor(255, 106, 255, 100), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                } else {
                    tError() << "SchemeTool::move() - Black!: " << h;
                    perPen = QPen(Qt::black, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
                }

                TupLineItem *perpendicularLine = new TupLineItem();
                perpendicularLine->setPen(perPen);
                perpendicularLine->setLine(QLineF(x0, y0, x1, y1));
                scene->includeObject(perpendicularLine);
            }

            QPointF right;
            QPointF left;

            if (k->previewPoint.x() < currentPoint.x()) {
                if (k->previewPoint.y() < currentPoint.y()) {
                    #ifdef K_DEBUG
                           tDebug() << "    -> SchemeTool::move() - Going down-right";
                           tDebug() << "";
                    #endif
                    if (y0 > y1) {
                        left = QPointF(x0, y0);
                        right = QPointF(x1, y1);
                    } else {
                        left = QPointF(x1, y1);
                        right = QPointF(x0, y0);
                    }

                    qreal endX = currentPoint.x() + k->arrowSize;
                    qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                    k->connector = QPoint(endX, endY);

                } else if (k->previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "    -> SchemeTool::move() - Going up-right";
                                  tDebug() << "";
                           #endif
                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               left = QPointF(x1, y1);
                               right = QPointF(x0, y0);
                           }

                           qreal endX = currentPoint.x() + k->arrowSize;
                           qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                           k->connector = QPoint(endX, endY);
                } else {
                     #ifdef K_DEBUG
                            tDebug() << "    -> SchemeTool::move() - Going right";
                            tDebug() << "";
                     #endif
                     if (y0 > y1) {
                         left = QPointF(x0, y0);
                         right = QPointF(x1, y1);
                     } else {
                         left = QPointF(x1, y1);
                         right = QPointF(x0, y0);
                     }

                     qreal endX = currentPoint.x() + k->arrowSize;
                     qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                     k->connector = QPoint(endX, endY);
                }
            } else if (k->previewPoint.x() > currentPoint.x()) {
                if (k->previewPoint.y() < currentPoint.y()) {
                     #ifdef K_DEBUG
                            tDebug() << "    -> SchemeTool::move() - Going down-left";
                            tDebug() << "";
                     #endif

                    if (y0 > y1) {
                        right = QPointF(x0, y0);
                        left = QPointF(x1, y1);
                    } else {
                        right = QPointF(x1, y1);
                        left = QPointF(x0, y0);
                    }

                    qreal endX = currentPoint.x() - k->arrowSize;
                    qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                    k->connector = QPoint(endX, endY);

                } else if (k->previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "    -> SchemeTool::move() - Going up-left";
                                  tDebug() << "";
                           #endif

                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               if (x0 < x1) {
                                   left = QPointF(x1, y1);
                                   right = QPointF(x0, y0);
                               } else { // x0 == x1
                                   if (y0 > y1) {
                                       left = QPointF(x1, y1);
                                       right = QPointF(x0, y0);
                                   } else {
                                       left = QPointF(x0, y0);
                                       right = QPointF(x1, y1);
                                   }
                               }
                           }

                           qreal endX = currentPoint.x() - k->arrowSize;
                           qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                           k->connector = QPoint(endX, endY);

                } else {
                     #ifdef K_DEBUG
                            tDebug() << "    -> SchemeTool::move() - Going left";
                            tDebug() << "";
                     #endif
                     if (y0 > y1) {
                         right = QPointF(x0, y0);
                         left = QPointF(x1, y1);
                     } else {
                         right = QPointF(x1, y1);
                         left = QPointF(x0, y0);
                     }

                     qreal endX = currentPoint.x() - k->arrowSize;
                     qreal endY = (m*(endX - currentPoint.x())) + currentPoint.y();
                     k->connector = QPoint(endX, endY);
                }
            } else if (k->previewPoint.x() == currentPoint.x()) {
                       if (k->previewPoint.y() > currentPoint.y()) {
                           #ifdef K_DEBUG
                                  tDebug() << "    -> SchemeTool::move() - Going up";
                                  tDebug() << "";
                           #endif
                           if (x0 > x1) {
                               left = QPointF(x0, y0);
                               right = QPointF(x1, y1);
                           } else {
                               left = QPointF(x1, y1);
                               right = QPointF(x0, y0);
                           }

                           qreal endX = currentPoint.x();
                           qreal endY = currentPoint.y() - k->arrowSize;
                           k->connector = QPoint(endX, endY);
                       } else {
                           #ifdef K_DEBUG
                                  tDebug() << "    -> SchemeTool::move() - Going down";
                                  tDebug() << "";
                           #endif
                           if (x0 > x1) {
                               right = QPointF(x0, y0);
                               left = QPointF(x1, y1);
                           } else {
                               right = QPointF(x1, y1);
                               left = QPointF(x0, y0);
                           }

                           qreal endX = currentPoint.x();
                           qreal endY = currentPoint.y() + k->arrowSize;
                           k->connector = QPoint(endX, endY);
                       }
            }

            if (k->configurator->structureType() == Configurator::Organic) {
                TupLineItem *rightAuxLine = new TupLineItem();
                QPen redAuxPen(QColor(255, 93, 0), 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
                rightAuxLine->setPen(redAuxPen);

                TupLineItem *leftAuxLine = new TupLineItem();
                QPen leftAuxPen(QColor(0, 195, 255), 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
                leftAuxLine->setPen(leftAuxPen);

                rightAuxLine->setLine(QLineF(k->previewPoint.x(), k->previewPoint.y(), right.x(), right.y()));
                leftAuxLine->setLine(QLineF(k->previewPoint.x(), k->previewPoint.y(), left.x(), left.y()));
                scene->includeObject(rightAuxLine);
                scene->includeObject(leftAuxLine);
            }

            if (k->configurator->structureType() != Configurator::Basic) {
                QPointF inception = left - QPointF(0.5, 0.5);
                TupEllipseItem *blueEllipse = new TupEllipseItem(QRectF(inception, QSize(1, 1)));
                QPen bluePen(Qt::blue, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                blueEllipse->setPen(bluePen);
                scene->includeObject(blueEllipse);

                inception = right - QPointF(0.5, 0.5);
                TupEllipseItem *redEllipse = new TupEllipseItem(QRectF(inception, QSize(1, 1)));
                QPen redPen(Qt::red, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                redEllipse->setPen(redPen);
                scene->includeObject(redEllipse);
            }

            k->pathRight.moveTo(k->oldPosRight);
            k->pathRight.lineTo(right);
            k->oldPosRight = right;

            k->pathLeft.moveTo(k->oldPosLeft);
            k->pathLeft.lineTo(left);
            k->oldPosLeft = left;

            if (k->configurator->structureType() == Configurator::Organic) {
                k->itemRight->setPath(k->pathRight);
                k->itemLeft->setPath(k->pathLeft);
            }

            k->oldPos = currentPoint;
        }

        QPointF inception = currentPoint - QPointF(0.5, 0.5);
        TupEllipseItem *m_ellipse = new TupEllipseItem(QRectF(inception, QSize(1, 1)));
        QPen pen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_ellipse->setPen(pen);
        scene->includeObject(m_ellipse);
    }

    k->previewPoint = currentPoint;
}

void SchemeTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(scene);

    QPointF currentPoint = input->pos();
    qreal radius = brushManager->pen().width();

    if (k->firstPoint == currentPoint && k->pathRight.elementCount() == 1) {
        k->pathRight.addEllipse(input->pos().x()-(radius/2), input->pos().y()-(radius/2), radius, radius);
        k->pathLeft.addEllipse(input->pos().x() - (radius/2 + 1), input->pos().y() - (radius/2 + 1), radius + 2, radius + 2);
    } else {
        QPointF inception = k->connector - QPointF(0.5, 0.5);
        TupEllipseItem *lastEllipse = new TupEllipseItem(QRectF(inception, QSize(1, 1)));
        QPen greenPen(QColor(255, 106, 255), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        lastEllipse->setPen(greenPen);
        scene->includeObject(lastEllipse);
    } 

    k->path.moveTo(k->oldPos);
    k->path.lineTo(k->connector);
    k->pathRight.moveTo(k->oldPosRight);
    k->pathRight.lineTo(k->connector);
    k->pathLeft.moveTo(k->oldPosLeft);
    k->pathLeft.lineTo(k->connector);

    k->item->setPath(k->path);

    if (k->configurator->structureType() == Configurator::Organic) {
        k->itemRight->setPath(k->pathRight);
        k->itemLeft->setPath(k->pathLeft);
    }

    if (k->configurator->runSimulation()) {

        QPen blackPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        if (k->firstPoint != currentPoint) {

            tError() << "SchemeTool::release() - Flag 1";
            tError() << "SchemeTool::release() - Connector: [" << k->connector.x() << ", " << k->connector.y() << "]";

            k->pathRight.moveTo(k->connector);
            k->pathRight.lineTo(QPoint(0, 0));
            k->pathLeft.moveTo(k->connector);
            k->pathLeft.lineTo(QPoint(0, 0));

            smoothPath(k->pathRight, k->configurator->smoothness());

            tError() << "SchemeTool::release() - Flag 2";

            smoothPath(k->pathLeft, k->configurator->smoothness());

            tError() << "SchemeTool::release() - Flag 3";

            TupPathItem *blackRight = new TupPathItem();
            blackRight->setPen(blackPen);
            blackRight->setPath(k->pathRight);
            scene->includeObject(blackRight);

            tError() << "SchemeTool::release() - Flag 4";

            TupPathItem *blackLeft = new TupPathItem();
            blackLeft->setPen(blackPen);
            blackLeft->setPath(k->pathLeft);
            scene->includeObject(blackLeft);

            tError() << "SchemeTool::release() - Flag 5";

            QDomDocument doc;
            doc.appendChild(blackRight->toXml(doc));
            TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                         scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                         TupLibraryObject::Item, TupProjectRequest::Add, doc.toString());
            emit requested(&request);

            doc.clear();
            doc.appendChild(blackLeft->toXml(doc));
            request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                        scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                        TupLibraryObject::Item, TupProjectRequest::Add, doc.toString());
            emit requested(&request);

        } else {
            QPointF distance((radius + 2)/2, (radius + 2)/2);
            TupEllipseItem *blackEllipse = new TupEllipseItem(QRectF(k->connector - distance, QSize(radius + 2, radius + 2)));
            blackEllipse->setPen(blackPen);
            scene->includeObject(blackEllipse);

            QDomDocument doc;
            doc.appendChild(blackEllipse->toXml(doc));
            TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                         scene->currentFrame()->graphics().count(), currentPoint, scene->spaceMode(),
                                                         TupLibraryObject::Item, TupProjectRequest::Add, doc.toString());
            emit requested(&request);
        }
    }
}

void SchemeTool::setupActions()
{
    TAction *schemePen = new TAction(QPixmap(THEME_DIR + "icons/scheme.png"), tr("Scheme"), this);
    schemePen->setShortcut(QKeySequence(tr("M")));
    schemePen->setCursor(k->cursor);

    k->actions.insert(tr("Scheme"), schemePen);
}

QMap<QString, TAction *> SchemeTool::actions() const
{
    return k->actions;
}

int SchemeTool::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *SchemeTool::configurator() 
{
    if (! k->configurator) {
        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(updateSpacing(int)), this, SLOT(updateSpacingVar(int)));
        connect(k->configurator, SIGNAL(updateSizeTolerance(int)), this, SLOT(updateSizeToleranceVar(int)));
    }

    return k->configurator;
}

void SchemeTool::aboutToChangeTool() 
{
}

void SchemeTool::saveConfig()
{
}

void SchemeTool::updateSpacingVar(int value)
{
    k->spacing = value;
}

void SchemeTool::updateSizeToleranceVar(int value)
{
    tError() << "SchemeTool::updateSizeToleranceVar() - Value: " << value;
    k->tolerance = (qreal)value/(qreal)100;
}

void SchemeTool::smoothPath(QPainterPath &path, double smoothness, int from, int to)
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

void SchemeTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    // } else if (event->modifiers() != Qt::ShiftModifier && event->modifiers() != Qt::ControlModifier) {
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor SchemeTool::cursor() const
{
    return k->cursor;
}

Q_EXPORT_PLUGIN2(kt_brush, SchemeTool);
