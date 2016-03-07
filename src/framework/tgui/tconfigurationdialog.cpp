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

#include "tconfigurationdialog.h"

////////////////

struct TConfigurationDialog::Private
{
    QListWidget *list;
    QStackedWidget *pageArea;
};

TConfigurationDialog::TConfigurationDialog(QWidget *parent) : QDialog(parent), k(new Private)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *pagesLayout = new QHBoxLayout;
    
    k->list = new QListWidget(this);
    k->list->setFlow(QListView::TopToBottom);
    k->list->setWrapping(false);
    k->list->setViewMode(QListView::IconMode);
    k->list->setIconSize(QSize(96, 84));
    k->list->setMovement(QListView::Static);
    k->list->setSpacing(10);
    
    connect(k->list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));

    QWidget *widget = new QWidget;
    widget->setFixedWidth(130);
    QVBoxLayout *listLayout = new QVBoxLayout(widget);
    listLayout->addWidget(k->list);

    k->pageArea = new QStackedWidget;
    pagesLayout->addWidget(widget);
    pagesLayout->addWidget(k->pageArea, 1);

    mainLayout->addLayout(pagesLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    
    mainLayout->addWidget(new TSeparator());
    mainLayout->addWidget(buttonBox);
}

TConfigurationDialog::~TConfigurationDialog()
{
}

void TConfigurationDialog::addPage(QWidget *page, const QString &label, const QIcon &icon)
{
    QListWidgetItem *pageItem = new QListWidgetItem(k->list);
    pageItem->setIcon(icon);
    pageItem->setText(label);
    pageItem->setTextAlignment(Qt::AlignHCenter);
    pageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    
    k->pageArea->addWidget(page);
}

QWidget *TConfigurationDialog::currentPage() const
{
    return k->pageArea->currentWidget();
}

void TConfigurationDialog::apply()
{
}

void TConfigurationDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;
    
    k->pageArea->setCurrentIndex(k->list->row(current));
}

void TConfigurationDialog::setCurrentItem(int row)
{
   k->list->setCurrentRow(row);
}
