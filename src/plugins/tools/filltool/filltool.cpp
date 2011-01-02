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

#include "filltool.h"

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>

#include "kglobal.h"
#include "kdebug.h"
#include "kpathhelper.h"

#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"
#include "ktpathitem.h"

#include "ktserializer.h"

#include "ktitemconverter.h"
#include "ktrequestbuilder.h"
#include "ktlibraryobject.h"

#include "ktscene.h"

#include <QGraphicsView>
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktbrushmanager.h"

#include "cliphelper.h"

FillTool::FillTool()
{
    setupActions();
}

FillTool::~FillTool()
{
}

void FillTool::init(KTGraphicsScene *scene)
{
    foreach (QGraphicsItem *item, scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, true);
             item->setFlag(QGraphicsItem::ItemIsFocusable, true);
    }
}

QStringList FillTool::keys() const
{
    return QStringList() << tr("Internal fill") << tr("Contour fill");
}

void FillTool::setupActions()
{
    KAction *action1 = new KAction(QIcon(THEME_DIR + "icons/inside.png"), tr("Internal fill"), this);
    action1->setShortcut(QKeySequence(tr("I")));
    action1->setCursor(QCursor(THEME_DIR + "cursors/paint.png"));
    m_actions.insert(tr("Internal fill"), action1);
    
    /*
    KAction *action2 = new KAction(QIcon(THEME_DIR + "icons/fillcolor.png"), tr("Shape fill"), this);
    action2->setShortcut(QKeySequence(tr("Ctrl+F")));
    action2->setCursor(QCursor(THEME_DIR + "cursors/paint.png"));
    m_actions.insert(tr("Shape fill"), action2);
    */
    
    KAction *action3 = new KAction(QIcon(THEME_DIR + "icons/contour.png"), tr("Contour fill"), this);
    action3->setShortcut(QKeySequence(tr("B")));
    action3->setCursor(QCursor(THEME_DIR + "cursors/contour_fill.png"));
    m_actions.insert(tr("Contour fill"), action3);
}

void FillTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    if (input->buttons() == Qt::LeftButton) {
        QGraphicsItem *clickedItem = scene->itemAt(input->pos());
        
        if (currentTool() == tr("Shape fill")) {
            KTPathItem *item = KTItemConverter::convertToPath(clickedItem);
            
            if (!item) { 
                #ifdef K_DEBUG
                       kFatal() << "FillTool::press() - No item found";
                #endif
                return;
            }
            
            QList<QGraphicsItem *> colls = clickedItem->collidingItems();
            QPainterPath res = mapPath(item);
            
            if (!colls.isEmpty()) {
                bool doSubs = false;
                foreach (QGraphicsItem *xit, colls) {
                         KTPathItem *path = KTItemConverter::convertToPath(xit);
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
                             KTPathItem *path = KTItemConverter::convertToPath(xit);
                             if (path) {
                                 QPointF localPoint = xit->mapFromScene(input->pos());
                                 if (!path->shape().contains(localPoint))
                                     subs = ClipHelper::unite(subs, mapPath(path));
                             }
                    }
                    
                    res = ClipHelper::subtract(res, subs);
                    
                    QList<QPainterPath> subpaths = KPathHelper::toSubpaths(res);
                    
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
            
            KTPathItem *intersection = new KTPathItem();
            intersection->setPath(res);
            
            intersection->setZValue(clickedItem->zValue()+1);
            
            intersection->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            intersection->setBrush(brushManager->pen().brush());
            
            QDomDocument doc;
            doc.appendChild(intersection->toXml(doc));

            KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                            scene->currentLayerIndex(), scene->currentFrameIndex(), 
                            scene->currentFrame()->graphics().count(), QPointF(),
                            KTLibraryObject::Item, KTProjectRequest::Add, 
                            doc.toString()); // Adds to end
            emit requested(&event);

        } else {

            if (QAbstractGraphicsShapeItem *shape = 
                            qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(clickedItem)) {
                int position = -1;

                if (scene->spaceMode() == KTProject::FRAMES_EDITION) {
                    position = scene->currentFrame()->indexOf(shape);
                } else {
                    KTBackground *bg = scene->scene()->background();
                    KTFrame *frame = bg->frame();
                    position = frame->indexOf(shape);
                }
                
                if (position >= 0) {
                    if (currentTool() == tr("Internal fill")) {
                        shape->setBrush(brushManager->pen().brush());
                    } else if (currentTool() == tr("Contour fill")) {
                               QPen pen = shape->pen();
                               pen.setBrush(brushManager->pen().brush());
                               shape->setPen(pen);
                    }
                    
                    QDomDocument doc;
                    doc.appendChild(KTSerializer::properties(shape, doc));
                    
                    KTProjectRequest event = KTRequestBuilder::createItemRequest( 
                                scene->currentSceneIndex(), scene->currentLayerIndex(),
                                scene->currentFrameIndex(), position, QPointF(), KTLibraryObject::Item, 
                                KTProjectRequest::Transform, doc.toString());

                    emit requested(&event);
                } else {
                    #ifdef K_DEBUG
                           kFatal() << "FillTool::press() - Invalid object index";
                    #endif
                }
            }
        }
    }
}

void FillTool::move(const KTInputDeviceInformation *, KTBrushManager *, KTGraphicsScene *)
{
}

void FillTool::release(const KTInputDeviceInformation *, KTBrushManager *, KTGraphicsScene *)
{
}

QMap<QString, KAction *> FillTool::actions() const
{
    return m_actions;
}

int FillTool::toolType() const
{
    return Fill;
}
        
QWidget *FillTool::configurator()
{
    return 0;
}

void FillTool::aboutToChangeScene(KTGraphicsScene *)
{
}

void FillTool::aboutToChangeTool() 
{
}

QPainterPath FillTool::mapPath(const QPainterPath &path, const QPointF &pos)
{
    QMatrix tr1;
    tr1.translate(pos.x(), pos.y());
    
    QPainterPath p1 = tr1.map(path);
    p1.closeSubpath();
    
    return p1;
}

QPainterPath FillTool::mapPath(const QGraphicsPathItem *item)
{
    return mapPath(item->path(), item->pos());
}

void FillTool::saveConfig()
{
}

Q_EXPORT_PLUGIN2(kt_fill, FillTool)
