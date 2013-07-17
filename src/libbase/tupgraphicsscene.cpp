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

#include "tupgraphicsscene.h"
#include "tupscene.h"
#include "tuplayer.h"

#include "tupgraphicobject.h"
#include "tupitemgroup.h"
#include "tupprojectloader.h"
#include "tupitemfactory.h"
#include "tuptoolplugin.h"
#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupframe.h"
#include "tupitemtweener.h"
#include "tupgraphiclibraryitem.h"
#include "tupsvgitem.h"
#include "tuppathitem.h"
#include "tuplineitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"

#include "tdebug.h"
#include "tupguideline.h"

#include <QGraphicsItem>
#include <QSvgRenderer>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDesktopWidget>

/**
 * This class defines the data structure and methods for handling animation scenes.
 * Here is where the set of photograms of one scene are processed.
 * @author David Cuadrado
*/

struct TupGraphicsScene::Private
{
    TupToolPlugin *tool;
    TupScene *scene;
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

    TupBrushManager *brushManager;
    TupInputDeviceInformation *inputInformation;

    bool isDrawing;
    int layerCounter;
    // int objectCounter;

    QList<TupLineGuide *> lines;
    TupProject::Mode spaceMode;   
};

TupGraphicsScene::TupGraphicsScene() : QGraphicsScene(), k(new Private)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    setItemIndexMethod(QGraphicsScene::NoIndex);

    k->framePosition.layer = -1;
    k->framePosition.frame = -1;
    k->spaceMode = TupProject::FRAMES_EDITION;

    setCurrentFrame(0, 0);

    k->onionSkin.next = 0;
    k->onionSkin.previous = 0;
    k->tool = 0;
    k->isDrawing = false;

    setBackgroundBrush(Qt::gray);

    k->inputInformation = new TupInputDeviceInformation(this);
    k->brushManager = new TupBrushManager(this);
}

TupGraphicsScene::~TupGraphicsScene()
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

void TupGraphicsScene::setCurrentFrame(int layer, int frame)
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

void TupGraphicsScene::drawCurrentPhotogram()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupLayer *layer = k->scene->layer(k->framePosition.layer);
    int frames = layer->framesTotal();

    if (k->framePosition.frame >= frames)
        k->framePosition.frame = frames - 1;

    if (k->spaceMode == TupProject::FRAMES_EDITION) {
        drawPhotogram(k->framePosition.frame);
    } else if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION) {
               cleanWorkSpace();
               drawBackground();
    }
}

void TupGraphicsScene::drawPhotogram(int photogram)
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

         TupLayer *layer = k->scene->layer(i);

         if (layer->framesTotal() > 0 && photogram < layer->framesTotal()) {

             TupFrame *mainFrame = layer->frame(photogram);

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
                                  TupFrame *frame = layer->frame(frameIndex);
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
                         addFrame(mainFrame, TupGraphicsScene::Current);

                         // Painting next frames
                         if (k->onionSkin.next > 0 && layer->framesTotal() > photogram + 1) {

                             double opacity = k->opacity;
                             double opacityFactor = opacity / (double)qMin(layer->frames().count(), k->onionSkin.next);

                             int limit = photogram + k->onionSkin.next;
                             if (limit >= layer->frames().count()) 
                                 limit = layer->frames().count() - 1;

                             QString frameLater = "";
                             for (int frameIndex = photogram+1; frameIndex <= limit; frameIndex++) {
                                  TupFrame * frame = layer->frame(frameIndex);
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
                         // addFrame(mainFrame, TupGraphicsScene::Current);
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

void TupGraphicsScene::drawBackground()
{
    /*
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif
    */

    Q_CHECK_PTR(k->scene);

    if (!k->scene)
        return;

    TupBackground *bg = k->scene->background();
    if (bg) {
        if (k->spaceMode == TupProject::DYNAMIC_BACKGROUND_EDITION || k->spaceMode == TupProject::FRAMES_EDITION) {
            TupFrame *frame = bg->dynamicFrame(k->framePosition.frame);
            if (frame)
                addFrame(frame, 1.0);
        } 

        if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION || k->spaceMode == TupProject::FRAMES_EDITION) {
            TupFrame *frame = bg->staticFrame();
            if (frame)
                addFrame(frame, 1.0);
        }
    }
}

void TupGraphicsScene::addFrame(TupFrame *frame, double opacity, Context mode)
{
    if (frame) {
        // k->objectCounter = 0;

        QList<int> indexes = frame->itemIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             TupGraphicObject *object = frame->graphic(indexes.at(i));
             if (mode != TupGraphicsScene::Current) {
                 if (!object->hasTween())
                     addGraphicObject(object, opacity);
             } else {
                 addGraphicObject(object, opacity);
             }
        }

        indexes = frame->svgIndexes();
        for (int i = 0; i < indexes.size(); ++i) {
             TupSvgItem *object = frame->svg(indexes.at(i));
             if (!object->hasTween()) {
                 addSvgObject(object, opacity);
             } else {
                 TupItemTweener *tween = object->tween(); 
                 if (k->framePosition.frame == tween->startFrame())
                     addSvgObject(object, opacity);
             }
        }
    }
}

void TupGraphicsScene::addGraphicObject(TupGraphicObject *object, double opacity)
{
    /*
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif
    */

        QGraphicsItem *item = object->item();
        k->onionSkin.opacityMap.insert(item, opacity);

        if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item))
            group->recoverChilds();

        if (! qgraphicsitem_cast<TupItemGroup *>(item->parentItem())) {

            item->setSelected(false);
            TupLayer *layer = k->scene->layer(k->framePosition.layer);

            if (layer) {

                TupFrame *frame = layer->frame(k->framePosition.frame);

                if (frame) {
                    item->setOpacity(opacity);
                    // k->objectCounter++;
                    addItem(item);
                }
            }
        } 
}

