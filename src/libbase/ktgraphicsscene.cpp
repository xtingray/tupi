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

#include "ktgraphicsscene.h"

#include <QGraphicsItem>
#include <QSvgRenderer>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include <ktscene.h>
#include <ktlayer.h>

#include "ktgraphicobject.h"
#include "ktitemgroup.h"
#include "ktprojectloader.h"
#include "ktitemfactory.h"
#include "kttoolplugin.h"
#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktframe.h"
#include "ktitemtweener.h"
#include "ktgraphiclibraryitem.h"
#include "ktsvgitem.h"
#include "ktpathitem.h"

#include "ktprojectresponse.h"

#include "kdebug.h"

#include "ktguideline.h"

/**
 * This class defines the data structure and methods for handling animation scenes.
 * Here is where the set of photograms of one scene are processed.
 * @author David Cuadrado <krawek@toonka.com>
*/

struct KTGraphicsScene::Private
{
    KTToolPlugin *tool;
    KTScene *scene;

    struct OnionSkin
     {
        int previous;
        int next;
        QHash<QGraphicsItem *, double> opacityMap;
     } onionSkin;

    struct FramePosition
     {
        int layer;
        int frame;
     } framePosition;

    KTBrushManager *brushManager;
    KTInputDeviceInformation *inputInformation;

    bool isDrawing;
    int layerCounter;
    int objectCounter;

    QList<KTLineGuide *> lines;
    KTProject::Mode spaceMode;   
};

KTGraphicsScene::KTGraphicsScene() : QGraphicsScene(), k(new Private)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    setItemIndexMethod(QGraphicsScene::NoIndex);

    k->framePosition.layer = -1;
    k->framePosition.frame = -1;
    k->spaceMode = KTProject::FRAMES_EDITION;

    setCurrentFrame(0, 0);

    k->onionSkin.next = 0;
    k->onionSkin.previous = 0;
    k->tool = 0;
    k->isDrawing = false;

    setBackgroundBrush(Qt::gray);

    k->inputInformation = new KTInputDeviceInformation(this);
    k->brushManager = new KTBrushManager(this);
}

KTGraphicsScene::~KTGraphicsScene()
{
    #ifdef K_DEBUG
           KEND;
    #endif

    clearFocus();
    clearSelection();

    foreach (QGraphicsView *view, this->views())
             view->setScene(0);

    foreach (QGraphicsItem *item, items())
             removeItem(item);

    delete k;
}

void KTGraphicsScene::setCurrentFrame(int layer, int frame)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    if ((frame != k->framePosition.frame && k->framePosition.frame >= 0) || 
        (layer != k->framePosition.layer && k->framePosition.layer >= 0)) { 
        if (k->tool->name().compare(tr("PolyLine")) == 0)
            k->tool->aboutToChangeScene(this);
    }

    k->framePosition.layer = layer;
    k->framePosition.frame = frame;

    foreach (QGraphicsView *view, views())
             view->setDragMode(QGraphicsView::NoDrag);
}

void KTGraphicsScene::drawCurrentPhotogram()
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    KTLayer *layer = k->scene->layer(k->framePosition.layer);
    int frames = layer->framesNumber();

    if (k->framePosition.frame >= frames)
        k->framePosition.frame = frames - 1;

    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawPhotogram(k->framePosition.frame);
    else
        drawBackground();
}

