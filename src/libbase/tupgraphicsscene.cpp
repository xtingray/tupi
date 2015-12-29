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
#include "tuppathitem.h"
#include "tuplineitem.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tupguideline.h"
#include "tuplibrary.h"

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

    QList<TupLineGuide *> lines;
    TupProject::Mode spaceContext;   
    TupLibrary *library;
    double layerOnProcessOpacity;
    int zLevel;
};

TupGraphicsScene::TupGraphicsScene() : QGraphicsScene(), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    setItemIndexMethod(QGraphicsScene::NoIndex);

    k->framePosition.layer = -1;
    k->framePosition.frame = -1;
    k->spaceContext = TupProject::FRAMES_EDITION;

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
        #ifdef Q_OS_WIN
            qDebug() << "[~TupGraphicsScene()]";
        #else
            TEND;
        #endif
    #endif

    clearFocus();
    clearSelection();

    // SQA: Check if these instructions are actually required
    // foreach (QGraphicsView *view, this->views())
    //          view->setScene(0);

    // SQA: Check if these instructions are actually required
    // foreach (QGraphicsItem *item, items())
    //          removeItem(item);

    delete k;
}

void TupGraphicsScene::setCurrentFrame(int layer, int frame)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setCurrentFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if ((frame != k->framePosition.frame && k->framePosition.frame >= 0) ||
        (layer != k->framePosition.layer && k->framePosition.layer >= 0)) {
        if (k->tool->name().compare(tr("PolyLine")) == 0 || k->tool->toolType() == TupToolInterface::Tweener)
            k->tool->aboutToChangeScene(this);
    }

    k->framePosition.layer = layer;
    k->framePosition.frame = frame;

    foreach (QGraphicsView *view, views())
             view->setDragMode(QGraphicsView::NoDrag);
}

void TupGraphicsScene::drawCurrentPhotogram()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::drawCurrentPhotogram()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    TupLayer *layer = k->scene->layer(k->framePosition.layer);
    int frames = layer->framesCount();

    if (k->framePosition.frame >= frames)
        k->framePosition.frame = frames - 1;

    if (k->spaceContext == TupProject::FRAMES_EDITION) {
        drawPhotogram(k->framePosition.frame, true);
    } else {
        cleanWorkSpace();
        drawSceneBackground(k->framePosition.frame);
    }
}

void TupGraphicsScene::drawPhotogram(int photogram, bool drawContext)
{ 
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::drawPhotogram()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (photogram < 0 || !k->scene) 
        return;

    cleanWorkSpace();
    // Painting the background
    drawSceneBackground(photogram);
    bool valid = false;

    // Drawing frames from every layer

    for (int i=0; i < k->scene->layersCount(); i++) {
         TupLayer *layer = k->scene->layer(i);
         k->layerOnProcessOpacity = layer->opacity();
         int framesCount = layer->framesCount();
         k->zLevel = (i + 2)*ZLAYER_LIMIT;

         if (photogram < framesCount) {
             TupFrame *mainFrame = layer->frame(photogram);
             QString currentFrame = "";

             if (mainFrame) {
                 if (layer) {
                     if (layer->isVisible()) {
                         if (drawContext) {
                             // Painting previews frames
                             int maximum = qMax(k->onionSkin.previous, k->onionSkin.next);
                             double opacityFactor = k->opacity / (double)maximum;
                             double opacity = k->opacity + ((maximum - k->onionSkin.previous)*opacityFactor);
                             if (k->onionSkin.previous > 0 && photogram > 0) {
                                 int limit = photogram - k->onionSkin.previous;
                                 if (limit < 0) 
                                     limit = 0;

                                 for (int frameIndex = limit; frameIndex < photogram; frameIndex++) {
                                      TupFrame *frame = layer->frame(frameIndex);
                                      if (frame)
                                          addFrame(frame, opacity, Previous);

                                      opacity += opacityFactor;
                                 }
                             }

                             // Painting next frames
                             if (k->onionSkin.next > 0 && framesCount > photogram + 1) {
                                 opacity = k->opacity + (opacityFactor*(maximum - 1));

                                 int limit = photogram + k->onionSkin.next;
                                 if (limit >= framesCount) 
                                     limit = framesCount - 1;

                                 for (int frameIndex = photogram+1; frameIndex <= limit; frameIndex++) {
                                      TupFrame * frame = layer->frame(frameIndex);
                                      if (frame)
                                          addFrame(frame, opacity, Next);
                      
                                      opacity -= opacityFactor;
                                 }
                             }
                         }

                         // Painting current frame
                         addFrame(mainFrame);

                         // addLipSyncObjects(layer, photogram, mainFrame->getTopZLevel());
                         addLipSyncObjects(layer, photogram, k->zLevel);

                         // SQA: Crashpoint when layers are deleted 
                         valid = true;
                     }
                  }
              }
         }
    }

    // Drawing tweening objects

    if (valid) {
        addTweeningObjects(photogram);
        addSvgTweeningObjects(photogram);
    }

    if (k->tool)
        k->tool->updateScene(this);
}

