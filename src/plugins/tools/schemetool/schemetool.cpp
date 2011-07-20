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
    QPen pen(Qt::gray, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    m_item->setPen(pen);

    scene->includeObject(m_item);

    KTEllipseItem *m_ellipse = new KTEllipseItem(QRectF(input->pos(), QSize(1, 1)));
    QPen firstPointPen(Qt::green, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_ellipse->setPen(firstPointPen);

    scene->includeObject(m_ellipse);

    itemRight = new KTPathItem();
    QPen rightPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
    itemRight->setPen(rightPen);

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
            m = 10;

        KTEllipseItem *m_ellipse = new KTEllipseItem(QRectF(input->pos(), QSize(1, 1)));
        QPen pen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_ellipse->setPen(pen);
        m_ellipse->setBrush(brushManager->brush());
        scene->includeObject(m_ellipse);

        m_path.moveTo(previewPoint);
        m_path.lineTo(currentPoint);

        m_item->setPath(m_path);

        qreal slopeVar = abs(oldSlope - m);
        qreal distance = sqrt(pow(abs(currentPoint.x() - m_oldPos.x()), 2) + pow(abs(currentPoint.y() - m_oldPos.y()), 2));

        // if ((dotsCounter % 2 == 0 && distance > 10) || ((slopeVar >= 2) && (distance > 10))) {

        if ((dotsCounter % spacing == 0 && distance > 10) || ((slopeVar >= 2) && (distance > 10))) {

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
                       tDebug() << "SchemeTool::move() - M(inv): " << pm;
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
                       tDebug() << "SchemeTool::move() - M(inv): NAN";
                #endif

                pm = 10;
                isNAN = true;

                x0 = currentPoint.x();
                y0 = currentPoint.y() - penWidth;

                x1 = currentPoint.x();
                y1 = currentPoint.y() + penWidth;
            }

            KTLineItem *perpendicularLine = new KTLineItem();
            QPen perPen(Qt::black, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            perpendicularLine->setPen(perPen);
            perpendicularLine->setLine(QLineF(x0, y0, x1, y1));
            scene->includeObject(perpendicularLine);

            KTLineItem *rightAuxLine = new KTLineItem();
            QPen redAuxPen(Qt::red, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            rightAuxLine->setPen(redAuxPen);

            KTLineItem *leftAuxLine = new KTLineItem();
            QPen leftAuxPen(Qt::blue, 0.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            leftAuxLine->setPen(leftAuxPen);

            QPointF right;
            QPointF left;

            if (previewPoint.x() < currentPoint.x()) {
                if (previewPoint.y() < currentPoint.y()) {
                    #ifdef K_DEBUG
                           tDebug() << "SchemeTool::move() - Going down-right";
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
                                  tDebug() << "SchemeTool::move() - Going up-right";
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
                            tDebug() << "SchemeTool::move() - Going right";
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
                            tDebug() << "SchemeTool::move() - Going down-left";
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
                                  tDebug() << "SchemeTool::move() - Going up-left";
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
                            tDebug() << "SchemeTool::move() - Going left";
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
                                  tDebug() << "SchemeTool::move() - Going up";
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
                                  tDebug() << "SchemeTool::move() - Going down";
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

            KTEllipseItem *blueEllipse = new KTEllipseItem(QRectF(left, QSize(1, 1)));
            QPen bluePen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            blueEllipse->setPen(bluePen);
            scene->includeObject(blueEllipse);

            KTEllipseItem *redEllipse = new KTEllipseItem(QRectF(right, QSize(1, 1)));
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

void SchemeTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(scene);

    if (m_firstPoint == input->pos() && pathRight.elementCount() == 1) {
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

Q_EXPORT_PLUGIN2(kt_brush, SchemeTool);