void KTGraphicsScene::drawPhotogram(int photogram)
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    Q_CHECK_PTR(k->scene);

    if (photogram < 0 || !k->scene) 
        return;

    cleanWorkSpace();

    bool valid = false;

    // Drawing frames from another layers

    for (int i=0; i < k->scene->layersTotal(); i++) {

         KTLayer *layer = k->scene->layer(i);

         if (layer->framesNumber() > 0 && photogram < layer->framesNumber()) {

             KTFrame *mainFrame = layer->frame(photogram);

             QString currentFrame = "";

             if (mainFrame) {
                 currentFrame = mainFrame->frameName();

                 if (layer) {
                     if (layer->isVisible()) {

                         // Painting the background
                         drawBackground();

                         // Painting previews frames
                         if (k->onionSkin.previous > 0 && photogram > 0) {
                             double opacityFactor = 0.5 / (double)qMin(layer->frames().count(), k->onionSkin.previous);
                             double opacity = 0.5;

                             int limit = photogram - k->onionSkin.previous;
                             if (limit < 0) 
                                 limit = 0;


                             QString frameBehind = ""; 
                             for (int frameIndex = photogram-1; frameIndex >= limit; frameIndex--) {
                                  KTFrame * frame = layer->frame(frameIndex);
                                  QString previousFrame = frame->frameName();
                                  if (frame && previousFrame.compare(currentFrame) != 0 
                                            && frameBehind.compare(previousFrame) != 0) {
                                      addFrame(frame, opacity, Preview);
                                  }

                                  frameBehind = previousFrame;
                                  opacity -= opacityFactor;
                             }

                         }

                         // Painting next frames
                         if (k->onionSkin.next > 0 && layer->framesNumber() > photogram+1) {
                             double opacityFactor = 0.5 / (double)qMin(layer->frames().count(), k->onionSkin.next);
                             double opacity = 0.5;

                             int limit = photogram + k->onionSkin.next;
                             if (limit >= layer->frames().count()) 
                                 limit = layer->frames().count() - 1;

                             QString frameLater = "";
                             for (int frameIndex = photogram+1; frameIndex <= limit; frameIndex++) {
                                  KTFrame * frame = layer->frame(frameIndex);
                                  QString nextFrame = frame->frameName();
                                  if (frame && nextFrame.compare(currentFrame) != 0 
                                            && frameLater.compare(nextFrame) != 0) {
                                      addFrame(frame, opacity, Next);
                                  }
                      
                                  frameLater = nextFrame;
                                  opacity -= opacityFactor;
                             }
                         }

                         // SQA: Crashpoint when layers are deleted 

                         valid = true;
                         k->layerCounter = i;
                         addFrame(mainFrame, Current);
                     }
                  }
              }
         }
    }

    // Drawing tweening objects

    if (valid) {
        addTweeningObjects(photogram);
        addSvgTweeningObjects(photogram);

        update();
    }

    if (k->tool)
        k->tool->updateScene(this);
}

void KTGraphicsScene::drawBackground()
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    Q_CHECK_PTR(k->scene);

    if (!k->scene)
        return;

    KTBackground *bg = k->scene->background();
    if (bg) {
        KTFrame *frame = bg->frame();
        if (frame)
            addFrame(frame, 1.0);
    }
}

void KTGraphicsScene::addFrame(KTFrame *frame, double opacity, Context mode)
{
    if (frame) {
        k->objectCounter = 0;

        QList<int> indexes = frame->itemIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             KTGraphicObject *object = frame->graphic(indexes.at(i));
             if (mode != Current) {
                 if (!object->hasTween())
                     addGraphicObject(object, opacity);
             } else {
                 addGraphicObject(object, opacity);
             }
        }

        indexes = frame->svgIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             KTSvgItem *object = frame->svg(indexes.at(i));
             addSvgObject(object, opacity);
        }
    }
}

void KTGraphicsScene::addGraphicObject(KTGraphicObject *object, double opacity)
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

        QGraphicsItem *item = object->item();
        k->onionSkin.opacityMap.insert(item, opacity);

        if (KTItemGroup *group = qgraphicsitem_cast<KTItemGroup *>(item))
            group->recoverChilds();

        if (! qgraphicsitem_cast<KTItemGroup *>(item->parentItem())) {

            item->setSelected(false);
            KTLayer *layer = k->scene->layer(k->framePosition.layer);

            if (layer) {

                KTFrame *frame = layer->frame(k->framePosition.frame);

                if (frame) {
                    item->setOpacity(opacity);
                    k->objectCounter++;
                    addItem(item);
                }
            }
        } 
}

void KTGraphicsScene::addSvgObject(KTSvgItem *svgItem, double opacity)
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    if (svgItem) {

        k->onionSkin.opacityMap.insert(svgItem, opacity);
        svgItem->setSelected(false);

        KTLayer *layer = k->scene->layer(k->framePosition.layer);

        if (layer) {

            KTFrame *frame = layer->frame(k->framePosition.frame);

            if (frame) {
                svgItem->setOpacity(opacity);
                k->objectCounter++;
                addItem(svgItem);
            }

        }
    } else {
        #ifdef K_DEBUG
               kFatal() << "KTGraphicsScene::addSvgObject() - Error: No SVG item!";
        #endif
    } 
} 

