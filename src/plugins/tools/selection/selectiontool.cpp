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

#include "selectiontool.h"
#include "taction.h"
#include "talgorithm.h"
#include "tupgraphicalgorithm.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupsvgitem.h"
#include "tupsvg2qt.h"
#include "tupitemgroup.h"
#include "tupellipseitem.h"
#include "tupgraphicobject.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "nodemanager.h"
#include "tupserializer.h"
#include "tuprequestbuilder.h"
#include "tupprojectresponse.h"
#include "tosd.h"

struct SelectionTool::Private
{
    QMap<QString, TAction *> actions;
    QList<QGraphicsItem *> selectedObjects;
    QList<NodeManager*> nodeManagers;
    TupGraphicsScene *scene;
    bool selectionFlag;
    qreal scaleFactor;
    qreal realFactor;
    int nodeZValue;
    TupEllipseItem *center;
    QGraphicsLineItem *target1;
    QGraphicsLineItem *target2;
    bool targetIsIncluded;
    QString key;
};

SelectionTool::SelectionTool(): k(new Private), panel(0)
{
    setupActions();
}

SelectionTool::~SelectionTool()
{
    delete k;
}

void SelectionTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::init()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif
 
    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();

    k->scene = scene;
    k->scene->clearSelection();
    k->nodeZValue = (2*ZLAYER_LIMIT) + (scene->scene()->layersCount() * ZLAYER_LIMIT);
    removeTarget();
    initItems(scene);
}

void SelectionTool::initItems(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::initItems()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    foreach (QGraphicsView *view, scene->views())
             view->setDragMode(QGraphicsView::RubberBandDrag);

    panel->enablePositionControls(false);
    removeTarget();
}

void SelectionTool::removeTarget()
{
    if (k->targetIsIncluded) {
        k->scene->removeItem(k->center);
        k->scene->removeItem(k->target1);
        k->scene->removeItem(k->target2);
        k->targetIsIncluded = false;
    }
}

QStringList SelectionTool::keys() const
{
    return QStringList() << tr("Selection");
}

void SelectionTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::press()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    Q_UNUSED(brushManager);

    k->selectionFlag = false;

    // If Control key is pressed / allow multiple selection picking items one by one 
    if (input->keyModifiers() != Qt::ControlModifier) {
        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 if (!nodeManager->isPress()) {
                     nodeManager->parentItem()->setSelected(false);
                     k->nodeManagers.removeAll(nodeManager);
                     scene->drawCurrentPhotogram();
                 } 
        }
        k->selectedObjects.clear();
    }

    if (scene->currentFrame()->indexOf(scene->mouseGrabberItem()) != -1) {
        k->selectedObjects << scene->mouseGrabberItem();
    } else {
        if (scene->selectedItems().count() > 0)
            k->selectedObjects = scene->selectedItems();
    }

    foreach (QGraphicsItem *item, k->selectedObjects) {
             if (item && (dynamic_cast<TupAbstractSerializable* > (item))) {
                 if (item->group() != 0)
                     item = qgraphicsitem_cast<QGraphicsItem *>(item->group());
                 bool found = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (item == nodeManager->parentItem()) {
                              found = true;
                              break;
                          }
                 }
            
                 if (!found) {
                     NodeManager *manager = new NodeManager(item, scene, k->nodeZValue);
                     manager->show();
                     manager->resizeNodes(k->realFactor);
                     k->nodeManagers << manager;
                 }
             } 
    }
}

void SelectionTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (input->buttons() == Qt::LeftButton && scene->selectedItems().count() > 0)
        QTimer::singleShot(0, this, SLOT(syncNodes()));
}

void SelectionTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::release()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    int position = -1;
    TupLibraryObject::Type type;

    k->selectedObjects = scene->selectedItems();

    if (k->selectedObjects.count() > 0) {
        k->selectionFlag = true;
        foreach (NodeManager *manager, k->nodeManagers) {
                 QGraphicsItem *item = manager->parentItem();
                 int parentIndex = k->selectedObjects.indexOf(item); 
                 if (parentIndex != -1) // Object is IN the list
                     k->selectedObjects.removeAt(parentIndex); // Remove node's item from selected objects list 
                 else // Object is NOT IN the selected objects list
                     delete k->nodeManagers.takeAt(k->nodeManagers.indexOf(manager)); // Removing node manager from nodes list 
        }

        foreach (QGraphicsItem *item, k->selectedObjects) {
                 if (item && (dynamic_cast<TupAbstractSerializable* > (item))) {
                     if (item->group() != 0)
                         item = qgraphicsitem_cast<QGraphicsItem *>(item->group());
                     bool found = false;
                     foreach (NodeManager *manager, k->nodeManagers) {
                              if (item == manager->parentItem()) {
                                  found = true;
                                  break;
                              }
                     }

                     if (!found) {
                         NodeManager *manager = new NodeManager(item, scene, k->nodeZValue);
                         manager->show();
                         manager->resizeNodes(k->realFactor);
                         k->nodeManagers << manager;
                     }
                 }
        }

        foreach (NodeManager *node, k->nodeManagers) {
                 if (node->isModified()) {
                     QGraphicsItem *item = node->parentItem();
                     QDomDocument doc;
                     doc.appendChild(TupSerializer::properties(item, doc));
                     TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);

                     if (svg) {
                         type = TupLibraryObject::Svg;
                         position = currentFrame()->indexOf(svg);
                     } else {
                         type = TupLibraryObject::Item;
                         position = currentFrame()->indexOf(node->parentItem());
                     }

                     // * SQA: What is the goal of this piece of code? It must be recovered because it is required by net architecture!
                     if (position >= 0) {
                         // Restore matrix
                         // node->restoreItem();

                         TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                    scene->currentSceneIndex(), 
                                    scene->currentLayerIndex(), 
                                    scene->currentFrameIndex(), position, QPointF(), 
                                    scene->spaceContext(), type,
                                    TupProjectRequest::Transform, doc.toString());

                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "SelectionTool::release() - Fatal Error: Invalid item position !!! [ " + QString::number(position) + " ]";
                             #ifdef Q_OS_WIN
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                     }
                 }
        }
        updateItemPosition();
    } else {
        panel->enablePositionControls(false);
        if (k->targetIsIncluded)
            k->targetIsIncluded = false;

        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 nodeManager->parentItem()->setSelected(false);
                 k->nodeManagers.removeAll(nodeManager);
        }

        scene->drawCurrentPhotogram();
    }
}

TupFrame* SelectionTool::currentFrame()
{
    TupFrame *frame = 0;
    if (k->scene->spaceContext() == TupProject::FRAMES_EDITION) {
        frame = k->scene->currentFrame();
    } else {
        TupScene *tupScene = k->scene->scene();
        TupBackground *bg = tupScene->background();
        if (k->scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
            frame = bg->staticFrame();
        } else if (k->scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                   frame = bg->dynamicFrame();
        }
    }

    return frame;
}

TupFrame* SelectionTool::frameAt(int sceneIndex, int layerIndex, int frameIndex)
{
    TupFrame *frame = 0;
    TupProject *project = k->scene->scene()->project();
    TupScene *scene = project->scene(sceneIndex);
    if (scene) {
        if (k->scene->spaceContext() == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerIndex);
            if (layer) {
                frame = layer->frame(frameIndex);
            } else {
                #ifdef K_DEBUG
                    QString msg = "SelectionTool::frameAt() - Fatal Error: Layer is NULL! -> " + QString::number(layerIndex);
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        } else {
            TupBackground *bg = scene->background();
            if (k->scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                frame = bg->staticFrame();
            } else if (k->scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                       frame = bg->dynamicFrame();
            }
       }
    } else {
       #ifdef K_DEBUG
           QString msg = "SelectionTool::frameAt() - Fatal Error: Scene is NULL! -> " + QString::number(sceneIndex);
           #ifdef Q_OS_WIN
               qDebug() << msg;
           #else
               tError() << msg;
           #endif
       #endif
    }

    return frame;
}

void SelectionTool::setupActions()
{
    k->targetIsIncluded = false;
    k->selectionFlag = false;
    k->scaleFactor = 1;
    k->realFactor = 1;

    TAction *select = new TAction(QPixmap(kAppProp->themeDir() + "icons/selection.png"), tr("Object Selection"), this);
    select->setShortcut(QKeySequence(tr("O")));
    select->setToolTip(tr("Object Selection") + " - " + "O");

    k->actions.insert(tr("Selection"), select);
}

QMap<QString, TAction *> SelectionTool::actions() const
{
    return k->actions;
}

int SelectionTool::toolType() const
{
    return TupToolInterface::Selection;
}

QWidget *SelectionTool::configurator() 
{
    if (!panel) {
        panel = new Settings;
        connect(panel, SIGNAL(callFlip(Settings::Flip)), this, SLOT(applyFlip(Settings::Flip)));
        connect(panel, SIGNAL(callOrderAction(Settings::Order)), this, SLOT(applyOrderAction(Settings::Order)));
        connect(panel, SIGNAL(callGroupAction(Settings::Group)), this, SLOT(applyGroupAction(Settings::Group)));
        connect(panel, SIGNAL(updateItemPosition(int, int)), this, SLOT(updateItemPosition(int, int)));
    }

    return panel;
}

