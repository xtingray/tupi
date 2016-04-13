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

#include "nodestool.h"

struct NodesTool::Private
{
    QMap<QString, TAction *> actions;
    TNodeGroup *nodeGroup;
    TupGraphicsScene *scene;
    int baseZValue;
    qreal realFactor;
    bool activeSelection;
    int currentLayer;
    int currentFrame;
};

NodesTool::NodesTool(): k(new Private)
{
    setupActions();
}

NodesTool::~NodesTool()
{
    delete k;
}

void NodesTool::init(TupGraphicsScene *scene)
{
    k->activeSelection = false;
    k->scene = scene;

    if (k->scene->selectedItems().count() > 0) {
        k->scene->clearSelection();
        if (k->nodeGroup) {
            k->nodeGroup->clear();
            k->nodeGroup = 0;
        }
    }

    k->baseZValue = (2*ZLAYER_LIMIT) + (k->scene->scene()->layersCount() * ZLAYER_LIMIT); 
}

QStringList NodesTool::keys() const
{
    return QStringList() << tr("Nodes Selection") ;
}

void NodesTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

void NodesTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

void NodesTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[NodesTool::release()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    Q_UNUSED(brushManager);

    QList<QGraphicsItem *> currentSelection = scene->selectedItems();
    if (!currentSelection.isEmpty()) {
        QGraphicsItem *selectedItem = currentSelection.at(0);
        TupFrame *frame = currentFrame();
        int itemIndex = frame->indexOf(selectedItem);

        if (qgraphicsitem_cast<TupSvgItem *>(selectedItem)) {
            TOsd::self()->display(tr("Error"), tr("SVG objects cannot be edited!"), TOsd::Error);
            return;
        }

        if (TupGraphicLibraryItem *libraryItem = qgraphicsitem_cast<TupGraphicLibraryItem *>(selectedItem)) {
            if (libraryItem->itemType() == TupLibraryObject::Image) {
                TOsd::self()->display(tr("Error"), tr("Images have no nodes!"), TOsd::Error);
                return;
            }
        }

        if (qgraphicsitem_cast<TupItemGroup *>(selectedItem)) {
            if (k->activeSelection)
                k->nodeGroup->clear();
            QPointF coord = input->pos();

            if (itemIndex >= 0) {
                TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                          scene->currentSceneIndex(),
                                          k->currentLayer, k->currentFrame,
                                          itemIndex, coord,
                                          scene->spaceContext(), TupLibraryObject::Item,
                                          TupProjectRequest::Ungroup);
                emit requested(&event);
            }
            return;
        }

        if (!qgraphicsitem_cast<TControlNode*>(selectedItem)) {
            if (!qgraphicsitem_cast<TupPathItem *>(selectedItem)) {
                TOsd::self()->display(tr("Error"), tr("Only pencil/ink lines can be edited!"), TOsd::Error);
                return;
            }
        }

        if (itemIndex == -1) {
            if (qgraphicsitem_cast<TControlNode*>(selectedItem)) {
                QGraphicsItem *item = k->nodeGroup->parentItem();
                int position = frame->indexOf(item);
                if (position >= 0) {
                    // if (qgraphicsitem_cast<QGraphicsPathItem *>(item)) {
                        QString path = qgraphicsitem_cast<TupPathItem *>(item)->pathToString();
                        TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(),
                                                  k->currentLayer, k->currentFrame, position,
                                                  QPointF(), scene->spaceContext(), TupLibraryObject::Item,
                                                  TupProjectRequest::EditNodes, path);
                        emit requested(&event);
                        k->nodeGroup->clearChangedNodes();
                    // }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "NodesTool::release() - Fatal Error: Invalid position [ " + QString::number(position) + " ]";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "NodesTool::release() - Invalid selected item index: " + QString::number(itemIndex);
                    #ifdef Q_OS_WIN
                       qDebug() << msg;
                    #else
                       tWarning() << msg;
                    #endif
                #endif
            }

            return;
        }

        // Avoiding to select the same item twice 
        if (k->activeSelection) { 
            TupFrame *frame = currentFrame();
            int oldIndex = frame->indexOf(k->nodeGroup->parentItem());
            if (oldIndex != itemIndex) {
                k->nodeGroup->clear();
                k->nodeGroup = new TNodeGroup(selectedItem, scene, TNodeGroup::LineSelection, k->baseZValue);
                k->nodeGroup->show();
                k->nodeGroup->resizeNodes(k->realFactor);
                if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(selectedItem)) {
                    if (path->isNotEdited()) 
                        path->saveOriginalPath();
                }
            } else {
                if (k->nodeGroup->hasChangedNodes()) {
                    QGraphicsItem *item = k->nodeGroup->parentItem();
                    int position = frame->indexOf(item);
                    if (position >= 0) {
                        QString path = qgraphicsitem_cast<TupPathItem *>(item)->pathToString();
                        TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(),
                                                  k->currentLayer, k->currentFrame, position,
                                                  QPointF(), scene->spaceContext(), TupLibraryObject::Item,
                                                  TupProjectRequest::EditNodes, path);
                        emit requested(&event);
                        k->nodeGroup->clearChangedNodes();
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "NodesTool::release() - Fatal Error: Invalid position [ " + QString::number(position) + " ]";
                            #ifdef Q_OS_WIN
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "NodesTool::release() - Node group has NO changes!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                }
            }
        } else {
            k->nodeGroup = new TNodeGroup(selectedItem, scene, TNodeGroup::LineSelection, k->baseZValue);
            k->nodeGroup->show();
            k->activeSelection = true;

            k->nodeGroup->resizeNodes(k->realFactor);
            if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(selectedItem)) {
                if (path->isNotEdited())
                    path->saveOriginalPath();
            }
        }
    } else {
        if (k->activeSelection) {
            #ifdef K_DEBUG
                QString msg = "NodesTool::release() - Empty selection! Removing nodes...";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif
            k->nodeGroup->clear();
            k->nodeGroup = 0;
            k->activeSelection = false;
        }
    } 
}

