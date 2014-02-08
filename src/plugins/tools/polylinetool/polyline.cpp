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

#include "polyline.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tuprequestbuilder.h"
#include "tuplibraryobject.h"
#include "tdebug.h"
#include "tglobal.h"
#include "taction.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupbrushmanager.h"
#include "tupprojectresponse.h"

#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QMatrix>
#include <QDir>

struct PolyLine::Private
{
    bool begin;
    QPointF center;
    QPointF right;
    QPointF mirror;
    
    TNodeGroup *nodegroup;
    QPainterPath path;
    
    QMap<QString, TAction *> actions;
    
    TupPathItem *item;
    TupGraphicsScene *scene;
    
    QGraphicsLineItem *line1;
    QGraphicsLineItem *line2;
    InfoPanel *configurator;
    QCursor cursor;
};

PolyLine::PolyLine(): k(new Private)
{
    k->configurator = 0;
    k->nodegroup = 0;
    k->item = 0;

    k->cursor = QCursor(kAppProp->themeDir() + "cursors" + QDir::separator() + "polyline.png");
    
    k->line1 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line1->setPen(QPen(QColor(55, 177, 50)));
    k->line2 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line2->setPen(QPen(QColor(55, 177, 50)));
    
    setupActions();
}

PolyLine::~PolyLine()
{
}

void PolyLine::setupActions()
{
    TAction *polyline = new TAction(QIcon(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "polyline.png"), tr("PolyLine"), this);
    polyline->setShortcut(QKeySequence(tr("S")));
    polyline->setCursor(k->cursor);

    k->actions.insert(tr("PolyLine"), polyline);
}

QMap<QString, TAction *> PolyLine::actions() const
{
    return k->actions;
}

void PolyLine::init(TupGraphicsScene *scene)
{
    k->scene = scene;
    initEnv();

    foreach (QGraphicsView *view,  scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
        
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

void PolyLine::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    scene->clearSelection();

    if (k->begin) {
        k->path = QPainterPath();
        k->path.moveTo(input->pos());

        k->item = new TupPathItem();
        k->item->setPath(k->path);
        scene->includeObject(k->item);
    } else {
        if (!scene->items().contains(k->item))
            scene->includeObject(k->item);
        
        k->path = k->item->path();

       // SQA: What are the k->right/k->mirror initial values?
        k->path.cubicTo(k->right, k->mirror, input->pos());
    }
    
    k->center = input->pos();
    k->item->setPen(brushManager->pen());
    
    if (!scene->items().contains(k->line1))
        scene->includeObject(k->line1); 

    if (!scene->items().contains(k->line2))
        scene->includeObject(k->line2);
}

void PolyLine::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    k->mirror = k->center - (input->pos() - k->center);

    if (k->begin) {
        k->right = input->pos();
    } else {
        for (int i=k->path.elementCount()-1; i>=0; i--) {
             if (k->path.elementAt(i).type == QPainterPath::CurveToElement) {
                 k->right = input->pos();
                 if (k->path.elementAt(i+1).type == QPainterPath::CurveToDataElement)
                     k->path.setElementPositionAt(i+1, k->mirror.x(), k->mirror.y());
                 break;
             }
        }
    }
    
    k->item->setPath(k->path);
    k->line1->setLine(QLineF(k->mirror, k->center));
    k->line2->setLine(QLineF(k->right, k->center));
}

void PolyLine::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (!k->nodegroup) {
        k->nodegroup = new TNodeGroup(k->item, scene, TNodeGroup::Polyline, k->item->zValue()+1);
        connect(k->nodegroup, SIGNAL(nodeReleased()), this, SLOT(nodeChanged()));
    } else {
        k->nodegroup->createNodes(k->item);
    }
    
    if (k->begin) {
        QDomDocument doc;
        doc.appendChild(k->item->toXml(doc));
        TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                       scene->currentLayerIndex(), 
                                                       scene->currentFrameIndex(), 
                                                       0, QPointF(), scene->spaceMode(), TupLibraryObject::Item, 
                                                       TupProjectRequest::Add, doc.toString());
        emit requested(&request);

        k->begin = false;
    } 
}

