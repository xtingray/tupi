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

#include "tweener.h"
#include "configurator.h"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QDomDocument>
#include <QGraphicsPathItem>

#include "kalgorithm.h"
#include "kglobal.h"
#include "kdebug.h"
#include "kaction.h"
#include "knodegroup.h"
#include "kosd.h"

#include <ktsvg2qt.h>

#include "ktinputdeviceinformation.h"
#include "ktbrushmanager.h"
#include "ktgraphicalgorithm.h"
#include "ktgraphicsscene.h"
#include "ktgraphicobject.h"
#include "ktitemtweener.h"
#include "ktrequestbuilder.h"
#include "ktprojectrequest.h"
#include "ktlibraryobject.h"
#include "ktscene.h"
#include "ktlayer.h"

struct Tweener::Private
{
    QMap<QString, KAction *> actions;
    Configurator *configurator;
    KTGraphicsScene *scene;
    bool creatingPath;
    bool selecting;
    QGraphicsPathItem *path;
    QList<QGraphicsItem *> objects;
    KNodeGroup *group;
    bool pathAdded;
};

Tweener::Tweener() : KTToolPlugin(), k(new Private)
{
    setupActions();
    k->configurator = 0;
    k->creatingPath = true;
    k->selecting = false;
    k->path = 0;
    k->group = 0;
    k->pathAdded = false;
}

Tweener::~Tweener()
{
    delete k;
}

void Tweener::init(KTGraphicsScene *scene)
{
    delete k->path;
    k->path = 0;
    delete k->group;
    k->group = 0;
    k->scene = scene;
    k->objects.clear();

    int framesTotal = 1;
    KTLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        framesTotal = layer->framesNumber(); 

    k->configurator->initCombo(framesTotal, k->scene->currentFrameIndex());
    k->configurator->cleanData();
    k->configurator->activateSelectionMode();
    setSelect();
}

QStringList Tweener::keys() const
{
    return QStringList() << tr("Tweener");
}

void Tweener::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->creatingPath) {
        if (!k->path) {
            k->path = new QGraphicsPathItem;
            QColor color = Qt::lightGray;
            color.setAlpha(200);
            QPen pen(QBrush(color), 1, Qt::DotLine);  
            k->path->setPen(pen);
            QPainterPath path;
            path.moveTo(input->pos());
            k->path->setPath(path);
        } else {
            QPainterPath path = k->path->path();
            path.cubicTo(input->pos(), input->pos(), input->pos());
            k->path->setPath(path);
        }
    } 
}

void Tweener::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

void Tweener::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (k->creatingPath) {

        if (!k->group) {
            k->group = new KNodeGroup(k->path, scene);
            connect(k->group, SIGNAL(nodeClicked()), SLOT(updatePath()));
            k->group->clearChangesNodes();
        } else {
            k->group->createNodes(k->path);
            k->group->expandAllNodes();
        }
        k->configurator->updateSteps(k->path);

    } else {

        if (scene->selectedItems().size() > 0) {
            k->objects = scene->selectedItems();

            if (!k->path) {
                k->path = new QGraphicsPathItem;
                QColor color(150, 150, 150, 200);
                QPen pen(QBrush(color), 1, Qt::DotLine);
                k->path->setPen(pen);
                QPainterPath path;
                path.moveTo(input->pos());
                k->path->setPath(path);
                scene->addItem(k->path);
                k->pathAdded = true;
            }
        }
    }
}

QMap<QString, KAction *> Tweener::actions() const
{
    return k->actions;
}

int Tweener::toolType() const
{
    return KTToolInterface::Brush;
}

QWidget *Tweener::configurator()
{
    if (!k->configurator) {
        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(clickedCreatePath()), this, SLOT(setCreatePath()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelect()));
        connect(k->configurator, SIGNAL(clickedResetTweener()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(clickedApplyTweener()), this, SLOT(applyTween()));
    }

    return k->configurator;
}

void Tweener::aboutToChangeScene(KTGraphicsScene *)
{
}

void Tweener::aboutToChangeTool()
{
    if (k->selecting) {
        if (k->objects.size() > 0) {
            foreach (QGraphicsItem *item, k->objects) {
                     if (item->isSelected())
                         item->setSelected(false);
            }
        }
        return;
    }

    if (k->creatingPath) {
        if (k->path) {
            k->scene->removeItem(k->path);
            k->pathAdded = false;
            delete k->group;
            k->group = 0;
        }
        return;
    }
}

bool Tweener::isComplete() const
{
    return true;
}

void Tweener::setupActions()
{
    KAction *translater = new KAction(QPixmap(THEME_DIR + "icons/tweener.png"), 
                                      tr("Motion Tween"), this);
    translater->setCursor(QCursor(THEME_DIR + "cursors/tweener.png"));
    translater->setShortcut(QKeySequence(tr("W")));

    k->actions.insert("Tweener", translater);
}

