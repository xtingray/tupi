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

#include "inktool.h"
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
#include "tconfig.h"

struct InkTool::Private
{
    QPointF firstPoint;
    QPointF oldPos;

    QPointF previewPoint;
    QPointF oldPosRight;
    QPointF oldPosLeft;
    QPointF connector;

    QPainterPath path;
    QPainterPath inkPath;
    QList<QPointF> leftPoints;

    Configurator *configurator;
    QMap<QString, TAction *> actions;

    TupPathItem *item;

    int dotsCounter;
    qreal penWidth;
    qreal oldSlope;
    int arrowSize;
    int firstArrow;
    QCursor cursor;

    int spacing;
    qreal tolerance;
    qreal widthVar;
    qreal smoothness;
};

InkTool::InkTool() : k(new Private)
{
    k->configurator = 0;
    k->item = 0;
    k->cursor = QCursor(kAppProp->themeDir() + "cursors/ink.png", 0, 16);

    setupActions();
}

InkTool::~InkTool()
{
}

void InkTool::init(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);

    /*
    k->spacing = k->configurator->spacingValue();
    k->tolerance = k->configurator->sizeToleranceValue()/(qreal)100;
    k->smoothness = 3;
    */

    k->spacing = 1;
    k->tolerance = 0;
    k->smoothness = 3;

    TCONFIG->beginGroup("BrushParameters");
    int thickness = TCONFIG->value("Thickness", 3).toInt();

    k->widthVar = k->tolerance*thickness;
    if (k->widthVar < 1)
        k->widthVar = 1; 
		
    /*
    tError() << "InkTool::init() - thickness: " << thickness;
    tError() << "InkTool::init() - k->tolerance: " << k->tolerance;
    tError() << "InkTool::init() - k->widthVar: " << k->widthVar;
    */
}

QStringList InkTool::keys() const
{
    return QStringList() << tr("Ink");
}

void InkTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    k->oldSlope = 0;
    k->penWidth = brushManager->pen().widthF()/2;

    k->dotsCounter = 1;
    k->firstPoint = input->pos();
    k->connector = k->firstPoint;

    k->path = QPainterPath();
    k->path.moveTo(k->firstPoint);

    k->inkPath = QPainterPath();
    k->inkPath.setFillRule(Qt::WindingFill);
    k->inkPath.moveTo(k->firstPoint);

    k->leftPoints.clear(); 
    k->leftPoints << k->firstPoint;

    k->oldPos = input->pos();
    k->oldPosRight = input->pos();
    k->oldPosLeft = input->pos();
    k->previewPoint = input->pos();

    k->item = new TupPathItem();
    QColor color(55, 155, 55, 200);
    QPen pen(QBrush(color), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    k->item->setPen(pen);

    scene->includeObject(k->item);

    k->firstArrow = rand() % 10 + 1;
    k->arrowSize = -1;
}

void InkTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
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
            m = 100; // mx = 0 -> path is vertical | 100 == infinite

        // k->path is the guideline to calculate the real QGraphicsPathItem
        k->path.moveTo(k->previewPoint);
        k->path.lineTo(currentPoint);
        k->item->setPath(k->path);

        qreal slopeVar = std::abs(k->oldSlope - m);
        qreal distance = sqrt(pow(std::abs(currentPoint.x() - k->oldPos.x()), 2) + pow(std::abs(currentPoint.y() - k->oldPos.y()), 2));

        // Time to calculate a new point of the QGraphicsPathItem
        if ((k->dotsCounter > k->firstArrow) && ((k->dotsCounter % k->spacing == 0) || ((slopeVar >= 1) && (distance > 10)))) {
            // Calculating the begining of the line (vertex "<")
            if (k->arrowSize == -1) {
                qreal pow1 = pow(currentPoint.x() - k->firstPoint.x(), 2);
                qreal pow2 = pow(currentPoint.y() - k->firstPoint.y(), 2); 
                k->arrowSize = sqrt(pow1 + pow2);
                if (k->arrowSize > 0)
                    k->arrowSize = (rand() % k->arrowSize) + 1; 
                else
                    k->arrowSize = 5;
            }

            k->oldSlope = m;

            qreal pm;  
            qreal x0;
            qreal y0;
            qreal x1;
            qreal y1;

            if (m == 0) // path is horizontal
                pm = 100; 
            else
                pm = (-1) * (1/m);

            #ifdef K_DEBUG
                   bool isNAN = false;
                   if (m == 0) // path is horizontal
                       isNAN = true;
					   
                   if (m == 100) { // path is vertical | 100 == infinite
                       QString msg = "InkTool::move() - M: NAN";
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   } else {
                       QString msg = "InkTool::move() - M: " + QString::number(m);
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   }

                   if (isNAN) {
                       QString msg = "InkTool::move() - M(inv): NAN";
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   } else {
                       QString msg = "InkTool::move() - M(inv): " + QString::number(pm);
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   }
            #endif

            qreal hypotenuse;

            if (fabs(pm) < 5) { // path's slope is close to 0
                int cutter = k->penWidth;
                bool found = false;
                qreal limit = 0;
                int iterations = 0;

                if (k->tolerance < 1) { // tolerance == decimal percent of tolerance [0.0 -> 1.0]  
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
            }

            QPointF right;
            QPointF left;

            if (k->previewPoint.x() < currentPoint.x()) {
                if (k->previewPoint.y() < currentPoint.y()) {
                    #ifdef K_DEBUG
                        QString msg = "    -> InkTool::move() - Going down-right";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tDebug() << msg;
                        #endif
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
                               QString msg = "    -> InkTool::move() - Going up-right";
                               #ifdef Q_OS_WIN
                                   qDebug() << msg;
                               #else
                                   tDebug() << msg;
                               #endif
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
                         QString msg = "    -> InkTool::move() - Going right";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tDebug() << msg;
                         #endif
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
                        QString msg = "    -> InkTool::move() - Going down-left";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tDebug() << msg;
                        #endif
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
                               QString msg = "    -> InkTool::move() - Going up-left";
                               #ifdef Q_OS_WIN
                                   qDebug() << msg;
                               #else
                                   tDebug() << msg;
                               #endif
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
                         QString msg = "    -> InkTool::move() - Going left";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tDebug() << msg;
                         #endif
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
                               QString msg = "    -> InkTool::move() - Going up";
                               #ifdef Q_OS_WIN
                                   qDebug() << msg;
                               #else
                                   tDebug() << msg;
                               #endif
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
                               QString msg = "    -> InkTool::move() - Going down";
                               #ifdef Q_OS_WIN
                                   qDebug() << msg;
                               #else
                                   tDebug() << msg;
                               #endif
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

            k->inkPath.moveTo(k->oldPosRight);
            k->inkPath.lineTo(right);
            k->oldPosRight = right;

            k->oldPosLeft = left;
            k->leftPoints << left;

            k->oldPos = currentPoint;
        }
    }

    k->previewPoint = currentPoint;
}

void InkTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    scene->removeItem(k->item);
    QPointF currentPoint = input->pos();
    // qreal radius = brushManager->pen().width();
    // int size = k->configurator->borderSizeValue();
    // QPen inkPen(brushManager->penColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    if (k->firstPoint == currentPoint && k->inkPath.elementCount() == 1) {
        qreal radius = brushManager->pen().width();
        QPointF distance((radius + 2)/2, (radius + 2)/2);
        QPen inkPen(brushManager->penColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        TupEllipseItem *blackEllipse = new TupEllipseItem(QRectF(k->connector - distance, QSize(radius + 2, radius + 2)));
        blackEllipse->setPen(inkPen);
        blackEllipse->setBrush(inkPen.brush());
        // blackEllipse->setBrush(brushManager->brush());
        scene->includeObject(blackEllipse);

        QDomDocument doc;
        doc.appendChild(blackEllipse->toXml(doc));
        TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                         0, currentPoint, scene->spaceContext(), TupLibraryObject::Item, TupProjectRequest::Add,
                                                                         doc.toString());
        emit requested(&request);
        return;
    }

    k->path.moveTo(k->oldPos);
    k->path.lineTo(k->connector);
    k->inkPath.moveTo(k->oldPosRight);
    k->inkPath.lineTo(k->connector);

    k->leftPoints << k->connector; 

    for (int i = k->leftPoints.size()-1; i > 0; i--) {
         k->inkPath.moveTo(k->leftPoints.at(i));
         k->inkPath.lineTo(k->leftPoints.at(i-1));
    }

    k->inkPath.moveTo(k->leftPoints.at(0));
    k->inkPath.lineTo(QPointF(0, 0));

    // smoothPath(k->inkPath, k->configurator->smoothness());
    smoothPath(k->inkPath, k->smoothness);

    TupPathItem *stroke = new TupPathItem();
    // stroke->setPen(QPen(Qt::NoPen));
    stroke->setPen(QPen(brushManager->penColor()));

    /*
    if (k->configurator->showBorder())
        stroke->setPen(inkPen);
    else
        stroke->setPen(QPen(Qt::NoPen));
    */

    stroke->setBrush(brushManager->penColor());
    stroke->setPath(k->inkPath);
    scene->includeObject(stroke);

    QDomDocument doc;
    doc.appendChild(stroke->toXml(doc));
    TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                         0, QPointF(), scene->spaceContext(), TupLibraryObject::Item, TupProjectRequest::Add, 
                                                                         doc.toString());
    emit requested(&request);
}

void InkTool::setupActions()
{
    TAction *inkPen = new TAction(QPixmap(kAppProp->themeDir() + "icons/ink.png"), tr("Ink"), this);
    inkPen->setShortcut(QKeySequence(tr("K")));
    inkPen->setToolTip(tr("Ink") + " - " + "K");
    inkPen->setCursor(k->cursor);

    k->actions.insert(tr("Ink"), inkPen);
}

QMap<QString, TAction *> InkTool::actions() const
{
    return k->actions;
}

int InkTool::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *InkTool::configurator() 
{
    /*
    if (!k->configurator) {
        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(updateSpacing(int)), this, SLOT(updateSpacingVar(int)));
        connect(k->configurator, SIGNAL(updateSizeTolerance(int)), this, SLOT(updateSizeToleranceVar(int)));
    }
    */

    return k->configurator;
}

void InkTool::aboutToChangeTool() 
{
}

void InkTool::saveConfig()
{
    /*
    if (k->configurator) {
        TCONFIG->beginGroup("InkTool");
        TCONFIG->setValue("DotsSpacing", k->configurator->spacingValue());
        TCONFIG->setValue("Tolerance", k->configurator->sizeToleranceValue());
        TCONFIG->setValue("Smoothness", k->configurator->smoothness());
        TCONFIG->setValue("ShowBorder", k->configurator->showBorder());
        TCONFIG->setValue("BorderSize", k->configurator->borderSizeValue());
    }
    */
}

void InkTool::updateSpacingVar(int value)
{
    k->spacing = value;
}

void InkTool::updateSizeToleranceVar(int value)
{
    k->tolerance = (qreal)value/(qreal)100;
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

void InkTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor InkTool::cursor() const
{
    return k->cursor;
}
