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

#ifndef KTSCENESLIST_H
#define KTSCENESLIST_H

#include "ktreelistwidget.h"
#include "ktscenesdelegate.h"

#include <QMouseEvent>

/**
 * @author Jorge Cuadrado
*/

class KTScenesList : public KTreeListWidget
{
    Q_OBJECT

    public:
        KTScenesList(QWidget *parent = 0);
        ~KTScenesList();
        void insertScene(int index, const QString &name);

        int removeCurrentScene();
        int indexCurrentScene();
        int scenesCount();

        QString nameCurrentScene();

        void removeScene(int index);
        void renameScene(int index, const QString &name);

        void selectScene(int index);

        bool nameExists(QString &name);
        void resetUI();

    protected:
        void mouseDoubleClickEvent(QMouseEvent *event);

    private slots:
        void callRename();
        void changeCurrentScene();

    public slots:
        int moveCurrentSceneUp();
        int moveCurrentSceneDown();

    signals:
        void itemRenamed(QTreeWidgetItem *);
        void changeCurrent(QString name, int index);

    private:
        struct Private;
        Private *const k;
};

#endif
