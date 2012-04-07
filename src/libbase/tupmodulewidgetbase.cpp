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

#include "tupmodulewidgetbase.h"
#include "tdebug.h"

#include <QEvent>
#include <QBoxLayout>
#include <QDialog>

struct TupModuleWidgetBase::Private
{
    QBoxLayout *container;
    QObjectList childs;
};

TupModuleWidgetBase::TupModuleWidgetBase(QWidget *parent, const char *name) : QWidget(parent),
                                                                            TupAbstractProjectResponseHandler(), k(new Private)
{
    setObjectName(name);

    k->container = new QVBoxLayout(this);
    k->container->setMargin(5);
    k->container->setSpacing(1);

    adjustSize();
    hide();
}

TupModuleWidgetBase::~TupModuleWidgetBase()
{
    delete k;
}

void TupModuleWidgetBase::addChild(QWidget* child, Qt::Alignment alignment)
{
    k->childs.append(child);
    k->container->invalidate();
    k->container->addWidget(child, 0,alignment);
}

void TupModuleWidgetBase::setFont(const QFont &f)
{
    QWidget::setFont(f);
    adjustSize();
}

bool TupModuleWidgetBase::event(QEvent * e)
{
    if (e->type() == QEvent::Hide) {
        emit activate(false);
    } else if (e->type() == QEvent::Show) {
               emit activate(true);
    }

    return QWidget::event(e);
}

void TupModuleWidgetBase::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
}

void TupModuleWidgetBase::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
}

QBoxLayout *TupModuleWidgetBase::boxLayout()
{
    return k->container;
}

bool TupModuleWidgetBase::handleProjectResponse(TupProjectResponse *response)
{
    return handleResponse(response);
}

void TupModuleWidgetBase::frameResponse(TupFrameResponse *frameResponse)
{
    Q_UNUSED(frameResponse);
}

void TupModuleWidgetBase::layerResponse(TupLayerResponse *layerResponse)
{
    Q_UNUSED(layerResponse);
}

void TupModuleWidgetBase::sceneResponse(TupSceneResponse *sceneResponse)
{
    Q_UNUSED(sceneResponse);
}

void TupModuleWidgetBase::projectResponse(TupProjectResponse *projectResponse)
{
    Q_UNUSED(projectResponse);
}

void TupModuleWidgetBase::itemResponse(TupItemResponse *itemResponse)
{
    Q_UNUSED(itemResponse);
}

void TupModuleWidgetBase::libraryResponse(TupLibraryResponse *libraryResponse)
{
    Q_UNUSED(libraryResponse);
}
