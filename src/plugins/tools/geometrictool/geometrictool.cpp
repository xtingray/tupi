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
#include "tglobal.h"
#include "tdebug.h"
#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"
#include "ktlibraryobject.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktscene.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktbrushmanager.h"

#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

struct GeometricTool::Private
{
    KTRectItem *rect;
    KTEllipseItem *ellipse;
    KTLineItem *line;
    InfoPanel *configurator;
    bool added;
    QPointF firstPoint;
    QMap<QString, TAction *> actions;
    bool proportion;
    QGraphicsItem *item;
    QCursor squareCursor;
    QCursor circleCursor;
    QCursor lineCursor;
};

GeometricTool::GeometricTool() : k(new Private)
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
    k->proportion = false;

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
    k->squareCursor = QCursor(THEME_DIR + "cursors/square.png");
    action1->setCursor(k->squareCursor);
    
    k->actions.insert(tr("Rectangle"), action1);
    
    TAction *action2 = new TAction(QIcon(THEME_DIR + "icons/ellipse.png"), tr("Ellipse"), this);
    action2->setShortcut(QKeySequence(tr("C")));
    k->circleCursor = QCursor(THEME_DIR + "cursors/circle.png");
    action2->setCursor(k->circleCursor);
    
    k->actions.insert(tr("Ellipse"), action2);
    
    TAction *action3 = new TAction(QIcon(THEME_DIR + "icons/line.png"), tr("Line"), this);
    action3->setShortcut(QKeySequence(tr("L")));
    k->lineCursor = QCursor(THEME_DIR + "cursors/line.png");
    action3->setCursor(k->lineCursor);

    k->actions.insert(tr("Line"), action3);
}

void GeometricTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    
    if (input->buttons() == Qt::LeftButton) {
        
        if (name() == tr("Rectangle")) {
            k->added = false;
            k->rect = new KTRectItem(QRectF(input->pos(), QSize(0,0)));
            k->rect->setPen(brushManager->pen());
            k->rect->setBrush(brushManager->brush());

            k->firstPoint = input->pos();

        } else if (name() == tr("Ellipse")) {

                   k->added = false;
                   k->ellipse = new KTEllipseItem(QRectF(input->pos(), QSize(0,0)));
                   k->ellipse->setPen(brushManager->pen());
                   k->ellipse->setBrush(brushManager->brush());

                   k->firstPoint = input->pos();

        } else if (name() == tr("Line")) {

                   k->added = false;
                   k->line = new KTLineItem();
                   k->line->setLine(QLineF(input->pos().x(), input->pos().y(), input->pos().x(), input->pos().y()));
                   k->line->setPen(brushManager->pen());

                   k->firstPoint = input->pos();
        }
    }
}

void GeometricTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
    
    if (name() == tr("Rectangle") || name() == tr("Ellipse")) {

        if (!k->added) {
            if (name() == tr("Rectangle"))
                scene->includeObject(k->rect);
            else
                scene->includeObject(k->ellipse);
            k->added = true;
        }

        int xMouse = input->pos().x();
        int yMouse = input->pos().y();
        int xInit = k->firstPoint.x();
        int yInit = k->firstPoint.y();

        QRectF rect;

        if (name() == tr("Rectangle"))
            rect = k->rect->rect();
        else
            rect = k->ellipse->rect();

        if (k->proportion) {

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
            k->rect->setRect(rect);
        else
            k->ellipse->setRect(rect);

    } else if (name() == tr("Line")) {

        if (!k->added) {
            scene->includeObject(k->line);
            k->added = true;
        }

        QLineF line(k->firstPoint.x(), k->firstPoint.y(), input->pos().x(), input->pos().y()); 
        k->line->setLine(line);
    }
}

void GeometricTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    QDomDocument doc;
    QPointF position;

    if (name() == tr("Rectangle")) {
        doc.appendChild(dynamic_cast<KTAbstractSerializable *>(k->rect)->toXml(doc));
        position = k->rect->pos();
    } else if (name() == tr("Ellipse")) {
               doc.appendChild(dynamic_cast<KTAbstractSerializable *>(k->ellipse)->toXml(doc));
               QRectF rect = k->ellipse->rect();
               position = rect.topLeft();
    } else if (name() == tr("Line")) {
               doc.appendChild(dynamic_cast<KTAbstractSerializable *>(k->line)->toXml(doc));
               position = k->line->pos(); 
    }
    
    KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), 
                             scene->currentFrameIndex(), scene->currentFrame()->graphics().count(), position,
                             scene->spaceMode(), KTLibraryObject::Item, KTProjectRequest::Add, doc.toString()); // Adds to end
    
    emit requested(&event);
}

QMap<QString, TAction *> GeometricTool::actions() const
{
    return k->actions;
}

int GeometricTool::toolType() const
{
    return Brush;
}
        
QWidget *GeometricTool::configurator()
{
    if (name() == tr("Rectangle") || name() == tr("Ellipse")) {
        k->configurator = new InfoPanel;
        return k->configurator;
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
    if (event->key() == Qt::Key_Shift) {
        tFatal() << "GeometricTool::keyPressEvent() - Typing key shift!";
        k->proportion = true;
    } else {
        QPair<int, int> flags = KTToolPlugin::setKeyAction(event->key());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void GeometricTool::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift)
        k->proportion = false;

    if (event->key() == Qt::Key_Escape)
        emit closeHugeCanvas();
}

QCursor GeometricTool::cursor() const
{
    if (name() == tr("Rectangle")) {
        return k->squareCursor;
    } else if (name() == tr("Ellipse")) {
               return k->circleCursor;
    } else if (name() == tr("Line")) {
               return k->lineCursor;
    }

    return 0;
}

Q_EXPORT_PLUGIN2(kt_geometric, GeometricTool)

