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
    if (k->scene->selectedItems().count() > 0)
        k->scene->clearSelection();
    k->baseZValue = 20000 + (k->scene->scene()->layersCount() * 10000); 
    reset(k->scene);
}

void NodesTool::reset(TupGraphicsScene *scene)
{
    int zBottomLimit = (scene->currentLayerIndex() + 2)*10000;
    int zTopLimit = zBottomLimit + 10000;

    foreach (QGraphicsItem *item, scene->items()) {
             int zValue = item->zValue();
             // qreal opacity = item->opacity();
             if (scene->spaceContext() == TupProject::FRAMES_EDITION) {
                 // if ((zValue >= zBottomLimit) && (zValue < zTopLimit) && (item->toolTip().length()==0) && (opacity == 1)) 
                 if ((zValue >= zBottomLimit) && (zValue < zTopLimit) && (item->toolTip().length()==0))
                      item->setFlags(QGraphicsItem::ItemIsSelectable);
                 else
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             } else {
                 if (scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                     item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                 } else if (scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                            if (zValue >= 10000) 
                                item->setFlags(QGraphicsItem::ItemIsSelectable);
                            else 
                                item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "NodesTool::reset() - Fatal Error: Invalid spaceContext!";
                                      #ifdef Q_OS_WIN
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return;
                 }
             }
    }
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
    Q_UNUSED(brushManager);

    if (scene->selectedItems().count() > 0) {
        QList<QGraphicsItem *> currentSelection = scene->selectedItems();
        QGraphicsItem *item = currentSelection.at(0);

        if (qgraphicsitem_cast<TupSvgItem *>(item)) {
            TOsd::self()->display(tr("Error"), tr("SVG objects cannot be edited!"), TOsd::Error);
            return;
        }

        if (TupGraphicLibraryItem *libraryItem = qgraphicsitem_cast<TupGraphicLibraryItem *>(item)) {
            if (libraryItem->itemType() == TupLibraryObject::Image) {
                TOsd::self()->display(tr("Error"), tr("Images have no nodes!"), TOsd::Error);
                return;
            }
        }

        if (qgraphicsitem_cast<TupItemGroup *>(item)) {
            if (k->activeSelection) 
                k->nodeGroup->clear();
            QPointF coord = input->pos();
            int itemIndex = currentFrame()->indexOf(item);

            if (itemIndex >= 0) {
                TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                          scene->currentSceneIndex(),
                                          scene->currentLayerIndex(),
                                          scene->currentFrameIndex(),
                                          itemIndex, coord,
                                          scene->spaceContext(), TupLibraryObject::Item,
                                          TupProjectRequest::Ungroup);
                emit requested(&event);
            }
            return;
        }

        if (k->activeSelection) { 
            TupFrame *frame = currentFrame();
            int index1 = frame->indexOf(k->nodeGroup->parentItem());
            int index2 = frame->indexOf(item);
            if (index1 == index2 || index2 < 0)
                return;
            else 
                k->nodeGroup->clear();
        }

        k->nodeGroup = new TNodeGroup(item, scene, TNodeGroup::LineSelection, k->baseZValue);
        k->nodeGroup->show();
        k->activeSelection = true;
        k->nodeGroup->resizeNodes(k->realFactor);

        if (!k->nodeGroup->changedNodes().isEmpty()) {
            int position = scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
            if (position >= 0 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem())) {
                QDomDocument doc;
                doc.appendChild(qgraphicsitem_cast<TupPathItem *>(k->nodeGroup->parentItem())->toXml(doc));
                    
                TupProjectRequest event = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                            scene->currentLayerIndex(), scene->currentFrameIndex(), position, 
                                            QPointF(), scene->spaceContext(), TupLibraryObject::Item, 
                                            TupProjectRequest::EditNodes, doc.toString());
                emit requested(&event);
            } else {
                #ifdef K_DEBUG
                    QString msg = "NodesTool::release() - Fatal Error: Invalid position [ " + QString::number(position) + " ]";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
            k->nodeGroup->clearChangesNodes();
        }
    } else {
        if (k->activeSelection) {
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
                     if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem()) == item) {
                         k->nodeGroup->show();
                         k->nodeGroup->syncNodesFromParent();
                         k->nodeGroup->saveParentProperties();
                         break;
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
                 reset(k->scene);
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
    TAction *nodes = new TAction(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "nodes.png"), tr("Nodes Selection"), this);
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
    k->realFactor = scaleFactor;
    if (k->activeSelection)
        k->nodeGroup->resizeNodes(scaleFactor);
}

void NodesTool::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}
