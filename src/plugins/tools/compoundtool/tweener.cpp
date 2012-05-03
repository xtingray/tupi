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

#include "tweener.h"

#include "tglobal.h"
#include "tdebug.h"
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
#include "tupitemtweener.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupscene.h"
#include "tuplayer.h"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QDomDocument>

struct Tweener::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    TupGraphicsScene *scene;
    QGraphicsPathItem *path;
    QList<QGraphicsItem *> objects;

    TupItemTweener *currentTween;

    TNodeGroup *group;

    bool pathAdded;
    int startPoint;

    TweenerPanel::Mode mode;
    TweenerPanel::TweenerType currentTweenType;
    TweenerPanel::EditMode editMode;

    QPointF itemObjectReference;
    QPointF pathOffset;
    QPointF firstNode;
};

Tweener::Tweener() : TupToolPlugin(), k(new Private)
{
    setupActions();

    k->configurator = 0;
    k->path = 0;
    k->group = 0;
    k->startPoint = 0;
}

Tweener::~Tweener()
{
    delete k;
}

/* This method initializes the plugin */

void Tweener::init(TupGraphicsScene *scene)
{
    delete k->path;
    k->path = 0;
    k->pathAdded = false;
    delete k->group;
    k->group = 0;

    k->scene = scene;
    k->objects.clear();

    k->pathOffset = QPointF(0, 0); 
    k->firstNode = QPointF(0, 0);
    k->itemObjectReference = QPointF(0, 0);

    k->mode = TweenerPanel::View;
    k->editMode = TweenerPanel::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(TupItemTweener::Compound);
    if (tweenList.size() > 0) {
        k->configurator->loadTweenList(tweenList);
        setCurrentTween(tweenList.at(0));
    }

    int total = framesTotal();
    k->configurator->initStartCombo(total, k->scene->currentFrameIndex());
}

void Tweener::updateStartPoint(int index)
{
    if (k->startPoint != index && index >= 0) {
        tFatal() << "Tweener::updateStartPoint() - New Start Point: " << k->startPoint;
        k->startPoint = index;
    }
}

/* This method returns the plugin name */

QStringList Tweener::keys() const
{
    return QStringList() << tr("Compound Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->editMode == TweenerPanel::TweenProperties && k->scene->currentFrameIndex() == k->startPoint) {
        if (k->currentTweenType == TweenerPanel::Position) {
            if (k->path) {
                QPointF point = k->path->mapFromParent(input->pos());
                QPainterPath path = k->path->path();
                path.cubicTo(point, point, point);
                k->path->setPath(path);
            }
        } else {
            tFatal() << "Tweener::press() - No position!";
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
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->startPoint) {

        if (k->editMode == TweenerPanel::TweenProperties) {

            if (k->currentTweenType == TweenerPanel::Position) {

                if (k->group) {
                    k->group->createNodes(k->path);
                    k->group->expandAllNodes();
                    k->configurator->updateSteps(k->path, k->pathOffset);
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
                }
            } else {
                tFatal() << "Tweener::release() - No position!";
            }

        } else {

            // if k->editMode == TweenerPanel::TweenSelection

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
                    k->path->setZValue(maxZValue());

                    QColor color = Qt::lightGray;
                    color.setAlpha(200);
                    QPen pen(QBrush(color), 1, Qt::DotLine);
                    k->path->setPen(pen);

                    QPainterPath path;
                    path.moveTo(newPos);
                    k->firstNode = newPos;
                    k->path->setPath(path);
                    scene->addItem(k->path);
                    k->pathAdded = true;

                    k->pathOffset = QPointF(0, 0);
                } else {
                    int distanceX = newPos.x() - oldPos.x();
                    int distanceY = newPos.y() - oldPos.y();
                    k->path->moveBy(distanceX, distanceY);

                    k->pathOffset = QPointF(distanceX, distanceY);
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
        k->mode = TweenerPanel::View;

        k->configurator = new Configurator;

        connect(k->configurator, SIGNAL(tweenPropertiesActivated(TweenerPanel::TweenerType)), 
                this, SLOT(updateCurrentTweenerType(TweenerPanel::TweenerType)));

        connect(k->configurator, SIGNAL(startingPointChanged(int)), this, SLOT(updateStartPoint(int)));

        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedTweenProperties()), this, SLOT(tweenListMode()));

        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(loadPath(bool, bool)), this, SLOT(setPath(bool, bool)));

        connect(k->configurator, SIGNAL(setMode(TweenerPanel::Mode)), this, SLOT(updateMode(TweenerPanel::Mode)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));

    } else {
        k->mode = k->configurator->mode();
    }

    return k->configurator;
}

