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

#include "kttoolplugin.h"
#include "ktbrushmanager.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"

#include "tdebug.h"

#include <QGraphicsView>

struct KTToolPlugin::Private
{
    QString currentTool;
};

KTToolPlugin::KTToolPlugin(QObject * parent) : QObject(parent), k(new Private)
{
}

KTToolPlugin::~KTToolPlugin()
{
    delete k;
}

void KTToolPlugin::init(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void KTToolPlugin::updateScene(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void KTToolPlugin::setName(const QString &tool)
{
    k->currentTool = tool;
}

QString KTToolPlugin::name() const
{
    return k->currentTool;
}

void KTToolPlugin::begin()
{
    tDebug("tools") << "Begin: " << k->currentTool;
}

void KTToolPlugin::end()
{
    tDebug("tools") << "End: " << k->currentTool;
}

void KTToolPlugin::sceneResponse(const KTSceneResponse *event)
{
    Q_UNUSED(event);
}

void KTToolPlugin::layerResponse(const KTLayerResponse *event)
{
    Q_UNUSED(event);
}

void KTToolPlugin::frameResponse(const KTFrameResponse *event)
{
    Q_UNUSED(event);
}

void KTToolPlugin::itemResponse(const KTItemResponse *event)
{
    Q_UNUSED(event);
}

void KTToolPlugin::doubleClick(const KTInputDeviceInformation *, KTGraphicsScene *)
{
}

void KTToolPlugin::aboutToChangeScene(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void KTToolPlugin::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KTToolPlugin::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

QMenu *KTToolPlugin::menu() const
{
    return 0;
}

void KTToolPlugin::saveConfig() 
{
}

QCursor KTToolPlugin::cursor() const
{
    return 0;
}

QPair<int, int> KTToolPlugin::setKeyAction(int key, Qt::KeyboardModifiers modifiers)
{
    KTToolPlugin::MenuIndex menu = KTToolPlugin::BrushesMenu;
    int tool = KTToolPlugin::PencilTool;

    switch (key) {
            case Qt::Key_P:
                 tool = KTToolPlugin::PencilTool;
            break;

            case Qt::Key_M:
                 tool = KTToolPlugin::SchemeTool;
            break;

            /* SQA: Temporarily disabled
            case Qt::Key_K:
                 tool = KTToolPlugin::InkTool;
            break;

            case Qt::Key_E:
                 tool = KTToolPlugin::EraserTool;
            break;
            */

            case Qt::Key_S:
                 tool = KTToolPlugin::PolyLineTool;
            break;

            case Qt::Key_L:
                 tool = KTToolPlugin::LineTool;
            break;

            case Qt::Key_R:
                 tool = KTToolPlugin::RectangleTool;
            break;

            case Qt::Key_C:
                 tool = KTToolPlugin::EllipseTool;
            break;

            /* SQA: Temporarily disabled
            case Qt::Key_T:
                 tool = KTToolPlugin::TextTool;
            break;
            */

            case Qt::Key_O:
                 menu = KTToolPlugin::SelectionMenu;
                 tool = KTToolPlugin::ObjectsTool;
            break;

            case Qt::Key_N:
                 menu = KTToolPlugin::SelectionMenu;
                 tool = KTToolPlugin::NodesTool;
            break;

            case Qt::Key_I:
                 menu = KTToolPlugin::FillMenu;
                 tool = KTToolPlugin::InsideTool;
            break;

            case Qt::Key_B:
                 menu = KTToolPlugin::FillMenu;
                 tool = KTToolPlugin::ContourTool;
            break;

            case Qt::Key_Z:
                 menu = KTToolPlugin::ZoomMenu;
                 tool = KTToolPlugin::ViewTool;
            break;

            case Qt::Key_H:
                 menu = KTToolPlugin::ZoomMenu;
                 tool = KTToolPlugin::HandTool;
            break;

            case Qt::Key_Right:
                 menu = KTToolPlugin::Arrows;
                 if (modifiers == Qt::ControlModifier)
                     tool = KTToolPlugin::QuickCopy;
                 else
                     tool = KTToolPlugin::FrameForward;
            break;

            case Qt::Key_Left:
                 menu = KTToolPlugin::Arrows;
                 tool = KTToolPlugin::FrameBack;
            break;

            case Qt::Key_Delete:
                 menu = KTToolPlugin::SelectionMenu;
                 tool = KTToolPlugin::Delete;
            break;

            default:
            {
                 menu = KTToolPlugin::InvalidMenu;
                 tool = KTToolPlugin::InvalidBrush;
            }
    }

    QPair<int, int> flags;
    flags.first = menu;
    flags.second = tool;

    return flags;
}


