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
#include "settings.h"

#include <QPointF>
#include <QKeySequence>
#include <QMatrix>
#include <QGraphicsView>
#include <QDomDocument>

#include "kglobal.h"
#include "kdebug.h"
#include "kaction.h"
#include "kosd.h"
#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicsscene.h"
#include "ktgraphicobject.h"
#include "ktsvgitem.h"
#include "ktpathitem.h"
#include "ktitemtweener.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktscene.h"
#include "ktlayer.h"

struct Tweener::Private
{
    QMap<QString, KAction *> actions;
    Configurator *configurator;

    KTGraphicsScene *scene;
    QList<QGraphicsItem *> objects;

    KTItemTweener *currentTween;

    int startPoint;

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
    kFatal() << "Tweener::init() - Rotation / Just tracing!";

    k->scene = scene;
    k->objects.clear();

    k->mode = Settings::View;
    k->editMode = Settings::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(KTItemTweener::Rotation);
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
    return QStringList() << tr("Rotation Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
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
           K_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->startPoint) {
        if (k->editMode == Settings::Selection) {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);
            }
        }
    }

}

/* This method returns the list of actions defined in this plugin */

QMap<QString, KAction *> Tweener::actions() const
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
        connect(k->configurator, SIGNAL(editModeOn()), this, SLOT(setEditEnv()));
        connect(k->configurator, SIGNAL(clickedDefineAngle()), this, SLOT(setAngleMode()));
    } else {
        kFatal() << "Tweener::configurator() - There is configurator already!";
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
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    KAction *translater = new KAction(QPixmap(THEME_DIR + "icons/rotation_tween.png"), 
                                      tr("Rotation Tween"), this);
    translater->setCursor(QCursor(THEME_DIR + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+R")));

    k->actions.insert(tr("Rotation Tween"), translater);
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

       // int total = k->startPoint + k->configurator->totalSteps();
       int framesNumber = framesTotal();

       if (k->configurator->startComboSize() < framesNumber)
           k->configurator->initStartCombo(framesNumber, k->startPoint);

    } else if (k->mode == Settings::Add) {

               int total = framesTotal();

               if (k->editMode == Settings::AngleRange) {
                   if (total > k->configurator->startComboSize()) {
                       k->configurator->activateSelectionMode();
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
    k->currentTween = scene->tween(name);
    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

void Tweener::setEditEnv()
{
    kFatal() << "Tweener::setEditEnv() - Just tracing!";

    k->mode = Settings::Edit;

    KTScene *scene = k->scene->scene();
    k->objects = scene->getItemsFromTween(k->currentTween->name());
}

int Tweener::framesTotal()
{
    int total = 1;
    KTLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesNumber();

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

/* This method initializes the "Select object" mode */

void Tweener::setSelect()
{
    kFatal() << "Tweener::setSelect() - Rotation / Just tracing!";

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
    }

}

void Tweener::setAngleMode()
{
    k->editMode = Settings::AngleRange;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    k->mode = Settings::View;
    k->editMode = Settings::None;
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    kFatal() << "Tweener::applyTween() - Applying rotation tween!";

    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        KOsd::self()->display(tr("Error"), tr("Tween name is missing!"), KOsd::Error);
        return;
    }

    if (!k->scene->scene()->tweenExists(name)) {

        foreach (QGraphicsItem *item, k->objects) {

                 KTLibraryObject::Type type = KTLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item);
                 QRectF rect = item->sceneBoundingRect();
                 QPointF point = item->transformOriginPoint();
                 QPointF origin = QPointF(point.x() + (rect.width()/2), point.y() + (rect.height()/2));

                 if (KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item)) {
                     type = KTLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<KTPathItem *>(item))
                         origin = rect.center();
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

        if (total > framesNumber) {
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

    }

    setCurrentTween(name);
}

Q_EXPORT_PLUGIN2(kt_tweener, Tweener);
