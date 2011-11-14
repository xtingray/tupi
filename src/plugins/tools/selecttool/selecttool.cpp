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

#include "selecttool.h"
#include "taction.h"
#include "tdebug.h"
#include "tglobal.h"
#include "talgorithm.h"
#include "ktgraphicalgorithm.h"
#include "ktscene.h"
#include "ktlayer.h"
#include "ktsvgitem.h"
#include "ktgraphicobject.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "nodemanager.h"
#include "ktserializer.h"
#include "ktrequestbuilder.h"
#include "ktprojectresponse.h"

#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <cmath>

struct SelectTool::Private
{
    QMap<QString, TAction *> actions;
    QList<NodeManager*> nodeManagers;
    KTGraphicsScene *scene;
    bool selectionFlag;
};

SelectTool::SelectTool(): k(new Private), m_configurator(0)
{
    setupActions();
}

SelectTool::~SelectTool()
{
    delete k;
}

void SelectTool::init(KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();
    k->scene = scene;

    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {

                      // SQA: Temporary code for debug issues
                      /*
                      QDomDocument dom;
                      dom.appendChild(dynamic_cast<KTAbstractSerializable *>(item)->toXml(dom));
                      QDomElement root = dom.documentElement();
                      tFatal() << "SelectTool::init() - XML: ";
                      tFatal() << dom.toString();
                      */

                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == KTProject::FRAMES_EDITION) {
                              if (item->zValue() >= 10000 && item->toolTip().length()==0) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                          }
                      }
             }
    }
}

QStringList SelectTool::keys() const
{
    return QStringList() << tr("Select");
}

void SelectTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    // If Control key is pressed / allow multiple selection 
    if (input->keyModifiers() != Qt::ControlModifier) {
        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 if (!nodeManager->isPress()) {
                     nodeManager->parentItem()->setSelected(false);
                     k->nodeManagers.removeAll(nodeManager);
                     // delete nodeManager;
                     scene->drawCurrentPhotogram();
                 } 
        }
    } 
    
    QList<QGraphicsItem *> selectedObjects = scene->selectedItems();
    
    if (scene->currentFrame()->indexOf(scene->mouseGrabberItem()) != -1)
        selectedObjects << scene->mouseGrabberItem();

    foreach (QGraphicsItem *item, selectedObjects) {
             if (item && dynamic_cast<KTAbstractSerializable* > (item)) {
                 bool found = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (item == nodeManager->parentItem()) {
                              found = true;
                              break;
                          }
                 }
            
                 if (!found) {
                     NodeManager *manager = new NodeManager(item, scene);
                     k->nodeManagers << manager;
                 }
             }
    }
    
    k->scene = scene;
}

void SelectTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (input->buttons() == Qt::LeftButton && scene->selectedItems().count() > 0)
        QTimer::singleShot(0, this, SLOT(syncNodes()));
}

void SelectTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (k->scene->selectedItems().count() > 0) {

        k->selectionFlag = true;

        QList<QGraphicsItem *> selectedObjects = k->scene->selectedItems();
        QList<NodeManager *>::iterator it = k->nodeManagers.begin();
        QList<NodeManager *>::iterator itEnd = k->nodeManagers.end();

        while (it != itEnd) {
               int parentIndex = k->scene->selectedItems().indexOf((*it)->parentItem());
            
               if (parentIndex != -1)
                   selectedObjects.removeAt(parentIndex);
               else
                   delete k->nodeManagers.takeAt(k->nodeManagers.indexOf((*it)));

               ++it;
        }
        
        foreach (QGraphicsItem *item, selectedObjects) {
                 if (item && dynamic_cast<KTAbstractSerializable* > (item)) {
                     NodeManager *manager = new NodeManager(item, scene);
                     k->nodeManagers << manager;
                 }
        }

        foreach (NodeManager *manager, k->nodeManagers) {
                 if (manager->isModified()) {
                     QDomDocument doc;
                     doc.appendChild(KTSerializer::properties(manager->parentItem(), doc));

                     QGraphicsItem *item = manager->parentItem();
                     KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item);

                     int position = -1;
                     KTLibraryObject::Type type;

                     if (svg) {

                         if (k->scene->spaceMode() == KTProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(svg);
                         } else if (k->scene->spaceMode() == KTProject::BACKGROUND_EDITION) {
                                    KTBackground *bg = k->scene->scene()->background();
                                    position = bg->frame()->indexOf(svg); 
                         }
                         type = KTLibraryObject::Svg;

                     } else {

                         if (k->scene->spaceMode() == KTProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(manager->parentItem());
                         } else if (k->scene->spaceMode() == KTProject::BACKGROUND_EDITION) {
                                    KTBackground *bg = k->scene->scene()->background();
                                    position = bg->frame()->indexOf(manager->parentItem());
                         }
                         type = KTLibraryObject::Item;
                     }

                     if (position != -1) {
                         // Restore matrix
                         foreach (QGraphicsView * view, k->scene->views())
                                  view->setUpdatesEnabled(false);

                         manager->restoreItem();

                         KTProjectRequest event = KTRequestBuilder::createItemRequest( 
                                    k->scene->currentSceneIndex(), 
                                    k->scene->currentLayerIndex(), 
                                    k->scene->currentFrameIndex(), position, QPointF(), 
                                    k->scene->spaceMode(), type,
                                    KTProjectRequest::Transform, doc.toString());
                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                                tFatal() << "SelectTool::release() - position is " << position; 
                         #endif
                     }
                 }
        }
    } 
}

void SelectTool::setupActions()
{
    TAction *select = new TAction(QPixmap(THEME_DIR + "icons/selection.png"), tr("Object Selection"), this);
    select->setShortcut(QKeySequence(tr("O")));

    k->actions.insert(tr("Select"), select);
}

QMap<QString, TAction *> SelectTool::actions() const
{
    return k->actions;
}

int SelectTool::toolType() const
{
    return KTToolInterface::Selection;
}

QWidget *SelectTool::configurator() 
{
    if (! m_configurator) {
        m_configurator = new InfoPanel;
        connect(m_configurator, SIGNAL(callFlip(InfoPanel::Flip)), this, SLOT(applyFlip(InfoPanel::Flip)));
    }

    return m_configurator;
}

void SelectTool::aboutToChangeScene(KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    init(scene);
}

void SelectTool::aboutToChangeTool()
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();

    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

