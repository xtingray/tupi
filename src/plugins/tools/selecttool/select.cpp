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

#include "select.h"

#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QGraphicsView>
#include <cmath>

#include "kaction.h"
#include "kdebug.h"
#include "kglobal.h"
#include "kalgorithm.h"

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

#include <QTimer>

struct Select::Private
{
    QMap<QString, KAction *> actions;
    QList<NodeManager*> nodeManagers;
    KTGraphicsScene *scene;
    NodeManager* changedManager;
    bool selectionFlag;
};

Select::Select(): k(new Private),  m_configurator(0)
{
    k->changedManager = 0;
    setupActions();
}

Select::~Select()
{
    delete k;
}

void Select::init(KTGraphicsScene *scene)
{
    K_FUNCINFOX("tools");

    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();
    k->changedManager = 0;

    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {

                      /*
                      QDomDocument dom;
                      dom.appendChild(dynamic_cast<KTAbstractSerializable *>(item)->toXml(dom));
                      QDomElement root = dom.documentElement();
                      kFatal() << "Select::init() - XML: ";
                      kFatal() << dom.toString();
                      */

                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == KTProject::FRAMES_EDITION) {
                              kFatal() << "Select::init() - zValue: " << item->zValue();
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

QStringList Select::keys() const
{
    return QStringList() << tr("Select");
}

void Select::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    // K_FUNCINFOX("tools");
    Q_UNUSED(brushManager);

    if (k->changedManager)
        k->changedManager = 0;
   
    // SQA: Why this condition is required? What is it with the Control key? 
    // Workspace rotation mode?
    if (input->keyModifiers() != Qt::ControlModifier) {
        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 if (scene->mouseGrabberItem() == nodeManager->parentItem()) {
                     nodeManager->toggleAction();
                     k->changedManager = nodeManager;
                     break;
                 } else if (!nodeManager->isPress()) {
                            nodeManager->parentItem()->setSelected(false);
                            k->nodeManagers.removeAll(nodeManager);
                            delete nodeManager;
                 }
        }
    } 
    
    QList<QGraphicsItem *> selectedObjects = scene->selectedItems();
    
    if (scene->currentFrame()->indexOf(scene->mouseGrabberItem()) != -1)
        selectedObjects << scene->mouseGrabberItem();

    foreach (QGraphicsItem *item, selectedObjects) {
             if (item && dynamic_cast<KTAbstractSerializable* > (item)) {
                 bool finded = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (nodeManager->parentItem() == nodeManager->parentItem()) {
                              finded = true;
                              break;
                          }
                 }
            
                 if (!finded) {
                     NodeManager *manager = new NodeManager(item, scene);
                     k->nodeManagers << manager;
                 }
             }
    }
    
    k->scene = scene;
}

void Select::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    // Q_UNUSED(input);
    Q_UNUSED(brushManager);
    // Q_UNUSED(scene);

    if (k->changedManager) {
        k->changedManager->toggleAction();
        k->changedManager = 0;
    }
    
    if (input->buttons() == Qt::LeftButton && scene->selectedItems().count() > 0)
        QTimer::singleShot(0, this, SLOT(syncNodes()));
}

void Select::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    // K_FUNCINFOX("tools") << scene->selectedItems().count();

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (k->scene->selectedItems().count() > 0) {

        k->selectionFlag = true;

        QList<QGraphicsItem *> selectedObjects = k->scene->selectedItems();
        QList<NodeManager *>::iterator it = k->nodeManagers.begin();
        QList<NodeManager *>::iterator itEnd = k->nodeManagers.end();

        while (it != itEnd) {
               int parentIndex = k->scene->selectedItems().indexOf((*it)->parentItem());
            
               if (parentIndex != -1 )
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
                     int position = -1;
                     QDomDocument doc;
                     doc.appendChild(KTSerializer::properties(manager->parentItem(), doc));

                     QGraphicsItem *item = manager->parentItem();
                     KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item);

                     KTLibraryObject::Type type;

                     if (svg) {
                         position  = k->scene->currentFrame()->indexOf(svg);
                         type = KTLibraryObject::Svg;
                     } else {
                         position  = k->scene->currentFrame()->indexOf(manager->parentItem());
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
                                    k->scene->currentFrameIndex(), position, QPointF(), type,
                                    KTProjectRequest::Transform, doc.toString());
                         emit requested(&event);
                     } else {
                         kDebug("selection") << "position is " << position; 
                     }
                 }
        }
    } 
}

void Select::setupActions()
{
    KAction *select = new KAction(QPixmap(THEME_DIR + "icons/selection.png"), tr("Object Selection"), this);
    select->setShortcut(QKeySequence(tr("O")));

    k->actions.insert(tr("Select"), select);
}

QMap<QString, KAction *> Select::actions() const
{
    return k->actions;
}

int Select::toolType() const
{
    return KTToolInterface::Selection;
}

QWidget *Select::configurator() 
{
    if (! m_configurator)
        m_configurator = new InfoPanel;

    return m_configurator;
}

void Select::aboutToChangeScene(KTGraphicsScene *scene)
{
    K_FUNCINFOX("tools");
    init(scene);
}

void Select::aboutToChangeTool()
{
    K_FUNCINFOX("tools");
    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();
}

void Select::itemResponse(const KTItemResponse *event)
{
    K_FUNCINFOX("tools");

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
            } else {
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
               kFatal() << "Select::itemResponse - Project does not exist";
        #endif
        return;
    }
    
    switch (event->action()) {

            case KTProjectRequest::Transform:
            {
                 if (item) {

                     foreach (QGraphicsView * view, k->scene->views())
                              view->setUpdatesEnabled(true);

                     foreach (NodeManager* node, k->nodeManagers) {
                              node->show();
                              node->syncNodesFromParent();
                              node->beginToEdit();
                              break;
                     }

                 } else {
                     #ifdef K_DEBUG
                            kFatal() << "Select::itemResponse - No item found";
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

void Select::syncNodes()
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

void Select::saveConfig()
{
}

void Select::keyPressEvent(QKeyEvent *event)
{
    if (k->scene) {
        if (!k->selectionFlag)
            return;
    } else {
        return;
    }

    if (event->key() == Qt::Key_R) { 
        if (event->modifiers() == Qt::AltModifier) {
            foreach (NodeManager *nodeManager, k->nodeManagers) {
                         nodeManager->toggleAction();
                         break;
            }

            return;
        }
    }

    if ((event->key() == Qt::Key_Left) || (event->key() == Qt::Key_Up) 
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
    }
}

void Select::updateItems(KTGraphicsScene *scene)
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

Q_EXPORT_PLUGIN2(kt_select, Select);