void SelectionTool::aboutToChangeScene(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::aboutToChangeScene()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    init(scene);
}

void SelectionTool::aboutToChangeTool()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::aboutToChangeTool()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif
    */
}

void SelectionTool::itemResponse(const TupItemResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[SelectionTool::itemResponse()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    QGraphicsItem *item = 0;
    TupFrame *frame = frameAt(event->sceneIndex(), event->layerIndex(), event->frameIndex());
    if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
        item = frame->svg(event->itemIndex());
    } else if (frame->graphicItemsCount()>0) {
               item = frame->item(event->itemIndex());
    }

    updateItemPosition();

    switch (event->action()) {
            case TupProjectRequest::Transform:
            {
                 if (item) {
                     foreach (NodeManager* node, k->nodeManagers) {
                              node->show();
                              node->syncNodesFromParent();
                              node->beginToEdit();
                     }
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "SelectionTool::itemResponse - No item found";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                 }
            }
            break;
            case TupProjectRequest::Move:
            {
                 syncNodes();
            }
            break;
            case TupProjectRequest::Group:
            {
                 k->nodeManagers.clear();
                 k->selectedObjects.clear();

                 k->selectedObjects << item;
                 item->setSelected(true);
                 NodeManager *node = new NodeManager(item, k->scene, k->nodeZValue);
                 node->resizeNodes(k->realFactor);
                 k->nodeManagers << node;

                 syncNodes();
            }
            break;
            case TupProjectRequest::Ungroup:
            {
                 foreach (QGraphicsItem *graphic, k->scene->selectedItems())
                          graphic->setSelected(false);

                 k->nodeManagers.clear();
                 k->selectedObjects.clear();

                 QString list = event->arg().toString();
                 QString::const_iterator itr = list.constBegin();
                 QList<int> positions = TupSvg2Qt::parseIntList(++itr);
                 qSort(positions.begin(), positions.end());
                 int total = positions.size();
                 for (int i=0; i<total; i++) {
                      QGraphicsItem *graphic = frame->item(positions.at(i));     
                      if (graphic) {
                          k->selectedObjects << graphic;
                          graphic->setSelected(true);
                          NodeManager *node = new NodeManager(graphic, k->scene, k->nodeZValue);
                          node->resizeNodes(k->realFactor);
                          k->nodeManagers << node;
                      }
                 }

                 syncNodes();
            }
            break;
            case TupProjectRequest::Remove:
            {
                 // Do nothing
            }
            break;
            default:
            {
                 syncNodes();
            }
            break;
    }
}

void SelectionTool::syncNodes()
{
    foreach (NodeManager* node, k->nodeManagers) {
             if (node) {
                 node->show();
                 if (node->parentItem())
                     node->syncNodesFromParent();
             }
    }
}

void SelectionTool::saveConfig()
{
}

void SelectionTool::keyPressEvent(QKeyEvent *event)
{
    k->key = "NONE";

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else if (event->modifiers() == Qt::AltModifier) {
               if (event->key() == Qt::Key_R) {
                   if (selectionIsActive()) {
                       foreach (NodeManager *nodeManager, k->nodeManagers) {
                                nodeManager->toggleAction();
                                break;
                       }
                   }
               }
    } else if ((event->key() == Qt::Key_Left) || (event->key() == Qt::Key_Up) 
               || (event->key() == Qt::Key_Right) || (event->key() == Qt::Key_Down)) {

               if (!selectionIsActive()) {
                   QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
                   if (flags.first != -1 && flags.second != -1)
                       emit callForPlugin(flags.first, flags.second);
               } else {
                   int delta = 5;

                   if (event->modifiers()==Qt::ShiftModifier)
                       delta = 1;

                   if (event->modifiers()==Qt::ControlModifier)
                       delta = 10;

                   k->selectedObjects = k->scene->selectedItems();

                   foreach (QGraphicsItem *item, k->selectedObjects) {
                            if (event->key() == Qt::Key_Left)
                                item->moveBy(-delta, 0);

                            if (event->key() == Qt::Key_Up)
                                item->moveBy(0, -delta);

                            if (event->key() == Qt::Key_Right)
                                item->moveBy(delta, 0);

                            if (event->key() == Qt::Key_Down)
                                item->moveBy(0, delta);

                            QTimer::singleShot(0, this, SLOT(syncNodes()));
                   }

                   updateItemPosition();
               }
    } else if (event->modifiers() == Qt::ShiftModifier) {
               k->key = "SHIFT";
               if (selectionIsActive()) {
                   foreach (NodeManager *nodeManager, k->nodeManagers)
                            nodeManager->setProportion(true);
               }
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void SelectionTool::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);

    if (k->key.compare("SHIFT") == 0) {
        if (selectionIsActive()) {
            foreach (NodeManager *nodeManager, k->nodeManagers)
                     nodeManager->setProportion(false);
        }
    }
}

