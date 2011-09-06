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

#include "ktlistprojectdialog.h"
#include "ktreelistwidget.h"
#include "ktreewidgetsearchline.h"

#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>

struct KTListProjectDialog::Private
{
    QTreeWidget *tree;
    KTreeWidgetSearchLine *search;
    QPushButton *accept, *cancel;
    int index;
};

KTListProjectDialog::KTListProjectDialog(const QString &serverName) : QDialog(), k(new Private)
{
    setWindowTitle(tr("Projects List from Server") + " - [ " + serverName  + " ]");
    setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    QHBoxLayout *search = new QHBoxLayout;

    k->tree = new QTreeWidget;
    k->tree->setHeaderLabels(QStringList() << tr("Name") << tr("Author") << tr("Description") << tr("Date"));
    k->tree->header()->show();

    k->tree->setColumnWidth(0, 150);
    k->tree->setColumnWidth(1, 100);
    k->tree->setColumnWidth(2, 200);
    k->tree->setColumnWidth(3, 55);

    k->search = new KTreeWidgetSearchLine(this, k->tree);
    search->addWidget(k->search);

    QToolButton *button = new QToolButton;
    button->setIcon(QIcon(THEME_DIR + "icons/magnifying.png"));

    search->addWidget(button);
    connect(button, SIGNAL(clicked()), k->search, SLOT(clear()));
    
    connect(k->tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(execAccept(QTreeWidgetItem * , int)));
    
    layout->addLayout(search);
    layout->addWidget(k->tree);
    
    //----
    QHBoxLayout *buttons = new QHBoxLayout;
    k->accept = new QPushButton(tr("OK"));
    k->cancel = new QPushButton("Cancel");
    connect(k->accept, SIGNAL(clicked ()), this, SLOT(accept()));
    connect(k->cancel, SIGNAL(clicked()), this, SLOT(reject()));

    buttons->addWidget(k->cancel);
    buttons->addWidget(k->accept);
    layout->addLayout(buttons);

    setMinimumWidth(615); 
    k->index = 0;
}

KTListProjectDialog::~KTListProjectDialog()
{
}

void KTListProjectDialog::addProject(const QString &name, const QString &author, const QString &description, const QString &date)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(k->tree);
    item->setText(0, name);
    item->setText(1, author);
    item->setText(2, description);
    item->setText(3, date);

    if (k->index == 0)
        k->tree->setCurrentItem(item);

    k->index++;
}

QString KTListProjectDialog::currentProject()
{
    QTreeWidgetItem *item = k->tree->currentItem();

    if (item)
        return item->text(0);

    return "";
}

void KTListProjectDialog::execAccept(QTreeWidgetItem *item, int index)
{
    if (index >= 0)
        accept();
}
