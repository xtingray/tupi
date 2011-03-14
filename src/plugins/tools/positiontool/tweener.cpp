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
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QDomDocument>

#include "kglobal.h"
#include "kdebug.h"
#include "kaction.h"
#include "knodegroup.h"
#include "kosd.h"
#include "ktsvg2qt.h"
#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicsscene.h"
#include "ktgraphicobject.h"
#include "ktsvgitem.h"
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
    QGraphicsPathItem *path;
    QList<QGraphicsItem *> objects;

    KTItemTweener *currentTween;

    KNodeGroup *group;

    bool pathAdded;
    int startPoint;

    Settings::Mode mode;
    Settings::EditMode editMode;
};

Tweener::Tweener() : KTToolPlugin(), k(new Private)
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

void Tweener::init(KTGraphicsScene *scene)
{
    delete k->path;
    k->path = 0;
    delete k->group;
    k->group = 0;
    k->scene = scene;
    k->objects.clear();

    k->pathAdded = false;

    k->mode = Settings::View;
    k->editMode = Settings::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(KTItemTweener::Position);
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
    return QStringList() << tr("Position Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->editMode == Settings::Path && k->scene->currentFrameIndex() == k->startPoint) {
        if (!k->path) {
            k->path = new QGraphicsPathItem;
            QColor color = Qt::lightGray;
            color.setAlpha(200);
            QPen pen(QBrush(color), 1, Qt::DotLine);  
            k->path->setPen(pen);
            QPainterPath path;
            path.moveTo(input->pos());
            k->path->setPath(path);
        } else {
            QPainterPath path = k->path->path();
            path.cubicTo(input->pos(), input->pos(), input->pos());
            k->path->setPath(path);
        }
    } 
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

    // if (k->scene->currentFrameIndex() == k->startPoint) {
    if (scene->currentFrameIndex() == k->startPoint) {

        if (k->editMode == Settings::Path) {

            if (k->group) {
                k->group->createNodes(k->path);
                k->group->expandAllNodes();
                k->configurator->updateSteps(k->path);
            }

        } else {

            if (scene->selectedItems().size() > 0) {

                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);

                if (!k->path) {
                    k->path = new QGraphicsPathItem;
                    k->path->setZValue(maxZValue());

                    // QColor color(150, 150, 150, 200);
                    // QPen pen(QBrush(color), 1, Qt::DotLine);
                    // k->path->setPen(pen);

                    QColor color = Qt::lightGray;
                    color.setAlpha(200);
                    QPen pen(QBrush(color), 1, Qt::DotLine);
                    k->path->setPen(pen);

                    QGraphicsItem *item = k->objects.at(0);
                    QRectF rect = item->sceneBoundingRect();

                    QPainterPath path;
                    // path.moveTo(input->pos());
                    path.moveTo(rect.center());
                    k->path->setPath(path);
                    scene->addItem(k->path);
                    k->pathAdded = true;
                }
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
        connect(k->configurator, SIGNAL(clickedCreatePath()), this, SLOT(setCreatePath()));

        kFatal() << "Tweener::configurator() - Connecting signal clickedSelect()";
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        // connect(k->configurator, SIGNAL(selectionModeOn()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(editModeOn()), this, SLOT(setEditEnv())); 
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));

    } else {
        k->mode = k->configurator->mode();
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
        return;
    }

    if (k->editMode == Settings::Path) {
        if (k->path) {
            k->scene->removeItem(k->path);
            k->pathAdded = false;
            delete k->group;
            k->group = 0;
        }
        return;
    }
}

/* SQA: What is it? */

bool Tweener::isComplete() const
{
    return true;
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    KAction *translater = new KAction(QPixmap(THEME_DIR + "icons/position_tween.png"), 
                                      tr("Position Tween"), this);
    translater->setCursor(QCursor(THEME_DIR + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("Shift+W")));

    k->actions.insert(tr("Position Tween"), translater);
}

/* This method initializes the "Create path" mode */

void Tweener::setCreatePath()
{
    if (k->path) {

        if (!k->pathAdded) {
            k->scene->addItem(k->path);
            k->pathAdded = true;
        } 

        if (k->group) {
            k->group->createNodes(k->path);
        } else {
            k->group = new KNodeGroup(k->path, k->scene);
            connect(k->group, SIGNAL(nodeReleased()), SLOT(updatePath()));
            k->group->createNodes(k->path);
        }
        k->group->expandAllNodes();

    }

    k->editMode = Settings::Path;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (item != k->path) {
                          item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                          item->setFlag(QGraphicsItem::ItemIsMovable, false);
                      }
             }
    }
}

/* This method initializes the "Select object" mode */