void TupGraphicsScene::drawSceneBackground(int photogram)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::drawSceneBackground()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_CHECK_PTR(k->scene);

    if (!k->scene) {
        #ifdef K_DEBUG
            QString msg = "TupGraphicsScene::drawSceneBackground() - Warning: k->scene is NULL!";
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif
        return;
    }

    TupBackground *bg = k->scene->background();
    if (bg) {
        if (k->spaceContext == TupProject::DYNAMIC_BACKGROUND_EDITION) {
            if (!bg->dynamicBgIsEmpty()) {
                TupFrame *frame = bg->dynamicFrame();
                if (frame) {
                    k->zLevel = 0;
                    addFrame(frame, frame->opacity());
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupGraphicsScene::drawSceneBackground() - Dynamic background frame is empty";
                    #ifdef Q_OS_WIN
                        qWarning() << msg;
                    #else
                        tWarning() << msg;
                    #endif
                #endif
            }
        } else if (k->spaceContext == TupProject::FRAMES_EDITION) {
                   if (!bg->dynamicBgIsEmpty()) {
                       if (bg->rasterRenderIsPending()) 
                           bg->renderDynamicView();

                       QPixmap pixmap = bg->dynamicView(photogram);
                       QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
                       item->setZValue(0);
                       TupFrame *frame = bg->dynamicFrame();
                       if (frame) 
                           item->setOpacity(frame->opacity());
                       addItem(item);
                   } else {
                       #ifdef K_DEBUG
                           QString msg = "TupGraphicsScene::drawSceneBackground() - Dynamic background frame is empty";
                           #ifdef Q_OS_WIN
                               qWarning() << msg;
                           #else
                               tWarning() << msg;
                           #endif
                       #endif
                   }
        }

        if (k->spaceContext == TupProject::STATIC_BACKGROUND_EDITION || k->spaceContext == TupProject::FRAMES_EDITION) {
            if (k->spaceContext == TupProject::STATIC_BACKGROUND_EDITION) {
                if (!bg->dynamicBgIsEmpty()) {
                    if (bg->rasterRenderIsPending())
                        bg->renderDynamicView();

                    QPixmap pixmap = bg->dynamicView(0);
                    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
                    item->setZValue(0);
                    TupFrame *frame = bg->dynamicFrame();
                    if (frame)
                        item->setOpacity(frame->opacity());
                    addItem(item);
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupGraphicsScene::drawSceneBackground() - Dynamic background frame is empty";
                        #ifdef Q_OS_WIN
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                }
            }

            if (!bg->staticBgIsEmpty()) {
                TupFrame *frame = bg->staticFrame();
                if (frame) {
                    k->zLevel = ZLAYER_LIMIT;
                    addFrame(frame, frame->opacity());
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupGraphicsScene::drawSceneBackground() - Static background frame is empty";
                    #ifdef Q_OS_WIN
                        qWarning() << msg;
                    #else
                        tWarning() << msg;
                    #endif
                #endif
            }
        }
    }
}

void TupGraphicsScene::addFrame(TupFrame *frame, double opacity, Context mode)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::addFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    TupFrame::FrameType frameType = frame->type();

    for (int i=0; i < frame->graphicItemsCount(); i++) {
         TupGraphicObject *object = frame->graphic(i);
         if (mode != TupGraphicsScene::Current) {
             if (!object->hasTween())
                 addGraphicObject(object, frameType, opacity);
         } else {
             addGraphicObject(object, frameType,  opacity);
         }
    }

    for (int i=0; i < frame->svgItemsCount(); i++) {
         TupSvgItem *object = frame->svg(i);
         if (!object->hasTween()) {
             addSvgObject(object, frameType, opacity);
         } else {
             TupItemTweener *tween = object->tween();
             if (k->framePosition.frame == tween->initFrame())
                 addSvgObject(object, frameType, opacity);
         }
    }
}