void PolyLine::itemResponse(const TupItemResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupProject *project = k->scene->scene()->project();
    QGraphicsItem *item = 0;
    TupScene *scene = 0;
    TupLayer *layer = 0;
    TupFrame *frame = 0;

    if (project) {
        scene = project->scene(response->sceneIndex());
        if (scene) {
            if (project->spaceContext() == TupProject::FRAMES_EDITION) {
                layer = scene->layer(response->layerIndex());
                if (layer) {
                    frame = layer->frame(response->frameIndex());
                    if (frame) {
                        item = frame->item(response->itemIndex());
                    } else {
                        #ifdef K_DEBUG
                               tError() << "PolyLine::itemResponse() - Fatal Error: Frame variable is NULL!";
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "PolyLine::itemResponse() - Fatal Error: Layer variable is NULL!";
                    #endif
                }
            } else {
                TupBackground *bg = scene->background();
                if (bg) {
                    if (project->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                        TupFrame *frame = bg->staticFrame();
                        if (frame) {
                            item = frame->item(response->itemIndex());
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "PolyLine::itemResponse() - Fatal Error: Static bg frame variable is NULL!";
                            #endif
                        }
                    } else if (project->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                               TupFrame *frame = bg->dynamicFrame();
                               if (frame) {
                                   item = frame->item(response->itemIndex());
                               } else {
                                   #ifdef K_DEBUG
                                          tError() << "PolyLine::itemResponse() - Fatal Error: Dynamic bg frame variable is NULL!";
                                   #endif
                               }
                    } else {
                        #ifdef K_DEBUG
                               tError() << "PolyLine::itemResponse() - Fatal Error: Invalid spaceMode!";
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "PolyLine::itemResponse() - Fatal Error: Scene bg variable is NULL!";
                    #endif
                }
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "PolyLine::itemResponse() - Fatal Error: Scene variable is NULL!";
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "PolyLine::itemResponse() - Fatal Error: Project variable is NULL!";
        #endif
    }
        
    switch (response->action()) {
        case TupProjectRequest::Add:
        {
            if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(item)) {
                if (k->item != path) {
                    k->item = path;
                    if (k->nodegroup)
                        k->nodegroup->setParentItem(path);
                }
            }
        }
        break;
        case TupProjectRequest::Remove:
        {
            // SQA: This code is really being used
            /*
            if (item == k->item) {
                k->path = QPainterPath();
                delete k->item;
                k->item = 0;
                delete k->nodegroup;
                k->nodegroup = 0;
            }
            */
        }
        break;
        case TupProjectRequest::EditNodes:
        {
            if (k->nodegroup && item) {
                if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodegroup->parentItem()) != item) {
                    delete k->item;
                    k->item = qgraphicsitem_cast<TupPathItem *>(item);
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

void PolyLine::keyPressEvent(QKeyEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else if (event->key() == Qt::Key_X) {
               initEnv();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void PolyLine::initEnv()
{
    k->begin = true;
    k->path = QPainterPath();
    k->item = 0;

    if (k->nodegroup) { 
        k->nodegroup->clear();
        k->nodegroup = 0;
    }

    if (k->line1) {
        if (k->scene->items().contains(k->line1))
            k->scene->removeItem(k->line1);
    }
    if (k->line2) {
        if (k->scene->items().contains(k->line2))
            k->scene->removeItem(k->line2);
    }
}

void PolyLine::nodeChanged()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->nodegroup) {
        if (!k->nodegroup->changedNodes().isEmpty()) {
            int position = -1;
            TupProject *project = k->scene->scene()->project();
            if (project->spaceContext() == TupProject::FRAMES_EDITION) {
                position = k->scene->currentFrame()->indexOf(k->nodegroup->parentItem());
            } else {
                TupBackground *bg = k->scene->scene()->background();
                if (bg) {
                    if (project->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                        TupFrame *frame = bg->staticFrame();
                        if (frame) {
                            position = frame->indexOf(k->nodegroup->parentItem());
                        } else {
                            #ifdef K_DEBUG
                                   tError() << "PolyLine::nodeChanged() - Fatal Error: Static bg frame is NULL!";
                            #endif
                            return;
                        }
                    } else if (project->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                               TupFrame *frame = bg->dynamicFrame();
                               if (frame) {
                                   position = frame->indexOf(k->nodegroup->parentItem());
                               } else {
                                   #ifdef K_DEBUG
                                          tError() << "PolyLine::nodeChanged() - Fatal Error: Dynamic bg frame is NULL!";
                                   #endif
                                   return;
                               }
                    } else {
                        #ifdef K_DEBUG
                               tError() << "PolyLine::nodeChanged() - Fatal Error: Invalid spaceMode!";
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "PolyLine::nodeChanged() - Fatal Error: Scene background variable is NULL!";
                    #endif
                }
            }

            if (position >= 0 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodegroup->parentItem())) {
                    QDomDocument doc;
                    doc.appendChild(qgraphicsitem_cast<TupPathItem *>(k->nodegroup->parentItem())->toXml(doc));
                
                    TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), 
                                                                                 position, QPointF(), k->scene->spaceMode(), TupLibraryObject::Item, TupProjectRequest::EditNodes, 
                                                                                 doc.toString());
                    emit requested(&event);
                    // k->nodegroup->restoreItem();
             } else {
               #ifdef K_DEBUG
                      tError() << "PolyLine::nodeChanged() - Fatal Error: Invalid object index || No nodegroup parent item";
               #endif
               return;
             }
        } else {
          #ifdef K_DEBUG
                 tError() << "PolyLine::nodeChanged() - Fatal Error: Array of changed nodes is empty!";
          #endif
          return;
        }
    } else {
        #ifdef K_DEBUG
               tError() << "PolyLine::nodeChanged() - Fatal Error: Array of nodes is empty!";
        #endif
        return;
    }
}

int PolyLine::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *PolyLine::configurator() 
{
    if (! k->configurator)
        k->configurator = new InfoPanel;

    return k->configurator;
}

void PolyLine::aboutToChangeScene(TupGraphicsScene *)
{
    initEnv();
}

void PolyLine::aboutToChangeTool()
{
    initEnv();
}

void PolyLine::saveConfig()
{
}

QCursor PolyLine::cursor() const
{
    return k->cursor;
}

Q_EXPORT_PLUGIN2(tup_polyline, PolyLine);
