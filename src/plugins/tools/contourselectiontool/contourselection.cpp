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

#include "contourselection.h"
#include "taction.h"
#include "tdebug.h"
#include "tglobal.h"
#include "talgorithm.h"
#include "tnodegroup.h"

#include "tupgraphicalgorithm.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupserializer.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"
#include "tuprequestbuilder.h"
#include "tupproxyitem.h"
#include "tuplibraryobject.h"

#include <QGraphicsView>
#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QDebug>
#include <QTimer>

struct ContourSelection::Private
{
    QMap<QString, TAction *> actions;
    QList<TNodeGroup*> nodeGroups; 
    TupGraphicsScene *scene;
};

ContourSelection::ContourSelection(): k(new Private)
{
    setupActions();
}

ContourSelection::~ContourSelection()
{
    delete k;
}

void ContourSelection::init(TupGraphicsScene *scene)
{
    k->scene = scene;

    qDeleteAll(k->nodeGroups);
    k->nodeGroups.clear();

    foreach (QGraphicsView * view, scene->views()) {
             view->setDragMode (QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (!qgraphicsitem_cast<TControlNode *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (item->zValue() >= 10000 && qgraphicsitem_cast<TupPathItem *>(item)) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                              }
                          } else {
                              item->setFlags(QGraphicsItem::ItemIsSelectable);
                          }
                      }

             }
    }
}

QStringList ContourSelection::keys() const
{
    return QStringList() << tr("Line Selection") ;
}

void ContourSelection::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    /*
    foreach (QGraphicsView *view, scene->views())
             view->setDragMode(QGraphicsView::RubberBandDrag);
    */

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
    
    // k->scene = scene;
}

void ContourSelection::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

void ContourSelection::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (scene->selectedItems().count() > 0) {

        QList<QGraphicsItem *> currentSelection = scene->selectedItems();
        QList<TNodeGroup *>::iterator it = k->nodeGroups.begin();
        QList<TNodeGroup *>::iterator itEnd = k->nodeGroups.end();

        while (it != itEnd) {
               int parentIndex = scene->selectedItems().indexOf((*it)->parentItem());
               if (parentIndex != -1)
                   currentSelection.removeAt(parentIndex);
               else
                   delete k->nodeGroups.takeAt(k->nodeGroups.indexOf((*it)));
               ++it;
        }

        foreach (QGraphicsItem *item, currentSelection) {
                 if (item) {
                     // SQA: Critical! TControlNode cast doesn't work / qgraphicsitem_cast issue / app crash!  
                     if (!qgraphicsitem_cast<TControlNode *>(item)) {
                         if (!qgraphicsitem_cast<TupPathItem*>(item)) {
                             TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                      scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                      scene->currentFrame()->indexOf(item), QPointF(), scene->spaceMode(),
                                                      TupLibraryObject::Item, TupProjectRequest::Convert, 2);
                             emit requested(&event);
                         } else {
                             k->nodeGroups << new TNodeGroup(item, scene, TNodeGroup::LineSelection);
                         }
                     }
            }
        }

        foreach (TNodeGroup *group, k->nodeGroups) {
                 if (!group->changedNodes().isEmpty()) {
                     int position  = scene->currentFrame()->indexOf(group->parentItem());
                     if (position != -1 && qgraphicsitem_cast<QGraphicsPathItem *>(group->parentItem())) {
                         QDomDocument doc;
                         doc.appendChild(qgraphicsitem_cast<TupPathItem *>(group->parentItem())->toXml(doc));
                    
                         TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                  scene->currentLayerIndex(), scene->currentFrameIndex(), position, 
                                                  QPointF(), scene->spaceMode(), TupLibraryObject::Item, 
                                                  TupProjectRequest::EditNodes, doc.toString());
                    
                         foreach (QGraphicsView * view, scene->views())
                                  view->setUpdatesEnabled(false);
                    
                         group->restoreItem();
                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                                tFatal() << "ContourSelection::release() - Position is " << position;
                         #endif
                     }
                     group->clearChangesNodes();
                 }
        }

    } else {

        foreach (TNodeGroup *group, k->nodeGroups)
                 group->clear();

        k->nodeGroups.clear();
        qDeleteAll(k->nodeGroups);
    }
}

void ContourSelection::itemResponse(const TupItemResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("selection");
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
                    if (frame)
                        item = frame->item(response->itemIndex());
                }
            } else {
                TupBackground *bg = scene->background();
                if (bg) {
                    TupFrame *frame = bg->frame();
                    if (frame)
                        item = frame->item(response->itemIndex());
                }
            }
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "ContourSelection::itemResponse() - Project not exist";
        #endif
    }
    
    switch (response->action()) {
        
            case TupProjectRequest::Convert:
            {
                 if (item && scene) {
                     TNodeGroup *node = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection);
                     k->nodeGroups << node;
                 }
            }
            break;

            case TupProjectRequest::EditNodes:
            {
                 if (item) {
                     foreach (QGraphicsView * view, k->scene->views())
                              view->setUpdatesEnabled(true);

                     foreach (TNodeGroup* group, k->nodeGroups) {
                              if (qgraphicsitem_cast<QGraphicsPathItem *>(group->parentItem()) == item) {
                                  group->show();
                                  group->syncNodesFromParent();
                                  group->saveParentProperties();
                                  break;
                              }
                     }
                 } else {
                     #ifdef K_DEBUG
                            tFatal() << "ContourSelection::itemResponse() - No item found";
                     #endif
                 }
            }
            break;

            case TupProjectRequest::Remove:
            {
                 return;
            }
            break;

            default:
            {
                foreach (TNodeGroup* node, k->nodeGroups) {
                         if (node) {
                             node->show();
                             if (node->parentItem()) {
                                 node->parentItem()->setSelected(true);
                                 node->syncNodesFromParent();
                             }
                         }
                }
            }
            break;
    }
}

void ContourSelection::keyPressEvent(QKeyEvent *event)
{
    /* SQA: This code seems to have to effect :S
    if (event->key() == Qt::Key_Delete) {
        bool deleted = false;
    
        foreach (TNodeGroup *nodegroup, k->nodeGroups)
                 deleted = deleted || (nodegroup->removeSelectedNodes() > 0);

        if (deleted)
            event->accept();
    }
    */

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void ContourSelection::setupActions()
{
    TAction *select = new TAction(QPixmap(kAppProp->themeDir() + "icons/nodes.png"), tr("Line Selection"), this);
    select->setShortcut(QKeySequence(tr("N")));

    k->actions.insert(tr("Line Selection"), select);
}

QMap<QString, TAction *> ContourSelection::actions() const
{
    return k->actions;
}

int ContourSelection::toolType() const
{
    return TupToolInterface::Selection;
}

QWidget *ContourSelection::configurator() 
{
    return 0;
}

void ContourSelection::aboutToChangeScene(TupGraphicsScene *scene)
{
    init(scene);
}

void ContourSelection::aboutToChangeTool()
{
    qDeleteAll(k->nodeGroups);
    k->nodeGroups.clear();

    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

void ContourSelection::saveConfig()
{
}

QCursor ContourSelection::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

Q_EXPORT_PLUGIN2(tup_editNodes, ContourSelection)