/* This method is called when there's a change on/of scene */
void Tweener::aboutToChangeScene(TupGraphicsScene *)
{
}

/* This method is called when this plugin is off */
void Tweener::aboutToChangeTool()
{
    if (k->editMode == TweenerPanel::Selection) {
        clearSelection();
        disableSelection();
    } else if (k->editMode == TweenerPanel::TweenProperties) {
        if (k->currentTweenType == TweenerPanel::Position) {
            if (k->path) {
                tFatal() << "Tweener::aboutToChangeTool() - Removing path!";
                k->scene->removeItem(k->path);
                k->pathAdded = false;
                delete k->group;
                k->group = 0;
            }
        }
    }

    k->mode = TweenerPanel::View;
    k->editMode = TweenerPanel::None;
    k->currentTweenType = TweenerPanel::Undefined;
}

/* SQA: What is it? 
bool Tweener::isComplete() const
{
    return true;
}
*/

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    TAction *translater = new TAction(QPixmap(kAppProp->themeDir() + "/"  + "icons/compound_tween.png"), 
                                      tr("Compound Tween"), this);
    translater->setCursor(QCursor(kAppProp->themeDir() + "/"  + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+X")));

    k->actions.insert(tr("Compound Tween"), translater);
}

/* This method initializes the "Create path" mode */

void Tweener::setCreatePath()
{
    if (k->path) {

        k->pathOffset = QPointF(0, 0);

        if (!k->pathAdded) {
            k->scene->addItem(k->path);
            k->pathAdded = true;
        } 

        if (k->group) {
            k->group->createNodes(k->path);
        } else {
            k->group = new TNodeGroup(k->path, k->scene, TNodeGroup::CompoundTween);
            connect(k->group, SIGNAL(nodeReleased()), SLOT(updatePath()));
            k->group->createNodes(k->path);
        }
        k->group->expandAllNodes();

    }

    disableSelection();
}

/* This method initializes the "Select object" mode */

void Tweener::setSelect()
{
    if (k->mode == TweenerPanel::Edit) {
        if (k->startPoint != k->scene->currentFrameIndex()) {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, TupProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    k->editMode = TweenerPanel::Selection;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if ((item->zValue() >= 10000) && (item->toolTip().length()==0)) {
                          item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                      }
             }
    }

    // When Object selection is enabled, previous selection is set
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                 item->setSelected(true);
        }
    }
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    tFatal() << "Tweener::applyReset() - Fire in the hole!";

    k->mode = TweenerPanel::View;
    k->editMode = TweenerPanel::None;
    k->currentTweenType = TweenerPanel::Undefined;

    clearSelection();
    disableSelection();

    k->pathAdded = false;
    k->path = 0;

    k->startPoint = k->scene->currentFrameIndex();
    k->configurator->cleanTweensForms();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Tween name is missing!"), TOsd::Error);
        return;
    }

    // SQA: Remove this code
    /*
    if (k->startPoint != k->scene->currentFrameIndex()) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                       k->scene->currentLayerIndex(),
                                                                       k->startPoint, TupProjectRequest::Select, "1");
        emit requested(&request);
    }
    */

    if (!k->scene->scene()->tweenExists(name, TupItemTweener::Compound))
        tFatal() << "Tweener::applyTween() - Tween " << name << " is NEW!!!"; 
    else
        tFatal() << "Tweener::applyTween() - Tween " << name << " is NOT NEW!!!";
  
    if (!k->scene->scene()->tweenExists(name, TupItemTweener::Compound)) {

        foreach (QGraphicsItem *item, k->objects) {   

                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item); 
                 QRectF rect = item->sceneBoundingRect();
                 QPointF point = rect.topLeft();

                 if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                     type = TupLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<TupPathItem *>(item) || qgraphicsitem_cast<TupEllipseItem *>(item) 
                         || qgraphicsitem_cast<TupLineItem *>(item) || qgraphicsitem_cast<TupRectItem *>(item))
                         point = item->pos();
                 }

                 tFatal() << "Tweener::applyTween() - Point 1: [" << point.x() << ", " << point.y() << "]";
                 QDomDocument dom;
                 dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));
                 tFatal() << "";
                 tFatal() << "Tweener::applyTween() - " << dom.toString();

                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), k->scene->spaceMode(), 
                                            type, TupProjectRequest::SetTween, 
                                            k->configurator->tweenToXml(k->startPoint, point));
                 emit requested(&request);
        }

        int framesNumber = framesTotal();
        int total = k->startPoint + k->configurator->totalSteps() - 1;

        if (total > framesNumber) {
            for (int i = framesNumber; i <= total; i++) {
                 TupProjectRequest requestFrame = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                                      k->scene->currentLayerIndex(),
                                                                                      i, TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                 emit requested(&requestFrame);
            }
        }

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(), 
                                                                        k->scene->currentLayerIndex(),
                                                                        k->startPoint, TupProjectRequest::Select, "1");
        emit requested(&request);

    } else {

        removeTweenFromProject(name);
        QList<QGraphicsItem *> newList;

        foreach (QGraphicsItem *item, k->objects) {

                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 TupScene *scene = k->scene->scene();
                 TupLayer *layer = scene->layer(k->scene->currentLayerIndex());
                 TupFrame *frame = layer->frame(k->currentTween->startFrame());
                 int objectIndex = frame->indexOf(item);

                 QRectF rect = item->sceneBoundingRect();
                 QPointF point = rect.topLeft();

                 if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                     type = TupLibraryObject::Svg;
                     objectIndex = frame->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<TupPathItem *>(item) || qgraphicsitem_cast<TupEllipseItem *>(item) 
                         || qgraphicsitem_cast<TupLineItem *>(item) || qgraphicsitem_cast<TupRectItem *>(item))
                         point = item->pos();
                 }

                 if (k->startPoint != k->currentTween->startFrame()) {
                     QDomDocument dom;
                     dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));

                     tFatal() << "Tweener::applyTween() - Point 2: [" << point.x() << ", " << point.y() << "]";
                     tFatal() << "";
                     tFatal() << "Tweener::applyTween() - " << dom.toString();

                     TupProjectRequest request = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), 
                                                                                    k->scene->currentLayerIndex(), 
                                                                                    k->startPoint, -1, 
                                                                                    QPointF(), k->scene->spaceMode(),
                                                                                    type, TupProjectRequest::Add, 
                                                                                    dom.toString());
                     emit requested(&request);

                     request = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(), 
                                                                   k->scene->currentLayerIndex(),
                                                                   k->currentTween->startFrame(),
                                                                   objectIndex, QPointF(), 
                                                                   k->scene->spaceMode(), type,
                                                                   TupProjectRequest::Remove);
                     emit requested(&request);

                     frame = layer->frame(k->startPoint);
                     if (type == TupLibraryObject::Item)
                         objectIndex = frame->graphicItemsCount() - 1;
                     else
                         objectIndex = frame->svgItemsCount() - 1;

                     newList.append(frame->graphic(objectIndex)->item());
                 }

                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), k->scene->spaceMode(), 
                                            type, TupProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint, point));
                 emit requested(&request);

                 int total = k->startPoint + k->configurator->totalSteps();

                 int framesNumber = framesTotal();

                 if (framesNumber < total) {
                     for (int i = framesNumber; i < total; i++) {
                          TupProjectRequest requestFrame = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                          k->scene->currentLayerIndex(),
                                                          i, TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                          emit requested(&requestFrame);
                     }
                 }

                 request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(),
                                                                k->startPoint, TupProjectRequest::Select, "1");
                 emit requested(&request);
        }

        if (newList.size() > 0)
            k->objects = newList;
    }

    setCurrentTween(name);

    TOsd::self()->display(tr("Info"), tr("Tween %1 applied!").arg(name), TOsd::Info);
}

