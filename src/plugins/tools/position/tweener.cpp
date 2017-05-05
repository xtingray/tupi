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

#include "tweener.h"
#include "configurator.h"
#include "taction.h"
#include "tnodegroup.h"
#include "tosd.h"
#include "tupsvg2qt.h"
#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicsscene.h"
#include "tupgraphicobject.h"
#include "tuppathitem.h"
#include "tupellipseitem.h"
#include "tuprectitem.h"
#include "tuplineitem.h"
#include "tupsvgitem.h"
#include "tupitemgroup.h"
#include "tupitemtweener.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupscene.h"
#include "tuplayer.h"
// #include "talgorithm.h"

struct Tweener::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    TupGraphicsScene *scene;
    QGraphicsPathItem *path;
    QList<QGraphicsItem *> objects;

    TupItemTweener *currentTween;
    TNodeGroup *nodesGroup;
    qreal realFactor;

    bool isPathInScene;
    int initFrame;
    int initLayer;
    int initScene;

    TupToolPlugin::Mode mode;
    TupToolPlugin::EditMode editMode;

    QPointF itemObjectReference;
    QPointF pathOffset;
    QPointF firstNode;
    QPointF objectPos;
    QList<TupEllipseItem *> dots; 

    int baseZValue;
};

Tweener::Tweener() : TupToolPlugin(), k(new Private)
{
    setupActions();

    k->isPathInScene = false;
    k->configurator = 0;
    k->path = 0;
    k->nodesGroup = 0;
    k->currentTween = 0;

    k->initFrame = 0;
    k->initLayer = 0;
    k->initScene = 0;
}

Tweener::~Tweener()
{
    delete k;
}

/* This method initialize the context of the plugin */

void Tweener::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::init()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->dots.clear();

    if (k->isPathInScene) {
        if (k->nodesGroup) {
            k->nodesGroup->clear();
            delete k->nodesGroup;
            k->nodesGroup = 0;
        }

        if (k->path) {
            delete k->path;
            k->path = 0;
        }

        k->isPathInScene = false;
    }

    k->scene = scene;
    k->objects.clear();
    k->baseZValue = (2*ZLAYER_LIMIT) + (scene->scene()->layersCount() * ZLAYER_LIMIT);

    k->pathOffset = QPointF(0, 0); 
    k->firstNode = QPointF(0, 0);
    k->itemObjectReference = QPointF(0, 0);

    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(TupItemTweener::Position);
    if (tweenList.size() > 0) {
        k->configurator->loadTweenList(tweenList);
        setCurrentTween(tweenList.at(0));
    } else {
        k->configurator->activeButtonsPanel(false);
    }

    int total = framesCount();
    k->configurator->initStartCombo(total, k->scene->currentFrameIndex());
}

void Tweener::updateStartFrame(int index)
{
    if (k->initFrame != index && index >= 0)
        k->initFrame = index - 1;
}

/* This method returns the plugin name */

QStringList Tweener::keys() const
{
    return QStringList() << tr("Position Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::press()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->editMode == TupToolPlugin::Properties && k->scene->currentFrameIndex() == k->initFrame) {
        if (k->path) {
            QPointF point = k->path->mapFromParent(input->pos());
            QPainterPath path = k->path->path();
            path.cubicTo(point, point, point);
            k->path->setPath(path);
        }
    } 
}

/* This method is executed while the mouse is pressed and on movement */

void Tweener::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method finishes the action started on the press method depending
 * on the active mode: Selecting an object or Creating a path
*/