void TupGraphicsScene::addSvgObject(TupSvgItem *svgItem, double opacity)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (svgItem) {

        k->onionSkin.opacityMap.insert(svgItem, opacity);
        svgItem->setSelected(false);

        TupLayer *layer = k->scene->layer(k->framePosition.layer);

        if (layer) {

            TupFrame *frame = layer->frame(k->framePosition.frame);

            if (frame) {
                svgItem->setOpacity(opacity);

                if (svgItem->symbolName().compare("dollar.svg")==0)
                    connect(svgItem, SIGNAL(enabledChanged()), this, SIGNAL(showInfoWidget()));

                addItem(svgItem);
            } else {
                #ifdef K_DEBUG
                       tFatal() << "TupGraphicsScene::addSvgObject() - Error: Frame #" << k->framePosition.frame << " NO available!";
                #endif
            }
        } else {
                #ifdef K_DEBUG
                       tFatal() << "TupGraphicsScene::addSvgObject() - Error: Layer #" << k->framePosition.layer << " NO available!";
                #endif
        }

    } else {
        #ifdef K_DEBUG
               tFatal() << "TupGraphicsScene::addSvgObject() - Error: No SVG item!";
        #endif
    } 
} 

void TupGraphicsScene::addTweeningObjects(int photogram)
{
    QList<TupGraphicObject *> tweenList = k->scene->tweeningGraphicObjects();

    for (int i=0; i < tweenList.count(); i++) {

         TupGraphicObject *object = tweenList.at(i);

         if (object->frame()->layer()->isVisible()) {
             int origin = object->frame()->index();

             if (TupItemTweener *tween = object->tween()) {

                 int adjustX = object->item()->boundingRect().width()/2;
                 int adjustY = object->item()->boundingRect().height()/2;

                 if (origin == photogram) {

                     TupTweenerStep *stepItem = tween->stepAt(0);
                     object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: 0"));

                     if (tween->type() == TupItemTweener::Compound) {
                         object->item()->setTransformOriginPoint(tween->transformOriginPoint());

                         if (stepItem->has(TupTweenerStep::Position)) {
                             // tFatal() << "TupGraphicsScene::addTweeningObjects() - Applying position...";
                             QPointF point = QPoint(-adjustX, -adjustY);
                             object->setLastTweenPos(stepItem->position() + point);
                             object->item()->setPos(tween->transformOriginPoint());
                         }

                         if (stepItem->has(TupTweenerStep::Rotation)) {
                             QRectF rect = object->item()->sceneBoundingRect();
                             object->item()->setTransformOriginPoint(rect.center());
                             double angle = stepItem->rotation();
                             object->item()->setRotation(angle);
                             // tFatal() << "TupGraphicsScene::addTweeningObjects() - Applying rotation - Angle: " << angle;
                         } else {
                             // tFatal() << "TupGraphicsScene::addTweeningObjects() - No rotation parameter!";
                         }
                         
                     } else {

                         if (stepItem->has(TupTweenerStep::Position)) {
                             QPointF point = QPoint(-adjustX, -adjustY);
                             object->setLastTweenPos(stepItem->position() + point);
                             object->item()->setPos(tween->transformOriginPoint());
                         }

                         if (stepItem->has(TupTweenerStep::Rotation)) {
                             double angle = stepItem->rotation();
                             object->item()->setTransformOriginPoint(tween->transformOriginPoint());
                             object->item()->setRotation(angle);
                         }

                         if (stepItem->has(TupTweenerStep::Scale)) {
                             QPointF point = tween->transformOriginPoint();
                             object->item()->setTransformOriginPoint(point);
                             object->item()->setScale(1.0);
                         }

                         if (stepItem->has(TupTweenerStep::Shear)) {
                             QTransform transform;
                             transform.shear(0, 0);
                             object->item()->setTransform(transform);
                         } 

                         if (stepItem->has(TupTweenerStep::Coloring)) {
                             QColor itemColor = stepItem->color();
                             if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(object->item())) {
                                 QPen pen = path->pen();
                                 pen.setColor(itemColor);
                                 path->setPen(pen);
                             } else if (TupEllipseItem *ellipse = qgraphicsitem_cast<TupEllipseItem *>(object->item())) {
                                        QPen pen = ellipse->pen();
                                        pen.setColor(itemColor);
                                        ellipse->setPen(pen);
                             } else if (TupLineItem *line = qgraphicsitem_cast<TupLineItem *>(object->item())) {
                                        QPen pen = line->pen();
                                        pen.setColor(itemColor);
                                        line->setPen(pen); 
                             } else if (TupRectItem *rect = qgraphicsitem_cast<TupRectItem *>(object->item())) {
                                        QPen pen = rect->pen();
                                        pen.setColor(itemColor);
                                        rect->setPen(pen);
                            }
                         }

                         if (stepItem->has(TupTweenerStep::Opacity))
                             object->item()->setOpacity(stepItem->opacity());
                     }

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {

                            int step = photogram - origin;
                            TupTweenerStep *stepItem = tween->stepAt(step);
                            object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: ") + QString::number(step));
                            if (tween->type() == TupItemTweener::Compound) {

                                if (stepItem->has(TupTweenerStep::Position)) {
                                    qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                    qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                    object->item()->moveBy(dx, dy);
                                    QPointF point = QPoint(-adjustX, -adjustY);
                                    object->setLastTweenPos(stepItem->position() + point);
                                }

                                if (stepItem->has(TupTweenerStep::Rotation)) {
                                    double angle = stepItem->rotation();
                                    object->item()->setRotation(angle);
                                    // tFatal() << "TupGraphicsScene::addTweeningObjects() - Applying rotation - Angle: " << angle;
                                }

                                addGraphicObject(object);

                            } else {

                                if (stepItem->has(TupTweenerStep::Position)) {
                                    qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                    qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                    object->item()->moveBy(dx, dy);
                                    QPointF point = QPoint(-adjustX, -adjustY);
                                    object->setLastTweenPos(stepItem->position() + point);
                                }

                                if (stepItem->has(TupTweenerStep::Rotation)) {
                                    double angle = stepItem->rotation();
                                    object->item()->setRotation(angle);
                                }

                                if (stepItem->has(TupTweenerStep::Scale)) {
                                    QPointF point = tween->transformOriginPoint();

                                    double scaleX = stepItem->horizontalScale();
                                    double scaleY = stepItem->verticalScale();
                                    QTransform transform;
                                    transform.translate(point.x(), point.y());
                                    transform.scale(scaleX, scaleY);
                                    transform.translate(-point.x(), -point.y());

                                    object->item()->setTransform(transform);
                                }

                                if (stepItem->has(TupTweenerStep::Shear)) {
                                    QPointF point = tween->transformOriginPoint();

                                    double shearX = stepItem->horizontalShear();
                                    double shearY = stepItem->verticalShear();
                                    QTransform transform;
                                    transform.translate(point.x(), point.y());
                                    transform.shear(shearX, shearY);
                                    transform.translate(-point.x(), -point.y());

                                    object->item()->setTransform(transform);
                                }

                                if (stepItem->has(TupTweenerStep::Coloring)) {
                                    QColor itemColor = stepItem->color();
                                    if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(object->item())) {
                                        QPen pen = path->pen();
                                        pen.setColor(itemColor);
                                        path->setPen(pen);
                                    } else if (TupEllipseItem *ellipse = qgraphicsitem_cast<TupEllipseItem *>(object->item())) {
                                               QPen pen = ellipse->pen();
                                               pen.setColor(itemColor);
                                               ellipse->setPen(pen);
                                    } else if (TupLineItem *line = qgraphicsitem_cast<TupLineItem *>(object->item())) {
                                               QPen pen = line->pen();
                                               pen.setColor(itemColor);
                                               line->setPen(pen);
                                    } else if (TupRectItem *rect = qgraphicsitem_cast<TupRectItem *>(object->item())) {
                                               QPen pen = rect->pen();
                                               pen.setColor(itemColor);
                                               rect->setPen(pen);
                                    }
                                }

                                addGraphicObject(object);

                                if (stepItem->has(TupTweenerStep::Opacity))
                                    object->item()->setOpacity(stepItem->opacity());
                            }    
                 }
             }
        }
    }
}