void TupGraphicsScene::addGraphicObject(TupGraphicObject *object, TupFrame::FrameType frameType, double opacity)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::addGraphicObject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QGraphicsItem *item = object->item();
    if (item) {
        k->onionSkin.opacityMap.insert(item, opacity);

        if (TupItemGroup *group = qgraphicsitem_cast<TupItemGroup *>(item)) {
            if (frameType == TupFrame::Regular)
                group->setOpacity(opacity * k->layerOnProcessOpacity);
            else
                group->setOpacity(opacity);

            group->setZValue(k->zLevel);
            k->zLevel++;
            group->setSelected(false);

            addItem(group);
            return;
        }

        if (frameType == TupFrame::Regular)
            item->setOpacity(opacity * k->layerOnProcessOpacity);
        else
            item->setOpacity(opacity);

        item->setZValue(k->zLevel);
        k->zLevel++;
        item->setSelected(false);

        addItem(item);
    }
}

void TupGraphicsScene::addSvgObject(TupSvgItem *svgItem, TupFrame::FrameType frameType, double opacity)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::addSvgObject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (svgItem) {
        k->onionSkin.opacityMap.insert(svgItem, opacity);
        svgItem->setSelected(false);

        TupLayer *layer = k->scene->layer(k->framePosition.layer);
        if (layer) {
            TupFrame *frame = layer->frame(k->framePosition.frame);
            if (frame) {
                if (frameType == TupFrame::Regular)
                    svgItem->setOpacity(opacity * k->layerOnProcessOpacity);
                else
                    svgItem->setOpacity(opacity);

                // SQA: Experimental code related to interactive features
                // if (svgItem->symbolName().compare("dollar.svg")==0)
                //     connect(svgItem, SIGNAL(enabledChanged()), this, SIGNAL(showInfoWidget()));

                svgItem->setZValue(k->zLevel);
                k->zLevel++;

                // SQA: Pending to implement SVG group support

                addItem(svgItem);
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupGraphicsScene::addSvgObject() - Error: Frame #" + QString::number(k->framePosition.frame) + " NO available!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tFatal() << msg;
                    #endif
                #endif
            }
        } else {
                #ifdef K_DEBUG
                    QString msg = "TupGraphicsScene::addSvgObject() - Error: Layer #" + QString::number(k->framePosition.layer) + " NO available!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tFatal() << msg;
                    #endif
                #endif
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupGraphicsScene::addSvgObject() - Error: No SVG item!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
    } 
} 