/* This method updates the data of the path into the tool panel 
 * and disables edition mode for nodes
*/

void Tweener::updatePath()
{
    k->configurator->updateSteps(k->path, k->pathOffset);
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(TupGraphicsScene *scene)
{
    k->mode = k->configurator->mode();

    if (k->mode == TweenerPanel::Edit) {

       tFatal() << "Tweener::updateScene() - Mode: TweenerPanel::Edit";

       int total = k->startPoint + k->configurator->totalSteps();

       if (k->editMode == TweenerPanel::TweenProperties) {
           if (k->currentTweenType == TweenerPanel::Position) {
               if (scene->currentFrameIndex() >= k->startPoint && scene->currentFrameIndex() < total) {
                   if (k->path && k->group) {
                       k->scene->addItem(k->path);            
                       k->group->createNodes(k->path);
                       k->group->expandAllNodes();
                   }
               }
           }
       } 

       int framesNumber = framesTotal();

       if (k->configurator->startComboSize() < framesNumber)
           k->configurator->initStartCombo(framesNumber, k->startPoint);

    } else if (k->mode == TweenerPanel::Add) {

               tFatal() << "Tweener::updateScene() - Mode: TweenerPanel::Add";

               int total = framesTotal();

               if (k->configurator->startComboSize() < total) {
                   k->configurator->initStartCombo(total, k->startPoint);
               } else {
                   if (scene->currentFrameIndex() != k->startPoint)
                       k->configurator->setStartFrame(scene->currentFrameIndex());
               }

               if (k->editMode == TweenerPanel::TweenProperties) {

                   if (k->currentTweenType == TweenerPanel::Position)
                       k->path = 0;

                   k->configurator->cleanTweensForms();

                   clearSelection();
                   k->configurator->activateMode(TweenerPanel::Selection);

               } else if (k->editMode == TweenerPanel::Selection) {

                          if (k->currentTweenType == TweenerPanel::Position)                       
                              k->path = 0;

                          if (scene->currentFrameIndex() != k->startPoint) {
                              clearSelection();
                              k->startPoint = scene->currentFrameIndex();
                              setSelect();
                          }

               } else if (k->editMode == TweenerPanel::TweenList) {

                          if (scene->currentFrameIndex() != k->startPoint) {
                              k->startPoint = scene->currentFrameIndex(); 
                              clearSelection();
                              k->configurator->activateMode(TweenerPanel::Selection);
                          }
               }

    } else {
             tFatal() << "Tweener::updateScene() - Mode: TweenerPanel::View";

             if (scene->currentFrameIndex() != k->startPoint) {
                 k->configurator->setStartFrame(scene->currentFrameIndex());
             }
    }
}

void Tweener::updateMode(TweenerPanel::Mode mode)
{
    k->mode = mode;

    if (k->mode == TweenerPanel::Edit)
        setEditEnv();
}

int Tweener::maxZValue()
{
    int max = -1;
    foreach (QGraphicsView *view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (item->zValue() > max)
                          max = item->zValue();

             }
    }

    return max + 1;
}

