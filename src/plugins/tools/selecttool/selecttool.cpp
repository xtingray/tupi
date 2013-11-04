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

#include "selecttool.h"
#include "taction.h"
#include "tdebug.h"
#include "tglobal.h"
#include "talgorithm.h"
#include "tupgraphicalgorithm.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupsvgitem.h"
#include "tupgraphicobject.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "nodemanager.h"
#include "tupserializer.h"
#include "tuprequestbuilder.h"
#include "tupprojectresponse.h"

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
    TupGraphicsScene *scene;
    bool selectionFlag;
    qreal scaleFactor;
    qreal realFactor;
};

SelectTool::SelectTool(): k(new Private), m_configurator(0)
{
    setupActions();
}

SelectTool::~SelectTool()
{
    delete k;
}

void SelectTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();
    k->scene = scene;
    k->scene->clearSelection();

    reset(scene);
}

void SelectTool::reset(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      // SQA: Temporary code for debugging issues
                      /*
                      QDomDocument dom;
                      dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));
                      QDomElement root = dom.documentElement();
                      tFatal() << "SelectTool::init() - XML: ";
                      tFatal() << dom.toString();
                      */

                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (item->zValue() >= 10000 && item->toolTip().length()==0) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                               } else {
                                   if (item->zValue() >= 10000) {
                                       item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                                   } else {
                                       item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                       item->setFlag(QGraphicsItem::ItemIsMovable, false);
                                   }
                               }
                          }
                      }
             }
    }
}

QStringList SelectTool::keys() const
{
    return QStringList() << tr("Select");
}

void SelectTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    k->selectionFlag = false;

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
             if (item && dynamic_cast<TupAbstractSerializable* > (item)) {
                 bool found = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (item == nodeManager->parentItem()) {
                              found = true;
                              break;
                          }
                 }
            
                 if (!found) {
                     NodeManager *manager = new NodeManager(item, scene);
                     manager->resizeNodes(k->realFactor);
                     k->nodeManagers << manager;
                 }
             }
    }
    
    k->scene = scene;
}

void SelectTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (input->buttons() == Qt::LeftButton && scene->selectedItems().count() > 0)
        QTimer::singleShot(0, this, SLOT(syncNodes()));
}

void SelectTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
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
                 if (item && dynamic_cast<TupAbstractSerializable* > (item)) {
                     NodeManager *manager = new NodeManager(item, scene);
                     manager->resizeNodes(k->realFactor);
                     k->nodeManagers << manager;
                 }
        }

        foreach (NodeManager *manager, k->nodeManagers) {
                 if (manager->isModified()) {
                     QDomDocument doc;
                     doc.appendChild(TupSerializer::properties(manager->parentItem(), doc));

                     QGraphicsItem *item = manager->parentItem();
                     TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);

                     int position = -1;
                     TupLibraryObject::Type type;

                     if (svg) {
                         if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(svg);
                         } else if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = k->scene->scene()->background();
                                    position = bg->staticFrame()->indexOf(svg); 
                         } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = k->scene->scene()->background();
                                    position = bg->dynamicFrame()->indexOf(svg);
                         }
                         type = TupLibraryObject::Svg;
                     } else {

                         if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(manager->parentItem());
                         } else if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = k->scene->scene()->background();
                                    position = bg->staticFrame()->indexOf(manager->parentItem());
                         } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = k->scene->scene()->background();
                                    position = bg->dynamicFrame()->indexOf(manager->parentItem());
                         }
                         type = TupLibraryObject::Item;
                     }

                     if (position != -1) {
                         // Restore matrix
                         foreach (QGraphicsView * view, k->scene->views())
                                  view->setUpdatesEnabled(false);

                         manager->restoreItem();

                         TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                    k->scene->currentSceneIndex(), 
                                    k->scene->currentLayerIndex(), 
                                    k->scene->currentFrameIndex(), position, QPointF(), 
                                    k->scene->spaceMode(), type,
                                    TupProjectRequest::Transform, doc.toString());
                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                                tError() << "SelectTool::release() - Fatal Error: Invalid position !!! (-1)"; 
                         #endif
                     }
                 }
        }
    } 
}

void SelectTool::setupActions()
{
    k->selectionFlag = false;
    k->scaleFactor = 1;
    k->realFactor = 1;

    TAction *select = new TAction(QPixmap(kAppProp->themeDir() + "icons/selection.png"), tr("Object Selection"), this);
    select->setShortcut(QKeySequence(tr("O")));

    k->actions.insert(tr("Select"), select);
}

QMap<QString, TAction *> SelectTool::actions() const
{
    return k->actions;
}

int SelectTool::toolType() const
{
    return TupToolInterface::Selection;
}