bool SelectionTool::selectionIsActive()
{
    return k->selectionFlag;
}

void SelectionTool::applyFlip(Settings::Flip flip)
{
    k->selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, k->selectedObjects) {
             QRectF rect = item->sceneBoundingRect();
             QPointF point = rect.topLeft();
             QMatrix m;
             m.translate(point.x(), point.y());

             if (flip == Settings::Horizontal)
                 m.scale(-1.0, 1.0);
             else if (flip == Settings::Vertical)
                      m.scale(1.0, -1.0);
             else if (flip == Settings::Crossed)
                      m.scale(-1.0, -1.0);

             m.translate(-point.x(), -point.y());
             item->setMatrix(m, true);
             rect = item->sceneBoundingRect();
             QPointF point2 = rect.topLeft();

             QPointF result = point - point2;
             item->moveBy(result.x(), result.y());

             foreach (NodeManager *node, k->nodeManagers) {
                      if (node->isModified()) {
                          QDomDocument doc;
                          doc.appendChild(TupSerializer::properties(item, doc));

                          TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(node->parentItem());
                          int position = -1;
                          TupLibraryObject::Type type = TupLibraryObject::Item;
                          if (svg) {
                              type = TupLibraryObject::Svg;
                              position = currentFrame()->indexOf(svg);
                          } else {
                              position = currentFrame()->indexOf(node->parentItem());
                          }

                          TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                                    k->scene->currentSceneIndex(),
                                                    k->scene->currentLayerIndex(),
                                                    k->scene->currentFrameIndex(), position, QPointF(), 
                                                    k->scene->spaceContext(), type,
                                                    TupProjectRequest::Transform, doc.toString());
                          emit requested(&event);
                      }
             }
    }
}

void SelectionTool::applyOrderAction(Settings::Order action)
{
    k->selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, k->selectedObjects) {
             TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
             int position = -1;
             TupLibraryObject::Type type = TupLibraryObject::Item;
             if (svg) {
                 type = TupLibraryObject::Svg;
                 position = currentFrame()->indexOf(svg);
             } else {
                 position = currentFrame()->indexOf(item);
             }

             TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                       k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), position, QPointF(),
                                       k->scene->spaceContext(), type, TupProjectRequest::Move, action);
             emit requested(&event);
    }
}

void SelectionTool::applyGroupAction(Settings::Group action)
{
    foreach (QGraphicsItem *item, k->selectedObjects) {
             TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
             if (svg) {
                 TOsd::self()->display(tr("Error"), tr("SVG objects can't be grouped/ungrouped yet"), TOsd::Error);
                 return;
             }
    }

    if (action == Settings::GroupItems) {
        k->selectedObjects = k->scene->selectedItems();
        int total = k->selectedObjects.count(); 
        if (total > 1) {
            QString items = "(";
            int i = 1;
            int position = -1; 

            foreach (QGraphicsItem *item, k->selectedObjects) {
                     int index = currentFrame()->indexOf(item);
                     if (index > -1) {
                         if (i == 1) {
                             position = index;
                         } else {
                             if (index < position) 
                                 position = index;
                         }
                         items += QString::number(index); 
                         if (i < total)
                             items += ", ";
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "SelectionTool::applyGroupAction() - Fatal Error: Index of item is invalid! -> -1";
                             #ifdef Q_OS_WIN
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                     }
                     i++;
            }
            items += ")";

            foreach (QGraphicsItem *item, k->selectedObjects)
                     item->setSelected(false);

            TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                      k->scene->currentLayerIndex(),
                                      k->scene->currentFrameIndex(), position, QPointF(), k->scene->spaceContext(),
                                      TupLibraryObject::Item, TupProjectRequest::Group, items);
            emit requested(&event);
        } else if (total == 1) {
                   k->nodeManagers.clear();
                   k->scene->drawCurrentPhotogram();
        }
    } else if (action == Settings::UngroupItems) {
               k->selectedObjects = k->scene->selectedItems();
               int total = k->selectedObjects.count();

               if (total > 0) {
                   bool noAction = true;
                   foreach (QGraphicsItem *item, k->selectedObjects) {
                            item->setSelected(false);
                            if (qgraphicsitem_cast<TupItemGroup *> (item)) {
                                noAction = false;
                                int itemIndex = currentFrame()->indexOf(item);

                                TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                                          k->scene->currentSceneIndex(),
                                                          k->scene->currentLayerIndex(),
                                                          k->scene->currentFrameIndex(),
                                                          itemIndex, QPointF(),
                                                          k->scene->spaceContext(), TupLibraryObject::Item,
                                                          TupProjectRequest::Ungroup);
                                emit requested(&event);
                            }
                   }

                   if (noAction) {
                       k->nodeManagers.clear();
                       k->scene->drawCurrentPhotogram();
                   }
               }
    }
}

