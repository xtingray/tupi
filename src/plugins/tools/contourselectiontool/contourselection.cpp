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
    // QList<TNodeGroup*> nodeGroups; 
    TNodeGroup *nodeGroup;
    TupGraphicsScene *scene;
    int baseZValue;
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
    k->baseZValue = 20000 + (scene->scene()->layersTotal() * 10000);
    k->nodeGroup = 0;

    // qDeleteAll(k->nodeGroups);
    // k->nodeGroups.clear();

    foreach (QGraphicsView * view, scene->views()) {
             // view->setDragMode (QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (!qgraphicsitem_cast<TControlNode *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (item->zValue() >= 20000 && qgraphicsitem_cast<TupPathItem *>(item)) {
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
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
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

        if (k->nodeGroup) {
            int index1 = scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
            int index2 = scene->currentFrame()->indexOf(currentSelection.at(0));

            tError() << "ContourSelection::release() - index1: " << index1;
            tError() << "ContourSelection::release() - index2: " << index2;

            if (index1 == index2 || index2 < 0) {
                tError() << "ContourSelection::release() - It is the same item!";
                return;
            } else {
                k->nodeGroup->clear();
            }
        }

        /*
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
        */

        foreach (QGraphicsItem *item, currentSelection) {
                 if (item) {
                     k->nodeGroup = new TNodeGroup(item, scene, TNodeGroup::LineSelection, k->baseZValue);
                     // SQA: Critical! TControlNode cast doesn't work / qgraphicsitem_cast issue / app crash!  
                     /*
                     if (!qgraphicsitem_cast<TControlNode *>(item)) {
                         if (!qgraphicsitem_cast<TupPathItem*>(item)) {
                             TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                      scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                      scene->currentFrame()->indexOf(item), QPointF(), scene->spaceMode(),
                                                      TupLibraryObject::Item, TupProjectRequest::Convert, 2);
                             emit requested(&event);
                         } else {
                             k->nodeGroup = new TNodeGroup(item, scene, TNodeGroup::LineSelection, k->baseZValue);
                         }
                     }
                     */
            }
        }

        //foreach (TNodeGroup *group, k->nodeGroups) {
                 if (!k->nodeGroup->changedNodes().isEmpty()) {
                     tError() << "release() : Tracing a node change!";
                     int position = scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
                     if (position >= 0 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem())) {
                         QDomDocument doc;
                         doc.appendChild(qgraphicsitem_cast<TupPathItem *>(k->nodeGroup->parentItem())->toXml(doc));
                    
                         TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                  scene->currentLayerIndex(), scene->currentFrameIndex(), position, 
                                                  QPointF(), scene->spaceMode(), TupLibraryObject::Item, 
                                                  TupProjectRequest::EditNodes, doc.toString());
                    
                         foreach (QGraphicsView * view, scene->views())
                                  view->setUpdatesEnabled(false);
                    
                         k->nodeGroup->restoreItem();
                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                                tError() << "ContourSelection::release() - Fatal Error: Invalid position [ " << position << " ]";
                         #endif
                         return;
                     }
                     k->nodeGroup->clearChangesNodes();
                 }
        // }

    } else {
        tError() << "No items... cleaning nodes!";
        // foreach (TNodeGroup *group, k->nodeGroups)
        if (k->nodeGroup) {
            k->nodeGroup->clear();
            k->nodeGroup = 0;
        }
        // k->nodeGroups.clear();
        // qDeleteAll(k->nodeGroups);
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
                // SQA: Add the Dynamic BG part
                TupBackground *bg = scene->background();
                if (bg) {
                    TupFrame *frame = bg->staticFrame();
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
                     // TNodeGroup *node 
                      k->nodeGroup = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection, k->baseZValue);
                     // k->nodeGroups << node;
                 }
            }
            break;

            case TupProjectRequest::EditNodes:
            {
                 if (item) {
                     // SQA: Check if this lines are required
                     // foreach (QGraphicsView * view, k->scene->views())
                     //          view->setUpdatesEnabled(true);

                     // foreach (TNodeGroup* group, k->nodeGroups) {
                              if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem()) == item) {
                                  k->nodeGroup->show();
                                  k->nodeGroup->syncNodesFromParent();
                                  k->nodeGroup->saveParentProperties();
                                  break;
                              }
                     // }
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
                // foreach (TNodeGroup* node, k->nodeGroups) {
                         if (k->nodeGroup) {
                             k->nodeGroup->show();
                             if (k->nodeGroup->parentItem()) {
                                 k->nodeGroup->parentItem()->setSelected(true);
                                 k->nodeGroup->syncNodesFromParent();
                             }
                         }
                // }
            }
            break;
    }
}

void ContourSelection::keyPressEvent(QKeyEvent *event)
{
    /* SQA: This code seems to have to effect :S
    if (event->key() == Qt::Key_Delete) {
        bool deleted = false;
    
        // foreach (TNodeGroup *nodegroup, k->nodeGroups)
        deleted = deleted || (k->nodeGroup->removeSelectedNodes() > 0);

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
    // qDeleteAll(k->nodeGroups);
    // k->nodeGroups.clear();

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
