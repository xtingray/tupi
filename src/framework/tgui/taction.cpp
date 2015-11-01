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

#include "taction.h"

TAction::TAction(QObject * parent, const QString &id) : QAction(parent)
{
    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QString & text, QObject * parent, const QString &id) : QAction(text, parent)
{
    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QIcon & icon, const QString & text, QObject * parent, const QString &id) : QAction(icon, text, parent)
{
    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QIcon & icon, QObject *parent, const QString &id) : QAction(parent)
{
    setIcon(icon);

    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m , id);
}

TAction::TAction(const QIcon & icon, const QString & text, const QString &key, QObject * parent, const QString &id) : QAction(icon, text, parent)
{
    setShortcut(QKeySequence(key));
    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QIcon & icon, const QKeySequence &key, QObject * parent, const QString &id) : QAction(parent)
{
    setIcon(icon);
    setShortcut(key);

    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QIcon & icon, const QString &text, const QKeySequence &key, QObject *reciever, const char *slot, QObject * parent, const QString &id) : QAction(icon, text, parent)
{
    setShortcut(key);
    connect(this, SIGNAL(triggered()), reciever, slot);

    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::TAction(const QString &text, const QKeySequence &key, QObject *reciever, const char *slot, QObject * parent, const QString &id) : QAction(text, parent)
{
    setShortcut(key);
    connect(this, SIGNAL(triggered()), reciever, slot);

    if (TActionManager *m = dynamic_cast<TActionManager *>(parent))
        initWithManager(m, id);
}

TAction::~TAction()
{
}

void TAction::initWithManager(TActionManager * parent, const QString &id)
{
    setIconVisibleInMenu(true);

    if (!id.isEmpty())
        parent->insert(this, id);
}

void TAction::init()
{
}

void TAction::setCursor(const QCursor &cursor)
{
    m_cursor = cursor;
}

QCursor TAction::cursor() const
{
    return m_cursor;
}