TupFrame* NodesTool::currentFrame()
{
    TupFrame *frame = 0;
    if (k->scene->spaceContext() == TupProject::FRAMES_EDITION) {
        frame = k->scene->currentFrame();
        k->currentLayer = k->scene->currentLayerIndex();
        k->currentFrame = k->scene->currentFrameIndex();
    } else {
        k->currentLayer = -1;
        k->currentFrame = -1;

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

void NodesTool::itemResponse(const TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[NodesTool::itemResponse()]";
        #else
            T_FUNCINFOX("NodesTool");
        #endif
    #endif

    QGraphicsItem *item = 0;

    if (response->action() != TupProjectRequest::Remove) {
        TupFrame *frame = currentFrame();
        if (response->action() == TupProjectRequest::Ungroup) {
            QPointF point = response->position();
            item = k->scene->itemAt(point, QTransform());
        } else {
            item = frame->item(response->itemIndex());
        }
    }

    switch (response->action()) {
            case TupProjectRequest::Convert:
            {
                 if (item) {
                     k->nodeGroup = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection, k->baseZValue);
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "NodesTool::itemResponse() - Fatal Error: No item was found";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                 }
            }
            break;
            case TupProjectRequest::EditNodes:
            {
                 if (item) {
                     if (k->activeSelection) {
                         if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem()) == item) {
                             k->nodeGroup->show();
                             k->nodeGroup->syncNodesFromParent();
                             k->nodeGroup->saveParentProperties();
                         }
                     } else {
                         k->nodeGroup = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection, k->baseZValue);
                         k->nodeGroup->show();
                         k->activeSelection = true;
                         k->nodeGroup->resizeNodes(k->realFactor);
                     }
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "NodesTool::itemResponse() - Fatal Error: No item was found";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                 }
            }
            break;
            case TupProjectRequest::Remove:
            {
                 return;
            }
            break;
            case TupProjectRequest::Ungroup:
            {
                 // reset(k->scene);
                 if (item) {
                     k->nodeGroup = new TNodeGroup(item, k->scene, TNodeGroup::LineSelection, k->baseZValue);
                     k->nodeGroup->show();
                     k->activeSelection = true;
                     k->nodeGroup->resizeNodes(k->realFactor);
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "NodesTool::itemResponse() - Fatal error: No item was found";
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                 }

                 return;
            }
            break;
            default:
            {
                 #ifdef K_DEBUG
                     qDebug() << "NodesTool::itemResponse() - default action"; 
                 #endif

                 if (k->activeSelection) {
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

void NodesTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void NodesTool::setupActions()
{
    k->activeSelection = false;

    TAction *nodes = new TAction(QPixmap(kAppProp->themeDir() + "icons/nodes.png"), tr("Nodes Selection"), this);
    nodes->setShortcut(QKeySequence(tr("N")));
    nodes->setToolTip(tr("Nodes Selection") + " - " + "N");

    k->actions.insert(tr("Nodes Selection"), nodes);
}

QMap<QString, TAction *> NodesTool::actions() const
{
    return k->actions;
}

int NodesTool::toolType() const
{
    return TupToolInterface::Selection;
}

QWidget *NodesTool::configurator() 
{
    return 0;
}

void NodesTool::aboutToChangeScene(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void NodesTool::aboutToChangeTool()
{
}

void NodesTool::saveConfig()
{
}

QCursor NodesTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

void NodesTool::resizeNodes(qreal scaleFactor)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[NodesTool::resizeNodes()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    k->realFactor = scaleFactor;
    if (k->activeSelection)
        k->nodeGroup->resizeNodes(scaleFactor);
}

void NodesTool::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}

void NodesTool::clearSelection()
{
    if (k->scene->selectedItems().count() > 0) {
        k->scene->clearSelection();
        if (k->activeSelection)
            k->activeSelection = false;
        if (k->nodeGroup) {
            k->nodeGroup->clear();
            k->nodeGroup = 0;
        }
    }
}

