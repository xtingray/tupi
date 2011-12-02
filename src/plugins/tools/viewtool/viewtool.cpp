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
#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"
#include "ktscene.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"

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
    KTGraphicsScene *scene;
    ZoomConfigurator *configurator;
    QCursor zoomCursor;
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

void ViewTool::init(KTGraphicsScene *scene)
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
    return QStringList() << tr("Zoom") << tr("Hand");
}

void ViewTool::setupActions()
{
    TAction *zoomAction = new TAction(QIcon(THEME_DIR + "icons/zoom.png"), tr("Zoom"), this);
    zoomAction->setShortcut(QKeySequence(tr("Z")));
    k->zoomCursor = QCursor(THEME_DIR + "cursors/zoom.png");
    zoomAction->setCursor(k->zoomCursor);
    
    k->actions.insert(tr("Zoom"), zoomAction);
    
    TAction *handAction = new TAction(QIcon(THEME_DIR + "icons/hand.png"), tr("Hand"), this);
    handAction->setShortcut(QKeySequence(tr("H")));
    k->handCursor = QCursor(THEME_DIR + "cursors/hand.png");
    handAction->setCursor(k->handCursor);

    k->actions.insert(tr("Hand"), handAction);
}

void ViewTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    k->added = false;
    k->rect = new QGraphicsRectItem(QRectF(input->pos(), QSize(0,0)));
    k->rect->setPen(QPen(Qt::red, 1, Qt::SolidLine));

    k->firstPoint = input->pos();
}

void ViewTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    foreach (QGraphicsView * view, scene->views()) {
             if (name() == tr("Zoom"))
                 view->setDragMode(QGraphicsView::NoDrag);
             else if (name() == tr("Hand"))
                      view->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    if (name() == tr("Hand")) {
        k->scene = scene;
    } else if (name() == tr("Zoom") && input->keyModifiers() == Qt::ControlModifier) {

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

void ViewTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (name() == tr("Zoom")) { 
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

            foreach (QGraphicsView *view, scene->views()) {
                     if (input->button() == Qt::LeftButton) {
                         // SQA: the function centerOn is not doing what we really need :S
                         view->centerOn(input->pos());
                         view->scale(1 + k->configurator->getFactor(), 1 + k->configurator->getFactor());
                     } else {
                         if (input->button() == Qt::RightButton) {
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
    return KTToolInterface::View;
}

QWidget *ViewTool::configurator()
{
    if (! k->configurator)
        k->configurator = new ZoomConfigurator;

    return k->configurator;
}

void ViewTool::aboutToChangeScene(KTGraphicsScene *)
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
        QPair<int, int> flags = KTToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor ViewTool::cursor() const
{
   if (name() == tr("Hand")) {
       return k->handCursor;
   } else if (name() == tr("Zoom")) {
              return k->zoomCursor;
   }

   return 0;
}

Q_EXPORT_PLUGIN2(kt_view, ViewTool)
