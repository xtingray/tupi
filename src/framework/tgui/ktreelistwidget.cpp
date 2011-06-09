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

#include "ktreelistwidget.h"
#include "tdebug.h"

#include <QHeaderView>
#include <QItemDelegate>
#include <QEvent>
#include <QLineEdit>

class KTreeListWidgetDelegate : public QItemDelegate
{
    public:
        KTreeListWidgetDelegate(QObject * parent = 0);
        ~KTreeListWidgetDelegate();
};

KTreeListWidgetDelegate::KTreeListWidgetDelegate(QObject * parent) : QItemDelegate(parent)
{
}

KTreeListWidgetDelegate::~KTreeListWidgetDelegate()
{
}

//////////////////

KTreeListWidget::KTreeListWidget(QWidget *parent) : QTreeWidget(parent)
{
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
    setHeaderLabels(QStringList() << "");
    
    header()->hide();
    setUniformRowHeights (true);
    
    setAlternatingRowColors(true);
    
    KTreeListWidgetDelegate *delegator = new KTreeListWidgetDelegate(this);
    
    setItemDelegate(delegator);
    
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(editDoubleClickedItem(QTreeWidgetItem *, int)));
}

KTreeListWidget::~KTreeListWidget()
{
}

void KTreeListWidget::editDoubleClickedItem(QTreeWidgetItem *item, int col)
{
    if (item && m_isEditable) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        editItem(item, col);
    }
}

void KTreeListWidget::addItems(const QStringList &items)
{
    QStringList::ConstIterator it = items.begin();
    
    while (it != items.end()) {
           QTreeWidgetItem *item = new QTreeWidgetItem(this);
           item->setText(0, *it);
           ++it;
    }
}

QList<QTreeWidgetItem *> KTreeListWidget::topLevelItems()
{
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < topLevelItemCount (); i++)
         items << topLevelItem(i);
    
    return items;
}

void KTreeListWidget::setEditable(bool isEditable)
{
    m_isEditable = isEditable;
}

bool KTreeListWidget::isEditable() const
{
    return m_isEditable;
}

void KTreeListWidget::closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);

    if (edit) {
        QTreeWidgetItem *item = currentItem();
        if (item)
            emit itemRenamed(item);
    }

    QTreeWidget::closeEditor(editor, hint);
}

void KTreeListWidget::removeAll()
{
    clear();
}
