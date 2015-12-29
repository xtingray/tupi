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

#include "shifttool.h"
#include "tconfig.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "tupscene.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"

struct ShiftTool::Private
{
    QString activeView;
    QMap<QString, TAction *> actions;
    QGraphicsRectItem *rect;
    bool added;
    QPointF currentCenter;
    QPointF firstPoint;
    TupGraphicsScene *scene;
    // ZoomConfigurator *configurator;
    QCursor shiftCursor;
    QSize projectSize;
};

ShiftTool::ShiftTool() : k(new Private)
{
    k->activeView = "WORKSPACE";
    k->rect = 0;
    k->scene = 0;
    // k->configurator = 0;

    setupActions();
}

ShiftTool::~ShiftTool()
{
}

void ShiftTool::init(TupGraphicsScene *scene)
{
    k->scene = scene;

    /*
    foreach (QGraphicsView *view, scene->views()) {
             k->currentCenter =  view->sceneRect().center();
             view->setDragMode(QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
    */
}

QStringList ShiftTool::keys() const
{
    return QStringList() << tr("Shift");
}

void ShiftTool::setupActions()
{
    TAction *shiftAction = new TAction(QIcon(kAppProp->themeDir() + "icons/shift.png"), tr("Shift"), this);
    shiftAction->setShortcut(QKeySequence(tr("H")));
    shiftAction->setToolTip(tr("Shift") + " - " + "H");

    k->shiftCursor = QCursor(kAppProp->themeDir() + "cursors/shift.png");
    shiftAction->setCursor(k->shiftCursor);

    k->actions.insert(tr("Shift"), shiftAction);
}

void ShiftTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    k->added = false;
    k->rect = new QGraphicsRectItem(QRectF(input->pos(), QSize(0,0)));
    k->rect->setPen(QPen(Qt::red, 1, Qt::SolidLine));

    k->firstPoint = input->pos();
}

void ShiftTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (name() == tr("Shift")) 
        k->scene = scene; // <- SQA: Trace this variable
}

void ShiftTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (name() == tr("Shift")) {
        k->currentCenter = input->pos();
        foreach (QGraphicsView *view, scene->views()) {
                 if (k->activeView.compare(view->accessibleName()) == 0) {
                     view->centerOn(k->currentCenter);
                     view->setSceneRect(input->pos().x() - (k->projectSize.width()/2), input->pos().y() - (k->projectSize.height()/2),
                                        k->projectSize.width(), k->projectSize.height());
                     break;
                 }  
        }
    } 
}

void ShiftTool::setProjectSize(const QSize size)
{
    k->projectSize = size;
}

QMap<QString, TAction *> ShiftTool::actions() const
{
    return k->actions;
}

int ShiftTool::toolType() const
{
    return TupToolInterface::View;
}

QWidget *ShiftTool::configurator()
{
    /*
    if (name() != tr("Shift")) {
        if (! k->configurator)
            k->configurator = new ZoomConfigurator;

        return k->configurator;
    }
    */

    return 0;
}

void ShiftTool::aboutToChangeScene(TupGraphicsScene *)
{
}

void ShiftTool::aboutToChangeTool()
{
    if (name() == tr("Shift")) {
       if (k->scene) {
           foreach (QGraphicsView * view, k->scene->views())
                    view->setDragMode(QGraphicsView::NoDrag);
       }
    }
}

void ShiftTool::saveConfig()
{
/*
    if (k->configurator) {
        TCONFIG->beginGroup("ZoomTool");
        TCONFIG->setValue("zoomFactor", k->configurator->getFactor());
    }
*/
}

void ShiftTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor ShiftTool::cursor() const
{
   if (name() == tr("Shift"))
       return k->shiftCursor;

   return QCursor(Qt::ArrowCursor);
}

void ShiftTool::setActiveView(const QString &viewID)
{
    k->activeView = viewID;
}