void TupGraphicsScene::addTweeningObjects(int photogram)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::addTweeningObjects()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QList<TupGraphicObject *> tweenList = k->scene->tweeningGraphicObjects();
    int total = tweenList.count();
    for (int i=0; i < total; i++) {
         TupGraphicObject *object = tweenList.at(i);

         if (object->layerIsVisible()) {
             int origin = object->frameIndex();

             if (TupItemTweener *tween = object->tween()) {
                 int adjustX = object->item()->boundingRect().width()/2;
                 int adjustY = object->item()->boundingRect().height()/2;

                 if (origin == photogram) {
                     TupTweenerStep *stepItem = tween->stepAt(0);
                     object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: 0"));

                     if (tween->type() == TupItemTweener::Composed) {
                         object->item()->setTransformOriginPoint(tween->transformOriginPoint());

                         if (stepItem->has(TupTweenerStep::Position)) {
                             QPointF point = QPoint(-adjustX, -adjustY);
                             object->setLastTweenPos(stepItem->position() + point);
                             object->item()->setPos(tween->transformOriginPoint());
                         }

                         if (stepItem->has(TupTweenerStep::Rotation)) {
                             QRectF rect = object->item()->sceneBoundingRect();
                             object->item()->setTransformOriginPoint(rect.center());
                             double angle = stepItem->rotation();
                             object->item()->setRotation(angle);
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
                             // tError() << "TupGraphicsScene::addTweeningObjects() - Origin point: " << point.x() << ", " << point.y();
                             object->item()->setTransformOriginPoint(point);
                             // object->item()->setScale(1.0);

                             QTransform transform = object->item()->transform();
                             transform.reset();
                             object->item()->setTransform(transform); 
                         }

                         if (stepItem->has(TupTweenerStep::Shear)) {
                             QTransform transform;
                             transform.shear(0, 0);
                             object->item()->setTransform(transform);
                         } 

                         if (stepItem->has(TupTweenerStep::Coloring)) {
                             if (tween->tweenColorFillType() == TupItemTweener::Line || tween->tweenColorFillType() == TupItemTweener::FillAll) {
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

                             if (tween->tweenColorFillType() == TupItemTweener::Internal || tween->tweenColorFillType() == TupItemTweener::FillAll) {
                                 QColor itemColor = stepItem->color();
                                 if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(object->item())) {
                                     QBrush brush = path->brush();
                                     brush.setColor(itemColor);
                                     path->setBrush(brush);
                                 } else if (TupEllipseItem *ellipse = qgraphicsitem_cast<TupEllipseItem *>(object->item())) {
                                            QBrush brush = ellipse->brush();
                                            brush.setColor(itemColor);
                                            ellipse->setBrush(brush);
                                 } else if (TupRectItem *rect = qgraphicsitem_cast<TupRectItem *>(object->item())) {
                                            QBrush brush = rect->brush();
                                            brush.setColor(itemColor);
                                            rect->setBrush(brush);
                                 }
                             }
                         }

                         if (stepItem->has(TupTweenerStep::Opacity))
                             object->item()->setOpacity(stepItem->opacity());
                     }

                 } else if ((origin < photogram) && (photogram < origin + tween->frames())) {
                            int step = photogram - origin;
                            TupTweenerStep *stepItem = tween->stepAt(step);
                            object->item()->setToolTip(tween->tweenType() + ": " + tween->name() + tr("/Step: ") + QString::number(step));
                            if (tween->type() == TupItemTweener::Composed) {
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

                                addGraphicObject(object, TupFrame::Regular);

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
                                    if (tween->tweenColorFillType() == TupItemTweener::Line || tween->tweenColorFillType() == TupItemTweener::FillAll) {
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

                                    if (tween->tweenColorFillType() == TupItemTweener::Internal || tween->tweenColorFillType() == TupItemTweener::FillAll) {
                                        QColor itemColor = stepItem->color();
                                        if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(object->item())) {
                                            QBrush brush = path->brush();
                                            brush.setColor(itemColor);
                                            path->setBrush(brush);
                                        } else if (TupEllipseItem *ellipse = qgraphicsitem_cast<TupEllipseItem *>(object->item())) {
                                                   QBrush brush = ellipse->brush();
                                                   brush.setColor(itemColor);
                                                   ellipse->setBrush(brush);
                                        } else if (TupRectItem *rect = qgraphicsitem_cast<TupRectItem *>(object->item())) {
                                                   QBrush brush = rect->brush();
                                                   brush.setColor(itemColor);
                                                   rect->setBrush(brush);
                                        }
                                    }
                                }

                                addGraphicObject(object, TupFrame::Regular);

                                if (stepItem->has(TupTweenerStep::Opacity))
                                    object->item()->setOpacity(stepItem->opacity());
                            }    
                 }
             } else {
                 #ifdef K_DEBUG
                     QString msg = "TupGraphicsScene::addTweeningObjects() - Error: Graphic object has no tween!";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
             }
        }
    }
}

