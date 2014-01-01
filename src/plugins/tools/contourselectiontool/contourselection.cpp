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
#include <QDir>

struct ContourSelection::Private
{
    QMap<QString, TAction *> actions;
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

    foreach (QGraphicsView * view, scene->views()) {
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
        QGraphicsItem *item = currentSelection.at(0);

        if (k->nodeGroup) {
            int index1 = scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
            int index2 = scene->currentFrame()->indexOf(item);
            if (index1 == index2 || index2 < 0) {
                return;
            } else {
                k->nodeGroup->clear();
            }
        }

        k->nodeGroup = new TNodeGroup(item, scene, TNodeGroup::LineSelection, k->baseZValue);

        if (!k->nodeGroup->changedNodes().isEmpty()) {
            int position = scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
            if (position >= 0 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem())) {
                QDomDocument doc;
                doc.appendChild(qgraphicsitem_cast<TupPathItem *>(k->nodeGroup->parentItem())->toXml(doc));
                    
                TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                            scene->currentLayerIndex(), scene->currentFrameIndex(), position, 
                                            QPointF(), scene->spaceMode(), TupLibraryObject::Item, 
                                            TupProjectRequest::EditNodes, doc.toString());
                emit requested(&event);
            } else {
                #ifdef K_DEBUG
                       tError() << "ContourSelection::release() - Fatal Error: Invalid position [ " << position << " ]";
                #endif
                return;
            }
            k->nodeGroup->clearChangesNodes();
        }
    } else {
        if (k->nodeGroup) {
            k->nodeGroup->clear();
            k->nodeGroup = 0;
        }
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
                    if (frame) {
                        item = frame->item(response->itemIndex());
                    } else {
                        #ifdef K_DEBUG
                               tError() << "ContourSelection::itemResponse() - Fatal Error: Frame variable is NULL!";
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "ContourSelection::itemResponse() - Fatal Error: Layer variable is NULL!";
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
                                   tError() << "ContourSelection::itemResponse() - Fatal Error: Static frame variable is NULL!";
                                   #endif
                        }
                    } else if (project->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) { 
                               TupFrame *frame = bg->dynamicFrame();
                               if (frame) {
                                   item = frame->item(response->itemIndex());
                               } else {
                                   #ifdef K_DEBUG
                                          tError() << "ContourSelection::itemResponse() - Fatal Error: Dynamic frame variable is NULL!";
                                   #endif
                               }
                    } else {
                        #ifdef K_DEBUG
                               tError() << "ContourSelection::itemResponse() - Fatal Error: Invalid spaceMode!";
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tError() << "ContourSelection::itemResponse() - Fatal Error: Scene background is NULL!";
                    #endif
                }
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "ContourSelection::itemResponse() - Fatal Error: Scene variable is NULL";
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "ContourSelection::itemResponse() - Fatal Error: Project variable is NULL";
        #endif
    }
    
    switch (response->action()) {
            case TupProjectRequest::Convert:
            {
                 if (item) {
                      k->nodeGroup = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection, k->baseZValue);
                 } else {
                     #ifdef K_DEBUG
                            tError() << "ContourSelection::itemResponse() - Fatal Error: No item was found";
                     #endif
                 }
            }
            break;
            case TupProjectRequest::EditNodes:
            {
                 if (item) {
                     if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem()) == item) {
                         k->nodeGroup->show();
                         k->nodeGroup->syncNodesFromParent();
                         k->nodeGroup->saveParentProperties();
                         break;
                     }
                 } else {
                     #ifdef K_DEBUG
                            tError() << "ContourSelection::itemResponse() - Fatal Error: No item was found";
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
                 if (k->nodeGroup) {
                     k->nodeGroup->show();
                     if (k->nodeGroup->parentItem()) {
                         k->nodeGroup->parentItem()->setSelected(true);
                         k->nodeGroup->syncNodesFromParent();
                     }
                 }
            }
            break;
    }
}

void ContourSelection::keyPressEvent(QKeyEvent *event)
{
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
    TAction *select = new TAction(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "nodes.png"), tr("Line Selection"), this);
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
    foreach (QGraphicsView *view, k->scene->views()) {
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

// Q_EXPORT_PLUGIN2(tup_editNodes, ContourSelection)
