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

#include "kconfigurationdialog.h"
#include "kseparator.h"
#include "kdebug.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QListWidget>

////////////////

struct KConfigurationDialog::Private
{
    QListWidget *list;
    QStackedWidget *pageArea;
};

KConfigurationDialog::KConfigurationDialog(QWidget *parent) : QDialog(parent), k(new Private)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QHBoxLayout *pages = new QHBoxLayout;
    
    k->list = new QListWidget(this);
    k->list->setViewMode(QListView::IconMode);
    k->list->setWrapping(false);
    k->list->setFlow(QListView::TopToBottom);
    k->list->setIconSize(QSize(96, 84));
    k->list->setMovement(QListView::Static);
    k->list->setMaximumWidth(128);
    //k->list->setMinimumWidth(95);
    k->list->setSpacing(12);
    
    connect(k->list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
    
    pages->addWidget(k->list);
    
    k->pageArea = new QStackedWidget;
    pages->addWidget(k->pageArea, 1);
    
    layout->addLayout(pages);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal, this);
    
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    
    layout->addWidget(new KSeparator());
    layout->addWidget(buttonBox);
}

KConfigurationDialog::~KConfigurationDialog()
{
    delete k;
}

void KConfigurationDialog::addPage(QWidget *page, const QString &label, const QIcon &icon)
{
    QListWidgetItem *pageItem = new QListWidgetItem(k->list);
    pageItem->setIcon(icon);
    pageItem->setText(label);
    pageItem->setTextAlignment(Qt::AlignHCenter);
    pageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    
    k->pageArea->addWidget(page);
}

QWidget *KConfigurationDialog::currentPage() const
{
    return k->pageArea->currentWidget();
}

void KConfigurationDialog::ok()
{
    accept();
}

void KConfigurationDialog::cancel()
{
    reject();
}

void KConfigurationDialog::apply()
{
}

void KConfigurationDialog::changePage(QListWidgetItem *curr, QListWidgetItem *prev)
{
    if (!curr)
        curr = prev;
    
    k->pageArea->setCurrentIndex(k->list->row(curr));
}

void KConfigurationDialog::setCurrentItem(int row)
{
   k->list->setCurrentRow(row);
}