void TupGraphicsScene::addSvgTweeningObjects(int photogram)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::addSvgTweeningObjects()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QList<TupSvgItem *> svgList = k->scene->tweeningSvgObjects();

    for (int i=0; i < svgList.count(); i++) {
         TupSvgItem *object = svgList.at(i);

         if (object->layerIsVisible()) {
             int origin = object->frameIndex();

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

                            addSvgObject(object, TupFrame::Regular);

                            if (stepItem->has(TupTweenerStep::Opacity))
                                object->setOpacity(stepItem->opacity());
                 }
             } else {
                 #ifdef K_DEBUG
                     QString msg = "TupGraphicsScene::addSvgTweeningObjects() - No tween found!";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
             }
         }
    }
}

void TupGraphicsScene::addLipSyncObjects(TupLayer *layer, int photogram, int zLevel)
{
    if (layer->lipSyncCount() > 0) {
        Mouths mouths = layer->lipSyncList();
        for (int i=0; i<mouths.count(); i++) {
             TupLipSync *lipSync = mouths.at(i);
             int initFrame = lipSync->initFrame();

             if ((photogram >= initFrame) && (photogram <= initFrame + lipSync->framesCount())) {
                 QString name = lipSync->name();
                 TupLibraryFolder *folder = k->library->getFolder(name);
                 if (folder) {
                     QList<TupVoice *> voices = lipSync->voices();
                     int total = voices.count();
                     for(int i=0; i < total; i++) {
                         TupVoice *voice = voices.at(i);
                         int index = photogram - initFrame; 
                         if (voice->contains(index)) {
                             // Add image here
                             QString phoneme = voice->getPhoneme(index);
                             TupLibraryObject *image = folder->getObject(phoneme + lipSync->picExtension());
                             if (image) {
                                 TupGraphicLibraryItem *item = new TupGraphicLibraryItem(image);
                                 if (item) {
                                     item->setPos(voice->mouthPos());
                                     item->setToolTip(tr("lipsync:") + name + ":" + QString::number(i));
                                     item->setZValue(zLevel);
                                     addItem(item);
                                 }
                             }
                         }
                     }
                 } 
             }
        }
    }
}

void TupGraphicsScene::cleanWorkSpace()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::cleanWorkSpace()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

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
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::currentFrameIndex()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    return k->framePosition.frame;
}

int TupGraphicsScene::currentLayerIndex() const
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::currentLayerIndex()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    return k->framePosition.layer;
}

int TupGraphicsScene::currentSceneIndex() const
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::currentSceneIndex()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (!k->scene) {
        #ifdef K_DEBUG
            QString msg = "TupGraphicsScene::currentSceneIndex() - Error: Scene index is -1";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return -1;
    }

    return k->scene->objectIndex();
}

void TupGraphicsScene::setNextOnionSkinCount(int n)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setNextOnionSkinCount()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    k->onionSkin.next = n;
    if (k->spaceContext == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

void TupGraphicsScene::setPreviousOnionSkinCount(int n)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setPreviousOnionSkinCount()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    k->onionSkin.previous = n;
    if (k->spaceContext == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

TupFrame *TupGraphicsScene::currentFrame()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN 
            qDebug() << "[TupGraphicsScene::currentFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->scene) {
        if (k->scene->layersCount() > 0) {
            if (k->framePosition.layer < k->scene->layersCount()) {
                TupLayer *layer = k->scene->layer(k->framePosition.layer);
                Q_CHECK_PTR(layer);
                if (layer) {
                    if (!layer->frames().isEmpty())
                        return layer->frame(k->framePosition.frame);
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupGraphicsScene::currentFrame - No layer available at -> " + QString::number(k->framePosition.frame); 
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tFatal() << msg;
                        #endif
                    #endif
                }
            } else {
                TupLayer *layer = k->scene->layer(k->scene->layersCount() - 1);
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setCurrentScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_CHECK_PTR(scene);

    setCurrentFrame(0, 0);

    if (k->tool)
        k->tool->aboutToChangeScene(this);

    qDeleteAll(k->lines);
    k->lines.clear();

    cleanWorkSpace();
    k->scene = scene;

    if (k->spaceContext == TupProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else if (k->spaceContext == TupProject::STATIC_BACKGROUND_EDITION) {
               drawSceneBackground(k->framePosition.frame);
    }
}

void TupGraphicsScene::setLayerVisible(int layerIndex, bool visible)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setLayerVisible()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (!k->scene)
        return;

    if (TupLayer *layer = k->scene->layer(layerIndex))
        layer->setVisible(visible);
}

TupScene *TupGraphicsScene::scene() const
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::scene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->scene)
        return k->scene;
    else
        return 0;
}

void TupGraphicsScene::setTool(TupToolPlugin *tool)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::setTool()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->spaceContext == TupProject::FRAMES_EDITION) {
        drawCurrentPhotogram();
    } else {
        cleanWorkSpace();
        drawSceneBackground(k->framePosition.frame);
    }

    if (k->tool)
        k->tool->aboutToChangeTool();

    k->tool = tool;
    k->tool->init(this);
}