QCursor SelectionTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

void SelectionTool::resizeNodes(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
    foreach (NodeManager *manager, k->nodeManagers)
             manager->resizeNodes(scaleFactor);
}

void SelectionTool::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}

void SelectionTool::sceneResponse(const TupSceneResponse *event)
{
    if (event->action() == TupProjectRequest::Select)
        initItems(k->scene);
}

void SelectionTool::updateItemPosition() 
{
    if (k->nodeManagers.count() == 1) {
        NodeManager *manager = k->nodeManagers.first();
        QGraphicsItem *item = manager->parentItem();
        QPoint point = item->mapToScene(item->boundingRect().center()).toPoint();
        panel->setPos(point.x(), point.y());
    } else { 
        if (k->nodeManagers.count() > 1) {
            NodeManager *manager = k->nodeManagers.first();
            QGraphicsItem *item = manager->parentItem();
            QPoint left = item->mapToScene(item->boundingRect().topLeft()).toPoint();  
            QPoint right = item->mapToScene(item->boundingRect().bottomRight()).toPoint();
            int minX = left.x();
            int maxX = right.x(); 
            int minY = left.y();
            int maxY = right.y();

            foreach (NodeManager *node, k->nodeManagers) {
                     QGraphicsItem *item = node->parentItem();
                     QPoint left = item->mapToScene(item->boundingRect().topLeft()).toPoint(); 
                     int leftX = left.x();
                     int leftY = left.y();
                     if (leftX < minX)
                         minX = leftX;
                     if (leftY < minY)
                         minY = leftY;
                     QPoint right = item->mapToScene(item->boundingRect().bottomRight()).toPoint();  
                     int rightX = right.x();
                     int rightY = right.y();
                     if (rightX > maxX)
                         maxX = rightX;
                     if (rightY > maxY)
                         maxY = rightY;
            }
            int x = minX + ((maxX - minX)/2); 
            int y = minY + ((maxY - minY)/2);
            panel->setPos(x, y);

            if (!k->targetIsIncluded) {
                k->center = new TupEllipseItem(QRectF(QPointF(x - 1, y - 1), QSize(2, 2)));
                k->target1 = new QGraphicsLineItem(x, y - 6, x, y + 6);
                k->target2 = new QGraphicsLineItem(x - 6, y, x + 6, y);
              
                QPen pen(QColor(255, 0, 0), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                k->center->setPen(pen);
                k->center->setBrush(QColor(255, 0, 0));
                k->center->setZValue(k->nodeZValue);
                k->scene->includeObject(k->center);

                k->target1->setPen(pen); 
                k->target1->setZValue(k->nodeZValue);
                k->target2->setPen(pen);
                k->target2->setZValue(k->nodeZValue);

                k->scene->includeObject(k->target1);
                k->scene->includeObject(k->target2);

                k->targetIsIncluded = true;
            } else {
                QPoint current = k->center->mapToScene(k->center->boundingRect().topLeft()).toPoint(); 
                int deltaX = x - current.x() - 1;
                int deltaY = y - current.y() - 1;
                k->center->moveBy(deltaX, deltaY);
                k->target1->moveBy(deltaX, deltaY);
                k->target2->moveBy(deltaX, deltaY);
            }
        }
    }
}

void SelectionTool::updateItemPosition(int x, int y) 
{
    if (k->nodeManagers.count() == 1) {
        NodeManager *manager = k->nodeManagers.first();
        QGraphicsItem *item = manager->parentItem();
        item->moveBy(x, y);
        manager->syncNodesFromParent();
    } else {
        if (k->nodeManagers.count() > 1) {
            foreach (NodeManager *node, k->nodeManagers) {
                     QGraphicsItem *item = node->parentItem();
                     item->moveBy(x, y);
                     node->syncNodesFromParent();
            }
            k->center->moveBy(x, y);
            k->target1->moveBy(x, y);
            k->target2->moveBy(x, y);
        }
    }
}
