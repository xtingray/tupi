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

#include "tglobal.h"
#include "titemselector.h"

#include <QListWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

struct TItemSelector::Private
{
    QListWidget *available;
    QListWidget *selected;
};

TItemSelector::TItemSelector(QWidget *parent) : QWidget(parent), k(new Private)
{
    QHBoxLayout *layout = new QHBoxLayout;

    k->available = new QListWidget;

    layout->addWidget(k->available);

    QVBoxLayout *controlBox = new QVBoxLayout;
    controlBox->setSpacing(0);
    controlBox->setMargin(2);
    controlBox->addStretch();

    QToolButton *nextButton = new QToolButton;
    nextButton->setIcon(QPixmap(THEME_DIR + "icons/export_scene.png"));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(addCurrent()));

    controlBox->addWidget(nextButton);
    controlBox->setSpacing(5);

    QToolButton *previousButton = new QToolButton;
    previousButton->setIcon(QPixmap(THEME_DIR + "icons/unexport_scene.png"));
    connect(previousButton, SIGNAL(clicked()), this, SLOT(removeCurrent()));

    controlBox->addWidget(previousButton);
    controlBox->addStretch();

    layout->addLayout(controlBox);

    k->selected = new QListWidget;
    layout->addWidget(k->selected);

    setLayout(layout);
}


TItemSelector::~TItemSelector()
{
    delete k;
}

void TItemSelector::selectFirstItem() {
     if (k->available->item(0)) {
         // k->available->item(0)->setSelected(true);
         k->available->setCurrentRow(0);
         emit changed();
     }
}

void TItemSelector::addCurrent()
{
    int row = k->available->currentRow();

    if (row >= 0) {
        QListWidgetItem *item = k->available->takeItem(row);
        k->selected->addItem(item);

        emit changed();
    }
}

void TItemSelector::removeCurrent()
{
    int row = k->selected->currentRow();
    if (row >= 0) {
        QListWidgetItem *item = k->selected->takeItem(row);
        k->available->addItem(item);

        emit changed();
    }
}

void TItemSelector::upCurrent()
{
    int row = k->selected->currentRow();
    if (row >= 0) {
        QListWidgetItem *item = k->selected->takeItem(row);
        k->selected->insertItem(row-1,item);
        k->selected->setCurrentItem(item);

        emit changed();
    }
}

void TItemSelector::downCurrent()
{
    int row = k->selected->currentRow();
    if (row >= 0) {
        QListWidgetItem *item = k->selected->takeItem(row);
        k->selected->insertItem(row+1,item);
        k->selected->setCurrentItem(item);

        emit changed();
    }
}


void TItemSelector::setItems(const QStringList &items)
{
    k->available->clear();
    addItems(items);
}

int TItemSelector::addItem(const QString &item)
{
    QListWidgetItem *it = new QListWidgetItem(item, k->available);
    int index = k->available->count()-1;
    it->setData(4321, index);

    return index;
}

void TItemSelector::addItems(const QStringList &items)
{
    foreach (QString item, items)
             addItem(item);
}

QStringList TItemSelector::selectedItems() const
{
    QStringList items;
    for (int row = 0; row < k->selected->count(); row++) {
         QListWidgetItem *item = k->selected->item(row);
         items << item->text();
    }

    return items;
}

QList<int> TItemSelector::selectedIndexes() const
{
    QList<int> indexes;
    for (int row = 0; row < k->selected->count(); row++) {
         QListWidgetItem *item = k->selected->item(row);
         indexes << item->data(4321).toInt();
    }

    return indexes;
}

void TItemSelector::clear()
{
    k->available->clear();
    reset();
}

void TItemSelector::reset()
{
    k->selected->clear();
    emit changed();
}