QWidget *SelectTool::configurator() 
{
    if (! m_configurator) {
        m_configurator = new InfoPanel;
        connect(m_configurator, SIGNAL(callFlip(InfoPanel::Flip)), this, SLOT(applyFlip(InfoPanel::Flip)));
    }

    return m_configurator;
}

void SelectTool::aboutToChangeScene(TupGraphicsScene *scene)
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

void SelectTool::itemResponse(const TupItemResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    QGraphicsItem *item = 0;
    TupScene *scene = 0;
    TupLayer *layer = 0;
    TupFrame *frame = 0;

    TupProject *project = k->scene->scene()->project();
    
    if (project) {

        scene = project->scene(event->sceneIndex());

        if (scene) {

            if (project->spaceContext() == TupProject::FRAMES_EDITION) {

                layer = scene->layer(event->layerIndex());

                if (layer) {
                    frame = layer->frame(event->frameIndex());
                    if (frame) {
                        if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
                            item = frame->svg(event->itemIndex());
                        } else if (frame->graphicItemsCount()>0) {
                                   item = frame->item(event->itemIndex());
                        }
                    } else {
                        return;
                    }
                }
            } else if (project->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                       TupBackground *bg = scene->background();
                       if (bg) {
                           TupFrame *frame = bg->staticFrame();
                           if (frame) {
                               if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
                                   item = frame->svg(event->itemIndex());
                               } else if (frame->graphicItemsCount()>0) {
                                          item = frame->item(event->itemIndex());
                               }
                           }
                       }
            } else if (project->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                       TupBackground *bg = scene->background();
                       if (bg) {
                           TupFrame *frame = bg->dynamicFrame();
                           if (frame) {
                               if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
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
               tError() << "SelectTool::itemResponse - Project does not exist";
        #endif
        return;
    }

    switch (event->action()) {

            case TupProjectRequest::Transform:
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
                            tError() << "SelectTool::itemResponse - No item found";
                     #endif
                 }
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
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
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

               if (!k->selectionFlag) {
                   QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
                   if (flags.first != -1 && flags.second != -1)
                       emit callForPlugin(flags.first, flags.second);
               } else {
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
               }
    } else if (event->modifiers() == Qt::ShiftModifier) {
               verifyActiveSelection();
               foreach (NodeManager *nodeManager, k->nodeManagers)
                        nodeManager->setProportion(true);
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
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

void SelectTool::updateItems(TupGraphicsScene *scene)
{
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
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
                          doc.appendChild(TupSerializer::properties(manager->parentItem(), doc));

                          TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(manager->parentItem());
                          int position = -1;
                          TupLibraryObject::Type type;

                          if (svg) {
                              position  = k->scene->currentFrame()->indexOf(svg);
                              type = TupLibraryObject::Svg;
                          } else {
                              position  = k->scene->currentFrame()->indexOf(manager->parentItem());
                              type = TupLibraryObject::Item;
                          }

                          foreach (QGraphicsView *view, k->scene->views())
                                   view->setUpdatesEnabled(false);

                          manager->restoreItem();

                          TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                                   k->scene->currentSceneIndex(),
                                                   k->scene->currentLayerIndex(),
                                                   k->scene->currentFrameIndex(), position, QPointF(), 
                                                   k->scene->spaceMode(), type,
                                                   TupProjectRequest::Transform, doc.toString());
                          emit requested(&event);
                      }
             }
    }
}

QCursor SelectTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

void SelectTool::resizeNodes(qreal scaleFactor)
{
    k->scaleFactor *= scaleFactor;
    updateRealZoomFactor();

    foreach (NodeManager *manager, k->nodeManagers)
             manager->resizeNodes(k->realFactor);
}

void SelectTool::updateZoomFactor(qreal globalFactor)
{
    k->scaleFactor = globalFactor;
    updateRealZoomFactor();
}

void SelectTool::updateRealZoomFactor()
{
    if (k->scaleFactor <= 1)
        k->realFactor = 1;
    else if (k->scaleFactor > 1 && k->scaleFactor < 1.5)
             k->realFactor = 0.8;
    else if (k->scaleFactor >= 1.5 && k->scaleFactor < 2)
             k->realFactor = 0.6;
    else if (k->scaleFactor >= 2 && k->scaleFactor <= 3)
             k->realFactor = 0.4;
    else if (k->scaleFactor > 3)
             k->realFactor = 0.3;
    else if (k->scaleFactor > 4)
             k->realFactor = 0.2;
}

void SelectTool::sceneResponse(const TupSceneResponse *event)
{
    if (event->action() == TupProjectRequest::Select)
        reset(k->scene);
}

Q_EXPORT_PLUGIN2(tup_select, SelectTool);