TupToolPlugin *TupGraphicsScene::currentTool() const
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::currentTool()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    return k->tool;
}

void TupGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mousePressEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QGraphicsScene::mousePressEvent(event);
    k->inputInformation->updateFromMouseEvent(event);
    k->isDrawing = false;

    // This condition locks all the tools while workspace is rotated 
    if ((event->buttons() != Qt::LeftButton) || (event->modifiers () != (Qt::ShiftModifier | Qt::ControlModifier))) {
        if (k->tool) {      
            if (k->tool->toolType() == TupToolPlugin::Brush && event->isAccepted())
                return;

            if (k->tool->toolType() == TupToolPlugin::Tweener && event->isAccepted()) {
                if (k->tool->currentEditMode() == TupToolPlugin::Properties)
                    return;
            } 

            // If there's no frame... the tool is disabled 
            if (currentFrame()) {
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
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mouseMoveEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    QGraphicsScene::mouseMoveEvent(event);
    mouseMoved(event);

    if (k->tool) {
        if (k->tool->name().compare(tr("Line")) == 0)
            k->tool->updatePos(event->scenePos());
    }
}

void TupGraphicsScene::mouseMoved(QGraphicsSceneMouseEvent *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mouseMoved()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    k->inputInformation->updateFromMouseEvent(event);

    if (k->tool && k->isDrawing)
        k->tool->move(k->inputInformation, k->brushManager,  this);
}

void TupGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mouseReleaseEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    // SQA: Temporal solution for cases when there's no current frame defined
    if (!currentFrame())
        return;

    QGraphicsScene::mouseReleaseEvent(event);
    mouseReleased(event);
}

void TupGraphicsScene::mouseReleased(QGraphicsSceneMouseEvent *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mouseReleased()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->tool->toolType() == TupToolInterface::Brush) {
        if (event->button() == Qt::RightButton) 
            return;
    }

    if (currentFrame()) {
        if (currentFrame()->isLocked()) {
            #ifdef K_DEBUG
                QString msg = "TupGraphicsScene::mouseReleased() - Frame is locked!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tFatal() << msg;
                #endif
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
    } 

    k->isDrawing = false;
}

void TupGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::mouseDoubleClickEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

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
// SQA: Check this code, not sure whether it does something or it's handy :S
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
        if (k->tool->toolType() == TupToolPlugin::ObjectsTool) {
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
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::sceneResponse()]";
        #else
            T_FUNCINFOX("scene");
        #endif
    #endif
    */

    if (k->tool)
        k->tool->sceneResponse(event);
}

void TupGraphicsScene::layerResponse(TupLayerResponse *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::layerResponse()]";
        #else
            T_FUNCINFOX("layer");
        #endif
    #endif
    */

    if (k->tool)
        k->tool->layerResponse(event);
}

void TupGraphicsScene::frameResponse(TupFrameResponse *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::frameResponse()]";
        #else
            T_FUNCINFOX("frame");
        #endif
    #endif
    */

    if (k->tool)
        k->tool->frameResponse(event);
}