void SelectTool::itemResponse(const KTItemResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    QGraphicsItem *item = 0;
    KTScene *scene = 0;
    KTLayer *layer = 0;
    KTFrame *frame = 0;

    KTProject *project = k->scene->scene()->project();
    
    if (project) {

        scene = project->scene(event->sceneIndex());

        if (scene) {

            if (project->spaceContext() == KTProject::FRAMES_EDITION) {

                layer = scene->layer(event->layerIndex());

                if (layer) {
                    frame = layer->frame(event->frameIndex());
                    if (frame) {
                        if (event->itemType() == KTLibraryObject::Svg && frame->svgItemsCount()>0) {
                            item = frame->svg(event->itemIndex());
                        } else if (frame->graphicItemsCount()>0) {
                                   item = frame->item(event->itemIndex());
                        }
                    } else {
                        return;
                    }
                }
            } else if (project->spaceContext() == KTProject::BACKGROUND_EDITION) {
                       KTBackground *bg = scene->background();
                       if (bg) {
                           KTFrame *frame = bg->frame();
                           if (frame) {
                               if (event->itemType() == KTLibraryObject::Svg && frame->svgItemsCount()>0) {
                                   item = frame->svg(event->itemIndex());
                               } else if (frame->graphicItemsCount()>0) {
                                          item = frame->item(event->itemIndex());
                               }
                           }
                       }
            }
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "SelectTool::itemResponse - Project does not exist";
        #endif
        return;
    }

    switch (event->action()) {

            case KTProjectRequest::Transform:
            {
                 if (item) {

                     foreach (QGraphicsView *view, k->scene->views())
                              view->setUpdatesEnabled(true);

                     foreach (NodeManager* node, k->nodeManagers) {
                              node->show();
                              node->syncNodesFromParent();
                              node->beginToEdit();
                              break;
                     }

                 } else {
                     #ifdef K_DEBUG
                            tFatal() << "SelectTool::itemResponse - No item found";
                     #endif
                 }
            }
            break;
            case KTProjectRequest::Remove:
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

void SelectTool::syncNodes()
{
    foreach (NodeManager* node, k->nodeManagers) {
             if (node) {
                 node->show();
                 if (node->parentItem()) {
                     node->parentItem()->setSelected(true);
                     node->syncNodesFromParent();
                 }
             }
    }
}

void SelectTool::saveConfig()
{
}

void SelectTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else if (event->modifiers() == Qt::AltModifier) {
               if (event->key() == Qt::Key_R) {
                   verifyActiveSelection();
                   foreach (NodeManager *nodeManager, k->nodeManagers) {
                            nodeManager->toggleAction();
                            break;
                   }
               }
    } else if ((event->key() == Qt::Key_Left) || (event->key() == Qt::Key_Up) 
               || (event->key() == Qt::Key_Right) || (event->key() == Qt::Key_Down)) {

               int delta = 5;

               if (event->modifiers()==Qt::ShiftModifier)
                   delta = 1;

               if (event->modifiers()==Qt::ControlModifier)
                   delta = 10;

               QList<QGraphicsItem *> selectedObjects = k->scene->selectedItems();

               foreach (QGraphicsItem *item, selectedObjects) {
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
    } else if (event->modifiers() == Qt::ShiftModifier) {
               verifyActiveSelection();
               foreach (NodeManager *nodeManager, k->nodeManagers)
                        nodeManager->setProportion(true);
    } else if (event->modifiers() != Qt::ShiftModifier && event->modifiers() != Qt::ControlModifier) {
               QPair<int, int> flags = KTToolPlugin::setKeyAction(event->key());
               if (flags.first != -1 && flags.second != -1)
                   emit callForPlugin(flags.first, flags.second);
    }
}

void SelectTool::verifyActiveSelection()
{
    if (k->scene) {
        if (!k->selectionFlag)
            return;
    } else {
        return;
    }
}

void SelectTool::updateItems(KTGraphicsScene *scene)
{
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == KTProject::FRAMES_EDITION) {
                              if (item->zValue() >= 10000) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                          }
                      }
             }
    }
}

void SelectTool::applyFlip(InfoPanel::Flip flip)
{
    QList<QGraphicsItem *> selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, selectedObjects) {

             QRectF rect = item->sceneBoundingRect();
             QPointF point =  rect.topLeft();

             QMatrix m;
             m.translate(point.x(), point.y());

             if (flip == InfoPanel::Horizontal)
                 m.scale(-1.0, 1.0);
             else if (flip == InfoPanel::Vertical)
                      m.scale(1.0, -1.0);
             else if (flip == InfoPanel::Crossed)
                      m.scale(-1.0, -1.0);

             m.translate(-point.x(), -point.y());
             item->setMatrix(m, true);
             rect = item->sceneBoundingRect();
             QPointF point2 =  rect.topLeft();

             QPointF result = point - point2;
             item->moveBy(result.x(), result.y());

             foreach (NodeManager *manager, k->nodeManagers) {
                      if (manager->isModified()) {

                          QDomDocument doc;
                          doc.appendChild(KTSerializer::properties(manager->parentItem(), doc));

                          KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(manager->parentItem());
                          int position = -1;
                          KTLibraryObject::Type type;

                          if (svg) {
                              position  = k->scene->currentFrame()->indexOf(svg);
                              type = KTLibraryObject::Svg;
                          } else {
                              position  = k->scene->currentFrame()->indexOf(manager->parentItem());
                              type = KTLibraryObject::Item;
                          }

                          foreach (QGraphicsView *view, k->scene->views())
                                   view->setUpdatesEnabled(false);

                          manager->restoreItem();

                          KTProjectRequest event = KTRequestBuilder::createItemRequest(
                                                   k->scene->currentSceneIndex(),
                                                   k->scene->currentLayerIndex(),
                                                   k->scene->currentFrameIndex(), position, QPointF(), 
                                                   k->scene->spaceMode(), type,
                                                   KTProjectRequest::Transform, doc.toString());
                          emit requested(&event);
                      }
             }
    }
}

QCursor SelectTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

Q_EXPORT_PLUGIN2(kt_select, SelectTool);
