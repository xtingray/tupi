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

#include "polylinetool.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tuprequestbuilder.h"
#include "tuplibraryobject.h"
#include "taction.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupbrushmanager.h"
#include "tupprojectresponse.h"

struct PolyLineTool::Private
{
    bool begin;
    QPointF center;
    QPointF right;
    QPointF mirror;
    
    TNodeGroup *nodeGroup;
    QPainterPath path;
    
    QMap<QString, TAction *> actions;
    
    TupPathItem *item;
    TupGraphicsScene *scene;
    
    QGraphicsLineItem *line1;
    QGraphicsLineItem *line2;
    InfoPanel *configurator;
    QCursor cursor;

    qreal realFactor;
};

PolyLineTool::PolyLineTool(): k(new Private)
{
    k->configurator = 0;
    k->nodeGroup = 0;
    k->item = 0;

    k->cursor = QCursor(kAppProp->themeDir() + "cursors/polyline.png", 4, 4);
    
    k->line1 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line1->setPen(QPen(QColor(55, 177, 50)));
    k->line2 = new QGraphicsLineItem(0, 0, 0, 0);
    k->line2->setPen(QPen(QColor(55, 177, 50)));
    
    setupActions();
}

PolyLineTool::~PolyLineTool()
{
}

void PolyLineTool::setupActions()
{
    TAction *polyline = new TAction(QIcon(kAppProp->themeDir() + "/icons/polyline.png"), tr("PolyLine"), this);
    polyline->setShortcut(QKeySequence(tr("S")));
    polyline->setToolTip(tr("PolyLine") + " - " + "S");
    polyline->setCursor(k->cursor);

    k->actions.insert(tr("PolyLine"), polyline);
}

QMap<QString, TAction *> PolyLineTool::actions() const
{
    return k->actions;
}

void PolyLineTool::init(TupGraphicsScene *scene)
{
    if (scene) {
        k->scene = scene;
    } else {
        #ifdef K_DEBUG
            QString msg = "PolyLineTool::init() - Fatal Error: TupGraphicsScene variable is NULL!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return;
    }

    initEnv(false);

    /*
    foreach (QGraphicsView *view,  scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
        
             foreach (QGraphicsItem *item, scene->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
    */
}

QStringList PolyLineTool::keys() const
{
    return QStringList() << tr("PolyLine");
}

void PolyLineTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PolyLineTool::press()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    scene->clearSelection();

    if (k->begin) { // This condition happens only in the beginning of the polyline 
        k->path = QPainterPath();
        k->path.moveTo(input->pos());

        k->item = new TupPathItem();
        k->item->setPen(brushManager->pen());
        k->item->setPath(k->path);
    } else { // This condition happens from the second point of the polyline and until the last one
        if (!scene->items().contains(k->item))
            scene->includeObject(k->item, true); // SQA: Polyline hack

        k->path.cubicTo(k->right, k->mirror, input->pos());
        k->item->setPath(k->path);
    }
    
    k->center = input->pos();
    
    if (!scene->items().contains(k->line1))
        scene->includeObject(k->line1); 

    if (!scene->items().contains(k->line2))
        scene->includeObject(k->line2);
}

void PolyLineTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    k->mirror = k->center - (input->pos() - k->center);

    if (k->begin) {
        k->right = input->pos();
    } else {
        for (int i=k->path.elementCount()-1; i>=0; i--) {
             if (k->path.elementAt(i).type == QPainterPath::CurveToElement) {
                 k->right = input->pos();
                 if (k->path.elementAt(i + 1).type == QPainterPath::CurveToDataElement)
                     k->path.setElementPositionAt(i + 1, k->mirror.x(), k->mirror.y());
                 break;
             }
        }
    }
    
    k->item->setPath(k->path);
    k->line1->setLine(QLineF(k->mirror, k->center));
    k->line2->setLine(QLineF(k->right, k->center));
}

void PolyLineTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PolyLineTool::release()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (k->begin) {
        QDomDocument doc;
        doc.appendChild(k->item->toXml(doc));
        TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), 
                                                       scene->currentLayerIndex(), 
                                                       scene->currentFrameIndex(), 
                                                       0, QPointF(), scene->spaceContext(), TupLibraryObject::Item, 
                                                       TupProjectRequest::Add, doc.toString());
        emit requested(&request);
 
        k->begin = false;
    } else {
        if (!k->nodeGroup) {
            k->nodeGroup = new TNodeGroup(k->item, k->scene, TNodeGroup::Polyline, k->item->zValue() + 1);
            connect(k->nodeGroup, SIGNAL(nodeReleased()), this, SLOT(nodeChanged()));
        } else {
            k->nodeGroup->createNodes(k->item);
        }

        k->nodeGroup->show();
        k->nodeGroup->resizeNodes(k->realFactor);
    }
}

void PolyLineTool::itemResponse(const TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PolyLineTool::itemResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QGraphicsItem *item = 0;
    TupScene *scene = 0;
    TupLayer *layer = 0;
    TupFrame *frame = 0;

    TupProject *project = k->scene->scene()->project();
    scene = project->scene(response->sceneIndex());
    if (scene) {
        if (k->scene->spaceContext() == TupProject::FRAMES_EDITION) {
            layer = scene->layer(response->layerIndex());
            if (layer) {
                frame = layer->frame(response->frameIndex());
                if (frame) {
                    item = frame->item(response->itemIndex());
                } else {
                    #ifdef K_DEBUG
                        QString msg = "PolyLineTool::itemResponse() - Fatal Error: Frame variable is NULL!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "PolyLineTool::itemResponse() - Fatal Error: Layer variable is NULL!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                if (k->scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                    TupFrame *frame = bg->staticFrame();
                    if (frame) {
                        item = frame->item(response->itemIndex());
                    } else {
                        #ifdef K_DEBUG
                           QString msg = "PolyLineTool::itemResponse() - Fatal Error: Static bg frame variable is NULL!";
                           #ifdef Q_OS_WIN
                               qDebug() << msg;
                           #else
                               tError() << msg;
                           #endif
                        #endif
                    }
                } else if (k->scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           TupFrame *frame = bg->dynamicFrame();
                           if (frame) {
                               item = frame->item(response->itemIndex());
                           } else {
                               #ifdef K_DEBUG
                                   QString msg = "PolyLineTool::itemResponse() - Fatal Error: Dynamic bg frame variable is NULL!";
                                   #ifdef Q_OS_WIN
                                       qDebug() << msg;
                                   #else
                                       tError() << msg;
                                   #endif
                               #endif
                           }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "PolyLineTool::itemResponse() - Fatal Error: Invalid spaceContext!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "PolyLineTool::itemResponse() - Fatal Error: Scene bg variable is NULL!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "PolyLineTool::itemResponse() - Fatal Error: Scene variable is NULL!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    switch (response->action()) {
        case TupProjectRequest::Add:
        {
            if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(item)) {
                if (k->item != path) {
                    k->item = path;
                    if (k->nodeGroup)
                        k->nodeGroup->setParentItem(path);
                }
            }
        }
        break;
        case TupProjectRequest::Remove:
        {
            // SQA: Is this code doing something for real?
            /*
            if (item == k->item) {
                k->path = QPainterPath();
                delete k->item;
                k->item = 0;
                delete k->nodeGroup;
                k->nodeGroup = 0;
            }
            */
        }
        break;
        case TupProjectRequest::EditNodes:
        {
            if (item && k->nodeGroup) {
                if (qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem()) == item) {
                    k->nodeGroup->show();
                    k->nodeGroup->syncNodesFromParent();
                    k->nodeGroup->saveParentProperties();

                    k->path = k->item->path();
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "PolyLineTool::itemResponse() - Fatal Error: No item was found";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        }
        break;
        default: 
        break;
    }
}

void PolyLineTool::keyPressEvent(QKeyEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PolyLineTool::keyPressEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else if (event->key() == Qt::Key_X) {
               initEnv();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void PolyLineTool::initEnv(bool postInit)
{
    if (postInit) {
    if (k->nodeGroup) { 
        k->nodeGroup->clear();
        k->nodeGroup = 0;
    } } else {
      if (k->nodeGroup)
          k->nodeGroup = 0;
   }

    k->begin = true;
    k->path = QPainterPath();
    k->item = 0;

    if (k->line1) {
        if (k->scene->items().contains(k->line1))
            k->scene->removeItem(k->line1);
    }

    if (k->line2) {
        if (k->scene->items().contains(k->line2))
            k->scene->removeItem(k->line2);
    }
}

void PolyLineTool::nodeChanged()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PolyLineTool::nodeChanged()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->nodeGroup) {
        if (!k->nodeGroup->changedNodes().isEmpty()) {
            int position = -1;
            if (k->scene->spaceContext() == TupProject::FRAMES_EDITION) {
                position = k->scene->currentFrame()->indexOf(k->nodeGroup->parentItem());
            } else {
                TupBackground *bg = k->scene->scene()->background();
                if (bg) {
                    if (k->scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                        TupFrame *frame = bg->staticFrame();
                        if (frame) {
                            position = frame->indexOf(k->nodeGroup->parentItem());
                        } else {
                            #ifdef K_DEBUG
                                QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Static bg frame is NULL!";
                                #ifdef Q_OS_WIN
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif
                            return;
                        }
                    } else if (k->scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                               TupFrame *frame = bg->dynamicFrame();
                               if (frame) {
                                   position = frame->indexOf(k->nodeGroup->parentItem());
                               } else {
                                   #ifdef K_DEBUG
                                       QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Dynamic bg frame is NULL!";
                                       #ifdef Q_OS_WIN
                                           qDebug() << msg;
                                       #else
                                           tError() << msg;
                                       #endif
                                   #endif
                                   return;
                               }
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Invalid spaceContext!";
                            #ifdef Q_OS_WIN
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Scene background variable is NULL!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            }

            if (position >= 0 && qgraphicsitem_cast<QGraphicsPathItem *>(k->nodeGroup->parentItem())) {
                    QDomDocument doc;
                    doc.appendChild(qgraphicsitem_cast<TupPathItem *>(k->nodeGroup->parentItem())->toXml(doc));
                
                    TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), 
                                                                                 position, QPointF(), k->scene->spaceContext(), TupLibraryObject::Item, TupProjectRequest::EditNodes, 
                                                                                 doc.toString());
                    emit requested(&event);
                    // k->nodeGroup->restoreItem();
             } else {
               #ifdef K_DEBUG
                   QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Invalid object index || No nodeGroup parent item";
                   #ifdef Q_OS_WIN
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               return;
             }
        } else {
          #ifdef K_DEBUG
              QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Array of changed nodes is empty!";
              #ifdef Q_OS_WIN
                qDebug() << msg;
              #else
                tError() << msg;
              #endif
          #endif
          return;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "PolyLineTool::nodeChanged() - Fatal Error: Array of nodes is empty!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }
}

int PolyLineTool::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *PolyLineTool::configurator() 
{
    if (! k->configurator)
        k->configurator = new InfoPanel;

    return k->configurator;
}

void PolyLineTool::aboutToChangeScene(TupGraphicsScene *)
{
    // initEnv(false);
}

void PolyLineTool::aboutToChangeTool()
{
    // initEnv(false);
}

void PolyLineTool::saveConfig()
{
}

QCursor PolyLineTool::cursor() const
{
    return k->cursor;
}

void PolyLineTool::resizeNodes(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
    if (k->nodeGroup)
        k->nodeGroup->resizeNodes(scaleFactor);
}

void PolyLineTool::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}
