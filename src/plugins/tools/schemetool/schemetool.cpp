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

#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicalgorithm.h"
#include "ktgraphicsscene.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"

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

SchemeTool::SchemeTool() : m_configurator(0), m_item(0)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setupActions();
}

SchemeTool::~SchemeTool()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void SchemeTool::init(KTGraphicsScene *scene)
{
    spacing = m_configurator->spacingValue();

    TCONFIG->beginGroup("PenParameters");
    int thickness = TCONFIG->value("Thickness", -1).toInt();
    tolerance = m_configurator->sizeToleranceValue()/(qreal)100;

    widthVar = tolerance*thickness;

    if (widthVar < 1)
        widthVar = 1; 

    tError() << "SchemeTool::init() - thickness: " << thickness;
    tError() << "SchemeTool::init() - tolerance: " << tolerance;
    tError() << "SchemeTool::init() - widthVar: " << widthVar;

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

void SchemeTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
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

    m_item = new KTPathItem();
    QPen pen(Qt::lightGray, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    m_item->setPen(pen);

    scene->includeObject(m_item);

    QPointF inception = input->pos() - QPointF(0.5, 0.5);
    KTEllipseItem *m_ellipse = new KTEllipseItem(QRectF(inception, QSize(1, 1)));
    QPen firstPointPen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_ellipse->setPen(firstPointPen);
    scene->includeObject(m_ellipse);

    /*
    QDomDocument doc;
    doc.appendChild(m_ellipse->toXml(doc));

    KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                   scene->currentFrame()->graphics().count(), inception, scene->spaceMode(),
                                                                   KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
    emit requested(&request);
    */

    itemRight = new KTPathItem();
    QPen rightPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    itemRight->setPen(rightPen);
    itemRight->setBrush(brushManager->brush());

    scene->includeObject(itemRight);

    itemLeft = new KTPathItem();
    QPen leftPen(Qt::blue, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    itemLeft->setPen(leftPen);

    scene->includeObject(itemLeft);
}

void SchemeTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
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
            m = 100;

        m_path.moveTo(previewPoint);
        m_path.lineTo(currentPoint);

        m_item->setPath(m_path);

        qreal slopeVar = abs(oldSlope - m);
        qreal distance = sqrt(pow(abs(currentPoint.x() - m_oldPos.x()), 2) + pow(abs(currentPoint.y() - m_oldPos.y()), 2));

        // if ((dotsCounter % spacing == 0 && distance > 10) || ((slopeVar >= 1.5) && (distance > 10))) {
        if ((dotsCounter % spacing == 0) || ((slopeVar >= 1) && (distance > 10))) {

            oldSlope = m;

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
                   tError() << "SchemeTool::move() - M: " << m;
                   if (isNAN)
                       tError() << "SchemeTool::move() - M(inv): NAN";
                   else
                       tError() << "SchemeTool::move() - M(inv): " << pm;
            #endif

            qreal hypotenuse;

            if (fabs(pm) < 5) { // Line's slope is close to 0

                int cutter = penWidth;
                bool found = false;
                qreal limit;
                int iterations = 0;

                if (tolerance < 1) {
                    while (!found) {
                       iterations++;
                       x0 = currentPoint.x() - cutter;
                       y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y();

                       x1 = currentPoint.x() + cutter;
                       y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                       hypotenuse = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

                       limit = hypotenuse - brushManager->pen().widthF();

                       if (fabs(limit) > widthVar) {
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

                       qreal  plus = (qreal)random/(qreal)100 * (penWidth*tolerance);

                       x0 = currentPoint.x() - plus;
                       y0 = (pm*(x0 - currentPoint.x())) + currentPoint.y();

                       x1 = currentPoint.x() + plus;
                       y1 = (pm*(x1 - currentPoint.x())) + currentPoint.y();
                       hypotenuse = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
                }

                tError() << "SchemeTool::move() - H1: " << hypotenuse;
                tError() << "SchemeTool::move() - Width: " << brushManager->pen().widthF();
                tError() << "SchemeTool::move() - limit: " << limit;
                tError() << "SchemeTool::move() - tolerance: " << tolerance;
                tError() << "SchemeTool::move() - iterations: " << iterations;
                    
            } else { // Line's slope is 0 or very very close to

                    tError() << "SchemeTool::move() - Line's slope is 0!";
                    tError() << "SchemeTool::move() - widthVar: " << widthVar;
                    tError() << "SchemeTool::move() - tolerance: " << tolerance;

                    qreal delta;
                    qreal plus;
                    int random = rand() % 101;
 
                    if (tolerance < 1)
                        plus = rand() % (int) widthVar;
                    else
                        plus = (qreal)random/(qreal)100 * (penWidth*tolerance);

                    delta = penWidth + plus;

                    x0 = currentPoint.x();
                    y0 = currentPoint.y() - delta;

                    x1 = currentPoint.x();
                    y1 = currentPoint.y() + delta;

                    qreal hypotenuse = fabs(y1 - y0);
                    tError() << "SchemeTool::move() - H2: " << hypotenuse;
            }

            QPen perPen;

            if ((int)hypotenuse == brushManager->pen().width())
                perPen = QPen(QColor(255, 106, 255), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            else
                perPen = QPen(Qt::black, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

            KTLineItem *perpendicularLine = new KTLineItem();
            perpendicularLine->setPen(perPen);
            perpendicularLine->setLine(QLineF(x0, y0, x1, y1));
            scene->includeObject(perpendicularLine);

            /*
            QDomDocument doc;
            doc.appendChild(perpendicularLine->toXml(doc));

            KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                           scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                                           KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
            emit requested(&request);
            */

            KTLineItem *rightAuxLine = new KTLineItem();
            QPen redAuxPen(QColor(255, 93, 0), 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            rightAuxLine->setPen(redAuxPen);

            KTLineItem *leftAuxLine = new KTLineItem();
            QPen leftAuxPen(QColor(0, 195, 255), 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            leftAuxLine->setPen(leftAuxPen);

            QPointF right;
            QPointF left;

            if (previewPoint.x() < currentPoint.x()) {
                if (previewPoint.y() < currentPoint.y()) {
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

                } else if (previewPoint.y() > currentPoint.y()) {
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
                }
            } else if (previewPoint.x() > currentPoint.x()) {
                if (previewPoint.y() < currentPoint.y()) {
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
                } else if (previewPoint.y() > currentPoint.y()) {
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
                }
            } else if (previewPoint.x() == currentPoint.x()) {
                       if (previewPoint.y() > currentPoint.y()) {
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
                       }
            }

            rightAuxLine->setLine(QLineF(previewPoint.x(), previewPoint.y(), right.x(), right.y()));
            leftAuxLine->setLine(QLineF(previewPoint.x(), previewPoint.y(), left.x(), left.y()));
            scene->includeObject(rightAuxLine);
            scene->includeObject(leftAuxLine);

            /*
            doc.clear();
            doc.appendChild(rightAuxLine->toXml(doc));

            request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                           scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                                           KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
            emit requested(&request);

            doc.clear();
            doc.appendChild(leftAuxLine->toXml(doc));
            request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                          scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                          KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
            emit requested(&request);
            */

            QPointF inception = left - QPointF(0.5, 0.5);
            KTEllipseItem *blueEllipse = new KTEllipseItem(QRectF(inception, QSize(1, 1)));
            QPen bluePen(Qt::blue, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            blueEllipse->setPen(bluePen);
            scene->includeObject(blueEllipse);

            /*
            doc.clear();
            doc.appendChild(blueEllipse->toXml(doc));
            request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                          scene->currentFrame()->graphics().count(), inception, scene->spaceMode(),
                                                          KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
            emit requested(&request);
            */

            inception = right - QPointF(0.5, 0.5);
            KTEllipseItem *redEllipse = new KTEllipseItem(QRectF(inception, QSize(1, 1)));
            QPen redPen(Qt::red, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            redEllipse->setPen(redPen);
            scene->includeObject(redEllipse);

            /*
            doc.clear();
            doc.appendChild(redEllipse->toXml(doc));
            request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                          scene->currentFrame()->graphics().count(), inception, scene->spaceMode(),
                                                          KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
            emit requested(&request);
            */

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

        QPointF inception = currentPoint - QPointF(0.5, 0.5);
        KTEllipseItem *m_ellipse = new KTEllipseItem(QRectF(inception, QSize(1, 1)));
        QPen pen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_ellipse->setPen(pen);
        scene->includeObject(m_ellipse);

        /*
        QDomDocument doc;
        doc.appendChild(m_ellipse->toXml(doc));
        KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                      scene->currentFrame()->graphics().count(), inception, scene->spaceMode(),
                                                      KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
        emit requested(&request);
        */
    }

    previewPoint = currentPoint;
}

void SchemeTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(scene);

    QPointF currentPoint = input->pos();

    if (m_firstPoint == input->pos() && pathRight.elementCount() == 1) {
        qreal radius = brushManager->pen().width();
        pathRight.addEllipse(input->pos().x()-(radius/2), input->pos().y()-(radius/2), radius, radius);
        pathLeft.addEllipse(input->pos().x() - (radius/2 + 1), input->pos().y() - (radius/2 + 1), radius + 2, radius + 2);
    } else {
        KTEllipseItem *lastEllipse = new KTEllipseItem(QRectF(currentPoint, QSize(1, 1)));
        QPen greenPen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        lastEllipse->setPen(greenPen);
        scene->includeObject(lastEllipse);

        /*
        QDomDocument doc;
        doc.appendChild(lastEllipse->toXml(doc));
        KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                      scene->currentFrame()->graphics().count(), currentPoint, scene->spaceMode(),
                                                      KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
        emit requested(&request);
        */
    } 

    m_item->setPath(m_path);

    pathRight.moveTo(oldPosRight);
    pathRight.lineTo(currentPoint);
    pathLeft.moveTo(oldPosLeft);
    pathLeft.lineTo(currentPoint);

    itemRight->setPath(pathRight);
    itemLeft->setPath(pathLeft);

    /*
    QDomDocument doc;
    doc.appendChild(m_item->toXml(doc));

    KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                   scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                                   KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
    emit requested(&request);

    doc.clear();
    doc.appendChild(itemRight->toXml(doc));
    request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                  scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                  KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
    emit requested(&request);

    doc.clear();
    doc.appendChild(itemLeft->toXml(doc));
    request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                  scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                  KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());
    emit requested(&request);
    */
}

void SchemeTool::setupActions()
{
    TAction *schemePen = new TAction(QPixmap(THEME_DIR + "icons/scheme.png"), tr("Scheme"), this);
    schemePen->setShortcut(QKeySequence(tr("CH")) );

    QPixmap pix(THEME_DIR + "cursors/contour.png");
    schemePen->setCursor(QCursor(pix, 0, pix.height()));

    m_actions.insert(tr("Scheme"), schemePen);
}

QMap<QString, TAction *> SchemeTool::actions() const
{
    return m_actions;
}

int SchemeTool::toolType() const
{
    return KTToolInterface::Brush;
}

QWidget *SchemeTool::configurator() 
{
    if (! m_configurator) {
        m_configurator = new Configurator;
        connect(m_configurator, SIGNAL(updateSpacing(int)), this, SLOT(updateSpacingVar(int)));
        connect(m_configurator, SIGNAL(updateSizeTolerance(int)), this, SLOT(updateSizeToleranceVar(int)));
    }

    return m_configurator;
}

void SchemeTool::aboutToChangeTool() 
{
}

void SchemeTool::saveConfig()
{
}

void SchemeTool::updateSpacingVar(int value)
{
    spacing = value;
}

void SchemeTool::updateSizeToleranceVar(int value)
{
    tError() << "SchemeTool::updateSizeToleranceVar() - Value: " << value;
    tolerance = (qreal)value/(qreal)100;
}

Q_EXPORT_PLUGIN2(kt_brush, SchemeTool);
