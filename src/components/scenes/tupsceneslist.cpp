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

#include "tupsceneslist.h"

// SQA: Add support for renaming and moving objects from the list

struct TupScenesList::Private
{
   int scenesCount;
};

TupScenesList::TupScenesList(QWidget *parent) : TreeListWidget(parent), k(new Private)
{
    k->scenesCount = 0;
    setHeaderLabels(QStringList() << "");
    // header()->setResizeMode(QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setColumnCount(1);

    setItemDelegate(new TupScenesDelegate(this));

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(changeCurrentScene())); 
}

TupScenesList::~TupScenesList()
{
}

void TupScenesList::insertScene(int index, const QString &name)
{
    k->scenesCount++;
    QTreeWidgetItem *newScene = new QTreeWidgetItem(this);
    newScene->setText(0, name);
    newScene->setFlags(newScene->flags() | Qt::ItemIsEditable);
    insertTopLevelItem(index, newScene);

    if (index == 0)
        setCurrentItem(newScene);
}

int TupScenesList::removeCurrentScene()
{
    int index = currentSceneIndex();

    if (currentItem()) {
        delete currentItem();
        k->scenesCount--;
        return index;
    }

    return -1;
}

void TupScenesList::removeScene(int index)
{
    k->scenesCount--;
    delete topLevelItem(index);
}

void TupScenesList::renameScene(int index, const QString &name)
{
    QTreeWidgetItem *item = topLevelItem(index);

    if (item)
        item->setText(0,name);
}

void TupScenesList::selectScene(int index)
{
    QTreeWidgetItem *item = topLevelItem(index);

    if (item)
        setCurrentItem(item);
}

void TupScenesList::changeCurrentScene()
{
    if (currentItem()) {
        int index = currentSceneIndex();
        emit changeCurrent(index);
    }
}

int TupScenesList::moveCurrentSceneUp()
{
    int index = currentSceneIndex();
    if (index > 0)
        insertTopLevelItem(index-1, takeTopLevelItem(index));

    return index;
}

int TupScenesList::moveCurrentSceneDown()
{
    int index = currentSceneIndex();
    if (index < topLevelItemCount()-1) 
        insertTopLevelItem(index+1, takeTopLevelItem(index));

    return index;
}

int TupScenesList::currentSceneIndex()
{
    int index = indexOfTopLevelItem(currentItem());
    if (index < 0)
        index = 0;

    return index;
}

QString TupScenesList::nameCurrentScene()
{
    return currentItem()->text(0);
}

int TupScenesList::scenesCount()
{
    return k->scenesCount;
}

void TupScenesList::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        callRename();
}

void TupScenesList::callRename()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit itemRenamed(item);
}

bool TupScenesList::nameExists(QString &name)
{
    for (int i = 0; i < topLevelItemCount(); i++) {
         QTreeWidgetItem *item = topLevelItem(i);
         if (item->text(0).compare(name) == 0)
             return true;
    }

    return false;
}

void TupScenesList::resetUI()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScenesList::resetUI()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    // blockSignals(true);
    clearSelection();
    k->scenesCount = 0;
    clear();
    // blockSignals(false);
}

void TupScenesList::keyPressEvent(QKeyEvent *event)
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
