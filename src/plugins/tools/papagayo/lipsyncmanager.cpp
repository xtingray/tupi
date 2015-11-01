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

#include "lipsyncmanager.h"

struct LipSyncManager::Private
{
    QListWidget *lipSyncList;
    TImageButton *addButton;
    TImageButton *editButton;
    TImageButton *delButton;

    QString target;
};

LipSyncManager::LipSyncManager(QWidget *parent) : QWidget(parent), k(new Private)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

#ifndef Q_OS_MAC
    QFont font = this->font();
    font.setPointSize(8);
    setFont(font);
#endif

    QBoxLayout *listLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    listLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    k->lipSyncList = new QListWidget;
    k->lipSyncList->setContextMenuPolicy(Qt::CustomContextMenu);
    k->lipSyncList->setViewMode(QListView::ListMode);
    k->lipSyncList->setFlow(QListView::TopToBottom);
    k->lipSyncList->setMovement(QListView::Static);
    k->lipSyncList->setFixedHeight(68);

    listLayout->addWidget(k->lipSyncList);

    k->addButton = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "plus_sign.png"), 22);
    k->addButton->setToolTip(tr("Import LipSync"));
    connect(k->addButton, SIGNAL(clicked()), this, SIGNAL(importLipSync()));

    k->editButton = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "edit_sign.png"), 22);
    k->editButton->setToolTip(tr("Edit LipSync"));
    connect(k->editButton, SIGNAL(clicked()), this, SLOT(editLipSync()));

    k->delButton = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "minus_sign.png"), 22);
    k->delButton->setToolTip(tr("Remove LipSync"));
    connect(k->delButton, SIGNAL(clicked()), this, SLOT(removeLipSync()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setAlignment(Qt::AlignHCenter);
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);
    buttonLayout->addWidget(k->addButton);
    buttonLayout->addWidget(k->editButton);
    buttonLayout->addWidget(k->delButton);

    layout->addLayout(listLayout);
    layout->addLayout(buttonLayout);
}

LipSyncManager::~LipSyncManager()
{
}

void LipSyncManager::loadLipSyncList(QList<QString> list)
{
    k->lipSyncList->clear();

    int total = list.size();
    for (int i=0; i < total; i++) {
        QListWidgetItem *item = new QListWidgetItem(k->lipSyncList);
#ifndef Q_OS_MAC
        QFont font = this->font();
        font.setPointSize(8);
        item->setFont(font);
#endif
        item->setText(list.at(i));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    k->lipSyncList->setCurrentRow(0);
}

bool LipSyncManager::itemExists(const QString &name)
{
    for (int i=0; i < k->lipSyncList->count(); i++) {
         QListWidgetItem *item = k->lipSyncList->item(i);
         if (name.compare(item->text()) == 0)
             return true;
    }

     return false;
}

void LipSyncManager::editLipSync()
{
    if (k->lipSyncList->count() > 0) {
        QListWidgetItem *item = k->lipSyncList->currentItem();
        if (item)
            emit editCurrentLipSync(item->text());
    }
}

void LipSyncManager::removeLipSync()
{
    if (k->lipSyncList->count() > 0) {
        QListWidgetItem *item = k->lipSyncList->currentItem();
        if (item) {
            k->lipSyncList->takeItem(k->lipSyncList->row(item));
            k->target = item->text();
            emit removeCurrentLipSync(k->target);
        }
    }
}

void LipSyncManager::resetUI()
{
    if (k->lipSyncList->count() > 0)
        k->lipSyncList->clear();
}

QString LipSyncManager::currentLipSyncName() const
{
    QListWidgetItem *item = k->lipSyncList->currentItem();
    return item->text();
}

int LipSyncManager::listSize()
{
    return k->lipSyncList->count();
}

void LipSyncManager::addNewRecord(const QString &name)
{
    QFont f = font();
    f.setPointSize(8);

    QListWidgetItem *item = new QListWidgetItem(k->lipSyncList);
#ifndef Q_OS_MAC
    QFont font = this->font();
    font.setPointSize(8);
    item->setFont(font);
#endif
    item->setText(name);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    k->lipSyncList->setCurrentItem(item);
}
