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

#include "viewtool.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tupscene.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

struct ViewTool::Private
{
    QMap<QString, TAction *> actions;
    QGraphicsRectItem *rect;
    bool added;
    QPointF firstPoint;
    TupGraphicsScene *scene;
    ZoomConfigurator *configurator;
    QCursor zoomInCursor;
    QCursor zoomOutCursor;
    QCursor handCursor;
};

ViewTool::ViewTool() : k(new Private)
{
    k->rect = 0;
    k->scene = 0;
    k->configurator = 0;

    setupActions();
}

ViewTool::~ViewTool()
{
}

void ViewTool::init(TupGraphicsScene *scene)
{
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

QStringList ViewTool::keys() const
{
    return QStringList() << tr("Zoom In") << tr("Zoom Out") << tr("Hand");
}

void ViewTool::setupActions()
{
    TAction *zoomIn = new TAction(QIcon(kAppProp->themeDir() + "icons/zoom_in.png"), tr("Zoom In"), this);
    zoomIn->setShortcut(QKeySequence(tr("Z")));
    k->zoomInCursor = QCursor(kAppProp->themeDir() + "cursors/zoom.png");
    zoomIn->setCursor(k->zoomInCursor);
    
    k->actions.insert(tr("Zoom In"), zoomIn);

    TAction *zoomOut = new TAction(QIcon(kAppProp->themeDir() + "icons/zoom_out.png"), tr("Zoom Out"), this);
    zoomOut->setShortcut(QKeySequence(tr("Shift+Z")));
    k->zoomOutCursor = QCursor(kAppProp->themeDir() + "cursors/zoom.png");
    zoomOut->setCursor(k->zoomOutCursor);
   
    k->actions.insert(tr("Zoom Out"), zoomOut);
    
    TAction *handAction = new TAction(QIcon(kAppProp->themeDir() + "icons/hand.png"), tr("Hand"), this);
    handAction->setShortcut(QKeySequence(tr("H")));
    k->handCursor = QCursor(kAppProp->themeDir() + "cursors/hand.png");
    handAction->setCursor(k->handCursor);

    k->actions.insert(tr("Hand"), handAction);
}

void ViewTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    tError() << "ViewTool::press() - Just tracing 1!";

    k->added = false;
    k->rect = new QGraphicsRectItem(QRectF(input->pos(), QSize(0,0)));
    k->rect->setPen(QPen(Qt::red, 1, Qt::SolidLine));

    k->firstPoint = input->pos();
}

void ViewTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    foreach (QGraphicsView * view, scene->views()) {
             if (name() == tr("Zoom In") || name() == tr("Zoom Out"))
                 view->setDragMode(QGraphicsView::NoDrag);
             else if (name() == tr("Hand"))
                      view->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    if (name() == tr("Hand")) {
        k->scene = scene;
    } else if (name() == tr("Zoom In") && input->keyModifiers() == Qt::ControlModifier) {

               if (!k->added) {
                   scene->addItem(k->rect);
                   k->added = true;
               }

               int xMouse = input->pos().x();
               int yMouse = input->pos().y();
               int xInit = k->firstPoint.x();
               int yInit = k->firstPoint.y();

               QRectF rect = k->rect->rect();

               if (xMouse >= xInit) {
                   if (yMouse >= yInit)
                       rect.setBottomRight(input->pos());
                   else
                       rect.setTopRight(input->pos());
               } else {
                   if (yMouse >= yInit)
                       rect.setBottomLeft(input->pos());
                   else
                       rect.setTopLeft(input->pos());
               }

               k->rect->setRect(rect);
               //rect = rect.normalized();

               if (rect.height() > 10 && rect.width() > 10)
                   k->rect->setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
               else
                   k->rect->setPen(QPen(QColor(250, 0, 0, 150), 1, Qt::SolidLine));
    }
}

void ViewTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (name() == tr("Zoom In") || name() == tr("Zoom Out")) { 
        // Zoom Square mode
        if (input->button() == Qt::LeftButton && input->keyModifiers() == Qt::ControlModifier) {    

            foreach (QGraphicsView *view, scene->views()) {
                     QRectF rect;
                     if (k->rect) {
                         rect = k->rect->rect();
                     } else {
                         int xMouse = input->pos().x();
                         int yMouse = input->pos().y();
                         int xInit = k->firstPoint.x();
                         int yInit = k->firstPoint.y();

                         QRectF rect = k->rect->rect();

                         if (xMouse >= xInit) {
                             if (yMouse >= yInit)
                                 rect.setBottomRight(input->pos());
                             else
                                 rect.setTopRight(input->pos());
                         } else {
                             if (yMouse >= yInit)
                                 rect.setBottomLeft(input->pos());
                             else
                                 rect.setTopLeft(input->pos());
                         }
                     }

                     view->fitInView(rect, Qt::KeepAspectRatio);
            }

        } else { // Normal Zoom

            tError() << "ViewTool::release() - Just tracing 1!";

            foreach (QGraphicsView *view, scene->views()) {
                     /*
                     if (input->button() == Qt::LeftButton) {
                         // SQA: the function centerOn is not doing what we really need :S
                         view->centerOn(input->pos());
                         view->scale(1 + k->configurator->getFactor(), 1 + k->configurator->getFactor());
                     } else {
                         if (input->button() == Qt::RightButton) {
                             tError() << "ViewTool::release() - Just tracing 2!";
                             view->centerOn(input->pos());
                             view->scale(1 - k->configurator->getFactor(), 1 - k->configurator->getFactor());
                         }
                     } 
                     */

                     if (name() == tr("Zoom In")) {
                         // SQA: the function centerOn is not doing what we really need :S
                         view->centerOn(input->pos());
                         view->scale(1 + k->configurator->getFactor(), 1 + k->configurator->getFactor());
                     } else {
                         if (name() == tr("Zoom Out")) {
                             tError() << "ViewTool::release() - Just tracing 2!";
                             view->centerOn(input->pos());
                             view->scale(1 - k->configurator->getFactor(), 1 - k->configurator->getFactor());
                         }
                     }
            }
        }

        if (k->rect) {
            delete k->rect;
            k->rect = 0;
        }
    }
}

QMap<QString, TAction *> ViewTool::actions() const
{
    return k->actions;
}

int ViewTool::toolType() const
{
    return TupToolInterface::View;
}

QWidget *ViewTool::configurator()
{
    if (! k->configurator)
        k->configurator = new ZoomConfigurator;

    return k->configurator;
}

void ViewTool::aboutToChangeScene(TupGraphicsScene *)
{
}

void ViewTool::aboutToChangeTool()
{
    if (name() == tr("Hand")) {
       if (k->scene) {
           foreach (QGraphicsView * view, k->scene->views())
                    view->setDragMode(QGraphicsView::NoDrag);
       }
    }
}

void ViewTool::saveConfig()
{
    if (k->configurator) {
        TCONFIG->beginGroup("ZoomTool");
        TCONFIG->setValue("zoomFactor", k->configurator->getFactor());
    }
}

void ViewTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    // } else if (event->modifiers() != Qt::ShiftModifier && event->modifiers() != Qt::ControlModifier) {
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor ViewTool::cursor() const
{
   if (name() == tr("Hand")) {
       return k->handCursor;
   } else if (name() == tr("Zoom In")) {
              return k->zoomInCursor;
   } else if (name() == tr("Zoom Out")) {
              return k->zoomOutCursor;
   } 

   return QCursor(Qt::ArrowCursor);
}

Q_EXPORT_PLUGIN2(tup_view, ViewTool)