void TupGraphicsScene::itemResponse(TupItemResponse *event)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::itemResponse()]";
        #else
            T_FUNCINFOX("item");
        #endif
    #endif
    */
   
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::aboutToMousePress()]";
        #else
            T_FUNCINFOX("item");
        #endif
    #endif

    if (k->onionSkin.opacityMap.empty() || k->tool->toolType() == TupToolInterface::Tweener)
        return;

    QHash<QGraphicsItem *, double>::iterator it = k->onionSkin.opacityMap.begin();
    QString tool = k->tool->name();

    if (tool.compare(tr("Object Selection")) == 0 || tool.compare(tr("Nodes Selection")) == 0) {
        while (it != k->onionSkin.opacityMap.end()) {
            if (it.value() != 1.0 || it.key()->toolTip().length() > 0) {
                it.key()->setAcceptedMouseButtons(Qt::NoButton);
                it.key()->setFlag(QGraphicsItem::ItemIsSelectable, false);
                it.key()->setFlag(QGraphicsItem::ItemIsMovable, false);
            } else {
                it.key()->setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton | Qt::XButton1
                                                  | Qt::XButton2);
                if (tool.compare(tr("Object Selection")) == 0) {
                    it.key()->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                } else {
                    it.key()->setFlag(QGraphicsItem::ItemIsSelectable, true);
                    it.key()->setFlag(QGraphicsItem::ItemIsMovable, false);
                }
            }
            ++it;
        }
    } else {
        while (it != k->onionSkin.opacityMap.end()) {
            it.key()->setAcceptedMouseButtons(Qt::NoButton);
            it.key()->setFlag(QGraphicsItem::ItemIsSelectable, false);
            it.key()->setFlag(QGraphicsItem::ItemIsMovable, false);
            ++it;
        }
    }
}

void TupGraphicsScene::includeObject(QGraphicsItem *object, bool isPolyLine) // SQA: Bool parameter is a hack for the Polyline tool. Must be fixed. 
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupGraphicsScene::includeObject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->spaceContext == TupProject::FRAMES_EDITION) {
        TupLayer *layer = k->scene->layer(k->framePosition.layer);
        if (layer) {
            TupFrame *frame = layer->frame(k->framePosition.frame);
            if (frame) {
                // int zLevel = frame->getTopZLevel();
                int zLevel = k->zLevel;
                if (isPolyLine) // SQA: Polyline hack
                    zLevel--;
                if (object) {
                    object->setOpacity(layer->opacity());
                    object->setZValue(zLevel);
                    addItem(object);
                    k->zLevel++;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupGraphicsScene::includeObject() - Fatal Error: Graphic item is NULL!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            }
        }
    } else {
        TupBackground *bg = k->scene->background();
        if (bg) {
            if (k->spaceContext == TupProject::STATIC_BACKGROUND_EDITION) {
                TupFrame *frame = bg->staticFrame();
                if (frame) {
                    int zLevel = frame->getTopZLevel();
                    object->setZValue(zLevel);
                    addItem(object);
                }
            } else if (k->spaceContext == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                       TupFrame *frame = bg->dynamicFrame();
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

TupProject::Mode TupGraphicsScene::spaceContext()
{
    return k->spaceContext;
}

void TupGraphicsScene::setSpaceMode(TupProject::Mode mode)
{
    k->spaceContext = mode;    
}

void TupGraphicsScene::setOnionFactor(double opacity)
{
    k->opacity = opacity;
    if (k->spaceContext == TupProject::FRAMES_EDITION)
        drawCurrentPhotogram();
}

double TupGraphicsScene::opacity()
{
    return k->opacity;
}

int TupGraphicsScene::framesCount() 
{
    TupLayer *layer = k->scene->layer(k->framePosition.layer);
    if (layer)
        return layer->framesCount();
    else
        return -1;
}

void TupGraphicsScene::setLibrary(TupLibrary *library)
{
    k->library = library;
}

void TupGraphicsScene::resetCurrentTool() 
{
    k->tool->init(this);
}
