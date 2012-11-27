/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "penciltool.h"
// #include "penciltool.xpm"

#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicalgorithm.h"
#include "tupgraphicsscene.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupellipseitem.h"

#include "taction.h"
#include "talgorithm.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>

struct PencilTool::Private
{
    QPointF firstPoint;
    QPointF oldPos;
    QPainterPath path;
    ExactnessConfigurator *configurator;
    QMap<QString, TAction *> actions;
    TupPathItem *item;
    QCursor cursor;
    TupGraphicsScene *scene;
};

PencilTool::PencilTool() : k(new Private)
{
    k->configurator = 0;
    k->item = 0;

    setupActions();
}

PencilTool::~PencilTool()
{
}

void PencilTool::setupActions()
{
    TAction *pencil = new TAction(QPixmap(kAppProp->themeDir() + "icons/pencil.png"), tr("Pencil"), this);
    pencil->setShortcut(QKeySequence(tr("P")));
    k->cursor = QCursor(kAppProp->themeDir() + "cursors/pencil.png", 0, 16);
    pencil->setCursor(k->cursor);

    k->actions.insert(tr("Pencil"), pencil);
}

void PencilTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("tools");
    #endif

    k->scene = scene;

    /*
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::NoDrag);
             Q_CHECK_PTR(view->scene());
             if (QGraphicsScene *scene = qobject_cast<QGraphicsScene *>(view->scene())) {
                 foreach (QGraphicsItem *item, scene->items()) {
                          item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                          item->setFlag(QGraphicsItem::ItemIsMovable, false);
                 }
             }
    }

    foreach (QGraphicsItem *item, scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             item->setFlag(QGraphicsItem::ItemIsMovable, false);
    }
    */

    reset(k->scene);
}

void PencilTool::reset(TupGraphicsScene *scene)
{
    foreach (QGraphicsItem *item, scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             item->setFlag(QGraphicsItem::ItemIsMovable, false);
    }
}

QStringList PencilTool::keys() const
{
    return QStringList() << tr("Pencil");
}

void PencilTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    k->firstPoint = input->pos();

    k->path = QPainterPath();
    k->path.moveTo(k->firstPoint);

    k->oldPos = input->pos();

    k->item = new TupPathItem();
    k->item->setPen(brushManager->pen());

    scene->includeObject(k->item);
}

void PencilTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    QPointF lastPoint = input->pos();

    foreach (QGraphicsView * view, scene->views())
             view->setDragMode(QGraphicsView::NoDrag);

    k->path.moveTo(k->oldPos);
    k->path.lineTo(lastPoint);

    k->item->setPath(k->path);
    k->oldPos = lastPoint;
}

void PencilTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (!k->item)
        return;

    double smoothness = k->configurator->exactness();

    if (k->firstPoint == input->pos() && k->path.elementCount() == 1) {
        smoothness = 0;
        qreal radius = ((qreal) brushManager->pen().width()) / ((qreal) 2);
        k->path.addEllipse(input->pos().x(), input->pos().y(), radius, radius);
    } 

    smoothPath(k->path, smoothness);

    k->item->setBrush(brushManager->brush());
    k->item->setPath(k->path);

    // Add TupProjectRequest

    QDomDocument doc;
    doc.appendChild(k->item->toXml(doc));

    TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                                   scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                                   TupLibraryObject::Item, TupProjectRequest::Add, doc.toString());

    emit requested(&request);
}

void PencilTool::smoothPath(QPainterPath &path, double smoothness, int from, int to)
{
    QPolygonF pol;
    QList<QPolygonF> polygons = path.toSubpathPolygons();

    QList<QPolygonF>::iterator it = polygons.begin();

    QPolygonF::iterator pointIt;

    while (it != polygons.end()) {
           pointIt = (*it).begin();

           while (pointIt <= (*it).end()-2) {
                  pol << (*pointIt);
                  pointIt += 2;
           }
           ++it;
    }

    if (smoothness > 0) {
        path = TupGraphicalAlgorithm::bezierFit(pol, smoothness, from, to);
    } else {
        path = QPainterPath();
        path.addPolygon(pol);
    }
}

QMap<QString, TAction *> PencilTool::actions() const
{
    return k->actions;
}

int PencilTool::toolType() const
{
    return TupToolInterface::Brush;
}

QWidget *PencilTool::configurator() 
{
    if (! k->configurator)
        k->configurator = new ExactnessConfigurator;

    return k->configurator;
}

void PencilTool::aboutToChangeTool() 
{
}

void PencilTool::saveConfig()
{
    if (k->configurator) {
        TCONFIG->beginGroup("PencilTool");
        TCONFIG->setValue("Smoothness", k->configurator->exactness());
    }
}

void PencilTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
        return;
    }

    QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
    if (flags.first != -1 && flags.second != -1)
        emit callForPlugin(flags.first, flags.second);
}

QCursor PencilTool::cursor() const
{
    return k->cursor;
}

void PencilTool::sceneResponse(const TupSceneResponse *event)
{
    if (event->action() == TupProjectRequest::Select)
        reset(k->scene);
}

Q_EXPORT_PLUGIN2(tup_brush, PencilTool);
