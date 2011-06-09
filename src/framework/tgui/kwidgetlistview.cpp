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

#include "kwidgetlistview.h"
#include "tdebug.h"

#include <QHeaderView>
#include <QResizeEvent>

KWidgetListView::KWidgetListView(QWidget * parent) : QTableWidget(0,1,parent)
{
    verticalHeader()->hide();
    horizontalHeader()->hide();
    horizontalHeader()->setResizeMode(QHeaderView::Custom);
}

KWidgetListView::~KWidgetListView()
{
}

QTableWidgetItem *KWidgetListView::addWidget(QWidget *widget)
{
    QTableWidgetItem *newItem = new QTableWidgetItem();

    int newRowIndex = rowCount();
    insertRow(newRowIndex);
    setItem(newRowIndex, 0, newItem);

    setIndexWidget(indexFromItem(newItem), widget);
    verticalHeader()->resizeSection(newRowIndex, widget->height());

    m_items.insert(widget, newItem);

    return newItem;
}

QTableWidgetItem *KWidgetListView::insertWidget(int pos, QWidget *widget)
{
    QTableWidgetItem *newItem = new QTableWidgetItem();

    insertRow(pos);
    setItem(pos, 0, newItem);

    setIndexWidget(indexFromItem(newItem), widget);
    verticalHeader()->resizeSection(pos, widget->height());

    m_items.insert(widget, newItem);

    return newItem;
}

QWidget *KWidgetListView::widget(QTableWidgetItem *treeItem)
{
    return indexWidget(indexFromItem(treeItem));
}

QTableWidgetItem *KWidgetListView::item(QWidget *widget)
{
    return m_items[widget];
}

void KWidgetListView::resizeEvent(QResizeEvent *e)
{
    horizontalHeader()->resizeSection(0, e->size().width());
}

void KWidgetListView::moveItemUp(int index)
{
    if (index > 0)
        verticalHeader()->moveSection(index, index-1);
}

void KWidgetListView::moveItemDown(int index)
{
    if (index < rowCount())
        verticalHeader()->moveSection(index, index+1);
}

int KWidgetListView::currentVisualRow() const
{
    return verticalHeader()->visualIndex(currentRow());
}
