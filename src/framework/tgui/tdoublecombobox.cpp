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

#include "tdoublecombobox.h"

#define PERCENT(v) (v * 100.0) / maximum()
#define VALUE(p) (p * maximum()) / 100.0

struct TDoubleComboBox::Private
{
    bool showAsPercent;
    QDoubleValidator *validator;
    QLineEdit *editor;
};

TDoubleComboBox::TDoubleComboBox(double min, double max, QWidget *parent)
 : QComboBox(parent), k(new Private)
{
    
    k->validator = new QDoubleValidator(this);
    k->editor = new QLineEdit;
    k->editor->setValidator(k->validator);
    setLineEdit(k->editor);
    
    setValidator(k->validator);
    setMinimum(min);
    setMaximum(max);

    setDuplicatesEnabled(false);
    setInsertPolicy(QComboBox::InsertAlphabetically);
    
    connect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
    connect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCurrentIndexChanged(int)));
    
    connect(k->editor, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    connect(k->editor, SIGNAL(returnPressed()), this, SIGNAL(editingFinished()));
    
    k->showAsPercent = false;
    
    setDecimals(2);
}

TDoubleComboBox::~TDoubleComboBox()
{
    delete k;
}

void TDoubleComboBox::setShowAsPercent(bool p)
{
    if (k->showAsPercent == p) 
        return;
    
    k->showAsPercent = p;
    
    for (int index = 0; index < count(); index++) {
         if (k->showAsPercent) {
             setItemText(index, QString::number(PERCENT(itemText(index).toDouble())));
         } else {
             double value = VALUE(itemText(index).toDouble());
             setItemText(index, QString::number(value));
        }
    }
}

bool TDoubleComboBox::showAsPercent() const
{
    return k->showAsPercent;
}

void TDoubleComboBox::setDecimals(int n)
{
    k->validator->setDecimals(n);
}

int TDoubleComboBox::decimals() const
{
    return k->validator->decimals();
}

void TDoubleComboBox::setMaximum(double max)
{
    k->validator->setTop(max);
}

void TDoubleComboBox::setMinimum(double min)
{
    k->validator->setBottom(min);
}

double TDoubleComboBox::maximum() const
{
    return k->validator->top();
}

double TDoubleComboBox::minimum() const
{
    return k->validator->bottom();
}

void TDoubleComboBox::addValue(double v)
{
    if (minimum() < v && v < maximum()) {
        if (k->showAsPercent)
            addItem(QString::number(PERCENT(v)));
        else
            addItem(QString::number(v));
    }
}

void TDoubleComboBox::addPercent(double p)
{
    if (p >= 0 && p <= 100) {
        if (k->showAsPercent)
            addItem(QString::number(p));
        else
            addItem(QString::number(VALUE(p)));
    }
}

void TDoubleComboBox::emitHighlighted(int index)
{
    emit highlighted(itemText(index).toDouble());
}

void TDoubleComboBox::emitActivated(int index)
{
    emit activated(itemText(index).toDouble());
}

void TDoubleComboBox::emitCurrentIndexChanged(int index)
{
    emit currentIndexChanged(itemText(index).toDouble());
}

double TDoubleComboBox::value()
{
    if (k->showAsPercent)
        return (VALUE(currentText().toDouble()));
    
    return currentText().toDouble();
}

void TDoubleComboBox::setValue(int index, double v)
{
    if (k->showAsPercent)
        setItemText(index, QString::number(PERCENT(v)));
    else
        setItemText(index, QString::number(v));
}

void TDoubleComboBox::setPercent(int index, double p)
{
    if (k->showAsPercent)
        setItemText(index, QString::number(p));
    else
        setItemText(index, QString::number(VALUE(p)));
}

double TDoubleComboBox::percent()
{
    if (k->showAsPercent)
        return currentText().toDouble();
    
    return PERCENT(currentText().toDouble());
}
