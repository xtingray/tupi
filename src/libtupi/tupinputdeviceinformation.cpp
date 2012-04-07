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

#include "tupinputdeviceinformation.h"
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QTabletEvent>

/**
 * This class captures and process all the mouse/tablet events.
 *
 * @author David Cuadrado
*/

struct TupInputDeviceInformation::Private
{
    struct TabletInfo
    {
        double pressure;
        double rotation;
        double tangentialPressure;
    } tabletInfo;
    
    struct MouseInfo
    {
        Qt::MouseButton button;
        Qt::MouseButtons buttons;
    } mouseInfo;
    
    QPointF position;
    Qt::KeyboardModifiers keyModifiers;
};

TupInputDeviceInformation::TupInputDeviceInformation(QObject *parent) : QObject(parent), k(new Private)
{
    k->mouseInfo.button = Qt::NoButton;
    k->mouseInfo.buttons = Qt::NoButton;
    
    k->tabletInfo.pressure = -1;
    k->tabletInfo.rotation = 0;
    k->tabletInfo.tangentialPressure = -1;
    
    k->keyModifiers = Qt::NoModifier;
}

TupInputDeviceInformation::~TupInputDeviceInformation()
{
    delete k;
}

void TupInputDeviceInformation::updateFromMouseEvent(QGraphicsSceneMouseEvent *event)
{
    k->mouseInfo.button = event->button();
    k->mouseInfo.buttons = event->buttons();
    
    k->position = event->scenePos();
    
    k->keyModifiers = event->modifiers();
    
    k->tabletInfo.pressure = -1;
    k->tabletInfo.rotation = 0;
    k->tabletInfo.tangentialPressure = -1;
}

void TupInputDeviceInformation::updateFromMouseEvent(QMouseEvent *event)
{
    k->mouseInfo.button = event->button();
    k->mouseInfo.buttons = event->buttons();
    
    k->position = event->pos();
    
    k->keyModifiers = event->modifiers();
    
    k->tabletInfo.pressure = -1;
    k->tabletInfo.rotation = 0;
    k->tabletInfo.tangentialPressure = -1;
}

void TupInputDeviceInformation::updateFromTabletEvent(QTabletEvent *event)
{
    k->tabletInfo.pressure = event->pressure();
    k->tabletInfo.rotation = event->rotation();
    k->tabletInfo.tangentialPressure = event->tangentialPressure();
    
    k->position = event->pos();
    
    k->keyModifiers = event->modifiers();
}

double TupInputDeviceInformation::pressure() const
{
    return k->tabletInfo.pressure;
}

double TupInputDeviceInformation::rotation() const
{
    return k->tabletInfo.rotation;
}

double TupInputDeviceInformation::tangentialPressure() const
{
    return k->tabletInfo.tangentialPressure;
}

Qt::MouseButton TupInputDeviceInformation::button() const
{
    return k->mouseInfo.button;
}

Qt::MouseButtons TupInputDeviceInformation::buttons() const
{
    return k->mouseInfo.buttons;
}

QPointF TupInputDeviceInformation::pos() const
{
    return k->position;
}

Qt::KeyboardModifiers TupInputDeviceInformation::keyModifiers() const
{
    return k->keyModifiers;
}
