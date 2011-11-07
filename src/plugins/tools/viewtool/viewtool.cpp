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

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"

#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"

#include "ktscene.h"

#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"

ViewTool::ViewTool() : m_rect(0), m_scene(0), m_configurator(0)
{
    TINIT;

    setupActions();
}

ViewTool::~ViewTool()
{
    TEND;
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
    zoomAction->setCursor(QCursor(THEME_DIR + "cursors/zoom.png"));
    
    m_actions.insert(tr("Zoom"), zoomAction);
    
    TAction *handAction = new TAction(QIcon(THEME_DIR + "icons/hand.png"), tr("Hand"), this);
    handAction->setShortcut(QKeySequence(tr("H")));
    handAction->setCursor(QCursor(THEME_DIR + "cursors/hand.png"));

    m_actions.insert(tr("Hand"), handAction);
}

void ViewTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    added = false;
    m_rect = new QGraphicsRectItem(QRectF(input->pos(), QSize(0,0)));
    m_rect->setPen(QPen(Qt::red, 1, Qt::SolidLine));

    firstPoint = input->pos();
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
        m_scene = scene;
    } else if (name() == tr("Zoom") && input->keyModifiers() == Qt::ControlModifier) {

               if (!added) {
                   scene->addItem(m_rect);
                   added = true;
               }

               int xMouse = input->pos().x();
               int yMouse = input->pos().y();
               int xInit = firstPoint.x();
               int yInit = firstPoint.y();

               QRectF rect = m_rect->rect();

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

               m_rect->setRect(rect);
               //rect = rect.normalized();

               if (rect.height() > 10 && rect.width() > 10)
                   m_rect->setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
               else
                   m_rect->setPen(QPen(QColor(250, 0, 0, 150), 1, Qt::SolidLine));
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
                     if (m_rect) {
                         rect = m_rect->rect();
                     } else {
                         int xMouse = input->pos().x();
                         int yMouse = input->pos().y();
                         int xInit = firstPoint.x();
                         int yInit = firstPoint.y();

                         QRectF rect = m_rect->rect();

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

            foreach (QGraphicsView * view, scene->views()) {
                     if (input->button() == Qt::LeftButton) {
                         // SQA: the function centerOn is not doing what we really need :S
                         view->centerOn(input->pos());
                         view->scale(1 + m_configurator->getFactor(), 1 + m_configurator->getFactor());
                     } else {
                         if (input->button() == Qt::RightButton) {
                             view->centerOn(input->pos());
                             view->scale(1 - m_configurator->getFactor(), 1 - m_configurator->getFactor());
                         }
                     } 
            }
        }

        if (m_rect) {
            delete m_rect;
            m_rect = 0;
        }
    }
}

QMap<QString, TAction *> ViewTool::actions() const
{
    return m_actions;
}

int ViewTool::toolType() const
{
    return View;
}

QWidget *ViewTool::configurator()
{
    if (! m_configurator)
        m_configurator = new ZoomConfigurator;

    return m_configurator;
}

void ViewTool::aboutToChangeScene(KTGraphicsScene *)
{
}

void ViewTool::aboutToChangeTool()
{
    if (name() == tr("Hand")) {
       if (m_scene) {
           foreach (QGraphicsView * view, m_scene->views())
                    view->setDragMode(QGraphicsView::NoDrag);
       }
    }
}

void ViewTool::saveConfig()
{
    if (m_configurator) {
        TCONFIG->beginGroup("ZoomTool");
        TCONFIG->setValue("zoomFactor", m_configurator->getFactor());
    }
}

Q_EXPORT_PLUGIN2(kt_view, ViewTool)
