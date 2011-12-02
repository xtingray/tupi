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
#include "knodegroup.h"

#include "ktgraphicalgorithm.h"
#include "ktscene.h"
#include "ktlayer.h"
#include "ktserializer.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktprojectresponse.h"
#include "ktrequestbuilder.h"
#include "ktproxyitem.h"
#include "ktlibraryobject.h"

#include <QGraphicsView>
#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QDebug>
#include <QTimer>

struct ContourSelection::Private
{
    QMap<QString, TAction *> actions;
    QList<KNodeGroup*> nodeGroups; 
    KTGraphicsScene *scene;
};

ContourSelection::ContourSelection(): k(new Private)
{
    setupActions();
}

ContourSelection::~ContourSelection()
{
    delete k;
}

void ContourSelection::init(KTGraphicsScene *scene)
{
    k->scene = scene;

    qDeleteAll(k->nodeGroups);
    k->nodeGroups.clear();

    foreach (QGraphicsView * view, scene->views()) {
             view->setDragMode (QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (!qgraphicsitem_cast<KControlNode *>(item)) {
                          if (scene->spaceMode() == KTProject::FRAMES_EDITION) {
                              if (item->zValue() >= 10000 && qgraphicsitem_cast<KTPathItem *>(item)) {
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

void ContourSelection::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
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

void ContourSelection::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

void ContourSelection::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (scene->selectedItems().count() > 0) {

        QList<QGraphicsItem *> currentSelection = scene->selectedItems();
        QList<KNodeGroup *>::iterator it = k->nodeGroups.begin();
        QList<KNodeGroup *>::iterator itEnd = k->nodeGroups.end();

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
                     // SQA: Critical! KControlNode cast doesn't work / qgraphicsitem_cast issue / app crash!  
                     if (!qgraphicsitem_cast<KControlNode *>(item)) {
                         if (!qgraphicsitem_cast<KTPathItem*>(item)) {
                             KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                      scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                      scene->currentFrame()->indexOf(item), QPointF(), scene->spaceMode(),
                                                      KTLibraryObject::Item, KTProjectRequest::Convert, 2);
                             emit requested(&event);
                         } else {
                             k->nodeGroups << new KNodeGroup(item, scene, KNodeGroup::LineSelection);
                         }
                     }
            }
        }

        foreach (KNodeGroup *group, k->nodeGroups) {
                 if (!group->changedNodes().isEmpty()) {
                     int position  = scene->currentFrame()->indexOf(group->parentItem());
                     if (position != -1 && qgraphicsitem_cast<QGraphicsPathItem *>(group->parentItem())) {
                         QDomDocument doc;
                         doc.appendChild(qgraphicsitem_cast<KTPathItem *>(group->parentItem())->toXml(doc));
                    
                         KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                  scene->currentLayerIndex(), scene->currentFrameIndex(), position, 
                                                  QPointF(), scene->spaceMode(), KTLibraryObject::Item, 
                                                  KTProjectRequest::EditNodes, doc.toString());
                    
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

        foreach (KNodeGroup *group, k->nodeGroups)
                 group->clear();

        k->nodeGroups.clear();
        qDeleteAll(k->nodeGroups);
    }
}

void ContourSelection::itemResponse(const KTItemResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("selection");
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
               tFatal() << "ContourSelection::itemResponse() - Project not exist";
        #endif
    }
    
    switch (response->action()) {
        
            case KTProjectRequest::Convert:
            {
                 if (item && scene) {
                     KNodeGroup *node = new KNodeGroup(item, k->scene, KNodeGroup::LineSelection);
                     k->nodeGroups << node;
                 }
            }
            break;

            case KTProjectRequest::EditNodes:
            {
                 if (item) {
                     foreach (QGraphicsView * view, k->scene->views())
                              view->setUpdatesEnabled(true);

                     foreach (KNodeGroup* group, k->nodeGroups) {
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

            case KTProjectRequest::Remove:
            {
                 return;
            }
            break;

            default:
            {
                foreach (KNodeGroup* node, k->nodeGroups) {
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
    
        foreach (KNodeGroup *nodegroup, k->nodeGroups)
                 deleted = deleted || (nodegroup->removeSelectedNodes() > 0);

        if (deleted)
            event->accept();
    }
    */

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else {
        QPair<int, int> flags = KTToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void ContourSelection::setupActions()
{
    TAction *select = new TAction(QPixmap(THEME_DIR + "icons/nodes.png"), tr("Line Selection"), this);
    select->setShortcut(QKeySequence(tr("N")));

    k->actions.insert(tr("Line Selection"), select);
}

QMap<QString, TAction *> ContourSelection::actions() const
{
    return k->actions;
}

int ContourSelection::toolType() const
{
    return KTToolInterface::Selection;
}

QWidget *ContourSelection::configurator() 
{
    return 0;
}

void ContourSelection::aboutToChangeScene(KTGraphicsScene *scene)
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

Q_EXPORT_PLUGIN2(kt_editNodes, ContourSelection)
