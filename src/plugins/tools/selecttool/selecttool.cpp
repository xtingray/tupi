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
#include "tupellipseitem.h"
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
    QList<QGraphicsItem *> selectedObjects;
    QList<NodeManager*> nodeManagers;
    TupGraphicsScene *scene;
    bool selectionFlag;
    qreal scaleFactor;
    qreal realFactor;
    int baseZValue;
    TupEllipseItem *center;
    bool targetIsIncluded;
};

SelectTool::SelectTool(): k(new Private), panel(0)
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
    k->baseZValue = 20000 + (scene->scene()->layersTotal() * 10000);
    k->targetIsIncluded = false;

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
                              if (item->zValue() >= 20000 && item->toolTip().length()==0) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                         if (item->zValue() >= 10000) {
                                             item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                                         } else {
                                             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                             item->setFlag(QGraphicsItem::ItemIsMovable, false);
                                         }
                              } else {
                                  #ifdef K_DEBUG
                                         tError() << "SelectTool::reset() - Fatal Error: Invalid spaceMode!";
                                  #endif
                                  return;
                              }
                          }
                      }
             }
    }

    panel->enablePositionControls(false);
    if (k->targetIsIncluded) {
        k->scene->removeItem(k->center);
        k->targetIsIncluded = false;
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
                     scene->drawCurrentPhotogram();
                 } 
        }
    } 
    
    k->selectedObjects = scene->selectedItems();
    
    if (scene->currentFrame()->indexOf(scene->mouseGrabberItem()) != -1)
        k->selectedObjects << scene->mouseGrabberItem();

    foreach (QGraphicsItem *item, k->selectedObjects) {
             if (item && dynamic_cast<TupAbstractSerializable* > (item)) {
                 bool found = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (item == nodeManager->parentItem()) {
                              found = true;
                              break;
                          }
                 }
            
                 if (!found) {
                     NodeManager *node = new NodeManager(item, scene, k->baseZValue);
                     node->resizeNodes(k->realFactor);
                     k->nodeManagers << node;
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
        k->selectedObjects = k->scene->selectedItems();
        QList<NodeManager *>::iterator it = k->nodeManagers.begin();
        QList<NodeManager *>::iterator itEnd = k->nodeManagers.end();

        while (it != itEnd) {
               int parentIndex = k->scene->selectedItems().indexOf((*it)->parentItem());
               if (parentIndex != -1)
                   k->selectedObjects.removeAt(parentIndex);
               else
                   delete k->nodeManagers.takeAt(k->nodeManagers.indexOf((*it)));

               ++it;
        }

        foreach (QGraphicsItem *item, k->selectedObjects) {
                 if (item && dynamic_cast<TupAbstractSerializable* > (item)) {
                     NodeManager *node = new NodeManager(item, scene, k->baseZValue);
                     node->resizeNodes(k->realFactor);
                     k->nodeManagers << node;
                 }
        }

        foreach (NodeManager *node, k->nodeManagers) {
                 if (node->isModified()) {
                     QGraphicsItem *item = node->parentItem();
                     QDomDocument doc;
                     doc.appendChild(TupSerializer::properties(item, doc));
                     int position = -1;
                     TupLibraryObject::Type type;
                     TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);

                     if (svg) {
                         type = TupLibraryObject::Svg;
                         if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(svg);
                         } else {
                             TupBackground *bg = k->scene->scene()->background();
                             if (bg) {
                                 if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                     position = bg->staticFrame()->indexOf(svg);
                                 } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                     position = bg->dynamicFrame()->indexOf(svg);
                                 } else {
                                     #ifdef K_DEBUG
                                            tError() << "SelectTool::release() - Fatal Error: Invalid spaceMode!";
                                     #endif
                                     return;
                                 }
                             } else {
                                 #ifdef K_DEBUG
                                        tError() << "SelectTool::release() - Fatal Error: Scene background object is NULL!";
                                 #endif
                                 return;
                             }
                         }
                     } else {
                         type = TupLibraryObject::Item;

                         if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = k->scene->currentFrame()->indexOf(node->parentItem());
                         } else {
                             TupBackground *bg = k->scene->scene()->background();
                             if (bg) {
                                 if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                     position = bg->staticFrame()->indexOf(node->parentItem());
                                 } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                            position = bg->dynamicFrame()->indexOf(node->parentItem());
                                 } else {
                                     #ifdef K_DEBUG
                                            tError() << "SelectTool::release() - Fatal Error: Invalid spaceMode!";
                                     #endif
                                     return;
                                 }
                             } else {
                                 #ifdef K_DEBUG
                                        tError() << "SelectTool::release() - Fatal Error: Scene background object is NULL!";
                                 #endif
                                 return;
                             }
                         }
                     }

                     if (position >= 0) {
                         // Restore matrix
                         // node->restoreItem();

                         TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                    k->scene->currentSceneIndex(), 
                                    k->scene->currentLayerIndex(), 
                                    k->scene->currentFrameIndex(), position, QPointF(), 
                                    k->scene->spaceMode(), type,
                                    TupProjectRequest::Transform, doc.toString());

                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                                tError() << "SelectTool::release() - Fatal Error: Invalid item position !!! [ " << position << " ]"; 
                         #endif
                     }
                 }
        }

        updateItemPosition();
    } else {
        panel->enablePositionControls(false);
        if (k->targetIsIncluded) {
            k->scene->removeItem(k->center);
            k->targetIsIncluded = false;
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
    if (!panel) {
        panel = new Settings;
        connect(panel, SIGNAL(callFlip(Settings::Flip)), this, SLOT(applyFlip(Settings::Flip)));
        connect(panel, SIGNAL(callOrderAction(Settings::Order)), this, SLOT(applyOrderAction(Settings::Order)));
        connect(panel, SIGNAL(updateItemPosition(int, int)), this, SLOT(updateItemPosition(int, int)));
    }

    return panel;
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
                        #ifdef K_DEBUG
                               tError() << "SelectTool::itemResponse - Fatal Error: frame is NULL"; 
                        #endif
                        return;
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "SelectTool::itemResponse - Fatal Error: layer is NULL";
                    #endif
                    return;
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
                           } else {
                               #ifdef K_DEBUG
                                      tError() << "SelectTool::itemResponse - Fatal Error: Static bg frame is NULL";
                               #endif
                               return;
                           }
                       } else {
                           #ifdef K_DEBUG
                                  tError() << "SelectTool::itemResponse - Fatal Error: Scene background is NULL";
                           #endif
                           return;
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
                           } else {
                               #ifdef K_DEBUG
                                      tError() << "SelectTool::itemResponse - Fatal Error: Dynamic bg frame is NULL";
                               #endif
                               return;
                           }
                       } else {
                           #ifdef K_DEBUG
                                  tError() << "SelectTool::itemResponse - Fatal Error: Scene background is NULL";
                           #endif
                           return;
                       }
            } else {
                #ifdef K_DEBUG
                       tError() << "SelectTool::itemResponse - Fatal Error: Invalid spaceMode!";
                #endif
                return;
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "SelectTool::itemResponse - Fatal Error: Current scene is NULL!";
            #endif
            return;
        }
    } else {
        #ifdef K_DEBUG
               tError() << "SelectTool::itemResponse - Project does not exist";
        #endif
        return;
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
                            tError() << "SelectTool::itemResponse - No item found";
                     #endif
                 }
            }
            break;
            case TupProjectRequest::Move:
            {
                 k->nodeManagers.clear();

                 foreach (QGraphicsItem *item, k->selectedObjects) {
                          if (item && dynamic_cast<TupAbstractSerializable* > (item)) {
                              NodeManager *node = new NodeManager(item, k->scene, k->baseZValue);
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

void SelectTool::syncNodes()
{
    foreach (NodeManager* node, k->nodeManagers) {
             if (node) {
                 node->show();
                 if (node->parentItem()) {
                     // node->parentItem()->setSelected(true);
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

// SQA: Check if reset() and updateItems can be defined as the same method 
void SelectTool::updateItems(TupGraphicsScene *scene)
{
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (item->zValue() >= 20000) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                         if (item->zValue() >= 10000) {
                                             item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                                         } else {
                                             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                             item->setFlag(QGraphicsItem::ItemIsMovable, false);
                                         }
                              } else {
                                  #ifdef K_DEBUG
                                         tError() << "SelectTool::updateItems() - Fatal Error: Invalid spaceMode!";
                                  #endif
                              }
                          }
                      }
             }
    }
}

void SelectTool::applyFlip(Settings::Flip flip)
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
                          TupLibraryObject::Type type = TupLibraryObject::Item;;
                          if (svg)
                              type = TupLibraryObject::Svg;

                          if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (svg) 
                                  position = k->scene->currentFrame()->indexOf(svg);
                              else 
                                  position = k->scene->currentFrame()->indexOf(node->parentItem());
                          } else {
                              TupBackground *bg = k->scene->scene()->background();
                              if (bg) {
                                  if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                      if (svg)
                                          position = bg->staticFrame()->indexOf(svg);
                                      else
                                          position = bg->staticFrame()->indexOf(node->parentItem());
                                  } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                             if (svg)
                                                 position = bg->dynamicFrame()->indexOf(svg);
                                             else
                                                 position = bg->dynamicFrame()->indexOf(node->parentItem());
                                  } else {
                                      #ifdef K_DEBUG
                                             tError() << "SelectTool::applyFlip() - Fatal Error: invalid spaceMode!";
                                      #endif
                                      return;
                                  }
                              } else {
                                  #ifdef K_DEBUG
                                         tError() << "SelectTool::applyFlip() - Fatal Error: Scene background object is NULL!";
                                  #endif
                                  return;
                              }
                          }

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

