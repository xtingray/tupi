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

#include "geometrictool.h"

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

#include "tglobal.h"
#include "tdebug.h"

#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"

#include "ktrequestbuilder.h"
#include "ktlibraryobject.h"

#include "ktscene.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktbrushmanager.h"

GeometricTool::GeometricTool()
{
    setupActions();
}

GeometricTool::~GeometricTool()
{
}

QStringList GeometricTool::keys() const
{
    return QStringList() << tr("Rectangle") << tr("Ellipse") << tr("Line");
}

void GeometricTool::init(KTGraphicsScene *scene)
{
    proportion = false;

    foreach (QGraphicsView * view, scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
             Q_CHECK_PTR(view->scene());
             if (QGraphicsScene *scene = qobject_cast<QGraphicsScene *>(view->scene())) {
                 foreach (QGraphicsItem *item, scene->items()) {
                          item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                          item->setFlag(QGraphicsItem::ItemIsMovable, false);
                 }
             }
    }
}

void GeometricTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(THEME_DIR + "icons/square.png"), tr("Rectangle"), this);
    action1->setShortcut(QKeySequence(tr("R")));
    action1->setCursor(QCursor(THEME_DIR + "cursors/square.png"));
    
    m_actions.insert( tr("Rectangle"), action1);
    
    TAction *action2 = new TAction(QIcon(THEME_DIR + "icons/ellipse.png"), tr("Ellipse"), this);
    action2->setShortcut(QKeySequence(tr("C")));
    action2->setCursor(QCursor(THEME_DIR + "cursors/circle.png"));
    
    m_actions.insert(tr("Ellipse"), action2);
    
    TAction *action3 = new TAction(QIcon(THEME_DIR + "icons/line.png"), tr("Line"), this);
    action3->setShortcut(QKeySequence(tr("L")));
    action3->setCursor(QCursor(THEME_DIR + "cursors/line.png"));

    m_actions.insert(tr("Line"), action3);
}

void GeometricTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (input->buttons() == Qt::LeftButton) {
        
        if (name() == tr("Rectangle")) {
            added = false;
            m_rect = new KTRectItem(QRectF(input->pos(), QSize(0,0)));
            m_rect->setPen(brushManager->pen());
            m_rect->setBrush(brushManager->brush());

            firstPoint = input->pos();

        } else if (name() == tr("Ellipse")) {

                   added = false;
                   m_ellipse = new KTEllipseItem(QRectF(input->pos(), QSize(0,0)));
                   m_ellipse->setPen(brushManager->pen());
                   m_ellipse->setBrush(brushManager->brush());

                   firstPoint = input->pos();

        } else if (name() == tr("Line")) {

                   added = false;
                   m_line = new KTLineItem();
                   m_line->setLine(QLineF(input->pos().x(), input->pos().y(), input->pos().x(), input->pos().y()));
                   m_line->setPen(brushManager->pen());

                   firstPoint = input->pos();
        }
    }
}

void GeometricTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
    
    if (name() == tr("Rectangle") || name() == tr("Ellipse")) {

        if (!added) {
            if (name() == tr("Rectangle"))
                scene->includeObject(m_rect);
            else
                scene->includeObject(m_ellipse);
            added = true;
        }

        int xMouse = input->pos().x();
        int yMouse = input->pos().y();
        int xInit = firstPoint.x();
        int yInit = firstPoint.y();

        QRectF rect;

        if (name() == tr("Rectangle"))
            rect = m_rect->rect();
        else
            rect = m_ellipse->rect();

        if (proportion) {

            int width = abs(xMouse - xInit);
            int height = abs(yMouse - yInit);

            bool xWins = false;
            if (width <= height)
                xWins = true; 

            QPointF target;

            if (xMouse >= xInit) {
                if (yMouse >= yInit) {
                    if (xWins)
                        target = QPointF(xInit + width, yInit + width);      
                    else
                        target = QPointF(xInit + height, yInit + height);

                    rect.setBottomRight(target);
                } else {
                    if (xWins)
                        target = QPointF(xInit + width, yInit - width);
                    else
                        target = QPointF(xInit + height, yInit - height);

                    rect.setTopRight(target);
                }
            } else {
                if (yMouse >= yInit) {
                    if (xWins)
                        target = QPointF(xInit - width, yInit + width);
                    else
                        target = QPointF(xInit - height, yInit + height);

                    rect.setBottomLeft(target);
                } else {
                    if (xWins)
                        target = QPointF(xInit - width, yInit - width);
                    else
                        target = QPointF(xInit - height, yInit - height);

                    rect.setTopLeft(target);
                }
            }
            
        } else {

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

        if (name() == tr("Rectangle"))
            m_rect->setRect(rect);
        else
            m_ellipse->setRect(rect);

    } else if (name() == tr("Line")) {

        if (!added) {
            scene->includeObject(m_line);
            added = true;
        }

        QLineF line(firstPoint.x(), firstPoint.y(), input->pos().x(), input->pos().y()); 
        m_line->setLine(line);
    }
}

void GeometricTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    QDomDocument doc;
    QPointF position;

    if (name() == tr("Rectangle")) {
        doc.appendChild(dynamic_cast<KTAbstractSerializable *>(m_rect)->toXml(doc));
        position = m_rect->pos();
    } else if (name() == tr("Ellipse")) {
               doc.appendChild(dynamic_cast<KTAbstractSerializable *>(m_ellipse)->toXml(doc));
               QRectF rect = m_ellipse->rect();
               position = rect.topLeft();
    } else if (name() == tr("Line")) {
               doc.appendChild(dynamic_cast<KTAbstractSerializable *>(m_line)->toXml(doc));
               position = m_line->pos(); 
    }
    
    KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), 
                             scene->currentFrameIndex(), scene->currentFrame()->graphics().count(), position,
                             KTLibraryObject::Item, KTProjectRequest::Add, doc.toString()); // Adds to end
    
    emit requested(&event);
}

QMap<QString, TAction *> GeometricTool::actions() const
{
    return m_actions;
}

int GeometricTool::toolType() const
{
    return Brush;
}
        
QWidget *GeometricTool::configurator()
{
    if (name() == tr("Rectangle") || name() == tr("Ellipse")) {
        m_configurator = new InfoPanel;
        return m_configurator;
    } else {
      return  0;
    }
}

void GeometricTool::aboutToChangeScene(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void GeometricTool::aboutToChangeTool() 
{
}

void GeometricTool::saveConfig()
{
}

void GeometricTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
        proportion = true;
}

void GeometricTool::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
        proportion = false;
}

Q_EXPORT_PLUGIN2(kt_geometric, GeometricTool)