void TupGraphicsScene::addSvgTweeningObjects(int photogram)
{
    QList<TupSvgItem *> svgList = k->scene->tweeningSvgObjects();

    for (int i=0; i < svgList.count(); i++) {

         TupSvgItem *object = svgList.at(i);

         if (object->frame()->layer()->isVisible()) {
             int origin = object->frame()->index();

             if (TupItemTweener *tween = object->tween()) {

                 int adjustX = object->boundingRect().width()/2;
                 int adjustY = object->boundingRect().height()/2;

                 if (origin == photogram) {

                     TupTweenerStep *stepItem = tween->stepAt(0);
                     object->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: 0"));

                     if (stepItem->has(TupTweenerStep::Position)) {
                         object->setPos(tween->transformOriginPoint());
                         QPointF offset = QPoint(-adjustX, -adjustY);
                         object->setLastTweenPos(stepItem->position() + offset);
                     }

                     if (stepItem->has(TupTweenerStep::Rotation)) {
                         double angle = stepItem->rotation();
                         object->setTransformOriginPoint(tween->transformOriginPoint());
                         object->setRotation(angle);
                     }

                     if (stepItem->has(TupTweenerStep::Scale)) {
                         QPointF point = tween->transformOriginPoint();
                         object->setTransformOriginPoint(point);
                         object->setScale(1.0);
                     }

                     if (stepItem->has(TupTweenerStep::Shear)) {
                         QTransform transform;
                         transform.shear(0, 0);
                         object->setTransform(transform);
                     }

                     if (stepItem->has(TupTweenerStep::Opacity))
                         object->setOpacity(stepItem->opacity());

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {
                             int step = photogram - origin;
                             TupTweenerStep *stepItem = tween->stepAt(step);
                             object->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: ") + QString::number(step));

                             if (stepItem->has(TupTweenerStep::Position)) {
                                 qreal dx = stepItem->position().x() - (object->lastTweenPos().x() + adjustX);
                                 qreal dy = stepItem->position().y() - (object->lastTweenPos().y() + adjustY);
                                 object->moveBy(dx, dy);
                                 QPointF offset = QPoint(-adjustX, -adjustY);
                                 object->setLastTweenPos(stepItem->position() + offset);
                             }

                             if (stepItem->has(TupTweenerStep::Rotation)) {
                                 double angle = stepItem->rotation();
                                 object->setRotation(angle);
                             }

                            if (stepItem->has(TupTweenerStep::Scale)) {
                                QPointF point = tween->transformOriginPoint();

                                double scaleX = stepItem->horizontalScale();
                                double scaleY = stepItem->verticalScale();
                                QTransform transform;
                                transform.translate(point.x(), point.y());
                                transform.scale(scaleX, scaleY);
                                transform.translate(-point.x(), -point.y());

                                object->setTransform(transform);
                            }

                            if (stepItem->has(TupTweenerStep::Shear)) {
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

                            if (stepItem->has(TupTweenerStep::Opacity))
                                object->setOpacity(stepItem->opacity());
                 }
             } else {
                 #ifdef K_DEBUG
                        tFatal() << "TupGraphicsScene::addSvgTweeningObjects() - No tween found!";
                 #endif
             }
         }
    }
}

void TupGraphicsScene::cleanWorkSpace()
{
    k->onionSkin.opacityMap.clear();

    foreach (QGraphicsItem *item, items()) {
             if (item->scene() == this)
                 removeItem(item);
    }

    foreach (TupLineGuide *line, k->lines)
             addItem(line);
}

int TupGraphicsScene::currentFrameIndex() const
{
    return k->framePosition.frame;
}

int TupGraphicsScene::currentLayerIndex() const
{
    return k->framePosition.layer;
}

int TupGraphicsScene::currentSceneIndex() const
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    if (!k->scene)
        return -1;

    return k->scene->objectIndex();
}

void TupGraphicsScene::setNextOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    k->onionSkin.next = n;
    if (k->spaceMode == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

void TupGraphicsScene::setPreviousOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    k->onionSkin.previous = n;
    if (k->spaceMode == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

TupFrame *TupGraphicsScene::currentFrame()
{
    if (k->scene) {

        if (k->scene->layersTotal() > 0) {

            if (k->framePosition.layer < k->scene->layersTotal()) {
  
                if (k->scene->layers().contains(k->framePosition.layer)) {
                    TupLayer *layer = k->scene->layer(k->framePosition.layer);
                    Q_CHECK_PTR(layer);
                    if (layer) {
                        if (!layer->frames().isEmpty())
                            return layer->frame(k->framePosition.frame);
                    } else {
                        #ifdef K_DEBUG
                               tFatal() << "TupGraphicsScene::currentFrame - No layer available: " << k->framePosition.frame;
                        #endif
                    }
                } else {
                    #ifdef K_DEBUG
                           tFatal() << "TupGraphicsScene::currentFrame - Layer index incorrect!"; 
                    #endif
                }
            } else {
                TupLayer *layer = k->scene->layer(k->scene->layersTotal() - 1);
                if (layer) {
                    if (!layer->frames().isEmpty())
                        return layer->frame(k->framePosition.frame);
                }
            }

        }

    }

    return 0;
}

void TupGraphicsScene::setCurrentScene(TupScene *scene)
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

    if (k->spaceMode == TupProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION) {
               drawBackground();
    }
}

void TupGraphicsScene::setLayerVisible(int layerIndex, bool visible)
{
    if (!k->scene)
        return;

    if (TupLayer *layer = k->scene->layer(layerIndex))
        layer->setVisible(visible);
}

TupScene *TupGraphicsScene::scene() const
{
    if (k->scene)
        return k->scene;
    else
        return 0;
}

void TupGraphicsScene::setTool(TupToolPlugin *tool)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->spaceMode == TupProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else {
        cleanWorkSpace();
        drawBackground();
    }

    /* SQA: Code under revision
    if (k->tool) {
        if (k->tool->toolType() == TupToolPlugin::Selection) {
            foreach (TupLineGuide *line, k->lines) {
                     line->setFlag(QGraphicsItem::ItemIsMovable, false);
                     line->setEnabledSyncCursor(true);
            }
        }
        k->tool->aboutToChangeTool();
    }

    k->tool = tool;

    if (tool->toolType() == TupToolPlugin::Selection) {
        foreach (TupLineGuide *line, k->lines) {
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

TupToolPlugin *TupGraphicsScene::currentTool() const
{
    return k->tool;
}

void TupGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

            if (k->tool->toolType() == TupToolPlugin::Brush && event->isAccepted())
                return;

            if (k->tool->toolType() == TupToolPlugin::Tweener && event->isAccepted()) {
                tFatal() << "TupGraphicsScene::mousePressEvent() - Tracing!";
                if (k->tool->currentEditMode() == TupToolPlugin::Path)
                    return;
            } 

            // If there's no frame... the tool is disabled 

            if (currentFrame()) {
                //if (event->buttons() == Qt::LeftButton && !currentFrame()->isLocked()) {
                //tFatal() << "TupGraphicsScene::mousePressEvent() - FLAG: " << currentFrame()->isLocked();
                if (event->buttons() == Qt::LeftButton) {
                    k->tool->begin();
                    k->isDrawing = true;
                    k->tool->press(k->inputInformation, k->brushManager, this);
                } 
            } 
        }
    }
}

void TupGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    mouseMoved(event);

    if (k->tool) {
        if (k->tool->name().compare(tr("Line")) == 0)
            k->tool->updatePos(event->scenePos());
    }
}

void TupGraphicsScene::mouseMoved(QGraphicsSceneMouseEvent *event)
{
    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool && k->isDrawing)
        k->tool->move(k->inputInformation, k->brushManager,  this);
}

void TupGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // SQA: Temporal solution for cases when there's no current frame defined
    if (!currentFrame()) {
        return;
    }

    QGraphicsScene::mouseReleaseEvent(event);
    mouseReleased(event);
}

void TupGraphicsScene::mouseReleased(QGraphicsSceneMouseEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->tool->toolType() == TupToolInterface::Brush) {
        if (event->button() == Qt::RightButton) {
            return;
        }
    }

    if (currentFrame()) {
        if (currentFrame()->isLocked()) {
            #ifdef K_DEBUG
                   tFatal() << "TupGraphicsScene::mouseReleased() - Frame is locked!";
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
           if (k->tool->name().compare(tr("Zoom In")) == 0 || k->tool->name().compare(tr("Zoom Out")) == 0) {
               if (event->button() != Qt::RightButton) 
                   k->tool->release(k->inputInformation, k->brushManager, this);
           }
        }
    }

    k->isDrawing = false;
}

void TupGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);

    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool)
        k->tool->doubleClick(k->inputInformation, this);
}

void TupGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    if (k->tool) {
        k->tool->keyPressEvent(event);
        
        if (event->isAccepted())
            return;
    }
    
    QGraphicsScene::keyPressEvent(event);
}

void TupGraphicsScene::keyReleaseEvent(QKeyEvent *event)
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

void TupGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
{
    if (event->mimeData()->hasFormat("tupi-ruler"))
        event->acceptProposedAction();

    TupLineGuide *line = 0;
    if (event->mimeData()->data("tupi-ruler") == "verticalLine") {
        line  = new TupLineGuide(Qt::Vertical, this);
        line->setPos(event->scenePos());
    } else {
        line = new TupLineGuide(Qt::Horizontal, this);
        line->setPos(event->scenePos());
    }

    if (line)
        k->lines << line;
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void  TupGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event)
{
    Q_UNUSED(event);

    removeItem(k->lines.last());
    delete k->lines.takeLast();
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void TupGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    if (!k->lines.isEmpty())
        k->lines.last()->setPos(event->scenePos());
}

// TODO: Check this code, not sure whether it does something or it is helping :S

void TupGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    Q_UNUSED(event);

    if (k->tool) {
        if (k->tool->toolType() == TupToolPlugin::Selection) {
            k->lines.last()->setEnabledSyncCursor(false);
            k->lines.last()->setFlag(QGraphicsItem::ItemIsMovable, true);
        }
    }
}

*/

bool TupGraphicsScene::event(QEvent *e)
{
    return QGraphicsScene::event(e);
}

void TupGraphicsScene::sceneResponse(TupSceneResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->sceneResponse(event);
}

void TupGraphicsScene::layerResponse(TupLayerResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->layerResponse(event);
}

void TupGraphicsScene::frameResponse(TupFrameResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool)
        k->tool->frameResponse(event);
}

void TupGraphicsScene::itemResponse(TupItemResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("scene");
    #endif

    if (k->tool) 
        k->tool->itemResponse(event);
}

bool TupGraphicsScene::isDrawing() const
{
    return k->isDrawing;
}

TupBrushManager *TupGraphicsScene::brushManager() const
{
    return k->brushManager;
}

void TupGraphicsScene::aboutToMousePress()
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

void TupGraphicsScene::includeObject(QGraphicsItem *object)
{
    if (k->spaceMode == TupProject::FRAMES_EDITION) {
        TupLayer *layer = k->scene->layer(k->framePosition.layer);
        if (layer) {
            TupFrame *frame = layer->frame(k->framePosition.frame);
            if (frame) {
                int zLevel = frame->getTopZLevel();
                object->setZValue(zLevel);
                addItem(object);
            }
        }
    } else {
        TupBackground *bg = k->scene->background();
        if (bg) {
            if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION) {
                TupFrame *frame = bg->staticFrame();
                if (frame) {
                    int zLevel = frame->getTopZLevel();
                    object->setZValue(zLevel);
                    addItem(object);
                }
            } else if (k->spaceMode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                       TupFrame *frame = bg->dynamicFrame(k->framePosition.frame);
                       if (frame) {
                           int zLevel = frame->getTopZLevel();
                           object->setZValue(zLevel);
                           addItem(object);
                       }
            }
        }
    }
}

void TupGraphicsScene::removeScene()
{
    cleanWorkSpace();
    k->scene = 0;
}

TupProject::Mode TupGraphicsScene::spaceMode()
{
    return k->spaceMode;
}

void TupGraphicsScene::setSpaceMode(TupProject::Mode mode)
{
    k->spaceMode = mode;    
}

void TupGraphicsScene::setOnionFactor(double opacity)
{
    k->opacity = opacity;
    if (k->spaceMode == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

double TupGraphicsScene::opacity()
{
    return k->opacity;
}

int TupGraphicsScene::framesTotal() 
{
    TupLayer *layer = k->scene->layer(k->framePosition.layer);
    if (layer)
        return layer->framesTotal();
    else
        return -1;
}
