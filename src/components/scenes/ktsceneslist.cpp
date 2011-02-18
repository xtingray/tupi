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

#include "ktsceneslist.h"
#include "kdebug.h"

#include <QTreeWidgetItem>
#include <QHeaderView>

// SQA: Add support for renaming and moving objects from the list

KTScenesList::KTScenesList(QWidget *parent) : KTreeListWidget(parent)
{
    setHeaderLabels(QStringList() << "");
    header()->setResizeMode(QHeaderView::ResizeToContents);
    setColumnCount(1);

    setItemDelegate(new KTScenesDelegate(this));

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(changeCurrentScene()));
}

KTScenesList::~KTScenesList()
{
}

void KTScenesList::insertScene(int index, const QString &name)
{
    QTreeWidgetItem *newScene = new QTreeWidgetItem(this);
    newScene->setText(0, name);
    newScene->setFlags(newScene->flags() | Qt::ItemIsEditable);
    insertTopLevelItem(index, newScene);
}

/*
void KTScenesList::changeCurrentName(QString name)
{
    currentItem()->setText(0, name);
} 
*/

int KTScenesList::removeCurrentScene()
{
    int index = indexCurrentScene();

    if (currentItem()) {
        delete currentItem();
    }

    return index;
}

void KTScenesList::removeScene(int index)
{
    delete topLevelItem(index);
}

void KTScenesList::renameScene(int index, const QString &name)
{
    QTreeWidgetItem *item = topLevelItem(index);

    if (item)
        item->setText(0,name);
}

void KTScenesList::selectScene(int index)
{
    QTreeWidgetItem *item = topLevelItem(index);

    if (item)
        setCurrentItem(item);
}

void KTScenesList::changeCurrentScene()
{
    if (currentItem()) {
        QString name = currentItem()->text(0);
        if (name.length() == 0)
            return;
        int index = indexCurrentScene();
        emit(changeCurrent(name, index));
    }
}

int KTScenesList::moveCurrentSceneUp()
{
    int index = indexCurrentScene();
    if (index > 0)
        insertTopLevelItem(index-1, takeTopLevelItem(index));

    return index;
}

int KTScenesList::moveCurrentSceneDown()
{
    int index = indexCurrentScene();
    if (index < topLevelItemCount()-1) 
        insertTopLevelItem(index+1, takeTopLevelItem(index));

    return index;
}

int KTScenesList::indexCurrentScene()
{
    return indexOfTopLevelItem(currentItem());
}

QString KTScenesList::nameCurrentScene()
{
    return currentItem()->text(0);
}

int KTScenesList::scenesCount()
{
    return topLevelItemCount();
}

void KTScenesList::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        callRename();
}

void KTScenesList::callRename()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit itemRenamed(item);
}
