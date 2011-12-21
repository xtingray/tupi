/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "postdialog.h"
#include "kitemselector.h"
#include "tapplicationproperties.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

struct PostDialog::Private
{
    KItemSelector *selector;
    QPushButton *postButton;
    QList<int> scenes;
};

PostDialog::PostDialog(QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Select scenes to export"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/export.png")));

    k->selector = new KItemSelector;
    connect(k->selector, SIGNAL(changed()), this, SLOT(updateState()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);

    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancel);

    k->postButton = new QPushButton(tr("Post Video"));
    k->postButton->setEnabled(false);
    connect(k->postButton, SIGNAL(clicked()), this, SLOT(setData()));
    k->postButton->setDefault(true);
    buttonLayout->addWidget(k->postButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(k->selector);
    layout->addLayout(buttonLayout);
    setLayout(layout);
}

PostDialog::~PostDialog()
{
}

void PostDialog::setScenes(const QList<KTScene *> &scenes)
{
    k->selector->clear();
    int pos = 1;

    foreach (KTScene *scene, scenes) {
             k->selector->addItem(QString("%1: ").arg(pos) + scene->sceneName());
             pos++;
    }

    k->selector->selectFirstItem();
}

void PostDialog::updateState()
{
    if (k->selector->selectedItems().count() > 0)
        k->postButton->setEnabled(true);
    else
        k->postButton->setEnabled(false);
}

void PostDialog::setData()
{
    k->scenes = k->selector->selectedIndexes();
    QDialog::accept();
}


QList<int> PostDialog::sceneIndexes() const
{
    return k->scenes;
} 