void SelectTool::applyOrderAction(Settings::Order action)
{
    k->selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, k->selectedObjects) {

             TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
             int position = -1;
             TupLibraryObject::Type type = TupLibraryObject::Item;
             if (svg)
                 type = TupLibraryObject::Svg;

             if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                 if (svg)
                      position = k->scene->currentFrame()->indexOf(svg);
                 else
                     position = k->scene->currentFrame()->indexOf(item);
             } else {
                 TupBackground *bg = k->scene->scene()->background();
                 if (bg) {
                     if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                         if (svg)
                             position = bg->staticFrame()->indexOf(svg);
                         else
                             position = bg->staticFrame()->indexOf(item);
                     } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                if (svg)
                                    position = bg->dynamicFrame()->indexOf(svg);
                                else
                                    position = bg->dynamicFrame()->indexOf(item);
                     } else {
                         #ifdef K_DEBUG
                                tError() << "SelectTool::applyOrderAction() - Fatal Error: invalid spaceMode!";
                         #endif
                         return;
                     }
                 } else {
                     #ifdef K_DEBUG
                            tError() << "SelectTool::applyOrderAction() - Fatal Error: Scene background object is NULL!";
                     #endif
                     return;
                 }
             }

             TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                       k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), position, QPointF(),
                                       k->scene->spaceMode(), type, TupProjectRequest::Move, action);
             emit requested(&event);
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

    foreach (NodeManager *node, k->nodeManagers)
             node->resizeNodes(k->realFactor);
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

void SelectTool::updateItemPosition() {
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
              k->center = new TupEllipseItem(QRectF(QPointF(x, y), QSize(8, 8)));
              QPen pen(QColor(255, 0, 0), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
              k->center->setPen(pen);
              k->center->setBrush(QColor(255, 0, 0));
              k->center->setZValue(k->baseZValue + 1);
              k->scene->includeObject(k->center);

              k->targetIsIncluded = true;
          } else {
              QPoint current = k->center->mapToScene(k->center->boundingRect().topLeft()).toPoint(); 
              int deltaX = x - current.x();
              int deltaY = y - current.y();
              k->center->moveBy(deltaX, deltaY);
          }
      }
    }
}

void SelectTool::updateItemPosition(int x, int y) {
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
        }
    }
}

Q_EXPORT_PLUGIN2(tup_select, SelectTool);
