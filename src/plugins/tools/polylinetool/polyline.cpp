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

#include "polyline.h"

#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>

#include <QPainterPath>
#include <QMatrix>

#include "ktscene.h"
#include "ktlayer.h"
#include "ktrequestbuilder.h"
#include "ktlibraryobject.h"

#include "tdebug.h"
#include "tglobal.h"
#include "taction.h"

#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktbrushmanager.h"

#include "ktprojectresponse.h"

struct PolyLine::Private
{
    bool begin;
    QPointF center;
    QPointF right;
    QPointF mirror;
    
    KNodeGroup *nodegroup;
    QPainterPath path;
    
    QMap<QString, TAction *> actions;
    
    KTPathItem *item;
    KTGraphicsScene *scene;
    
    QGraphicsLineItem *line1, *line2;
};

PolyLine::PolyLine(): k(new Private), m_configurator(0)
{
    k->begin = false;
    k->nodegroup = 0;
    k->item = 0;
    
    k->line1 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line1->setPen(QPen(QColor(55, 177, 50)));
    k->line2 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line2->setPen(QPen(QColor(55, 177, 50)));
    
    setupActions();
}

PolyLine::~PolyLine()
{
}

void PolyLine::init(KTGraphicsScene *scene)
{
    k->scene = scene;

    endItem();

    foreach (QGraphicsView *view,  scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
        
             Q_CHECK_PTR(view->scene());
             foreach (QGraphicsItem *item, scene->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

QStringList PolyLine::keys() const
{
    return QStringList() << tr("PolyLine");
}

void PolyLine::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    if (input->button() == Qt::RightButton) {
        endItem();
        return;
    }

    scene->clearSelection();

    // scene->addItem(k->line1);
    // scene->addItem(k->line2);
    
    if (!k->item) {
        k->path = QPainterPath();
        k->path.moveTo(input->pos());
        k->item = new KTPathItem();
        k->item->setPath(k->path);
        
        scene->includeObject(k->item);

        k->begin = true;
    } else {
        if (!scene->items().contains(k->item))
            scene->includeObject(k->item);
        
        k->begin = false;
        k->path = k->item->path();

        k->path.cubicTo(k->right, k->mirror, input->pos());
    }
    
    k->center = input->pos();
    k->item->setPen(brushManager->pen());
    
    if (!scene->items().contains(k->line1))
        scene->includeObject(k->line1); 

    if (!scene->items().contains(k->line2))
        scene->includeObject(k->line2);
}

void PolyLine::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
    
    foreach (QGraphicsView *view,  scene->views())
             view->setDragMode(QGraphicsView::NoDrag);
    
    k->mirror = k->center - (input->pos() - k->center);

    if (k->begin) {
        k->right = input->pos();
    } else {
        for (int i = k->path.elementCount()-1; i >= 0; i--) {
             if (k->path.elementAt(i).type == QPainterPath::CurveToElement) {
                 k->right = input->pos();
                 if (k->path.elementAt(i+1).type == QPainterPath::CurveToDataElement)
                     k->path.setElementPositionAt(i+1, k->mirror.x(), k->mirror.y());
                 break;
            }
        }
    }
    
    Q_CHECK_PTR(k->item);

    if (k->item)
        k->item->setPath(k->path);
    
    k->line1->setLine(QLineF(k->mirror, k->center));
    k->line2->setLine(QLineF(k->right, k->center));
}

void PolyLine::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    // k->scene = scene;
    
    if (!k->nodegroup) {
        k->nodegroup = new KNodeGroup(k->item, scene);
        connect(k->nodegroup, SIGNAL(nodeReleased()), this, SLOT(nodeChanged()));
    } else {
        k->nodegroup->createNodes(k->item);
    }
    
    QDomDocument doc;
    if (k->begin) {
        doc.appendChild(k->item->toXml(doc));

        KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                                       scene->currentLayerIndex(), 
                                                                       scene->currentFrameIndex(), 
                                                                       scene->currentFrame()->graphicItemsCount(), 
                                                                       QPointF(), scene->spaceMode(), KTLibraryObject::Item, 
                                                                       KTProjectRequest::Add, doc.toString());
        emit requested(&request);
    } else if (!k->nodegroup->isSelected()) {
               int position = scene->currentFrame()->indexOf(k->item);
        
               if (position != -1 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodegroup->parentItem())) {
                   doc.appendChild(qgraphicsitem_cast<KTPathItem *>(k->nodegroup->parentItem())->toXml(doc));
            
                   KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), 
                                                                                scene->currentFrameIndex(), position, 
                                                                                QPointF(), scene->spaceMode(), KTLibraryObject::Item, 
                                                                                KTProjectRequest::EditNodes, doc.toString());
                   k->nodegroup->restoreItem();
                   emit requested(&event);
               } else {
                   tDebug("selection") << "position is " << position; 
               }

               k->nodegroup->clearChangesNodes();
    }
}

