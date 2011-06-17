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

#include "brush.h"
#include "brush.xpm"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>

#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicalgorithm.h"
#include "ktgraphicsscene.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktellipseitem.h"

#include "taction.h"
#include "talgorithm.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"

Brush::Brush() : m_configurator(0), m_item(0)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setupActions();
}

Brush::~Brush()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void Brush::init(KTGraphicsScene *scene)
{
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

QStringList Brush::keys() const
{
    return QStringList() << tr("Pencil");
}

void Brush::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    m_firstPoint = input->pos();

    m_path = QPainterPath();
    m_path.moveTo(m_firstPoint);

    m_oldPos = input->pos();

    m_item = new KTPathItem();
    m_item->setPen(brushManager->pen());

    scene->includeObject(m_item);
}

void Brush::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    QPointF lastPoint = input->pos();

    foreach (QGraphicsView * view, scene->views())
             view->setDragMode(QGraphicsView::NoDrag);

    m_path.moveTo(m_oldPos);
    m_path.lineTo(lastPoint);

    m_item->setPath(m_path);
    m_oldPos = lastPoint;
}

void Brush::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(scene);

    double smoothness = m_configurator->exactness();

    if (m_firstPoint == input->pos() && m_path.elementCount() == 1) {
        smoothness = 0;
        qreal radius = ((qreal) brushManager->pen().width()) / ((qreal) 2);
        m_path.addEllipse(input->pos().x(), input->pos().y(), radius, radius);
    }

    m_firstPoint = QPoint(0,0);

    smoothPath(m_path, smoothness);

    m_item->setBrush(brushManager->brush());
    m_item->setPath(m_path);

    // Add KTProjectRequest

    QDomDocument doc;
    doc.appendChild(m_item->toXml(doc));

    KTProjectRequest request = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(), 
                                                                   scene->currentFrame()->graphics().count(), QPointF(), scene->spaceMode(),
                                                                   KTLibraryObject::Item, KTProjectRequest::Add, doc.toString());

    emit requested(&request);
}

void Brush::smoothPath(QPainterPath &path, double smoothness, int from, int to)
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
        path = KTGraphicalAlgorithm::bezierFit(pol, smoothness, from, to);
    } else {
        path = QPainterPath();
        path.addPolygon(pol);
    }
}

void Brush::setupActions()
{
    TAction *pencil = new TAction(QPixmap(THEME_DIR + "icons/pencil.png"), tr("Pencil"), this);
    pencil->setShortcut(QKeySequence(tr("P")) );

    QPixmap pix(THEME_DIR + "cursors/pencil.png");
    pencil->setCursor(QCursor(pix, 0, pix.height()));

    m_actions.insert(tr("Pencil"), pencil);
}

QMap<QString, TAction *> Brush::actions() const
{
    return m_actions;
}

int Brush::toolType() const
{
    return KTToolInterface::Brush;
}

QWidget *Brush::configurator() 
{
    if (! m_configurator)
        m_configurator = new ExactnessConfigurator;

    return m_configurator;
}

void Brush::aboutToChangeTool() 
{
}

void Brush::saveConfig()
{
    if (m_configurator) {
        TCONFIG->beginGroup("BrushTool");
        TCONFIG->setValue("Smoothness", m_configurator->exactness());
    }
}

Q_EXPORT_PLUGIN2(kt_brush, Brush);
