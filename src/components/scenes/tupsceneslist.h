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

#ifndef TUPSCENESLIST_H
#define TUPSCENESLIST_H

#include "treelistwidget.h"
#include "tupscenesdelegate.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QTreeWidgetItem>

/**
 * @author Jorge Cuadrado
*/

class TupScenesList : public TreeListWidget
{
    Q_OBJECT

    public:
        TupScenesList(QWidget *parent = 0);
        ~TupScenesList();
        void insertScene(int index, const QString &name);

        int removeCurrentScene();
        int currentSceneIndex();
        int scenesCount();

        QString nameCurrentScene();

        void removeScene(int index);
        void renameScene(int index, const QString &name);

        void selectScene(int index);

        bool nameExists(QString &name);
        void resetUI();

    public slots:
        int moveCurrentSceneUp();
        int moveCurrentSceneDown();

    protected:
        virtual void keyPressEvent(QKeyEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);

    signals:
        void itemRenamed(QTreeWidgetItem *);
        void changeCurrent(int index);

    private slots:
        void callRename();
        void changeCurrentScene();

    private:
        struct Private;
        Private *const k;
};

#endif
