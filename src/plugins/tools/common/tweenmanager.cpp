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

#include "tweenmanager.h"

struct TweenManager::Private
{
    QLineEdit *input;
    QListWidget *tweensList;
    TImageButton *addButton;

    QString target;
};

TweenManager::TweenManager(QWidget *parent) : QWidget(parent), k(new Private)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    k->input = new QLineEdit;
    k->addButton = new TImageButton(QPixmap(kAppProp->themeDir() + "/icons/plus_sign.png"), 22);
    k->addButton->setToolTip(tr("Create a new Tween"));
    connect(k->input, SIGNAL(returnPressed()), this, SLOT(addTween()));
    connect(k->addButton, SIGNAL(clicked()), this, SLOT(addTween()));

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lineLayout->setMargin(0);
    lineLayout->setSpacing(0);
    lineLayout->addWidget(k->input);
    lineLayout->addWidget(k->addButton);

    layout->addLayout(lineLayout);

    QBoxLayout *listLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    listLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    k->tweensList = new QListWidget;
    k->tweensList->setContextMenuPolicy(Qt::CustomContextMenu);
    k->tweensList->setViewMode(QListView::ListMode);
    k->tweensList->setFlow(QListView::TopToBottom);
    k->tweensList->setMovement(QListView::Static);
    k->tweensList->setFixedHeight(68);
    connect(k->tweensList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showMenu(const QPoint &)));
    connect(k->tweensList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(updateTweenData(QListWidgetItem *)));
    connect(k->tweensList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editTween(QListWidgetItem *)));

    listLayout->addWidget(k->tweensList);

    layout->addLayout(listLayout);
}

TweenManager::~TweenManager()
{
}

void TweenManager::loadTweenList(QList<QString> tweenList)
{
    k->tweensList->clear();

    for (int i=0; i < tweenList.size(); i++) {
        QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
        tweenerItem->setText(tweenList.at(i));
        tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    k->tweensList->setCurrentRow(0);
}

bool TweenManager::itemExists(const QString &name)
{
     for (int i=0; i < k->tweensList->count(); i++) {
          QListWidgetItem *item = k->tweensList->item(i);
          if (name.compare(item->text()) == 0)
              return true;
     }

     return false;
}

void TweenManager::addTween()
{
    QString name = k->input->text();

    if (name.length() > 0) {
        if (!itemExists(name)) {
            QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
            tweenerItem->setText(name);
            tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            k->input->clear();
            k->tweensList->setCurrentItem(tweenerItem);

            emit addNewTween(name);
        } else {
            TOsd::self()->display(tr("Error"), tr("Tween name already exists!"), TOsd::Error);
        }
    } else {
        int i = 0;
        while (true) {
               QString num = QString::number(i); 
               if (i < 10)
                   num = "0" + QString::number(i);

               QString name = "tween" + num; 
               QList<QListWidgetItem *> items = k->tweensList->findItems(name, Qt::MatchExactly);
               if (items.count() == 0) {
                   k->input->setText(name);
                   break;
               }
               i++;
        }
    }
}

void TweenManager::editTween()
{
    QListWidgetItem *item = k->tweensList->currentItem();
    emit editCurrentTween(item->text());
}

void TweenManager::editTween(QListWidgetItem *item)
{
    emit editCurrentTween(item->text());
}

void TweenManager::removeTween()
{
    removeItemFromList();

    emit removeCurrentTween(k->target);
}

void TweenManager::removeItemFromList()
{
    QListWidgetItem *item = k->tweensList->currentItem();
    k->tweensList->takeItem(k->tweensList->row(item));
    k->target = item->text();
}

void TweenManager::showMenu(const QPoint &point)
{
    if (k->tweensList->count() > 0) {
        QAction *edit = new QAction(tr("Edit"), this);
        connect(edit, SIGNAL(triggered()), this, SLOT(editTween()));
        QAction *remove = new QAction(tr("Remove"), this);
        connect(remove, SIGNAL(triggered()), this, SLOT(removeTween()));

        QMenu *menu = new QMenu(tr("Options"));
        menu->addAction(edit);
        menu->addAction(remove);

        QPoint globalPos = k->tweensList->mapToGlobal(point);
        menu->exec(globalPos);
    }
}

void TweenManager::updateTweenData(QListWidgetItem *item)
{
    emit getTweenData(item->text());
}

void TweenManager::resetUI()
{
    k->input->clear();

    if (k->tweensList->count() > 0)
        k->tweensList->clear();
}

QString TweenManager::currentTweenName() const
{
    QListWidgetItem *item = k->tweensList->currentItem();
    return item->text();
}

int TweenManager::listSize()
{
    return k->tweensList->count();
}

void TweenManager::updateTweenName(const QString &name)
{
    QListWidgetItem *item = k->tweensList->currentItem();
    item->setText(name);
    k->target = name;
}
