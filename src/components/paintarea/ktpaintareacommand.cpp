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

#include "ktpaintareacommand.h"
#include "ktpaintarea.h"
#include "ktpaintareaevent.h"
#include "ktbrushmanager.h"

#include "kdebug.h" 

#include <QVariant>

/**
 * This class defines the undo/redo functions for the paint area
 * @author David Cuadrado <krawek@toonka.com>
*/

struct KTPaintAreaCommand::Private
{
    KTPaintArea *paintArea;
    KTPaintAreaEvent *event;
    QVariant oldData;
};

KTPaintAreaCommand::KTPaintAreaCommand(KTPaintArea *area, const KTPaintAreaEvent *event) : QUndoCommand(), k(new Private)
{
    k->paintArea = area;
    k->event = event->clone();
}

KTPaintAreaCommand::~KTPaintAreaCommand()
{
    delete k;
}

void KTPaintAreaCommand::undo()
{
    switch(k->event->action()) {
           case KTPaintAreaEvent::ChangePen:
                k->paintArea->brushManager()->setPen(qvariant_cast<QPen>(k->oldData));
                break;
           case KTPaintAreaEvent::ChangeColorPen:
                k->paintArea->brushManager()->setPenColor(qvariant_cast<QColor>(k->oldData));
                break;
           case KTPaintAreaEvent::ChangeBrush:
                k->paintArea->brushManager()->setBrush(qvariant_cast<QBrush>(k->oldData));
                break;
           default: 
                break;
    }
}

void KTPaintAreaCommand::redo()
{
    switch (k->event->action()) {
            case KTPaintAreaEvent::ChangePen:
                 {
                   k->oldData = k->paintArea->brushManager()->pen();
                   QPen pen = qvariant_cast<QPen>(k->event->data());
                   if (!pen.color().isValid()) {
                       QPen old = k->paintArea->brushManager()->pen();
                       pen.setColor(old.color());
                       pen.setBrush(old.brush());
                   } 
                   k->paintArea->brushManager()->setPen(pen);
                 }
                 break;

            case KTPaintAreaEvent::ChangeColorPen:
                 {
                   k->oldData = k->paintArea->brushManager()->pen().color();
                   k->paintArea->brushManager()->setPenColor(qvariant_cast<QColor>(k->event->data()));
                 }
                 break;

            case KTPaintAreaEvent::ChangeBrush:
                 {
                   k->oldData = k->paintArea->brushManager()->brush();
                   k->paintArea->brushManager()->setBrush(qvariant_cast<QBrush>(k->event->data()));
                 }
                 break;

            default: 
                 break;
    }
}
