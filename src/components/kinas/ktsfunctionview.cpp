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

#include "ktsfunctionview.h"
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QItemDelegate>

KTSFunctionView::KTSFunctionView(QWidget *parent) : QTreeWidget(parent)
{
    setItemDelegate(new QItemDelegate(this));
    setColumnCount(1);
    setHeaderLabels(QStringList() << "Members");
    setRootIsDecorated (true);

    header()->setResizeMode(QHeaderView::Stretch);

    setEditTriggers(QAbstractItemView::AnyKeyPressed);

    addModule("A Module", "A section", QStringList() << "Function1" << "Function2" << "Function3" << "Function4" );

    setDragEnabled ( true);
}

KTSFunctionView::~KTSFunctionView()
{
}

void KTSFunctionView::addModule(const QString &module, const QString &section, QStringList functions)
{
    // TODO: mirar si los modulos y secciones existen, sino crearlos

    QTreeWidgetItem *itemModule = new QTreeWidgetItem(this);
    itemModule->setText(0, module);

    QTreeWidgetItem *itemSection = new QTreeWidgetItem(itemModule);
    itemSection->setText(0, section);

    QStringListIterator iterator(functions);

    while (iterator.hasNext()) {
           QTreeWidgetItem *itemFunction = new QTreeWidgetItem(itemSection);
           itemFunction->setText(0, iterator.next());
    }
}

bool KTSFunctionView::dropMimeData ( QTreeWidgetItem * parent, int index, const QMimeData * data, Qt::DropAction action )
{
    return true;
}