void Tweener::removeTweenFromProject(const QString &name)
{
    TupScene *scene = k->scene->scene();
    scene->removeTween(name, TupItemTweener::Compound);

    foreach (QGraphicsView * view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      QString tip = item->toolTip();
                      if (tip.startsWith(tr("Compound Tween") + ": " + name))
                          item->setToolTip("");
             }
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::setCurrentTween(const QString &name)
{
    tFatal() << "Tweener::setCurrentTween(Tweener::setCurrentTween() - Updating tweener: " << name;

    TupScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, TupItemTweener::Compound);
    if (k->currentTween) {
        k->configurator->setCurrentTween(k->currentTween);
    } 
}

void Tweener::setEditEnv()
{
    tFatal() << "void Tweener::setEditEnv() - Just tracing!!!";

    k->startPoint = k->currentTween->startFrame();
    if (k->startPoint != k->scene->currentFrameIndex()) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                       k->scene->currentLayerIndex(),
                                                                       k->startPoint, TupProjectRequest::Select, "1");
        emit requested(&request);
    }

    k->mode = TweenerPanel::Edit;

    TupScene *scene = k->scene->scene();
    k->objects = scene->getItemsFromTween(k->currentTween->name(), TupItemTweener::Compound);
    QGraphicsItem *item = k->objects.at(0);
    QRectF rect = item->sceneBoundingRect();
    k->itemObjectReference = rect.center();

    if (k->currentTween->contains(TupItemTweener::Position)) {

        tFatal() << "void Tweener::setEditEnv() - Adding path!";

        k->path = k->currentTween->graphicsPath();
        k->path->setZValue(maxZValue());

        QPainterPath::Element e  = k->path->path().elementAt(0);
        k->firstNode = QPointF(e.x, e.y);

        QPointF oldPos = QPointF(e.x, e.y);
        QPointF newPos = rect.center();

        int distanceX = newPos.x() - oldPos.x();
        int distanceY = newPos.y() - oldPos.y();
        k->path->moveBy(distanceX, distanceY);
        k->pathOffset = QPointF(distanceX, distanceY);

        QColor color = Qt::lightGray;
        color.setAlpha(200);
        QPen pen(QBrush(color), 1, Qt::DotLine);
        k->path->setPen(pen);

        // setCreatePath();
    }
}

