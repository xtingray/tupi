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

#include <QPointF>
#include <QKeySequence>
#include <QMatrix>
#include <QGraphicsView>
#include <QDomDocument>

#include "tglobal.h"
#include "tdebug.h"
#include "taction.h"
#include "tosd.h"
#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicsscene.h"
#include "tupgraphicobject.h"
#include "tupsvgitem.h"
#include "tupitemtweener.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupscene.h"
#include "tuplayer.h"

struct Tweener::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    TupGraphicsScene *scene;
    QList<QGraphicsItem *> objects;

    TupItemTweener *currentTween;
    int startPoint;

    TupToolPlugin::Mode mode;
    TupToolPlugin::EditMode editMode;
};

Tweener::Tweener() : TupToolPlugin(), k(new Private)
{
    setupActions();

    k->configurator = 0;
    k->startPoint = 0;
}

Tweener::~Tweener()
{
    delete k;
}

/* This method initializes the plugin */

void Tweener::init(TupGraphicsScene *scene)
{
    k->scene = scene;
    k->objects.clear();

    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(TupItemTweener::Opacity);
    if (tweenList.size() > 0) {
        k->configurator->loadTweenList(tweenList);
        setCurrentTween(tweenList.at(0));
    }

    int total = framesTotal();
    k->configurator->initStartCombo(total, k->scene->currentFrameIndex());
}

void Tweener::updateStartPoint(int index)
{
     if (k->startPoint != index && index >= 0)
         k->startPoint = index;
}

/* This method returns the plugin name */

QStringList Tweener::keys() const
{
    return QStringList() << tr("Opacity Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
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
        if (k->editMode == TupToolPlugin::Selection) {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);
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

        connect(k->configurator, SIGNAL(startingPointChanged(int)), this, SLOT(updateStartPoint(int)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedDefineProperties()), this, SLOT(setPropertiesMode()));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(setMode(TupToolPlugin::Mode)), this, SLOT(updateMode(TupToolPlugin::Mode))); 
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
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
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    TAction *translater = new TAction(QPixmap(kAppProp->themeDir() + "icons/opacity_tween.png"), 
                                      tr("Opacity Tween"), this);
    translater->setCursor(QCursor(kAppProp->themeDir() + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+O")));

    k->actions.insert(tr("Opacity Tween"), translater);
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(TupGraphicsScene *scene)
{
    tFatal() << "Tweener::updateScene() - Just tracing!";
    k->mode = k->configurator->mode();
}

void Tweener::setCurrentTween(const QString &name)
{
    TupScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, TupItemTweener::Opacity);
    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

int Tweener::framesTotal()
{
    int total = 1;
    TupLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesTotal();

    return total;
}

/* This method clears selection */

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

void Tweener::setSelect()
{
    tFatal() << "Tweener::setSelect() - Selection mode activated!";

    if (k->mode == TupToolPlugin::Edit) {
        if (k->startPoint != k->scene->currentFrameIndex()) {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, TupProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    k->editMode = TupToolPlugin::Selection;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if ((item->zValue() >= 20000) && (item->toolTip().length()==0)) {
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
        k->configurator->notifySelection(true);
    }

}

void Tweener::setPropertiesMode()
{
    k->editMode = TupToolPlugin::Properties;
    disableSelection();

    if (k->objects.isEmpty()) {
        k->objects = k->scene->scene()->getItemsFromTween(k->currentTween->name(), TupItemTweener::Opacity);
    }
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    disableSelection();
    clearSelection();

    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    k->startPoint = k->scene->currentFrameIndex();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Tween name is missing!"), TOsd::Error);
        return;
    }

    if (k->startPoint != k->scene->currentFrameIndex()) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                       k->scene->currentLayerIndex(),
                                                                       k->startPoint, TupProjectRequest::Select, "1");
        emit requested(&request);
    }

    if (!k->scene->scene()->tweenExists(name, TupItemTweener::Opacity)) {

        foreach (QGraphicsItem *item, k->objects) {

                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item);

                 if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                     type = TupLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 }

                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), k->scene->spaceMode(), 
                                            type, TupProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint));
                 emit requested(&request);
        }

        int framesNumber = framesTotal();

        int total = k->startPoint + k->configurator->totalSteps() - 1;

        if (total >= framesNumber) {
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

                 if (k->startPoint != k->currentTween->startFrame()) {
                     QDomDocument dom;
                     dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));

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
                                            QPointF(), k->scene->spaceMode(), type,
                                            TupProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint));
                 emit requested(&request);

                 int total = k->startPoint + k->configurator->totalSteps();

                 int framesNumber = framesTotal();

                 if (total >= framesNumber) {
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

void Tweener::removeTweenFromProject(const QString &name)
{
    TupScene *scene = k->scene->scene();
    scene->removeTween(name, TupItemTweener::Opacity);

    foreach (QGraphicsView * view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      QString tip = item->toolTip();
                      if (tip.startsWith(tr("Opacity Tween") + ": " + name))
                          item->setToolTip("");
             }
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::updateMode(TupToolPlugin::Mode mode)
{
    tFatal() << "Tweener::updateMode() - Updating mode: " << mode;

    k->mode = mode;

    if (k->mode == TupToolPlugin::Edit) {
        k->startPoint = k->currentTween->startFrame();
        if (k->startPoint != k->scene->currentFrameIndex()) {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, TupProjectRequest::Select, "1");
            emit requested(&request);
        }
    }
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