void Tweener::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::release()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->initFrame) {
        if (k->editMode == TupToolPlugin::Properties) {
            if (k->nodesGroup) {
                k->nodesGroup->createNodes(k->path);
                k->nodesGroup->show();
                k->nodesGroup->resizeNodes(k->realFactor);
                k->nodesGroup->expandAllNodes();
                k->configurator->updateSteps(k->path);
                QPainterPath::Element e = k->path->path().elementAt(0);
                QPointF begin = QPointF(e.x, e.y);

                if (begin != k->firstNode) {
                    QPointF oldPos = k->firstNode;
                    QPointF newPos = begin;

                    int distanceX = newPos.x() - oldPos.x();
                    int distanceY = newPos.y() - oldPos.y();

                    if (k->objects.size() > 0) {
                        foreach (QGraphicsItem *item, k->objects)
                                 item->moveBy(distanceX, distanceY);
                        QGraphicsItem *item = k->objects.at(0);
                        QRectF rect = item->sceneBoundingRect();
                        k->itemObjectReference = rect.center();
                    }

                    k->firstNode = newPos;
                }

                updateTweenPoints();
            }
        } else {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);

                QGraphicsItem *item = k->objects.at(0);
                QRectF rect = item->sceneBoundingRect();
                QPointF newPos = rect.center();
                QPointF oldPos = k->itemObjectReference;
                k->itemObjectReference = newPos;

                if (!k->path) {
                    k->path = new QGraphicsPathItem;
                    k->path->setZValue(k->baseZValue);

                    QColor color(55, 155, 55, 200);
                    QPen pen(QBrush(color), 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                    k->path->setPen(pen);

                    QPainterPath path;
                    path.moveTo(newPos);
                    k->firstNode = newPos;
                    k->path->setPath(path);
                    scene->addItem(k->path);
                    k->isPathInScene = true;

                    k->pathOffset = QPointF(0, 0);
                } else {
                    int distanceX = newPos.x() - oldPos.x();
                    int distanceY = newPos.y() - oldPos.y();
                    k->path->moveBy(distanceX, distanceY);
                    k->pathOffset = QPointF(distanceX, distanceY);

                    // k->firstNode = newPos;
                }
            } 
        }
    } 
}

/* This method returns the list of actions defined in this plugin */

QMap<QString, TAction *> Tweener::actions() const
{
    return k->actions;
}

/* This method returns the list of actions defined in this plugin */

int Tweener::toolType() const
{
    return TupToolInterface::Tweener;
}

/* This method returns the tool panel associated to this plugin */

QWidget *Tweener::configurator()
{
    if (!k->configurator) {
        k->mode = TupToolPlugin::View;

        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(startingFrameChanged(int)), this, SLOT(updateStartFrame(int)));
        connect(k->configurator, SIGNAL(clickedCreatePath()), this, SLOT(setTweenPath()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelection()));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(setMode(TupToolPlugin::Mode)), this, SLOT(updateMode(TupToolPlugin::Mode)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));
        connect(k->configurator, SIGNAL(framesTotalChanged()), this, SLOT(updateTweenPoints()));
    } else {
        k->mode = k->configurator->mode();
    }

    return k->configurator;
}

/* This method is called when there's a change on/of scene */

void Tweener::aboutToChangeScene(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

/* This method is called when this plugin is off */

void Tweener::aboutToChangeTool()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::aboutToChangeTool()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    resetGUI();
}

void Tweener::resetGUI()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::resetGUI()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->configurator->clearData();

    if (k->editMode == TupToolPlugin::Selection) {
        clearSelection();
        disableSelection();
        return;
    }

    if (k->editMode == TupToolPlugin::Properties) {
        if (k->path) {
            k->scene->removeItem(k->path);
            k->isPathInScene = false;
            if (k->nodesGroup) {
                k->nodesGroup->clear();
                k->nodesGroup = 0;
            }

            delete k->path;
            k->path = 0;
        }
        return;
    }
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    k->realFactor = 1;

    TAction *action = new TAction(QPixmap(kAppProp->themeDir() + "icons/position_tween.png"), tr("Position Tween"), this);
    action->setCursor(QCursor(kAppProp->themeDir() + "cursors/tweener.png", 0 ,0));
    action->setShortcut(QKeySequence(tr("Shift+W")));

    k->actions.insert(tr("Position Tween"), action);
}

/* This method initializes the "Create path" mode */

void Tweener::setTweenPath()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::setTweenPath()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->path) {
        k->pathOffset = QPointF(0, 0);

        if (!k->isPathInScene) {
            k->scene->addItem(k->path);
            k->isPathInScene = true;
        } 

        if (k->nodesGroup) {
            k->nodesGroup->createNodes(k->path);
        } else {
            k->nodesGroup = new TNodeGroup(k->path, k->scene, TNodeGroup::PositionTween, k->baseZValue);
            connect(k->nodesGroup, SIGNAL(nodeReleased()), SLOT(updatePath()));
            k->nodesGroup->createNodes(k->path);
        }
        k->nodesGroup->show();
        k->nodesGroup->resizeNodes(k->realFactor);
        k->nodesGroup->expandAllNodes();

        paintTweenPoints();
    }

    k->editMode = TupToolPlugin::Properties;
    disableSelection();
}

