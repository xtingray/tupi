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

#include "geometrictool.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tuppathitem.h"
#include "tuplibraryobject.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupscene.h"
#include "tupframe.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tupbrushmanager.h"

#include <cmath>
#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

struct GeometricTool::Private
{
    TupRectItem *rect;
    TupEllipseItem *ellipse;
    TupLineItem *line;
    TupPathItem *path;
    TupGraphicsScene *scene;
    InfoPanel *configurator;
    bool added;
    QPointF currentPoint;
    QPointF lastPoint;
    QMap<QString, TAction *> actions;
    bool proportion;
    QGraphicsItem *item;
    QCursor squareCursor;
    QCursor circleCursor;
    QCursor lineCursor;
};

GeometricTool::GeometricTool() : k(new Private)
{
    k->scene = 0;
    k->path = 0;
    setupActions();
}

GeometricTool::~GeometricTool()
{
}

QStringList GeometricTool::keys() const
{
    return QStringList() << tr("Rectangle") << tr("Ellipse") << tr("Line");
}

void GeometricTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->scene = scene;
    delete k->path;
    k->path = 0;
    k->proportion = false;

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

void GeometricTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(kAppProp->themeDir() + "/"  + "icons/square.png"), tr("Rectangle"), this);
    action1->setShortcut(QKeySequence(tr("R")));
    k->squareCursor = QCursor(kAppProp->themeDir() + "/"  + "cursors/square.png", 0, 0);
    action1->setCursor(k->squareCursor);
    
    k->actions.insert(tr("Rectangle"), action1);
    
    TAction *action2 = new TAction(QIcon(kAppProp->themeDir() + "/"  + "icons/ellipse.png"), tr("Ellipse"), this);
    action2->setShortcut(QKeySequence(tr("C")));
    k->circleCursor = QCursor(kAppProp->themeDir() + "/"  + "cursors/circle.png");
    action2->setCursor(k->circleCursor);
    
    k->actions.insert(tr("Ellipse"), action2);
    
    TAction *action3 = new TAction(QIcon(kAppProp->themeDir() + "/"  + "icons/line.png"), tr("Line"), this);
    action3->setShortcut(QKeySequence(tr("L")));
    k->lineCursor = QCursor(kAppProp->themeDir() + "/"  + "cursors/line.png", 0, 0);
    action3->setCursor(k->lineCursor);

    k->actions.insert(tr("Line"), action3);
}

void GeometricTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (input->buttons() == Qt::LeftButton) {
        
        if (name() == tr("Rectangle")) {
            k->added = false;
            k->rect = new TupRectItem(QRectF(input->pos(), QSize(0,0)));
            k->rect->setPen(brushManager->pen());
            k->rect->setBrush(brushManager->brush());

            k->currentPoint = input->pos();

        } else if (name() == tr("Ellipse")) {

                   k->added = false;
                   k->ellipse = new TupEllipseItem(QRectF(input->pos(), QSize(0,0)));
                   k->ellipse->setPen(brushManager->pen());
                   k->ellipse->setBrush(brushManager->brush());

                   k->currentPoint = input->pos();

        } else if (name() == tr("Line")) {
                   k->currentPoint = input->pos();

                   if (k->path) {
                       QPainterPath path = k->path->path();
                       path.cubicTo(k->lastPoint, k->lastPoint, k->lastPoint);
                       k->path->setPath(path);
                   } else {
                       k->path = new TupPathItem;
                       k->path->setPen(brushManager->pen());
                       k->path->setBrush(brushManager->brush());

                       QPainterPath path;
                       path.moveTo(k->currentPoint);
                       k->path->setPath(path);
                       scene->includeObject(k->path);

                       k->line = new TupLineItem();
                       k->line->setPen(brushManager->pen());
                       k->line->setLine(QLineF(input->pos().x(), input->pos().y(), input->pos().x(), input->pos().y()));

                       TupFrame *frame = scene->currentFrame();
                       int zLevel = frame->getTopZLevel();
                       k->line->setZValue(zLevel);

                       scene->addItem(k->line);
                   }
        }
    } 
}

void GeometricTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
    
    if (name() == tr("Rectangle") || name() == tr("Ellipse")) {

        if (!k->added) {
            if (name() == tr("Rectangle"))
                scene->includeObject(k->rect);
            else
                scene->includeObject(k->ellipse);
            k->added = true;
        }

        int xMouse = input->pos().x();
        int yMouse = input->pos().y();
        int xInit = k->currentPoint.x();
        int yInit = k->currentPoint.y();

        QRectF rect;

        if (name() == tr("Rectangle"))
            rect = k->rect->rect();
        else
            rect = k->ellipse->rect();

        if (k->proportion) {

            int width = abs(xMouse - xInit);
            int height = abs(yMouse - yInit);

            bool xWins = false;
            if (width <= height)
                xWins = true; 

            QPointF target;

            if (xMouse >= xInit) {
                if (yMouse >= yInit) {
                    if (xWins)
                        target = QPointF(xInit + width, yInit + width);      
                    else
                        target = QPointF(xInit + height, yInit + height);

                    rect.setBottomRight(target);
                } else {
                    if (xWins)
                        target = QPointF(xInit + width, yInit - width);
                    else
                        target = QPointF(xInit + height, yInit - height);

                    rect.setTopRight(target);
                }
            } else {
                if (yMouse >= yInit) {
                    if (xWins)
                        target = QPointF(xInit - width, yInit + width);
                    else
                        target = QPointF(xInit - height, yInit + height);

                    rect.setBottomLeft(target);
                } else {
                    if (xWins)
                        target = QPointF(xInit - width, yInit - width);
                    else
                        target = QPointF(xInit - height, yInit - height);

                    rect.setTopLeft(target);
                }
            }
            
        } else {

            if (xMouse >= xInit) {
                if (yMouse >= yInit)
                    rect.setBottomRight(input->pos());
                else
                    rect.setTopRight(input->pos());
            } else {
                if (yMouse >= yInit)
                    rect.setBottomLeft(input->pos());
                else
                    rect.setTopLeft(input->pos());
            }
        }

        if (name() == tr("Rectangle"))
            k->rect->setRect(rect);
        else
            k->ellipse->setRect(rect);
    } 
}

void GeometricTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    QDomDocument doc;
    QPointF position;

    if (name() == tr("Rectangle")) {
        doc.appendChild(dynamic_cast<TupAbstractSerializable *>(k->rect)->toXml(doc));
        position = k->rect->pos();
    } else if (name() == tr("Ellipse")) {
               doc.appendChild(dynamic_cast<TupAbstractSerializable *>(k->ellipse)->toXml(doc));
               QRectF rect = k->ellipse->rect();
               position = rect.topLeft();
    } else if (name() == tr("Line")) {
               return;

               // doc.appendChild(dynamic_cast<TupAbstractSerializable *>(k->line)->toXml(doc));
               // position = k->line->pos();
               doc.appendChild(dynamic_cast<TupAbstractSerializable *>(k->path)->toXml(doc));
               position = k->path->boundingRect().topLeft(); 
    }
    
    TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), 
                             scene->currentFrameIndex(), scene->currentFrame()->graphics().count(), position,
                             scene->spaceMode(), TupLibraryObject::Item, TupProjectRequest::Add, doc.toString()); // Adds to end
    
    emit requested(&event);
}

QMap<QString, TAction *> GeometricTool::actions() const
{
    return k->actions;
}

int GeometricTool::toolType() const
{
    return Brush;
}
        
QWidget *GeometricTool::configurator()
{
    InfoPanel::ToolType toolType = InfoPanel::Line;

    if (name() == tr("Rectangle"))
        toolType = InfoPanel::Rectangle;
    else if (name() == tr("Ellipse"))
             toolType = InfoPanel::Ellipse;

    k->configurator = new InfoPanel(toolType);
    return k->configurator;
}

void GeometricTool::aboutToChangeScene(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(scene);

    endItem();
}

void GeometricTool::aboutToChangeTool() 
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    endItem();
}

void GeometricTool::saveConfig()
{
}

void GeometricTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
        return;
    } else if (event->key() == Qt::Key_Shift) {
               k->proportion = true;
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void GeometricTool::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
        k->proportion = false;
}

QCursor GeometricTool::cursor() const
{
    if (name() == tr("Rectangle")) {
        return k->squareCursor;
    } else if (name() == tr("Ellipse")) {
               return k->circleCursor;
    } else if (name() == tr("Line")) {
               return k->lineCursor;
    }

    return QCursor(Qt::ArrowCursor);
}

void GeometricTool::endItem()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->path) {
        QDomDocument doc;
        QPointF position;

        doc.appendChild(dynamic_cast<TupAbstractSerializable *>(k->path)->toXml(doc));
        position = QPointF(0, 0);

        TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(),
                                 k->scene->currentFrameIndex(), k->scene->currentFrame()->graphics().count(), position,
                                 k->scene->spaceMode(), TupLibraryObject::Item, TupProjectRequest::Add, doc.toString());

        emit requested(&event);

        k->path = 0;
    }
}

void GeometricTool::updatePos(QPointF pos)
{
    if (k->path) {
        QLineF line;
        if (k->proportion) {
            qreal dx = pos.x() - k->currentPoint.x();
            qreal dy = pos.y() - k->currentPoint.y();
            qreal m = fabs(dx/dy);

            if (m > 1) {
                line = QLineF(k->currentPoint.x(), k->currentPoint.y(), pos.x(), k->currentPoint.y());
                k->lastPoint = QPointF(pos.x(), k->currentPoint.y());
            } else {
                line = QLineF(k->currentPoint.x(), k->currentPoint.y(), k->currentPoint.x(), pos.y());
                k->lastPoint = QPointF(k->currentPoint.x(), pos.y());
            }

        } else {
            line = QLineF(k->currentPoint, pos);
            k->lastPoint = pos;
        }
        if (k->line)
            k->line->setLine(line);
    }
}

void GeometricTool::doubleClick(const TupInputDeviceInformation *input, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(scene);

    endItem();
}

void GeometricTool::sceneResponse(const TupSceneResponse *event)
{
    Q_UNUSED(event);
    if (name() == tr("Line")) 
        init(k->scene);
}

void GeometricTool::layerResponse(const TupLayerResponse *event)
{
    Q_UNUSED(event);
    if (name() == tr("Line")) 
        init(k->scene);
}

void GeometricTool::frameResponse(const TupFrameResponse *event)
{
    Q_UNUSED(event);
    if (name() == tr("Line")) 
        init(k->scene);
}

Q_EXPORT_PLUGIN2(tup_geometric, GeometricTool)

