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
#include "tdebug.h"

#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>

struct KTListProjectDialog::Private
{
    QTreeWidget *works;
    QTreeWidget *contributions;
    QList<QString> workList;
    QList<QString> contribList;
    int index;
    QString filename;
};

KTListProjectDialog::KTListProjectDialog(int works, int contributions, const QString &serverName) : QDialog(), k(new Private)
{
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/open.png")));
    setWindowTitle(tr("Projects List from Server") + " - [ " + serverName  + " ]");
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    if (works > 0) {
        k->works = tree();
        connect(k->works, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(updateWorkTree()));
        connect(k->works, SIGNAL(itemSelectionChanged()), this, SLOT(updateWorkTree()));
        connect(k->works, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(execAccept(QTreeWidgetItem *, int)));
    }

    if (contributions > 0) {
        k->contributions = tree();
        connect(k->contributions, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(updateContribTree()));
        connect(k->contributions, SIGNAL(itemSelectionChanged()), this, SLOT(updateContribTree()));
        connect(k->contributions, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(execAccept(QTreeWidgetItem *, int)));
    }

    QHBoxLayout *search = new QHBoxLayout;
    KTreeWidgetSearchLine *searchLine = 0;
    QToolButton *button = new QToolButton;
    button->setIcon(QIcon(THEME_DIR + "icons/zoom.png"));

    QLabel *worksLabel = new QLabel(tr("My works:"));
    QLabel *contribLabel = new QLabel(tr("My contributions:"));

    if (works > 0 && contributions > 0) {
        QList<QTreeWidget *> trees;
        trees << k->works << k->contributions;
        searchLine = new KTreeWidgetSearchLine(this, trees);
        search->addWidget(searchLine);
        search->addWidget(button);

        layout->addLayout(search);
        layout->addWidget(worksLabel);
        layout->addWidget(k->works);
        layout->addWidget(contribLabel);
        layout->addWidget(k->contributions);
    } else if (works > 0) {
               searchLine = new KTreeWidgetSearchLine(this, k->works);
               search->addWidget(searchLine);
               search->addWidget(button);

               layout->addLayout(search);
               layout->addWidget(worksLabel);
               layout->addWidget(k->works);
    } else if (contributions > 0) {
               searchLine = new KTreeWidgetSearchLine(this, k->contributions);
               search->addWidget(searchLine);
               search->addWidget(button);

               layout->addLayout(search);
               layout->addWidget(contribLabel);
               layout->addWidget(k->contributions);
    }

    connect(button, SIGNAL(clicked()), searchLine, SLOT(clear()));

    //----
    QHBoxLayout *buttons = new QHBoxLayout;
    QPushButton *accept = new QPushButton(tr("OK"));
    accept->setDefault(true);
    QPushButton *cancel = new QPushButton("Cancel");
    connect(accept, SIGNAL(clicked ()), this, SLOT(accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

    buttons->addWidget(cancel);
    buttons->addWidget(accept);
    layout->addLayout(buttons);

    setMinimumWidth(615); 
    k->index = 0;
}

KTListProjectDialog::~KTListProjectDialog()
{
}

QTreeWidget *KTListProjectDialog::tree()
{
    QTreeWidget *tree = new QTreeWidget;
    tree->setFixedHeight(120);
    tree->setHeaderLabels(QStringList() << tr("Name") << tr("Author") << tr("Description") << tr("Date"));
    tree->header()->show();

    tree->setColumnWidth(0, 150);
    tree->setColumnWidth(1, 100);
    tree->setColumnWidth(2, 200);
    tree->setColumnWidth(3, 55);

    return tree;
}

void KTListProjectDialog::addWork(const QString &filename, const QString &name, const QString &author, const QString &description, const QString &date)
{
    k->workList.append(filename);

    QTreeWidgetItem *item = new QTreeWidgetItem(k->works);
    item->setText(0, name);
    item->setText(1, author);
    item->setText(2, description);
    item->setText(3, date);

    if (k->index == 0) {
        k->works->setCurrentItem(item);
        k->filename = filename;
    }

    k->index++;
}

void KTListProjectDialog::addContribution(const QString &filename, const QString &name, const QString &author, const QString &description, const QString &date)
{
    k->contribList.append(filename);

    QTreeWidgetItem *item = new QTreeWidgetItem(k->contributions);
    item->setText(0, name);
    item->setText(1, author);
    item->setText(2, description);
    item->setText(3, date);
}

QString KTListProjectDialog::projectID()
{
    return k->filename;
}

void KTListProjectDialog::execAccept(QTreeWidgetItem *item, int index)
{
    Q_UNUSED(item);

    if (index >= 0)
        accept();
}

void KTListProjectDialog::updateWorkTree()
{
    if (k->works->hasFocus()) {
        if (k->contribList.size() > 0)
            k->contributions->clearSelection();
        int index = k->works->currentIndex().row(); 
        k->filename = k->workList.at(index);
    }
}

void KTListProjectDialog::updateContribTree()
{
    if (k->contributions->hasFocus()) {
        if (k->workList.size() > 0)
            k->works->clearSelection();
        int index = k->contributions->currentIndex().row();
        k->filename = k->contribList.at(index);
    }
}
