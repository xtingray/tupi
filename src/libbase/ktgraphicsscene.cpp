/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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
#include "ktscene.h"
#include "ktlayer.h"

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
#include "ktlineitem.h"
#include "ktrectitem.h"
#include "ktellipseitem.h"

#include "tdebug.h"
#include "ktguideline.h"

#include <QGraphicsItem>
#include <QSvgRenderer>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

/**
 * This class defines the data structure and methods for handling animation scenes.
 * Here is where the set of photograms of one scene are processed.
 * @author David Cuadrado
*/

struct KTGraphicsScene::Private
{
    KTToolPlugin *tool;
    KTScene *scene;
    double opacity;

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
    // int objectCounter;

    QList<KTLineGuide *> lines;
    KTProject::Mode spaceMode;   
};

KTGraphicsScene::KTGraphicsScene() : QGraphicsScene(), k(new Private)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
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
           TEND;
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
           T_FUNCINFO;
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
           T_FUNCINFO;
    #endif

    KTLayer *layer = k->scene->layer(k->framePosition.layer);
    int frames = layer->framesTotal();

    if (k->framePosition.frame >= frames)
        k->framePosition.frame = frames - 1;

    if (k->spaceMode == KTProject::FRAMES_EDITION) {
        drawPhotogram(k->framePosition.frame);
    } else if (k->spaceMode == KTProject::BACKGROUND_EDITION) {
               cleanWorkSpace();
               drawBackground();
    }
}

void KTGraphicsScene::drawPhotogram(int photogram)
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif
    */

    // Q_CHECK_PTR(k->scene);

    if (photogram < 0 || !k->scene) 
        return;

    cleanWorkSpace();

    bool valid = false;

    // Drawing frames from another layers

    for (int i=0; i < k->scene->layersTotal(); i++) {

         KTLayer *layer = k->scene->layer(i);

         if (layer->framesTotal() > 0 && photogram < layer->framesTotal()) {

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

                             double opacity = k->opacity;
                             double opacityFactor = opacity / (double)qMin(layer->frames().count(), k->onionSkin.previous);

                             int limit = photogram - k->onionSkin.previous;
                             if (limit < 0) 
                                 limit = 0;

                             QString frameBehind = ""; 
                             for (int frameIndex = photogram-1; frameIndex >= limit; frameIndex--) {
                                  KTFrame *frame = layer->frame(frameIndex);
                                  QString previousFrame = frame->frameName();
                                  if (frame && previousFrame.compare(currentFrame) != 0 
                                            && frameBehind.compare(previousFrame) != 0) {
                                      addFrame(frame, opacity, Preview);
                                  } 

                                  frameBehind = previousFrame;
                                  opacity -= opacityFactor;
                             }

                         }

                         // valid = true;
                         // k->layerCounter = i;
                         addFrame(mainFrame, KTGraphicsScene::Current);

                         // Painting next frames
                         if (k->onionSkin.next > 0 && layer->framesTotal() > photogram + 1) {

                             double opacity = k->opacity;
                             double opacityFactor = opacity / (double)qMin(layer->frames().count(), k->onionSkin.next);

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
                         // addFrame(mainFrame, KTGraphicsScene::Current);
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
    /*
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif
    */

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
        // k->objectCounter = 0;

        QList<int> indexes = frame->itemIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             KTGraphicObject *object = frame->graphic(indexes.at(i));
             if (mode != KTGraphicsScene::Current) {
                 if (!object->hasTween())
                     addGraphicObject(object, opacity);
             } else {
                 addGraphicObject(object, opacity);
             }
        }

        indexes = frame->svgIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             KTSvgItem *object = frame->svg(indexes.at(i));
             if (!object->hasTween()) {
                 addSvgObject(object, opacity);
             } else {
                 KTItemTweener *tween = object->tween(); 
                 if (k->framePosition.frame == tween->startFrame())
                     addSvgObject(object, opacity);
             }
        }
    }
}

void KTGraphicsScene::addGraphicObject(KTGraphicObject *object, double opacity)
{
    /*
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif
    */

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
                    // k->objectCounter++;
                    addItem(item);
                }
            }
        } 
}