int Tweener::framesTotal()
{
    int total = 1;
    TupLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesTotal();

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

void Tweener::updateCurrentTweenerType(TweenerPanel::TweenerType type)
{
    tFatal() << "updateCurrentTweenerType() - Just following type: " << type;
    k->currentTweenType = type;
    k->editMode = TweenerPanel::TweenProperties;

    if (k->currentTweenType == TweenerPanel::Position) {
        tFatal() << "Tweener::updateCurrentTweenerType() - Setting path!";
        setCreatePath();
    } else  {
        tFatal() << "Tweener::updateCurrentTweenerType() - Type is not Position!";
    }
}

void Tweener::setPath(bool isEnabled, bool reset)
{
    if (isEnabled) {
        k->editMode = TweenerPanel::TweenProperties;
        setCreatePath();
    } else {
        if (k->group) {
            k->group->clear();
            k->group = 0;
        }

        if (k->path) {

            if (k->startPoint == k->scene->currentFrameIndex())
                k->scene->removeItem(k->path);

            k->pathAdded = false;

            if (reset) {
                QPainterPath::Element e  = k->path->path().elementAt(0);
                QPointF newPos = QPointF(e.x, e.y);

                k->path = new QGraphicsPathItem;
                k->path->setZValue(maxZValue());

                QColor color = Qt::lightGray;
                color.setAlpha(200);
                QPen pen(QBrush(color), 1, Qt::DotLine);
                k->path->setPen(pen);

                QPainterPath path;
                path.moveTo(newPos);
                k->firstNode = newPos;
                k->path->setPath(path);
            }
        }
    }
}

void Tweener::tweenListMode()
{
   k->editMode = TweenerPanel::TweenList;
   disableSelection();
}

void Tweener::sceneResponse(const TupSceneResponse *event)
{
    if ((event->action() == TupProjectRequest::Remove || event->action() == TupProjectRequest::Reset)
        && (k->scene->currentSceneIndex() == event->sceneIndex())) {
        init(k->scene);
    }

    if (event->action() == TupProjectRequest::Select)
        init(k->scene);
}

void Tweener::layerResponse(const TupLayerResponse *event)
{
    if (event->action() == TupProjectRequest::Remove)
        init(k->scene);
}

void Tweener::frameResponse(const TupFrameResponse *event)
{
    if (event->action() == TupProjectRequest::Remove && k->scene->currentLayerIndex() == event->layerIndex())
        init(k->scene);
}

Q_EXPORT_PLUGIN2(tup_tweener, Tweener);
