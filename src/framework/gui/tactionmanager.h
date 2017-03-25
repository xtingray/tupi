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

#ifndef TACTIONMANAGER_H
#define TACTIONMANAGER_H

#include "tglobal.h"
#include "taction.h"

#include <QMenu>
#include <QObject>
#include <QWidget>
#include <QList>
#include <QHash>
#include <QMenuBar>
#include <QToolBar>

typedef QList<QAction *> TActionList;
typedef QHash<QString, QAction *> TActionDict;
typedef QHash<QString, TActionDict> TActionContainer;

class QToolBar;
class QMenuBar;

/**
 * @short class TActionManager provides an action manager, this handler makes easier the access and sort of any action of the app.
 * @author David Cuadrado
*/

class T_GUI_EXPORT TActionManager : public QObject
{
    Q_OBJECT

    public:
        TActionManager(QObject *parent = 0);
        ~TActionManager();

        bool insert(QAction *action, const QString &id, const QString &container = "default");
        void remove(QAction* action, const QString &container = QString());

        QAction *take(QAction* action, const QString &container = QString());
        QAction *find(const QString &id, const QString &container = QString()) const;
        QAction *operator[](const QString &id) const;
        void enable(const QString &id, bool flag);
        void exec(const QString &id);

        QMenuBar *setupMenuBar(QMenuBar *menu, const QStringList &containers, bool clear = true);
        QMenu *setupMenu(QMenu *menu, const QString &container, bool clear = true);
        QToolBar *setupToolBar(QToolBar *toolBar, const QString &container, bool clear = true);

    private:
        TActionContainer m_actionContainer;
};

#endif