/* This method initializes the "Select object" mode */

void Tweener::setSelection()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::setSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->mode == TupToolPlugin::Edit) {
        if (k->initFrame != k->scene->currentFrameIndex()) {
            QString selection = QString::number(k->initLayer) + "," + QString::number(k->initLayer) + ","
                                + QString::number(k->initFrame) + "," + QString::number(k->initFrame);

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentTween->initScene(),
                                                                              k->currentTween->initLayer(),
                                                                              k->currentTween->initFrame(), 
                                                                              TupProjectRequest::Select, selection);
            emit requested(&request);
        }
    }

    if (k->path) {
        k->scene->removeItem(k->path);
        k->isPathInScene = false;
        if (k->nodesGroup) {
            k->nodesGroup->clear();
            k->nodesGroup = 0;
        }
    }

    k->editMode = TupToolPlugin::Selection;

    k->scene->enableItemsForSelection();
    foreach (QGraphicsView *view, k->scene->views())
             view->setDragMode(QGraphicsView::RubberBandDrag);
    // When Object selection is enabled, previous selection is set
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                 item->setSelected(true);
        }
    }
}

/* This method transforms the path created into a QString representation */

QString Tweener::pathToCoords()
{
    QString strPath = "";
    QChar t;
    int offsetX = k->pathOffset.x();
    int offsetY = k->pathOffset.y();

    QPainterPath path = k->path->path();
    int total = path.elementCount(); 
    for (int i=0; i < total; i++) {
         QPainterPath::Element e = path.elementAt(i);
         switch (e.type) {
             case QPainterPath::MoveToElement:
             {
                 if (t != 'M') {
                     t = 'M';
                     strPath += "M " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                 } else {
                     strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                 }
             }
             break;
             case QPainterPath::LineToElement:
             {
                  if (t != 'L') {
                      t = 'L';
                      strPath += " L " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  } else {
                      strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  }
             }
             break;
             case QPainterPath::CurveToElement:
             {
                  if (t != 'C') {
                      t = 'C';
                      strPath += " C " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  } else {
                      strPath += "  " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  }
             }
             break;
             case QPainterPath::CurveToDataElement:
             {
                  if (t == 'C')
                      strPath +=  " " + QString::number(e.x + offsetX) + "  " + QString::number(e.y + offsetY) + " ";
             }
             break;
         }
    }

    return strPath.trimmed();
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::applyReset()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    clearSelection();
    disableSelection();

    if (k->nodesGroup) {
        k->nodesGroup->clear();
        k->nodesGroup = 0;
    }

    if (k->path) {
        removeTweenPoints();
        k->scene->removeItem(k->path);
        k->isPathInScene = false;
        k->path = 0;
    }

    k->initFrame = k->scene->currentFrameIndex();
    k->initLayer = k->scene->currentLayerIndex();
    k->initScene = k->scene->currentSceneIndex();

    k->configurator->clearData();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::applyTween()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString name = k->configurator->currentTweenName();
    if (name.length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Tween name is missing!"), TOsd::Error);
        return;
    }

    if (!k->scene->scene()->tweenExists(name, TupItemTweener::Position)) {
        k->initFrame = k->scene->currentFrameIndex();
        k->initLayer = k->scene->currentLayerIndex();
        k->initScene = k->scene->currentSceneIndex();

        foreach (QGraphicsItem *item, k->objects) {   
                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item); 
                 QPointF point = item->pos();

                 if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                     type = TupLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 }

                 QString route = pathToCoords();
                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                             k->initScene, k->initLayer, k->initFrame,
                                             objectIndex,
                                             QPointF(), k->scene->spaceContext(), type,
                                             TupProjectRequest::SetTween, 
                                             k->configurator->tweenToXml(k->initScene, k->initLayer, k->initFrame, point, route));
                 emit requested(&request);
        }
    } else {
        removeTweenFromProject(name);
        QList<QGraphicsItem *> newList;

        k->initFrame = k->configurator->startFrame();
        k->initLayer = k->currentTween->initLayer();
        k->initScene = k->currentTween->initScene();

        foreach (QGraphicsItem *item, k->objects) {
                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 TupScene *scene = k->scene->scene();
                 TupLayer *layer = scene->layerAt(k->initLayer);
                 TupFrame *frame = layer->frameAt(k->currentTween->initFrame());
                 int objectIndex = frame->indexOf(item);

                 /*
                 QPainterPath path = k->path->path();
                 QPolygonF points = path.toFillPolygon(); 
                 QPointF point = points.at(0) - QPointF(item->boundingRect().width(), item->boundingRect().height());
                 */

                 QPointF point = item->pos();
                 TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item); 

                 if (svg) {
                     type = TupLibraryObject::Svg;
                     objectIndex = frame->indexOf(svg);
                 }

                 if (k->initFrame != k->currentTween->initFrame()) {
                     QDomDocument dom;
                     if (type == TupLibraryObject::Svg)
                         dom.appendChild(svg->toXml(dom));
                     else
                         dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));

                     TupProjectRequest request = TupRequestBuilder::createItemRequest(k->initScene, k->initLayer, 
                                                 k->initFrame, 0, QPointF(), 
                                                 k->scene->spaceContext(), type, 
                                                 TupProjectRequest::Add, dom.toString());
                     emit requested(&request);

                     request = TupRequestBuilder::createItemRequest(k->currentTween->initScene(), 
                               k->currentTween->initLayer(),
                               k->currentTween->initFrame(),
                               objectIndex, QPointF(), 
                               k->scene->spaceContext(), type,
                               TupProjectRequest::Remove);
                     emit requested(&request);

                     frame = layer->frameAt(k->initFrame);
                     if (type == TupLibraryObject::Item) {
                         objectIndex = frame->graphicItemsCount() - 1;
                         newList.append(frame->graphicAt(objectIndex)->item());
                     } else {
                         objectIndex = frame->svgItemsCount() - 1;
                         newList.append(frame->svgAt(objectIndex));
                     }
                 }

                 QString route = pathToCoords();
                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                             k->initScene, k->initLayer, k->initFrame,
                                             objectIndex,
                                             QPointF(), k->scene->spaceContext(), type,
                                             TupProjectRequest::SetTween,
                                             k->configurator->tweenToXml(k->initScene, k->initLayer, k->initFrame, point, route));
                 emit requested(&request);
        }

        if (newList.size() > 0)
            k->objects = newList;
    }

    int framesNumber = framesCount();
    // int total = k->initFrame + k->configurator->totalSteps() - 1;
    int total = k->initFrame + k->configurator->totalSteps();
    TupProjectRequest request;

    if (total > framesNumber) {
        int layersCount = k->scene->scene()->layersCount();
        for (int i = framesNumber; i < total; i++) {
             for (int j = 0; j < layersCount; j++) {
                  request = TupRequestBuilder::createFrameRequest(k->initScene, j, i, TupProjectRequest::Add, tr("Frame"));
                  emit requested(&request);
             }
        }
    }

    QString selection = QString::number(k->initLayer) + "," + QString::number(k->initLayer) + ","
                        + QString::number(k->initFrame) + "," + QString::number(k->initFrame);

    request = TupRequestBuilder::createFrameRequest(k->initScene, k->initLayer, k->initFrame,
                                                    TupProjectRequest::Select, selection);
    emit requested(&request);

    setCurrentTween(name);
    TOsd::self()->display(tr("Info"), tr("Tween %1 applied!").arg(name), TOsd::Info);

    QApplication::restoreOverrideCursor();

    /*
    // SQA: Debugging code
    // QColor color = TAlgorithm::randomColor(false);
    QColor color = Qt::red;
    QPolygonF points = k->path->path().toFillPolygon();
    points.removeLast();
    for (int i=0; i<points.size(); i++) {
        QPen inkPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        TupEllipseItem *ellipse = new TupEllipseItem(QRectF(points.at(i), QSize(3, 3)));
        ellipse->setPen(inkPen);
        ellipse->setBrush(inkPen.brush());
        k->scene->addItem(ellipse);
    }
    */
}

