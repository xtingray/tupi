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
#include <QGraphicsView>

#include "ktbrushmanager.h"
#include "ktinputdeviceinformation.h"
#include "ktprojectresponse.h"
#include "ktgraphicsscene.h"

#include "kdebug.h"

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
    kDebug("tools") << "Begin: " << k->currentTool;
}

void KTToolPlugin::end()
{
    kDebug("tools") << "End: " << k->currentTool;
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

QMenu *KTToolPlugin::menu() const
{
    return 0;
}

void KTToolPlugin::saveConfig() 
{
}
