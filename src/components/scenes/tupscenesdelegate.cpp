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

#include "tupscenesdelegate.h"
#include "tdebug.h"

#include <QtGui>

TupScenesDelegate::TupScenesDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

TupScenesDelegate::~TupScenesDelegate()
{
}

void TupScenesDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
}

QWidget *TupScenesDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    Q_UNUSED(option);

    QVariant originalValue = index.model()->data(index, Qt::DisplayRole);
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(false);

    return lineEdit;
}

void TupScenesDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::DisplayRole);
    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor))
        lineEdit->setText(value.toString());
}

void TupScenesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit->isModified())
        return;

    QString text = lineEdit->text();
    if (text.length() == 0)
        return;

    const QValidator *validator = lineEdit->validator();
    if (validator) {
        int pos;
        if (validator->validate(text, pos) != QValidator::Acceptable)
            return;
    }

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
    QVariant value = text;

    model->setData(index, value.toString(), Qt::DisplayRole);
    model->setData(index, value, Qt::UserRole);
}