/* This method updates the data of the path into the tool panel 
 * and disables edition mode for nodes
*/

void Tweener::updatePath()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::updatePath()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QPainterPath::Element e = k->path->path().elementAt(0);
    QPointF point = QPointF(e.x, e.y);
    if (point != k->firstNode) {
        int distanceX = point.x() - k->firstNode.x();
        int distanceY = point.y() - k->firstNode.y();
        k->firstNode = point;
        k->pathOffset = QPointF(distanceX, distanceY);

        if (k->objects.size() > 0) {
            int i = 0;
            foreach (QGraphicsItem *item, k->objects) {
                     item->moveBy(distanceX, distanceY);
                     if (i == 0) {
                         QRectF rect = item->sceneBoundingRect();
                         k->itemObjectReference = rect.center();
                     }
                     i++;
            }
        }
    }

    k->configurator->updateSteps(k->path);
    updateTweenPoints();
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::updateScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->mode = k->configurator->mode();

    if (k->mode == TupToolPlugin::Edit) {
        int total = k->initFrame + k->configurator->totalSteps();

        if (k->editMode == TupToolPlugin::Properties) {
            if (scene->currentFrameIndex() >= k->initFrame && scene->currentFrameIndex() < total) {
                if (k->path && k->nodesGroup) {
                    k->scene->addItem(k->path);            
                    k->nodesGroup->createNodes(k->path);
                    k->nodesGroup->show();
                    k->nodesGroup->resizeNodes(k->realFactor);
                    k->nodesGroup->expandAllNodes();

                    k->dots.clear();
                    paintTweenPoints();
                }
            }
        } 

        int framesNumber = framesCount();
        if (k->configurator->startComboSize() < framesNumber)
            k->configurator->initStartCombo(framesNumber, k->initFrame);

    } else if (k->mode == TupToolPlugin::Add) {
               int total = framesCount();
               if (k->configurator->startComboSize() < total) {
                   k->configurator->initStartCombo(total, k->initFrame);
               } else {
                   if (scene->currentFrameIndex() != k->initFrame)
                       k->configurator->setStartFrame(scene->currentFrameIndex());
               }

           if (k->editMode == TupToolPlugin::Properties) {
               k->path = 0;
               k->configurator->clearData();
               k->configurator->activateMode(TupToolPlugin::Selection);
               clearSelection();
               setSelection();
           } else if (k->editMode == TupToolPlugin::Selection) {
                      k->path = 0;
                      if (scene->currentFrameIndex() != k->initFrame)
                          clearSelection();
                      k->initFrame = scene->currentFrameIndex();
                      setSelection();
           } 
    } else {
        if (scene->currentFrameIndex() != k->initFrame) 
            k->configurator->setStartFrame(scene->currentFrameIndex());
    }
}