void Tweener::setCreatePath()
{
    if (k->path) {

        if (!k->pathAdded) {
            k->scene->addItem(k->path);
            k->pathAdded = true;
        }

        if (k->group) {
            k->group->createNodes(k->path);
        } else {
            k->group = new KNodeGroup(k->path, k->scene);
            connect(k->group, SIGNAL(nodeClicked()), SLOT(updatePath()));
            k->group->createNodes(k->path);
        }
        k->group->expandAllNodes();

    }

    k->creatingPath = true;
    k->selecting = false;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (item != k->path) {
                          item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                          item->setFlag(QGraphicsItem::ItemIsMovable, false);
                      }
             }
    }
}

void Tweener::setSelect()
{
    if (k->path) {
        k->scene->removeItem(k->path);
        k->pathAdded = false;
        delete k->group;
        k->group = 0;
    }

    k->creatingPath = false;
    k->selecting = true;

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if (item != k->path) {
                          item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                      }
             }
    }

    // When Object selection is enabled, previous selection is set
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects)
                 item->setSelected(true);
    }

}

QString Tweener::pathToCoords()
{
    kFatal() << "Tweener::pathToCoords() - Converting path to xml!";

    QString strPath = "";
    QChar t;

    for (int i=0; i < k->path->path().elementCount(); i++) {
         QPainterPath::Element e = k->path->path().elementAt(i);
         switch (e.type) {
            case QPainterPath::MoveToElement:
            {
                if (t != 'M') {
                    t = 'M';
                    strPath += "M " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::LineToElement:
            {
                if (t != 'L') {
                    t = 'L';
                    strPath += " L " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::CurveToElement:
            {
                if (t != 'C') {
                    t = 'C';
                    strPath += " C " + QString::number(e.x) + " " + QString::number(e.y) + " ";
                } else {
                    strPath += "  "+ QString::number(e.x) + " " + QString::number(e.y) + " ";
                }
            }
            break;
            case QPainterPath::CurveToDataElement:
            {
                if (t == 'C')
                    strPath +=  " " + QString::number(e.x) + "  " + QString::number(e.y) + " ";
            }
            break;
        }
    }

    return strPath;
}

void Tweener::applyReset()
{
    k->objects.clear();
    k->configurator->cleanData();

    if (k->group) {
        k->group->clean();
        k->group = 0;
    }

    if (k->path) {
        k->scene->removeItem(k->path);
        k->pathAdded = false;
        k->path = 0;
    }
}

void Tweener::applyTween()
{
    if (k->objects.size() == 0) {
        KOsd::self()->display(tr("Error"), tr("No items selected for Tweening"), KOsd::Error);
        return;
    }

    if (k->configurator->totalSteps() == 0) {
        KOsd::self()->display(tr("Error"), tr("No path created for Tweening"), KOsd::Error);
        return;
    }

    QString name = k->configurator->currentTweenName();

    KTScene *source = k->scene->scene();
    bool exists = false;
    foreach (KTGraphicObject *object, source->tweeningObjects()) {
             if (KTItemTweener *tweener = object->tweener()) {
                 kFatal() << "Tweener::applyTween() - Item name: " << tweener->name();
                 if (tweener->name().compare(name) == 0)
                     exists = true;
             }
    }

    if (!exists) {

    foreach (QGraphicsItem *item, k->objects) {   

             kFatal() << "Tweener::applyTween() - Applying tween!";
             QString route = pathToCoords();

             KTProjectRequest request = KTRequestBuilder::createItemRequest(
                                        k->scene->currentSceneIndex(),
                                        k->scene->currentLayerIndex(),
                                        k->scene->currentFrameIndex(),
                                        k->scene->currentFrame()->indexOf(item),
                                        QPointF(), KTLibraryObject::Item,
                                        KTProjectRequest::AddTween, 
                                        k->configurator->tweenToXml(k->scene->currentFrameIndex(), route));
             emit requested(&request);

             int total = k->scene->currentFrameIndex() + k->configurator->totalSteps() - 1;
             int start = k->scene->currentFrameIndex() + 1;

             for (int i = start; i <= total; i++) {
                  KTProjectRequest requestFrame = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(), 
                                                  k->scene->currentLayerIndex(), 
                                                  i, KTProjectRequest::Add);
                  emit requested(&requestFrame);
             }

             request = KTRequestBuilder::createFrameRequest(k->scene->currentSceneIndex(), k->scene->currentLayerIndex(),
                                                            k->scene->currentFrameIndex(), KTProjectRequest::Select, "1");
             emit requested(&request);
    }

    int framesTotal = 1;
    KTLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        framesTotal = layer->framesNumber();

    k->configurator->initCombo(framesTotal, k->scene->currentFrameIndex());
    } else {
      kFatal() << "Tweener::applyTween() - Update tween right here!";
    }
}

void Tweener::updatePath()
{
    kFatal() << "Tweener::updatePath() - Just tracing!";
    k->configurator->updateSteps(k->path);
}

void Tweener::saveConfig()
{
}

void Tweener::updateScene(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);

    kFatal() << "Tweener::updateScene() - Just tracing!";

    if (k->creatingPath) {
        if (k->path && k->group) {
            k->scene->addItem(k->path);            
            k->group->createNodes(k->path);
            k->group->expandAllNodes();
        }
    }
}

Q_EXPORT_PLUGIN2(kt_tweener, Tweener);

