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
    KTreeWidgetSearchLine *search;
    QPushButton *accept, *cancel;
    QList<QString> workList;
    QList<QString> contribList;
    int index;
    QString filename;
};

KTListProjectDialog::KTListProjectDialog(int works, int contributions, const QString &serverName) : QDialog(), k(new Private)
{
    setWindowTitle(tr("Projects List from Server") + " - [ " + serverName  + " ]");
    setModal(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    QHBoxLayout *search = new QHBoxLayout;

    if (works > 0) {
        k->works = tree();
        connect(k->works, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(updateSelection()));
        connect(k->works, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
        connect(k->works, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(execAccept(QTreeWidgetItem *, int)));

        k->search = new KTreeWidgetSearchLine(this, k->works);
        search->addWidget(k->search);
        QToolButton *button = new QToolButton;
        button->setIcon(QIcon(THEME_DIR + "icons/zoom.png"));
        search->addWidget(button);
        connect(button, SIGNAL(clicked()), k->search, SLOT(clear()));

        QLabel *worksLabel = new QLabel(tr("My works:"));

        layout->addLayout(search);
        layout->addWidget(worksLabel);
        layout->addWidget(k->works);
    }

    if (contributions > 0) {
        k->contributions = tree();
        connect(k->contributions, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(updateSelection()));
        connect(k->contributions, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelection()));
        connect(k->contributions, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(execAccept(QTreeWidgetItem *, int)));
        QLabel *contribLabel = new QLabel(tr("My contributions:"));

        layout->addWidget(contribLabel);
        layout->addWidget(k->contributions);
    }
    
    //----
    QHBoxLayout *buttons = new QHBoxLayout;
    k->accept = new QPushButton(tr("OK"));
    k->accept->setDefault(true);
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

void KTListProjectDialog::updateSelection()
{
    if (k->works->hasFocus()) {
        k->contributions->clearSelection();
        int index = k->works->currentIndex().row(); 
        k->filename = k->workList.at(index);
    }

    if (k->contributions->hasFocus()) {
        k->works->clearSelection();
        int index = k->contributions->currentIndex().row();
        k->filename = k->contribList.at(index);
    }
}
