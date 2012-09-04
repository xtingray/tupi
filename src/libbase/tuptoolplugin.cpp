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

#include "tuptoolplugin.h"
#include "tupbrushmanager.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"

#include "tdebug.h"

#include <QGraphicsView>
#include <QCursor>
#include <QMenu>

struct TupToolPlugin::Private
{
    QString currentTool;
};

TupToolPlugin::TupToolPlugin(QObject * parent) : QObject(parent), k(new Private)
{
}

TupToolPlugin::~TupToolPlugin()
{
    delete k;
}

void TupToolPlugin::init(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void TupToolPlugin::updateScene(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void TupToolPlugin::setName(const QString &tool)
{
    k->currentTool = tool;
}

QString TupToolPlugin::name() const
{
    return k->currentTool;
}

void TupToolPlugin::begin()
{
    tDebug("tools") << "Begin: " << k->currentTool;
}

void TupToolPlugin::end()
{
    tDebug("tools") << "End: " << k->currentTool;
}

void TupToolPlugin::sceneResponse(const TupSceneResponse *event)
{
    Q_UNUSED(event);
}

void TupToolPlugin::layerResponse(const TupLayerResponse *event)
{
    Q_UNUSED(event);
}

void TupToolPlugin::frameResponse(const TupFrameResponse *event)
{
    Q_UNUSED(event);
}

void TupToolPlugin::itemResponse(const TupItemResponse *event)
{
    Q_UNUSED(event);
}

void TupToolPlugin::doubleClick(const TupInputDeviceInformation *, TupGraphicsScene *)
{
}

void TupToolPlugin::aboutToChangeScene(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void TupToolPlugin::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void TupToolPlugin::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

QMenu *TupToolPlugin::menu() const
{
    QMenu *menu = new QMenu(new QWidget);
    return menu;
}

void TupToolPlugin::saveConfig() 
{
}

QCursor TupToolPlugin::cursor() const
{
    return QCursor(Qt::ArrowCursor); 
}

void TupToolPlugin::updatePos(QPointF pos)
{
    Q_UNUSED(pos);
}

void TupToolPlugin::resizeNodes(qreal factor)
{
    Q_UNUSED(factor);
}

void TupToolPlugin::updateZoomFactor(qreal factor)
{
    Q_UNUSED(factor);
}

void TupToolPlugin::autoZoom()
{
}

void TupToolPlugin::setProjectSize(const QSize size)
{
}

QPair<int, int> TupToolPlugin::setKeyAction(int key, Qt::KeyboardModifiers modifiers)
{
    TupToolPlugin::MenuIndex menu = TupToolPlugin::BrushesMenu;
    int tool = TupToolPlugin::PencilTool;

    switch (key) {
            case Qt::Key_P:
                 if (modifiers == Qt::ShiftModifier) {
                     menu = TupToolPlugin::ColorMenu;
                     tool = TupToolPlugin::ColorTool;
                 } else {
                     tool = TupToolPlugin::PencilTool;
                 }
            break;

            case Qt::Key_K:
                 tool = TupToolPlugin::InkTool;
            break;

            /*
            case Qt::Key_E:
                 tool = TupToolPlugin::EraserTool;
            break;
            */

            case Qt::Key_S:
                 tool = TupToolPlugin::PolyLineTool;
            break;

            case Qt::Key_L:
                 tool = TupToolPlugin::LineTool;
            break;

            case Qt::Key_R:
                 tool = TupToolPlugin::RectangleTool;
            break;

            case Qt::Key_C:
                 tool = TupToolPlugin::EllipseTool;
            break;

            /* SQA: Temporarily disabled
            case Qt::Key_T:
                 tool = TupToolPlugin::TextTool;
            break;
            */

            case Qt::Key_O:
                 menu = TupToolPlugin::SelectionMenu;
                 tool = TupToolPlugin::ObjectsTool;
            break;

            case Qt::Key_N:
                 menu = TupToolPlugin::SelectionMenu;
                 tool = TupToolPlugin::NodesTool;
            break;

            case Qt::Key_I:
                 menu = TupToolPlugin::FillMenu;
                 tool = TupToolPlugin::InsideTool;
            break;

            case Qt::Key_B:
                 menu = TupToolPlugin::FillMenu;
                 tool = TupToolPlugin::ContourTool;
            break;

            case Qt::Key_Z:
                 menu = TupToolPlugin::ZoomMenu;
                 if (modifiers == Qt::ShiftModifier)
                     tool = TupToolPlugin::ZoomOutTool;
                 else
                     tool = TupToolPlugin::ZoomInTool;
            break;

            case Qt::Key_H:
                 menu = TupToolPlugin::ZoomMenu;
                 tool = TupToolPlugin::HandTool;
            break;

            case Qt::Key_Right:
                 menu = TupToolPlugin::Arrows;
                 if (modifiers == Qt::ControlModifier)
                     tool = TupToolPlugin::QuickCopy;
                 else
                     tool = TupToolPlugin::FrameForward;
            break;

            case Qt::Key_Left:
                 menu = TupToolPlugin::Arrows;
                 tool = TupToolPlugin::FrameBack;
            break;

            case Qt::Key_Delete:
                 menu = TupToolPlugin::SelectionMenu;
                 tool = TupToolPlugin::Delete;
            break;

            default:
            {
                 menu = TupToolPlugin::InvalidMenu;
                 tool = TupToolPlugin::InvalidBrush;
            }
    }

    QPair<int, int> flags;
    flags.first = menu;
    flags.second = tool;

    return flags;
}




