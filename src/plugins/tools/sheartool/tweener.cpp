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
#include "configurator.h"
#include "target.h"

#include <QPointF>
#include <QKeySequence>
#include <QMatrix>
#include <QGraphicsView>
#include <QDomDocument>

#include "tglobal.h"
#include "tdebug.h"
#include "taction.h"
#include "tosd.h"
#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicsscene.h"
#include "ktgraphicobject.h"
#include "ktsvgitem.h"
#include "ktpathitem.h"
#include "ktpixmapitem.h"
#include "ktitemtweener.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktscene.h"
#include "ktlayer.h"

struct Tweener::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    KTGraphicsScene *scene;
    QList<QGraphicsItem *> objects;

    KTItemTweener *currentTween;
    int startPoint;

    QPointF origin;
    Target *target;

    Settings::Mode mode;
    Settings::EditMode editMode;
};

Tweener::Tweener() : KTToolPlugin(), k(new Private)
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

void Tweener::init(KTGraphicsScene *scene)
{
    k->scene = scene;
    k->objects.clear();

    k->mode = Settings::View;
    k->editMode = Settings::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(KTItemTweener::Shear);
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
    return QStringList() << tr("Shear Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method is executed while the mouse is pressed and on movement */

void Tweener::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method finishes the action started on the press method depending
 * on the active mode: Selecting an object or Creating a path
*/

void Tweener::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->startPoint) {
        if (k->editMode == Settings::Selection) {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);
                QGraphicsItem *item = k->objects.at(0);
                QRectF rect = item->sceneBoundingRect();
                k->origin = rect.center();
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
    return KTToolInterface::Tweener;
}

/* This method returns the tool panel associated to this plugin */

QWidget *Tweener::configurator()
{
    if (!k->configurator) {
        k->mode = Settings::View;

        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(startingPointChanged(int)), this, SLOT(updateStartPoint(int)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(setMode(Settings::Mode)), this, SLOT(updateMode(Settings::Mode)));
        connect(k->configurator, SIGNAL(clickedDefineProperties()), this, SLOT(setPropertiesMode()));
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
    } 

    return k->configurator;
}

/* This method is called when there's a change on/of scene */
void Tweener::aboutToChangeScene(KTGraphicsScene *)
{
}

/* This method is called when this plugin is off */

void Tweener::aboutToChangeTool()
{
    if (k->editMode == Settings::Selection) {
        clearSelection();
        disableSelection();

        return;
    }

    if (k->editMode == Settings::Properties)
        k->scene->removeItem(k->target);
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    TAction *translater = new TAction(QPixmap(THEME_DIR + "icons/shear_tween.png"), 
                                      tr("Shear Tween"), this);
    translater->setCursor(QCursor(THEME_DIR + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+H")));

    k->actions.insert(tr("Shear Tween"), translater);
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(KTGraphicsScene *scene)
{ 
    k->mode = k->configurator->mode();

    if (k->mode == Settings::Edit) {

       int framesNumber = framesTotal();

       if (k->configurator->startComboSize() < framesNumber)
           k->configurator->initStartCombo(framesNumber, k->startPoint);

       if (scene->currentFrameIndex() == k->startPoint)
           k->scene->addItem(k->target);

    } else if (k->mode == Settings::Add) {

               int total = framesTotal();

               if (k->editMode == Settings::Properties) {
                   if (total > k->configurator->startComboSize()) {
                       k->configurator->activatePropertiesMode(Settings::Selection);
                       clearSelection();
                       setSelect();
                   } 
               } else if (k->editMode == Settings::Selection) {
                          if (scene->currentFrameIndex() != k->startPoint)
                              clearSelection();
                          k->startPoint = scene->currentFrameIndex();
                          setSelect();
               }

               if (k->configurator->startComboSize() < total) {
                   k->configurator->initStartCombo(total, k->startPoint);
               } else {
                   if (scene->currentFrameIndex() != k->startPoint)
                       k->configurator->setStartFrame(scene->currentFrameIndex());
               }

    } else {
             if (scene->currentFrameIndex() != k->startPoint)
                 k->configurator->setStartFrame(scene->currentFrameIndex());
    }
}

void Tweener::setCurrentTween(const QString &name)
{
    KTScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, KTItemTweener::Shear);

    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

int Tweener::framesTotal()
{
    int total = 1;
    KTLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesNumber();

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

/* This method initializes the "Select object" mode */

void Tweener::setSelect()
{
    if (k->editMode == Settings::Properties)
        k->scene->removeItem(k->target);

    if (k->mode == Settings::Edit) {
        if (k->startPoint != k->scene->currentFrameIndex()) {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, KTProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    k->editMode = Settings::Selection;

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
        k->configurator->notifySelection(true);
    }

}

void Tweener::setPropertiesMode()
{
    k->editMode = Settings::Properties;
    disableSelection();

    if (k->objects.isEmpty()) {
        k->objects = k->scene->scene()->getItemsFromTween(k->currentTween->name(), KTItemTweener::Shear);
        k->origin = k->currentTween->transformOriginPoint();
    }

    addTarget();
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    if ((k->mode == Settings::Edit || k->mode == Settings::Add) && k->editMode == Settings::Properties)
        k->scene->removeItem(k->target);

    disableSelection();
    clearSelection();

    k->mode = Settings::View;
    k->editMode = Settings::None;

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
        KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                       k->scene->currentLayerIndex(),
                                                                       k->startPoint, KTProjectRequest::Select, "1");
        emit requested(&request);
    }

    if (!k->scene->scene()->tweenExists(name, KTItemTweener::Shear)) {

        foreach (QGraphicsItem *item, k->objects) {

                 KTLibraryObject::Type type = KTLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item);
                 QRectF rect = item->sceneBoundingRect();
                 QPointF origin = item->mapFromParent(k->origin);

                 if (KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item)) {
                     type = KTLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<KTPathItem *>(item))
                         origin = k->origin;
                 }

                 KTProjectRequest request = KTRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), type,
                                            KTProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint, origin));
                 emit requested(&request);
        }

        int framesNumber = framesTotal();

        int total = k->startPoint + k->configurator->totalSteps() - 1;

        if (total >= framesNumber) {
            for (int i = framesNumber; i <= total; i++) {
                 KTProjectRequest requestFrame = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                   k->scene->currentLayerIndex(),
                                                                   i, KTProjectRequest::Add, tr("Frame %1").arg(i + 1));
                 emit requested(&requestFrame);
            }
        }

        KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                        k->scene->currentLayerIndex(),
                                                                        k->startPoint, KTProjectRequest::Select, "1");
        emit requested(&request);

    } else {

        removeTweenFromProject(name);
        QList<QGraphicsItem *> newList;

        foreach (QGraphicsItem *item, k->objects) {

                 KTLibraryObject::Type type = KTLibraryObject::Item;
                 KTScene *scene = k->scene->scene();
                 KTLayer *layer = scene->layer(k->scene->currentLayerIndex());
                 KTFrame *frame = layer->frame(k->currentTween->startFrame());
                 int objectIndex = frame->indexOf(item);

                 QPointF origin = item->mapFromParent(k->origin);

                 if (KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item)) {
                     type = KTLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<KTPathItem *>(item))
                         origin = k->origin;
                 }

                 if (k->startPoint != k->currentTween->startFrame()) {
                     QDomDocument dom;
                     dom.appendChild(dynamic_cast<KTAbstractSerializable *>(item)->toXml(dom));

                     KTProjectRequest request = KTRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                                                                    k->scene->currentLayerIndex(),
                                                                                    k->startPoint, -1,
                                                                                    QPointF(), type, KTProjectRequest::Add,
                                                                                    dom.toString());
                     emit requested(&request);

                     request = KTRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                                                   k->scene->currentLayerIndex(),
                                                                   k->currentTween->startFrame(),
                                                                   objectIndex, QPointF(), type,
                                                                   KTProjectRequest::Remove);
                     emit requested(&request);

                     frame = layer->frame(k->startPoint);
                     if (type == KTLibraryObject::Item)
                         objectIndex = frame->graphicItemsCount() - 1;
                     else
                         objectIndex = frame->svgItemsCount() - 1;

                     newList.append(frame->graphic(objectIndex)->item());
                 }

                 KTProjectRequest request = KTRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), type,
                                            KTProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint, origin));
                 emit requested(&request);

                 int total = k->startPoint + k->configurator->totalSteps();

                 int framesNumber = framesTotal();

                 if (total >= framesNumber) {
                     for (int i = framesNumber; i < total; i++) {
                          KTProjectRequest requestFrame = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                          k->scene->currentLayerIndex(),
                                                          i, KTProjectRequest::Add, tr("Frame %1").arg(i + 1));
                          emit requested(&requestFrame);
                     }
                 }

                 request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(),
                                                                k->startPoint, KTProjectRequest::Select, "1");
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
    KTScene *scene = k->scene->scene();
    scene->removeTween(name, KTItemTweener::Shear);

    foreach (QGraphicsView * view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      QString tip = item->toolTip();
                      if (tip.startsWith(tr("Shear Tween") + ": " + name))
                          item->setToolTip("");
             }
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::updateOriginPoint(const QPointF &point)
{
    k->origin = point;
}

void Tweener::addTarget()
{
    if (k->mode == Settings::Add) {
        k->target = new Target(k->origin, maxZValue(), k->scene);
        connect(k->target, SIGNAL(positionUpdated(const QPointF &)), this, SLOT(updateOriginPoint(const QPointF &)));
    } else {
        if (k->objects.size() > 0) {
            QGraphicsItem *item = k->objects.at(0);
            k->origin = item->mapToParent(k->currentTween->transformOriginPoint());
            k->target = new Target(k->origin, maxZValue(), k->scene);
            connect(k->target, SIGNAL(positionUpdated(const QPointF &)), this, SLOT(updateOriginPoint(const QPointF &)));
        }
    }
}

void Tweener::updateMode(Settings::Mode mode)
{
    k->mode = mode;

    if (k->mode == Settings::Edit) {
        k->startPoint = k->currentTween->startFrame();
        if (k->startPoint != k->scene->currentFrameIndex()) {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, KTProjectRequest::Select, "1");
            emit requested(&request);
        }
    }
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

Q_EXPORT_PLUGIN2(kt_tweener, Tweener);