void Tweener::updateMode(TupToolPlugin::Mode mode)
{
    k->mode = mode;

    if (k->mode == TupToolPlugin::Edit)
        setEditEnv();
}

void Tweener::removeTweenFromProject(const QString &name)
{
    TupScene *scene = k->scene->scene();
    bool removed = scene->removeTween(name, TupItemTweener::Position);

    if (removed) {
        foreach (QGraphicsView * view, k->scene->views()) {
                 foreach (QGraphicsItem *item, view->scene()->items()) {
                          QString tip = item->toolTip();
                          if (tip.startsWith(tr("Position Tween") + ": " + name))
                              item->setToolTip("");
                 }
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "Tweener::removeTweenFromProject() - Position tween couldn't be removed -> " + name;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::setCurrentTween(const QString &name)
{
    TupScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, TupItemTweener::Position);
    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

void Tweener::setEditEnv()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::setEditEnv()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->initFrame = k->currentTween->initFrame();
    k->initLayer = k->currentTween->initLayer();
    k->initScene = k->currentTween->initScene();

    if (k->initFrame != k->scene->currentFrameIndex() || k->initLayer != k->scene->currentLayerIndex() || k->initScene != k->scene->currentSceneIndex()) {
        QString selection = QString::number(k->initLayer) + "," + QString::number(k->initLayer) + ","
                            + QString::number(k->initFrame) + "," + QString::number(k->initFrame);

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, k->initLayer, k->initFrame, TupProjectRequest::Select, selection);
        emit requested(&request);
    }

    k->mode = TupToolPlugin::Edit;

    TupScene *scene = k->scene->scene();
    k->objects = scene->getItemsFromTween(k->currentTween->name(), TupItemTweener::Position);

    if (!k->objects.isEmpty()) {
        QGraphicsItem *item = k->objects.at(0);
        QRectF rect = item->sceneBoundingRect();
        k->itemObjectReference = rect.center();

        k->path = k->currentTween->graphicsPath();
        k->path->setZValue(k->baseZValue);

        QPainterPath::Element e = k->path->path().elementAt(0);
        k->firstNode = QPointF(e.x, e.y);

        QPointF oldPos = QPointF(e.x, e.y);
        QPointF newPos = rect.center();

        int distanceX = newPos.x() - oldPos.x();
        int distanceY = newPos.y() - oldPos.y();
        k->path->moveBy(distanceX, distanceY);
        k->pathOffset = QPointF(distanceX, distanceY);

        QColor color(55, 155, 55, 200);
        QPen pen(QBrush(color), 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
        k->path->setPen(pen);

        setTweenPath();
    } else {
        #ifdef K_DEBUG
            QString msg = "Tweener::setEditEnv() - Fatal Error: Position tween wasn't found -> " + k->currentTween->name();
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

int Tweener::framesCount()
{
    int total = 1;
    TupLayer *layer = k->scene->scene()->layerAt(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesCount();

    return total;
}

/* This method clear selection */

void Tweener::clearSelection()
{
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 if (item->isSelected())
                     item->setSelected(false);
        }
        k->objects.clear();
        k->configurator->notifySelection(false);
    }
}

/* This method disables object selection */

void Tweener::disableSelection()
{
    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

void Tweener::sceneResponse(const TupSceneResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::sceneResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if ((event->action() == TupProjectRequest::Remove || event->action() == TupProjectRequest::Reset)
        && (k->scene->currentSceneIndex() == event->sceneIndex())) {
        init(k->scene);
    }

    if (event->action() == TupProjectRequest::Select)
        init(k->scene);
}

void Tweener::layerResponse(const TupLayerResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::layerResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (event->action() == TupProjectRequest::Remove)
        init(k->scene);        
}

void Tweener::frameResponse(const TupFrameResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::frameResponse()] " << event->frameIndex();
        #else
            T_FUNCINFO << event->frameIndex();
        #endif
    #endif

    if (event->action() == TupProjectRequest::Remove && k->scene->currentLayerIndex() == event->layerIndex()) {
        k->isPathInScene = false;
        init(k->scene);
        return;
    }

    if (event->action() == TupProjectRequest::Select) {
        if (k->mode == TupToolPlugin::Edit) {
            if (k->editMode == TupToolPlugin::Properties)
                paintTweenPoints();
        }

        if (k->initLayer != event->layerIndex() || k->initScene != event->sceneIndex()) {
            resetGUI();
            init(k->scene);
        }
    }
}

TupToolPlugin::Mode Tweener::currentMode()
{
    return k->mode;
}

TupToolPlugin::EditMode Tweener::currentEditMode()
{
    return k->editMode;
}

void Tweener::removeTweenPoints()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::removeTweenPoints()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int total = k->dots.size();
    for (int i=0; i<total; i++)
         k->scene->removeItem(k->dots.at(i));
    k->dots.clear();
}

void Tweener::paintTweenPoints()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Tweener::paintTweenPoints()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->dots.isEmpty()) {
        QColor color = Qt::black;
        QList<QPointF> points = k->configurator->tweenPoints();
        int total = points.size();
        for (int i=0; i<total; i++) {
             QPen inkPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
             TupEllipseItem *ellipse = new TupEllipseItem(QRectF(points.at(i) - QPointF(2,2), QSize(4, 4)));
             ellipse->setPen(inkPen);
             ellipse->setBrush(inkPen.brush());
             k->scene->addItem(ellipse);
             k->dots << ellipse;
        }
    } 
}

void Tweener::updateTweenPoints()
{
    removeTweenPoints();
    paintTweenPoints();
}

void Tweener::resizeNodes(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
    k->nodesGroup->resizeNodes(scaleFactor);
}

void Tweener::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}
