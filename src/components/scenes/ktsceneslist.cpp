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
#include "tdebug.h"

#include <QTreeWidgetItem>
#include <QHeaderView>

// SQA: Add support for renaming and moving objects from the list

struct KTScenesList::Private
{
   int scenesTotal;
};

KTScenesList::KTScenesList(QWidget *parent) : KTreeListWidget(parent), k(new Private)
{
    k->scenesTotal = 0;
    setHeaderLabels(QStringList() << "");
    header()->setResizeMode(QHeaderView::ResizeToContents);
    setColumnCount(1);

    setItemDelegate(new KTScenesDelegate(this));

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(changeCurrentScene())); 
}

KTScenesList::~KTScenesList()
{
}

void KTScenesList::insertScene(int index, const QString &name)
{
    k->scenesTotal++;
    QTreeWidgetItem *newScene = new QTreeWidgetItem(this);
    newScene->setText(0, name);
    newScene->setFlags(newScene->flags() | Qt::ItemIsEditable);
    insertTopLevelItem(index, newScene);

    if (index == 0)
        setCurrentItem(newScene);
}

int KTScenesList::removeCurrentScene()
{
    int index = currentSceneIndex();

    if (currentItem()) {
        delete currentItem();
        k->scenesTotal--;
        return index;
    }

    return -1;
}

void KTScenesList::removeScene(int index)
{
    k->scenesTotal--;
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
        int index = currentSceneIndex();
        emit changeCurrent(index);
    }
}

int KTScenesList::moveCurrentSceneUp()
{
    int index = currentSceneIndex();
    if (index > 0)
        insertTopLevelItem(index-1, takeTopLevelItem(index));

    return index;
}

int KTScenesList::moveCurrentSceneDown()
{
    int index = currentSceneIndex();
    if (index < topLevelItemCount()-1) 
        insertTopLevelItem(index+1, takeTopLevelItem(index));

    return index;
}

int KTScenesList::currentSceneIndex()
{
    int index = indexOfTopLevelItem(currentItem());
    if (index < 0)
        index = 0;

    return index;
}

QString KTScenesList::nameCurrentScene()
{
    return currentItem()->text(0);
}

int KTScenesList::scenesCount()
{
    return k->scenesTotal;
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

bool KTScenesList::nameExists(QString &name)
{
    for (int i = 0; i < topLevelItemCount(); i++) {
         QTreeWidgetItem *item = topLevelItem(i);
         if (item->text(0).compare(name) == 0)
             return true;
    }

    return false;
}

void KTScenesList::resetUI()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    blockSignals(true);
    clearSelection();
    k->scenesTotal = 0;
    clear();
    blockSignals(false);
}

void KTScenesList::keyPressEvent(QKeyEvent *event)
{
    int index = currentSceneIndex();

    if (event->key() == Qt::Key_Down)
        index++;

    if (event->key() == Qt::Key_Up)
        index--;

    if (index > -1) {
        QTreeWidgetItem *item = topLevelItem(index);
        if (item) {
            setCurrentItem(item);
            emit changeCurrent(index);
        }
    }
}