void KTGraphicsScene::addTweeningObjects(int photogram)
{
    QList<KTGraphicObject *> tweenList = k->scene->tweeningGraphicObjects();
    for (int i=0; i < tweenList.count(); i++) {

         KTGraphicObject *object = tweenList.at(i);

         if (object->frame()->layer()->isVisible()) {
             int origin = object->frame()->index();

             if (KTItemTweener *tween = object->tween()) {

                 int adjustX = object->item()->boundingRect().width()/2;
                 int adjustY = object->item()->boundingRect().height()/2;

                 if (origin == photogram) {

                     KTTweenerStep *stepItem = tween->stepAt(0);
                     object->item()->setToolTip(tr("Tween: %1/Step: 0").arg(tween->name()));

                     if (tween->type() == KTItemTweener::Position || tween->type() == KTItemTweener::All) {
                         if (qgraphicsitem_cast<KTPathItem *>(object->item())) {
                             object->setLastTweenPos(stepItem->position());
                             object->item()->setPos(QPointF(0, 0));
                         } else {
                             QPointF point(stepItem->position().x() - adjustX, stepItem->position().y() - adjustY);
                             object->item()->setPos(point);
                         }
                     }

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {

                            int step = photogram - origin;
                            KTTweenerStep *stepItem = tween->stepAt(step);
                            object->item()->setToolTip(tr("Tween: %1/Step: ").arg(tween->name()) + QString::number(step));

                            if (tween->type() == KTItemTweener::Position || tween->type() == KTItemTweener::All) {
                                if (qgraphicsitem_cast<KTPathItem *>(object->item())) {
                                    qreal dx = stepItem->position().x() - object->lastTweenPos().x();
                                    qreal dy = stepItem->position().y() - object->lastTweenPos().y();
                                    object->item()->moveBy(dx, dy);
                                    object->setLastTweenPos(stepItem->position());
                                } else {
                                    object->item()->setPos(stepItem->position().x() - adjustX, 
                                                           stepItem->position().y() - adjustY);
                                }
                            }

                            addGraphicObject(object);
                 }
             }
        }
    }
}

void KTGraphicsScene::addSvgTweeningObjects(int photogram)
{
    QList<KTSvgItem *> svgList = k->scene->tweeningSvgObjects();

    for (int i=0; i < svgList.count(); i++) {

         KTSvgItem *object = svgList.at(i);

         if (object->frame()->layer()->isVisible()) {
             int origin = object->frame()->index();

             if (KTItemTweener *tween = object->tween()) {

                 int adjustX = object->boundingRect().width()/2;
                 int adjustY = object->boundingRect().height()/2;

                 if (origin == photogram) {

                     KTTweenerStep *stepItem = tween->stepAt(0);
                     object->setToolTip(tr("Tween: %1/Step: 0").arg(tween->name()));

                     if (tween->type() == KTItemTweener::Position || tween->type() == KTItemTweener::All) {
                         QPointF point(stepItem->position().x() - adjustX, stepItem->position().y() - adjustY);
                         object->setPos(point);
                     }

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {
                             int step = photogram - origin;
                             KTTweenerStep *stepItem = tween->stepAt(step);
                             object->setToolTip(tr("Tween: %1/Step: ").arg(tween->name()) + QString::number(step));

                             if (tween->type() == KTItemTweener::Position || tween->type() == KTItemTweener::All) {
                                 QPointF point(stepItem->position().x() - adjustX, stepItem->position().y() - adjustY);
                                 object->setPos(point);
                             }

                             addSvgObject(object);
                 }
             } else {
                 #ifdef K_DEBUG
                           kFatal() << "KTGraphicsScene::addSvgTweeningObjects() - No tween found!";
                 #endif
             }
         }
    }
}

void KTGraphicsScene::cleanWorkSpace()
{
    k->onionSkin.opacityMap.clear();

    foreach (QGraphicsItem *item, items()) {
             if (item->scene() == this)
                 removeItem(item);
    }

    foreach (KTLineGuide *line, k->lines)
             addItem(line);
}

int KTGraphicsScene::currentFrameIndex() const
{
    return k->framePosition.frame;
}

int KTGraphicsScene::currentLayerIndex() const
{
    return k->framePosition.layer;
}

int KTGraphicsScene::currentSceneIndex() const
{
    if (!k->scene)
        return -1;

    return k->scene->objectIndex();
}

void KTGraphicsScene::setNextOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    k->onionSkin.next = n;
    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

void KTGraphicsScene::setPreviousOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    k->onionSkin.previous = n;
    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

KTFrame *KTGraphicsScene::currentFrame()
{
    if (k->scene) {

        if (k->scene->layersTotal() > 0) {

            if (k->framePosition.layer < k->scene->layersTotal()) {
  
                if (k->scene->layers().contains(k->framePosition.layer)) {
                    KTLayer *layer = k->scene->layer(k->framePosition.layer);
                    Q_CHECK_PTR(layer);
                    if (layer) {
                        if (!layer->frames().isEmpty())
                            return layer->frame(k->framePosition.frame);
                    } else {
                        #ifdef K_DEBUG
                               kFatal() << "KTGraphicsScene::currentFrame - No layer available: " << k->framePosition.frame;
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           kFatal() << "KTGraphicsScene::currentFrame - Layer index incorrect!"; 
                    #endif
                }
            } else {
                KTLayer *layer = k->scene->layer(k->scene->layersTotal() - 1);
                if (layer) {
                    if (!layer->frames().isEmpty())
                        return layer->frame(k->framePosition.frame);
                }
            }

        }

    }

    return 0;
}

void KTGraphicsScene::setCurrentScene(KTScene *scene)
{
    Q_CHECK_PTR(scene);

    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    if (k->tool)
        k->tool->aboutToChangeScene(this);

    qDeleteAll(k->lines);
    k->lines.clear();

    cleanWorkSpace();
    k->scene = scene;

    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawCurrentPhotogram();
    else
        drawBackground();
}

void KTGraphicsScene::setLayerVisible(int layerIndex, bool visible)
{
    if (!k->scene)
        return;

    if (KTLayer *layer = k->scene->layer(layerIndex))
        layer->setVisible(visible);
}

KTScene *KTGraphicsScene::scene() const
{
    if (k->scene)
        return k->scene;
    else
        return 0;
}

void KTGraphicsScene::setTool(KTToolPlugin *tool)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    if (k->spaceMode == KTProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else {
        cleanWorkSpace();
        drawBackground();
    }

    if (k->tool) {
        if (k->tool->toolType() == KTToolPlugin::Selection) {
            foreach (KTLineGuide *line, k->lines) {
                     line->setFlag(QGraphicsItem::ItemIsMovable, false);
                     line->setEnabledSyncCursor(true);
            }
        }
        k->tool->aboutToChangeTool();
    }

    k->tool = tool;

    if (tool->toolType() == KTToolPlugin::Selection) {
        foreach (KTLineGuide *line, k->lines) {
                 line->setFlag(QGraphicsItem::ItemIsMovable, true);
                 line->setEnabledSyncCursor(false);
        }
    }

    k->tool->init(this);
}

KTToolPlugin *KTGraphicsScene::currentTool() const
{
    return k->tool;
}

void KTGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    QGraphicsScene::mousePressEvent(event);
    k->inputInformation->updateFromMouseEvent(event);
    k->isDrawing = false;

    // This condition locks all the tools while workspace is rotated 
    if ((event->buttons() != Qt::LeftButton) || (event->modifiers () != (Qt::ShiftModifier | Qt::ControlModifier))) {
        if (k->tool) {      
            if ((k->tool->toolType() == KTToolPlugin::Brush || k->tool->toolType() == KTToolPlugin::Tweener) 
                 && event->isAccepted()) {
                return;
            }

            // If there's no frame... the tool is disabled 

            if (currentFrame()) {
                //if (event->buttons() == Qt::LeftButton && !currentFrame()->isLocked()) {
                //kFatal() << "KTGraphicsScene::mousePressEvent() - FLAG: " << currentFrame()->isLocked();
                    if (event->buttons() == Qt::LeftButton) {
                        k->tool->begin();
                        k->isDrawing = true;
                        k->tool->press(k->inputInformation, k->brushManager, this);
                    } else {
                        if (k->tool->name().compare(tr("Zoom")) || k->tool->name().compare(tr("PolyLine"))) {
                            k->tool->press(k->inputInformation, k->brushManager, this);
                            return;
                        }
                    }
            } 
        }
    }
}

void KTGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    mouseMoved(event);
}

void KTGraphicsScene::mouseMoved(QGraphicsSceneMouseEvent *event)
{
    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool && k->isDrawing)
        k->tool->move(k->inputInformation, k->brushManager,  this);
}

void KTGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    mouseReleased(event);
}

void KTGraphicsScene::mouseReleased(QGraphicsSceneMouseEvent *event)
{
    if (currentFrame()) {
        if (currentFrame()->isLocked()) {
            #ifdef K_DEBUG
                   kFatal() << "KTGraphicsScene::mouseReleased() - Frame is locked!";
            #endif
            return;
        }
    }

    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool && k->isDrawing) {
        k->tool->release(k->inputInformation, k->brushManager, this);
        k->tool->end();
    } else {
        // TODO: To find the way to enhance this condition only for zoom tool
        // if (k->tool->toolType() == KTToolPlugin::View) 
           if (k->tool->name().compare(tr("Zoom")))
               k->tool->release(k->inputInformation, k->brushManager, this);
    }

    k->isDrawing = false;
}

void KTGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);

    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool)
        k->tool->doubleClick(k->inputInformation, this);
}

void KTGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    if (k->tool) {
        k->tool->keyPressEvent(event);
        
        if (event->isAccepted())
            return;
    }
    
    QGraphicsScene::keyPressEvent(event);
}

/*

// TODO: Check this code, not sure whether it does something or it's handy :S

void KTGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    if (event->mimeData()->hasFormat("tupi-ruler"))
        event->acceptProposedAction();

    KTLineGuide *line = 0;
    if (event->mimeData()->data("tupi-ruler") == "verticalLine") {
        line  = new KTLineGuide(Qt::Vertical, this);
        line->setPos(event->scenePos());
    } else {
        line = new KTLineGuide(Qt::Horizontal, this);
        line->setPos(event->scenePos());
    }

    if (line)
        k->lines << line;
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void  KTGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event)
{
    Q_UNUSED(event);

    removeItem(k->lines.last());
    delete k->lines.takeLast();
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void KTGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    if (!k->lines.isEmpty())
        k->lines.last()->setPos(event->scenePos());
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void KTGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    Q_UNUSED(event);

    if (k->tool) {
        if (k->tool->toolType() == KTToolPlugin::Selection) {
            k->lines.last()->setEnabledSyncCursor(false);
            k->lines.last()->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
}

*/