void Tweener::setSelect()
{
    kFatal() << "Tweener::setSelect() - Just tracing... Position";

    if (k->mode == Settings::Edit) {
        if (k->startPoint != k->scene->currentFrameIndex()) {
            KTProjectRequest request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(),
                                                                            k->scene->currentLayerIndex(),
                                                                            k->startPoint, KTProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    if (k->path) {
        k->scene->removeItem(k->path);
        k->pathAdded = false;
        delete k->group;
        k->group = 0;
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

/* This method transforms the path created into a QString representation */

QString Tweener::pathToCoords()
{
    QString strPath = "";
    QChar t;

    for (int i=0; i < k->path->path().elementCount(); i++) {
         QPainterPath::Element e = k->path->path().elementAt(i);
         switch (e.type) {
            case QPainterPath::MoveToElement:
            {
                if (t != 'M') {
                    t = 'M';
                    strPath += "M " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::LineToElement:
            {
                if (t != 'L') {
                    t = 'L';
                    strPath += " L " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::CurveToElement:
            {
                if (t != 'C') {
                    t = 'C';
                    strPath += " C " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += "  " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::CurveToDataElement:
            {
                if (t == 'C')
                    strPath +=  " " + QString::number(e.x) + "  " + QString::number(e.y) + " ";
            }
            break;
        }
    }

    return strPath.trimmed();
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    k->mode = Settings::View;
    k->editMode = Settings::None;

    clearSelection();

    if (k->group) {
        k->group->clear();
        k->group = 0;
    }

    if (k->path) {
        if (k->startPoint == k->scene->currentFrameIndex())
            k->scene->removeItem(k->path);
        k->pathAdded = false;
        k->path = 0;
    }

    k->startPoint = k->scene->currentFrameIndex();
    k->configurator->cleanData();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        KOsd::self()->display(tr("Error"), tr("Tween name is missing!"), KOsd::Error);
        return;
    }

    if (!k->scene->scene()->tweenExists(name)) {

        foreach (QGraphicsItem *item, k->objects) {   

                 KTLibraryObject::Type type = KTLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item); 

                 if (KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item)) {
                     type = KTLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 }

                 QString route = pathToCoords();

                 KTProjectRequest request = KTRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), type,
                                            KTProjectRequest::SetTween, 
                                            k->configurator->tweenToXml(k->startPoint, item->transformOriginPoint(), route));
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

    } else {

        QList<QGraphicsItem *> newList;

        foreach (QGraphicsItem *item, k->objects) {

                 KTLibraryObject::Type type = KTLibraryObject::Item;
                 KTScene *scene = k->scene->scene();
                 KTLayer *layer = scene->layer(k->scene->currentLayerIndex());
                 KTFrame *frame = layer->frame(k->currentTween->startFrame());
                 int objectIndex = frame->indexOf(item);

                 if (KTSvgItem *svg = qgraphicsitem_cast<KTSvgItem *>(item)) {
                     type = KTLibraryObject::Svg;
                     objectIndex = frame->indexOf(svg);
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

                 QString route = pathToCoords();

                 KTProjectRequest request = KTRequestBuilder::createItemRequest(
                                            k->scene->currentSceneIndex(),
                                            k->scene->currentLayerIndex(),
                                            k->startPoint,
                                            objectIndex,
                                            QPointF(), type,
                                            KTProjectRequest::SetTween,
                                            k->configurator->tweenToXml(k->startPoint, item->transformOriginPoint(), route));
                 emit requested(&request);

                 int total = k->startPoint + k->configurator->totalSteps();

                 int framesNumber = framesTotal();

                 if (framesNumber < total) {
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
}

/* This method updates the data of the path into the tool panel 
 * and disables edition mode for nodes
*/

void Tweener::updatePath()
{
    k->configurator->updateSteps(k->path);
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

       int total = k->startPoint + k->configurator->totalSteps();

       if (k->editMode == Settings::Path) {
           if (scene->currentFrameIndex() >= k->startPoint && scene->currentFrameIndex() < total) {
               if (k->path && k->group) {
                   k->scene->addItem(k->path);            
                   k->group->createNodes(k->path);
                   k->group->expandAllNodes();
               }
           }
       } 

       int framesNumber = framesTotal();

       if (k->configurator->startComboSize() < framesNumber)
           k->configurator->initStartCombo(framesNumber, k->startPoint);

    } else if (k->mode == Settings::Add) {

               int total = framesTotal();

               if (k->configurator->startComboSize() < total) {
                   k->configurator->initStartCombo(total, k->startPoint);
               } else {
                   if (scene->currentFrameIndex() != k->startPoint)
                       k->configurator->setStartFrame(scene->currentFrameIndex());
               }

               if (k->editMode == Settings::Path) {

                       k->path = 0;
                       k->configurator->cleanData();
                       k->configurator->activateSelectionMode();
                       clearSelection();
                       setSelect();

               } else if (k->editMode == Settings::Selection) {
                       
                       k->path = 0;
                       if (scene->currentFrameIndex() != k->startPoint)
                           clearSelection();
                       k->startPoint = scene->currentFrameIndex();
                       setSelect();
               } 

    } else {
             if (scene->currentFrameIndex() != k->startPoint)
                 k->configurator->setStartFrame(scene->currentFrameIndex());
    }
}

int Tweener::maxZValue()
{
    int max = -1;
    foreach (QGraphicsItem *item, k->objects) {
             if (item->zValue() > max)
                 max = item->zValue();
    }

    return max + 1;
}

void Tweener::removeTween(const QString &name)
{
    KTScene *scene = k->scene->scene();
    scene->removeTween(name);

    applyReset();
}

void Tweener::setCurrentTween(const QString &name)
{
    KTScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name);
    if (k->currentTween) {
        k->configurator->setCurrentTween(k->currentTween);
    } 
}

void Tweener::setEditEnv()
{
    k->mode = Settings::Edit;

    KTScene *scene = k->scene->scene();
    k->objects = scene->getItemsFromTween(k->currentTween->name());
    k->path = k->currentTween->graphicsPath();

    k->path->setZValue(maxZValue());

    QColor color = Qt::lightGray;
    color.setAlpha(200);
    QPen pen(QBrush(color), 1, Qt::DotLine);
    k->path->setPen(pen);

    setCreatePath();
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

Q_EXPORT_PLUGIN2(kt_tweener, Tweener);