void PolyLine::itemResponse(const KTItemResponse *response)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    KTProject *project = k->scene->scene()->project();
    QGraphicsItem *item = 0;
    KTScene *scene = 0;
    KTLayer *layer = 0;
    KTFrame *frame = 0;

    if (project) {

        scene = project->scene(response->sceneIndex());
        if (scene) {
            if (project->spaceContext() == KTProject::FRAMES_EDITION) {
                layer = scene->layer(response->layerIndex());
                if (layer) {
                    frame = layer->frame(response->frameIndex());
                    if (frame)
                        item = frame->item(response->itemIndex());
                }
            } else {
                KTBackground *bg = scene->background();
                if (bg) {
                    KTFrame *frame = bg->frame();
                    if (frame)
                        item = frame->item(response->itemIndex());
                }
            }
        }

    } else {
        #ifdef K_DEBUG
               tFatal() << "PolyLine::itemResponse() - Project not exist";
        #endif
    }
        
    switch (response->action()) {

        case KTProjectRequest::Add:
        {
            if (KTPathItem *path = qgraphicsitem_cast<KTPathItem *>(item)) {
                if (k->item != path) {
                    k->item = path;
                    if (k->nodegroup)
                        k->nodegroup->setParentItem(path);
                }
            }
        }
        break;

        case KTProjectRequest::Remove:
        {
            if (item == k->item) {
                k->path = QPainterPath();
                delete k->item;
                k->item = 0;
                delete k->nodegroup;
                k->nodegroup = 0;
            }
        }
        break;

        case KTProjectRequest::EditNodes:
        {
            if (k->nodegroup && item) {
                foreach (QGraphicsView * view, k->scene->views())
                         view->setUpdatesEnabled(true);
                
                if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodegroup->parentItem()) != item) {
                    delete k->item;
                    k->item = qgraphicsitem_cast<KTPathItem *>(item);
                    k->nodegroup->setParentItem(k->item);
                }

                k->nodegroup->createNodes(k->item);
                k->nodegroup->saveParentProperties();
                k->nodegroup->expandAllNodes();
            }
        }
        break;
        
        default: 
        break;
    }
}

/*
void PolyLine::keyPressEvent(QKeyEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (event->key() == Qt::Key_Escape) {
        tFatal() << "PolyLine::keyPressEvent(QKeyEvent *event) - Tracing Esc!";
        emit closeHugeCanvas();
    } else if (event->key() == Qt::Key_X) {
               tFatal() << "PolyLine::keyPressEvent(QKeyEvent *event) - Tracing X key!";
               endItem();
               event->accept();
    } else {
        tFatal() << "PolyLine::keyPressEvent(QKeyEvent *event) - Tracing any other key!";
        event->ignore();
        return;
    }
}
*/

void PolyLine::endItem()
{
    if (k->item) {
        if (k->line1 && k->line2) {
            if (k->scene->items().contains(k->line1))
                k->scene->removeItem(k->line1);
            if (k->scene->items().contains(k->line2))
                k->scene->removeItem(k->line2);
        }

        k->path = QPainterPath();
        k->item = 0;
        delete k->nodegroup;
        k->nodegroup = 0;
    }
}

void PolyLine::nodeChanged()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->nodegroup) {
        // SHOW_VAR(!d->nodegroup->changedNodes().isEmpty());
        if (!k->nodegroup->changedNodes().isEmpty()) {
            int position = -1;
            KTProject *project = k->scene->scene()->project();
            if (project->spaceContext() == KTProject::FRAMES_EDITION) {
                position = k->scene->currentFrame()->indexOf(k->nodegroup->parentItem());
            } else {
                KTBackground *bg = k->scene->scene()->background();
                KTFrame *frame = bg->frame();
                position = frame->indexOf(k->nodegroup->parentItem());
            }

            if (position != -1 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodegroup->parentItem())) {
                    QDomDocument doc;
                    doc.appendChild(qgraphicsitem_cast<KTPathItem *>(k->nodegroup->parentItem())->toXml(doc));
                
                    KTProjectRequest event = KTRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), 
                                                                                 position, QPointF(), k->scene->spaceMode(), KTLibraryObject::Item, KTProjectRequest::EditNodes, 
                                                                                 doc.toString());
                    foreach (QGraphicsView * view, k->scene->views())
                             view->setUpdatesEnabled(false);

                    k->nodegroup->restoreItem();
                    emit requested(&event);
             } else {
               #ifdef K_DEBUG
                      tFatal() << "PolyLine::nodeChanged() -> ERROR: position == -1 && No nodegroup parent item";
               #endif
             }
        } else {
          #ifdef K_DEBUG
                 tFatal() << "PolyLine::nodeChanged() -> ERROR: There are no changedNodes!";
          #endif
        }
    } else {
      #ifdef K_DEBUG
             tFatal() << "PolyLine::nodeChanged() -> ERROR: There's no nodegroup!";
      #endif
    }
}

void PolyLine::setupActions()
{
    TAction *pencil = new TAction(QIcon(THEME_DIR + "icons/polyline.png"), tr("PolyLine"), this);
    pencil->setShortcut(QKeySequence(tr("S")));
    pencil->setCursor(QCursor(THEME_DIR + "cursors/polyline.png"));
    
    k->actions.insert(tr("PolyLine"), pencil);
}

QMap<QString, TAction *> PolyLine::actions() const
{
    return k->actions;
}

int PolyLine::toolType() const
{
    return KTToolInterface::Brush;
}

QWidget *PolyLine::configurator() 
{
    if (! m_configurator)
        m_configurator = new InfoPanel;

    return m_configurator;
}

void PolyLine::aboutToChangeScene(KTGraphicsScene *)
{
    endItem();

    // SQA: Check if this code does something
    /*
    k->path = QPainterPath();
    k->item = 0;
    delete k->nodegroup;
    k->nodegroup = 0;
    
    if (k->nodegroup) {
        k->nodegroup->show();
        if (k->nodegroup->parentItem()) {
            k->nodegroup->parentItem()->setSelected(true);
            k->nodegroup->syncNodesFromParent();
        }
    }
    */
}

void PolyLine::aboutToChangeTool()
{
    endItem();
}

void PolyLine::saveConfig()
{
}

Q_EXPORT_PLUGIN2(kt_polyline, PolyLine);