bool KTGraphicsScene::event(QEvent *e)
{
    return QGraphicsScene::event(e);
}

void KTGraphicsScene::itemResponse(KTItemResponse *event)
{
    #ifdef K_DEBUG
           K_FUNCINFOX("scene");
    #endif

    if (k->tool) {
        // k->tool->init(this); // FIXME:k->tool->init(this); in itemResponse ???
        k->tool->itemResponse(event);
    }
}

bool KTGraphicsScene::isDrawing() const
{
    return k->isDrawing;
}

KTBrushManager *KTGraphicsScene::brushManager() const
{
    return k->brushManager;
}

void KTGraphicsScene::aboutToMousePress()
{
    QHash<QGraphicsItem *, double>::iterator it = k->onionSkin.opacityMap.begin();

    while (it != k->onionSkin.opacityMap.end()) {
            if (it.value() != 1.0) {
                it.key()->setAcceptedMouseButtons(Qt::NoButton);
                it.key()->setFlag(QGraphicsItem::ItemIsSelectable, false);
            } else {
                it.key()->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton | Qt::XButton1
                                                  | Qt::XButton2);
            }
            ++it;
    }
}

void KTGraphicsScene::includeObject(QGraphicsItem *object)
{
    if (k->spaceMode == KTProject::FRAMES_EDITION) {
        KTLayer *layer = k->scene->layer(k->framePosition.layer);
        if (layer) {
            KTFrame *frame = layer->frame(k->framePosition.frame);
            if (frame) {
                int zLevel = frame->getTopZLevel();
                object->setZValue(zLevel);
                addItem(object);
            }
        }
    } else {
        KTBackground *bg = k->scene->background();
        KTFrame *frame = bg->frame();
        if (frame) {
            int zLevel = frame->getTopZLevel();
            object->setZValue(zLevel);
            addItem(object);
        }
    }
}

void KTGraphicsScene::removeScene()
{
    cleanWorkSpace();
    k->scene = 0;
}

KTProject::Mode KTGraphicsScene::spaceMode()
{
    return k->spaceMode;
}

void KTGraphicsScene::setSpaceMode(KTProject::Mode mode)
{
    k->spaceMode = mode;    
}