void KTGraphicsScene::addSvgObject(KTSvgItem *svgItem, double opacity)
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif
    */

    if (svgItem) {

        k->onionSkin.opacityMap.insert(svgItem, opacity);
        svgItem->setSelected(false);

        KTLayer *layer = k->scene->layer(k->framePosition.layer);

        if (layer) {

            KTFrame *frame = layer->frame(k->framePosition.frame);

            if (frame) {
                svgItem->setOpacity(opacity);
                // k->objectCounter++;
                // tDebug() << "KTGraphicsScene::addSvgObject() - Item added successful! [" 
                //          << k->framePosition.layer << ", " << k->framePosition.frame << "]"; 
                addItem(svgItem);
            } else {
                #ifdef K_DEBUG
                       tFatal() << "KTGraphicsScene::addSvgObject() - Error: Frame #" << k->framePosition.frame << " NO available!";
                #endif
            }
        } else {
                #ifdef K_DEBUG
                       tFatal() << "KTGraphicsScene::addSvgObject() - Error: Layer #" << k->framePosition.layer << " NO available!";
                #endif
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "KTGraphicsScene::addSvgObject() - Error: No SVG item!";
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
                     object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: 0"));

                     if (tween->type() == KTItemTweener::Compound) {
                         object->item()->setTransformOriginPoint(tween->transformOriginPoint());

                         if (stepItem->has(KTTweenerStep::Position)) {
                             // tFatal() << "KTGraphicsScene::addTweeningObjects() - Applying position...";
                             QPointF point = QPoint(-adjustX, -adjustY);
                             object->setLastTweenPos(stepItem->position() + point);
                             object->item()->setPos(tween->transformOriginPoint());
                         }

                         if (stepItem->has(KTTweenerStep::Rotation)) {
                             QRectF rect = object->item()->sceneBoundingRect();
                             object->item()->setTransformOriginPoint(rect.center());
                             double angle = stepItem->rotation();
                             object->item()->setRotation(angle);
                             // tFatal() << "KTGraphicsScene::addTweeningObjects() - Applying rotation - Angle: " << angle;
                         } else {
                             // tFatal() << "KTGraphicsScene::addTweeningObjects() - No rotation parameter!";
                         }
                         
                     } else {

                         if (stepItem->has(KTTweenerStep::Position)) {
                             QPointF point = QPoint(-adjustX, -adjustY);
                             object->setLastTweenPos(stepItem->position() + point);
                             object->item()->setPos(tween->transformOriginPoint());
                         }

                         if (stepItem->has(KTTweenerStep::Rotation)) {
                             double angle = stepItem->rotation();
                             object->item()->setTransformOriginPoint(tween->transformOriginPoint());
                             object->item()->setRotation(angle);
                         }

                         if (stepItem->has(KTTweenerStep::Scale)) {
                             QPointF point = tween->transformOriginPoint();
                             object->item()->setTransformOriginPoint(point);
                             object->item()->setScale(1.0);
                         }

                         if (stepItem->has(KTTweenerStep::Shear)) {
                             QTransform transform;
                             transform.shear(0, 0);
                             object->item()->setTransform(transform);
                         } 

                         if (stepItem->has(KTTweenerStep::Coloring)) {
                             QColor itemColor = stepItem->color();
                             if (KTPathItem *path = qgraphicsitem_cast<KTPathItem *>(object->item())) {
                                 QPen pen = path->pen();
                                 pen.setColor(itemColor);
                                 path->setPen(pen);
                             } else if (KTEllipseItem *ellipse = qgraphicsitem_cast<KTEllipseItem *>(object->item())) {
                                        QPen pen = ellipse->pen();
                                        pen.setColor(itemColor);
                                        ellipse->setPen(pen);
                             } else if (KTLineItem *line = qgraphicsitem_cast<KTLineItem *>(object->item())) {
                                        QPen pen = line->pen();
                                        pen.setColor(itemColor);
                                        line->setPen(pen); 
                             } else if (KTRectItem *rect = qgraphicsitem_cast<KTRectItem *>(object->item())) {
                                        QPen pen = rect->pen();
                                        pen.setColor(itemColor);
                                        rect->setPen(pen);
                            }
                         }

                         if (stepItem->has(KTTweenerStep::Opacity))
                             object->item()->setOpacity(stepItem->opacity());
                     }

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {

                            int step = photogram - origin;
                            KTTweenerStep *stepItem = tween->stepAt(step);
                            object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: ") + QString::number(step));
                            if (tween->type() == KTItemTweener::Compound) {

                                if (stepItem->has(KTTweenerStep::Position)) {
                                    qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                    qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                    object->item()->moveBy(dx, dy);
                                    QPointF point = QPoint(-adjustX, -adjustY);
                                    object->setLastTweenPos(stepItem->position() + point);
                                }

                                if (stepItem->has(KTTweenerStep::Rotation)) {
                                    double angle = stepItem->rotation();
                                    object->item()->setRotation(angle);
                                    // tFatal() << "KTGraphicsScene::addTweeningObjects() - Applying rotation - Angle: " << angle;
                                }

                                addGraphicObject(object);

                            } else {

                                if (stepItem->has(KTTweenerStep::Position)) {
                                    qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                    qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                    object->item()->moveBy(dx, dy);
                                    QPointF point = QPoint(-adjustX, -adjustY);
                                    object->setLastTweenPos(stepItem->position() + point);
                                }

                                if (stepItem->has(KTTweenerStep::Rotation)) {
                                    double angle = stepItem->rotation();
                                    object->item()->setRotation(angle);
                                }

                                if (stepItem->has(KTTweenerStep::Scale)) {
                                    QPointF point = tween->transformOriginPoint();

                                    double scaleX = stepItem->horizontalScale();
                                    double scaleY = stepItem->verticalScale();
                                    QTransform transform;
                                    transform.translate(point.x(), point.y());
                                    transform.scale(scaleX, scaleY);
                                    transform.translate(-point.x(), -point.y());

                                    object->item()->setTransform(transform);
                                }

                                if (stepItem->has(KTTweenerStep::Shear)) {
                                    QPointF point = tween->transformOriginPoint();

                                    double shearX = stepItem->horizontalShear();
                                    double shearY = stepItem->verticalShear();
                                    QTransform transform;
                                    transform.translate(point.x(), point.y());
                                    transform.shear(shearX, shearY);
                                    transform.translate(-point.x(), -point.y());

                                    object->item()->setTransform(transform);
                                }

                                if (stepItem->has(KTTweenerStep::Coloring)) {
                                    QColor itemColor = stepItem->color();
                                    if (KTPathItem *path = qgraphicsitem_cast<KTPathItem *>(object->item())) {
                                        QPen pen = path->pen();
                                        pen.setColor(itemColor);
                                        path->setPen(pen);
                                    } else if (KTEllipseItem *ellipse = qgraphicsitem_cast<KTEllipseItem *>(object->item())) {
                                               QPen pen = ellipse->pen();
                                               pen.setColor(itemColor);
                                               ellipse->setPen(pen);
                                    } else if (KTLineItem *line = qgraphicsitem_cast<KTLineItem *>(object->item())) {
                                               QPen pen = line->pen();
                                               pen.setColor(itemColor);
                                               line->setPen(pen);
                                    } else if (KTRectItem *rect = qgraphicsitem_cast<KTRectItem *>(object->item())) {
                                               QPen pen = rect->pen();
                                               pen.setColor(itemColor);
                                               rect->setPen(pen);
                                    }
                                }

                                addGraphicObject(object);

                                if (stepItem->has(KTTweenerStep::Opacity))
                                    object->item()->setOpacity(stepItem->opacity());
                            }    
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
                     object->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: 0"));

                     if (stepItem->has(KTTweenerStep::Position)) {
                         object->setPos(tween->transformOriginPoint());
                         QPointF offset = QPoint(-adjustX, -adjustY);
                         object->setLastTweenPos(stepItem->position() + offset);
                     }

                     if (stepItem->has(KTTweenerStep::Rotation)) {
                         double angle = stepItem->rotation();
                         object->setTransformOriginPoint(tween->transformOriginPoint());
                         object->setRotation(angle);
                     }

                     if (stepItem->has(KTTweenerStep::Scale)) {
                         QPointF point = tween->transformOriginPoint();
                         object->setTransformOriginPoint(point);
                         object->setScale(1.0);
                     }

                     if (stepItem->has(KTTweenerStep::Shear)) {
                         QTransform transform;
                         transform.shear(0, 0);
                         object->setTransform(transform);
                     }

                     if (stepItem->has(KTTweenerStep::Opacity))
                         object->setOpacity(stepItem->opacity());

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {
                             int step = photogram - origin;
                             KTTweenerStep *stepItem = tween->stepAt(step);
                             object->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: ") + QString::number(step));

                             if (stepItem->has(KTTweenerStep::Position)) {
                                 qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                 qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                 object->moveBy(dx, dy);
                                 QPointF offset = QPoint(-adjustX, -adjustY);
                                 object->setLastTweenPos(stepItem->position() + offset);
                             }

                             if (stepItem->has(KTTweenerStep::Rotation)) {
                                 double angle = stepItem->rotation();
                                 object->setRotation(angle);
                             }

                            if (stepItem->has(KTTweenerStep::Scale)) {
                                QPointF point = tween->transformOriginPoint();

                                double scaleX = stepItem->horizontalScale();
                                double scaleY = stepItem->verticalScale();
                                QTransform transform;
                                transform.translate(point.x(), point.y());
                                transform.scale(scaleX, scaleY);
                                transform.translate(-point.x(), -point.y());

                                object->setTransform(transform);
                            }

                            if (stepItem->has(KTTweenerStep::Shear)) {
                                QPointF point = tween->transformOriginPoint();

                                double shearX = stepItem->horizontalShear();
                                double shearY = stepItem->verticalShear();
                                QTransform transform;
                                transform.translate(point.x(), point.y());
                                transform.shear(shearX, shearY);
                                transform.translate(-point.x(), -point.y());

                                object->setTransform(transform);
                            }

                            addSvgObject(object);

                            if (stepItem->has(KTTweenerStep::Opacity))
                                object->setOpacity(stepItem->opacity());
                 }
             } else {
                 #ifdef K_DEBUG
                           tFatal() << "KTGraphicsScene::addSvgTweeningObjects() - No tween found!";
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
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    if (!k->scene)
        return -1;

    return k->scene->objectIndex();
}

void KTGraphicsScene::setNextOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    k->onionSkin.next = n;
    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

void KTGraphicsScene::setPreviousOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
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
                               tFatal() << "KTGraphicsScene::currentFrame - No layer available: " << k->framePosition.frame;
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tFatal() << "KTGraphicsScene::currentFrame - Layer index incorrect!"; 
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
           T_FUNCINFO;
    #endif

    setCurrentFrame(0, 0);

    if (k->tool)
        k->tool->aboutToChangeScene(this);

    qDeleteAll(k->lines);
    k->lines.clear();

    cleanWorkSpace();
    k->scene = scene;

    if (k->spaceMode == KTProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else if (k->spaceMode == KTProject::BACKGROUND_EDITION) {
               drawBackground();
    }
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
           T_FUNCINFO;
    #endif

    if (k->spaceMode == KTProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else {
        cleanWorkSpace();
        drawBackground();
    }

    /* SQA: Code under revision
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
    */

    if (k->tool)
        k->tool->aboutToChangeTool();

    k->tool = tool;
    k->tool->init(this);
}

KTToolPlugin *KTGraphicsScene::currentTool() const
{
    return k->tool;
}

void KTGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
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
                //tFatal() << "KTGraphicsScene::mousePressEvent() - FLAG: " << currentFrame()->isLocked();
                    if (event->buttons() == Qt::LeftButton) {
                        k->tool->begin();
                        k->isDrawing = true;
                        k->tool->press(k->inputInformation, k->brushManager, this);
                    } else {
                        // if (k->tool->name().compare(tr("Zoom")) == 0 || k->tool->name().compare(tr("PolyLine")) == 0) {
                        if (k->tool->name().compare(tr("PolyLine")) == 0) {
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

    if (k->tool) {
        if (k->tool->name().compare(tr("Line")) == 0)
            k->tool->updatePos(event->scenePos());
    }
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
                   tFatal() << "KTGraphicsScene::mouseReleased() - Frame is locked!";
            #endif
            return;
        }
    }

    k->inputInformation->updateFromMouseEvent(event);

    if (k->isDrawing) {
        if (k->tool) {
            k->tool->release(k->inputInformation, k->brushManager, this);
            k->tool->end();
        }
    } else {
        if (k->tool) { 
           if (k->tool->name().compare(tr("Zoom")))
               k->tool->release(k->inputInformation, k->brushManager, this);
        }
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

void KTGraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
    if (k->tool) {
        k->tool->keyReleaseEvent(event);

        if (event->isAccepted())
            return;
    }
   
    QGraphicsScene::keyReleaseEvent(event);
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

void KTGraphicsScene::sceneResponse(KTSceneResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->sceneResponse(event);
}

void KTGraphicsScene::layerResponse(KTLayerResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->layerResponse(event);
}

void KTGraphicsScene::frameResponse(KTFrameResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->frameResponse(event);
}

void KTGraphicsScene::itemResponse(KTItemResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool) 
        k->tool->itemResponse(event);
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

void KTGraphicsScene::setOnionFactor(double opacity)
{
    k->opacity = opacity;
    if (k->spaceMode == KTProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

int KTGraphicsScene::framesTotal() 
{
    KTLayer *layer = k->scene->layer(k->framePosition.layer);
    if (layer)
        return layer->framesTotal();
    else
        return -1;
}
