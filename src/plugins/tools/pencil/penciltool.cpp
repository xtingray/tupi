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

#include "penciltool.h"
#include "tuppaintareaevent.h"

#include <QGraphicsEllipseItem> 

struct PencilTool::Private
{
    QPointF firstPoint;
    QPointF oldPos;
    QPainterPath path;
    Settings *configurator;
    QMap<QString, TAction *> actions;
    TupPathItem *item;
    QCursor cursor;
    TupGraphicsScene *scene;
    TupBrushManager *brushManager;
    TupInputDeviceInformation *input;

    bool resize;
    QGraphicsEllipseItem *penCircle;
    int circleZValue;
    QPointF penCirclePos;
    int penWidth;
};

PencilTool::PencilTool() : TupToolPlugin(), k(new Private)
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
    k->cursor = QCursor(kAppProp->themeDir() + "cursors/pencil.png", 0, 15);
    TAction *pencil = new TAction(QPixmap(kAppProp->themeDir() + "icons/pencil.png"), tr("Pencil"), this);
    pencil->setShortcut(QKeySequence(tr("P")));
    pencil->setToolTip(tr("Pencil") + " - " + "P");
    pencil->setCursor(k->cursor);

    k->actions.insert(tr("Pencil"), pencil);
}

void PencilTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PencilTool::init()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    k->scene = scene;
    k->brushManager = k->scene->brushManager();
    k->input = k->scene->inputDeviceInformation();
    k->resize = false;
    k->circleZValue = (2*ZLAYER_LIMIT) + (scene->scene()->layersCount() * ZLAYER_LIMIT);

    TCONFIG->beginGroup("BrushParameters");
    k->penWidth = TCONFIG->value("Thickness", 3).toInt();

    foreach (QGraphicsView * view, scene->views())
             view->setDragMode(QGraphicsView::NoDrag);
}

QStringList PencilTool::keys() const
{
    return QStringList() << tr("Pencil");
}

void PencilTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    if (!k->resize) {
        k->firstPoint = input->pos();

        k->path = QPainterPath();
        k->path.moveTo(k->firstPoint);

        k->oldPos = input->pos();

        k->item = new TupPathItem();
        k->item->setPen(brushManager->pen());

        scene->includeObject(k->item);
    }
}

void PencilTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->resize) {
        QPointF point = input->pos();
        QPointF result = k->penCirclePos - point;
        k->penWidth = sqrt(pow(result.x(), 2) + pow(result.y(), 2));

        QPointF topLeft(k->penCirclePos.x() - (k->penWidth/2), k->penCirclePos.y() - (k->penWidth/2));
        QSize size(k->penWidth, k->penWidth);
        QRectF rect(topLeft, size);
        k->penCircle->setRect(rect);
    } else {
        if (!k->item)
            return;

        QPointF lastPoint = input->pos();

        k->path.moveTo(k->oldPos);
        k->path.lineTo(lastPoint);

        k->item->setPath(k->path);
        k->oldPos = lastPoint;
    }
}

void PencilTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (!k->resize) {
        if (!k->item)
            return;

        if (k->firstPoint == input->pos() && k->path.elementCount() == 1) {
            QPointF currentPoint = input->pos();
            scene->removeItem(k->item);
 
            qreal radius = brushManager->pen().width();
            QPointF distance((radius + 2)/2, (radius + 2)/2);
            QPen inkPen(brushManager->penColor(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            TupEllipseItem *blackEllipse = new TupEllipseItem(QRectF(currentPoint - distance, QSize(radius + 2, radius + 2)));
            blackEllipse->setPen(inkPen);
            blackEllipse->setBrush(inkPen.brush());
            scene->includeObject(blackEllipse);

            QDomDocument doc;
            doc.appendChild(blackEllipse->toXml(doc));
            TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(),
                                                                             0, currentPoint, scene->spaceContext(), TupLibraryObject::Item, TupProjectRequest::Add,
                                                                             doc.toString());
            emit requested(&request);
            return;
        } else {
            double smoothness = k->configurator->smoothness();
            if (smoothness > 0)
                smoothPath(k->path, smoothness);
        }

        k->item->setBrush(brushManager->brush());
        k->item->setPath(k->path);

        QDomDocument doc;
        doc.appendChild(k->item->toXml(doc));

        TupProjectRequest request = TupRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                                         0, QPoint(), scene->spaceContext(), TupLibraryObject::Item, TupProjectRequest::Add, 
                                                                         doc.toString());
        emit requested(&request);
    }
}

void PencilTool::smoothPath(QPainterPath &path, double smoothness, int from, int to)
{
    QPolygonF polygon;
    QList<QPolygonF> polygons = path.toSubpathPolygons();
    QList<QPolygonF>::iterator it = polygons.begin();
    QPolygonF::iterator pointIt;

    while (it != polygons.end()) {
           pointIt = (*it).begin();

           while (pointIt <= (*it).end()-2) {
                  polygon << (*pointIt);
                  pointIt += 2;
           }
           ++it;
    }

    if (smoothness > 0) {
        path = TupGraphicalAlgorithm::bezierFit(polygon, smoothness, from, to);
    } else {
        path = QPainterPath();
        path.addPolygon(polygon);
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
        k->configurator = new Settings;

    return k->configurator;
}

void PencilTool::aboutToChangeTool() 
{
}

void PencilTool::saveConfig()
{
    if (k->configurator) {
        TCONFIG->beginGroup("PencilTool");
        TCONFIG->setValue("Smoothness", k->configurator->smoothness());
    }
}

void PencilTool::keyPressEvent(QKeyEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PencilTool::keyPressEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (event->modifiers() == Qt::ShiftModifier) {
        k->resize = true;
        k->input = k->scene->inputDeviceInformation();
        int diameter = k->brushManager->penWidth();
        int radius = diameter/2;
        k->penCirclePos = k->input->pos();

        k->penCircle = new QGraphicsEllipseItem(k->penCirclePos.x() - radius, k->penCirclePos.y() - radius, diameter, diameter);
        k->penCircle->setZValue(k->circleZValue);
        k->scene->addItem(k->penCircle);
        return;
    }

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
        return;
    }

    QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
    if (flags.first != -1 && flags.second != -1)
        emit callForPlugin(flags.first, flags.second);
}

void PencilTool::keyReleaseEvent(QKeyEvent *event) 
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[PencilTool::keyReleaseEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(event);

    if (k->resize) {
        k->resize = false;
        k->scene->removeItem(k->penCircle);

        TCONFIG->beginGroup("BrushParameters");
        TCONFIG->setValue("Thickness", k->penWidth);

        emit penWidthChanged(k->penWidth);
    }
}

QCursor PencilTool::cursor() const
{
    return k->cursor;
}

void PencilTool::sceneResponse(const TupSceneResponse *event)
{
    Q_UNUSED(event);
}
