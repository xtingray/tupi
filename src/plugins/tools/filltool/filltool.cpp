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

#include "filltool.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tpathhelper.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tuppathitem.h"
#include "tupserializer.h"
#include "tupitemconverter.h"
#include "tuprequestbuilder.h"
#include "tuplibraryobject.h"
#include "tupscene.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupbrushmanager.h"

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

struct FillTool::Private
{
    QMap<QString, TAction *> actions;
    TupGraphicsScene *scene;
    QCursor insideCursor;
    QCursor contourCursor;
};

FillTool::FillTool() : k(new Private)
{
    setupActions();
}

FillTool::~FillTool()
{
}

void FillTool::init(TupGraphicsScene *scene)
{
    foreach (QGraphicsItem *item, scene->items()) {
             if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                 if (item->zValue() >= 10000 && item->toolTip().length()==0) {
                     item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
                 } else {
                     item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                     item->setFlag(QGraphicsItem::ItemIsFocusable, false);
                 }
             } else {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
             }
    }

    k->scene = scene;
}

QStringList FillTool::keys() const
{
    return QStringList() << tr("Internal fill") << tr("Line fill");
}

void FillTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(kAppProp->themeDir() + "icons/inside.png"), tr("Internal fill"), this);
    action1->setShortcut(QKeySequence(tr("I")));
    k->insideCursor = QCursor(kAppProp->themeDir() + "cursors/paint.png");
    action1->setCursor(k->insideCursor);
    k->actions.insert(tr("Internal fill"), action1);
    
    /*
    TAction *action2 = new TAction(QIcon(kAppProp->themeDir() + "icons/fillcolor.png"), tr("Shape fill"), this);
    action2->setShortcut(QKeySequence(tr("Ctrl+F")));
    action2->setCursor(QCursor(kAppProp->themeDir() + "cursors/paint.png"));
    k->actions.insert(tr("Shape fill"), action2);
    */
    
    TAction *action3 = new TAction(QIcon(kAppProp->themeDir() + "icons/contour.png"), tr("Line fill"), this);
    action3->setShortcut(QKeySequence(tr("B")));
    k->contourCursor = QCursor(kAppProp->themeDir() + "cursors/contour_fill.png");
    action3->setCursor(k->contourCursor);
    k->actions.insert(tr("Line fill"), action3);
}

void FillTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    if (input->buttons() == Qt::LeftButton) {
        QGraphicsItem *clickedItem = scene->itemAt(input->pos());
        
        if (name() == tr("Shape fill")) {
            TupPathItem *item = TupItemConverter::convertToPath(clickedItem);
            
            if (!item) { 
                #ifdef K_DEBUG
                       tFatal() << "FillTool::press() - No item found";
                #endif
                return;
            }
            
            // QList<QGraphicsItem *> colls = clickedItem->collidingItems();
            QPainterPath res = mapPath(item);

            /*
            if (!colls.isEmpty()) {
                bool doSubs = false;
                foreach (QGraphicsItem *xit, colls) {
                         TupPathItem *path = TupItemConverter::convertToPath(xit);
                         if (path) {
                             QPointF localPoint = xit->mapFromScene(input->pos());
                             if (path->shape().contains(localPoint) && 
                                        path->scenePos() != item->scenePos()) {
                                 res = ClipHelper::intersect(res, mapPath(path));
                             } else {
                                 doSubs = true;
                             }
                         }
                }
                
                if (doSubs) {
                    QPainterPath subs;
                    
                    foreach (QGraphicsItem *xit, colls) {
                             TupPathItem *path = TupItemConverter::convertToPath(xit);
                             if (path) {
                                 QPointF localPoint = xit->mapFromScene(input->pos());
                                 if (!path->shape().contains(localPoint))
                                     subs = ClipHelper::unite(subs, mapPath(path));
                             }
                    }
                    
                    res = ClipHelper::subtract(res, subs);
                    
                    QList<QPainterPath> subpaths = TPathHelper::toSubpaths(res);
                    
                    if (subpaths.count() > 1) {
                        foreach (QPainterPath subpath, subpaths) {
                                 if (subpath.contains(input->pos())) {
                                     res = subpath;
                                     break;
                                 }
                        }
                    }
                }
            }
            */
            
            TupPathItem *intersection = new TupPathItem();
            intersection->setPath(res);
            
            intersection->setZValue(clickedItem->zValue()+1);
            
            intersection->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            intersection->setBrush(brushManager->pen().brush());
            
            QDomDocument doc;
            doc.appendChild(intersection->toXml(doc));

            TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                            scene->currentLayerIndex(), scene->currentFrameIndex(), 
                            scene->currentFrame()->graphics().count(), QPointF(),
                            scene->spaceMode(), TupLibraryObject::Item, TupProjectRequest::Add, 
                            doc.toString()); // Adds to end
            emit requested(&event);

        } else {

            if (QAbstractGraphicsShapeItem *shape = 
                            qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(clickedItem)) {
                int position = -1;

                if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                    position = scene->currentFrame()->indexOf(shape);
                } else {
                    TupBackground *bg = scene->scene()->background();
                    TupFrame *frame = bg->staticFrame();
                    position = frame->indexOf(shape);
                }
                
                if (position >= 0) {
                    if (name() == tr("Internal fill")) {
                        shape->setBrush(brushManager->pen().brush());
                    } else if (name() == tr("Line fill")) {
                               QPen pen = shape->pen();
                               pen.setBrush(brushManager->pen().brush());
                               shape->setPen(pen);
                    }
                    
                    QDomDocument doc;
                    doc.appendChild(TupSerializer::properties(shape, doc));
                    
                    TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                scene->currentSceneIndex(), scene->currentLayerIndex(),
                                scene->currentFrameIndex(), position, QPointF(), 
                                scene->spaceMode(), TupLibraryObject::Item, 
                                TupProjectRequest::Transform, doc.toString());

                    emit requested(&event);
                } else {
                    #ifdef K_DEBUG
                           tFatal() << "FillTool::press() - Invalid object index";
                    #endif
                }
            }
        }
    }
}

void FillTool::move(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

void FillTool::release(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

QMap<QString, TAction *> FillTool::actions() const
{
    return k->actions;
}

int FillTool::toolType() const
{
    return TupToolInterface::Fill;
}
        
QWidget *FillTool::configurator()
{
    return 0;
}

void FillTool::aboutToChangeScene(TupGraphicsScene *)
{
}

void FillTool::aboutToChangeTool() 
{
    foreach (QGraphicsItem *item, k->scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             item->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

QPainterPath FillTool::mapPath(const QPainterPath &path, const QPointF &pos)
{
    QMatrix transform;
    transform.translate(pos.x(), pos.y());
    
    QPainterPath painter = transform.map(path);
    painter.closeSubpath();
    
    return painter;
}

QPainterPath FillTool::mapPath(const QGraphicsPathItem *item)
{
    return mapPath(item->path(), item->pos());
}

void FillTool::saveConfig()
{
}

void FillTool::keyPressEvent(QKeyEvent *event)
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

QCursor FillTool::cursor() const
{
    if (name() == tr("Internal fill")) {
        return k->insideCursor;
    } else if (name() == tr("Line fill")) {
               return k->contourCursor;
    }

    return QCursor(Qt::ArrowCursor);
}

Q_EXPORT_PLUGIN2(tup_fill, FillTool)
