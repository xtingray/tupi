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

#include "tupcolorvalue.h"
#include "tdebug.h"
#include "tdoublecombobox.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QLineEdit>

#include <cmath>

struct TupItemColorValue::Private
{
    QSpinBox *value;

    /*
    ~Private()
     {
       delete value;
     };
    */
};

TupItemColorValue::TupItemColorValue(const QString &text, QWidget *parent) : QFrame(parent), k(new Private)
{
    QHBoxLayout * layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    setLayout(layout);
    QLabel *labelText = new QLabel(text, this);
    labelText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    k->value = new QSpinBox(this);
    k->value->setMaximum(255);
    k->value->setMinimum(0);
    k->value->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(k->value, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
    connect(k->value, SIGNAL(valueChanged(const QString &)), this, SIGNAL(valueChanged(const QString &)));
    connect(k->value, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
    layout->addWidget(labelText);
    layout->addWidget(k->value);
}

TupItemColorValue::~TupItemColorValue()
{
    delete k;
}

void TupItemColorValue::setValue(int val)
{
    k->value->setValue(val);
}

int  TupItemColorValue::value()
{
    return k->value->value();
}

void TupItemColorValue::setMax(int max)
{
    k->value->setMaximum(max);
}

void TupItemColorValue::setRange(int minimum, int maximum)
{
    k->value->setRange(minimum, maximum);
}

void TupItemColorValue::setSuffix(const QString &suffix)
{
    k->value->setSuffix(suffix);
}

struct TupColorValue::Private
{
    TupItemColorValue *valueR; 
    TupItemColorValue *valueG;
    TupItemColorValue *valueB;
    TupItemColorValue *valueH;
    TupItemColorValue *valueS;
    TupItemColorValue *valueV; 

    TDoubleComboBox *valueA;
    bool ok;
};

TupColorValue::TupColorValue(QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->ok = true;

    setLayout(new QVBoxLayout());
    setupForm();
}

TupColorValue::~TupColorValue()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupColorValue::setupForm()
{
    QGridLayout *gridLayout = new QGridLayout;
    k->valueR = new TupItemColorValue("R");
    connect(k->valueR, SIGNAL(editingFinished()), this, SLOT(syncValuesRgb()));

    k->valueG = new TupItemColorValue("G", this);
    connect(k->valueG, SIGNAL(editingFinished()), this, SLOT(syncValuesRgb()));

    k->valueB = new TupItemColorValue("B", this);
    connect(k->valueB, SIGNAL(editingFinished()), this, SLOT(syncValuesRgb()));

    k->valueH = new TupItemColorValue("H", this);
    k->valueH->setMax(359);
    connect(k->valueH, SIGNAL(valueChanged(int)), this, SIGNAL(hueChanged(int)));

    k->valueS = new TupItemColorValue("S", this);
    connect(k->valueS, SIGNAL(valueChanged(int)), this, SIGNAL(saturationChanged( int)));

    k->valueV = new TupItemColorValue("V", this);
    connect(k->valueV, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged( int)));

    QHBoxLayout *boxLayout = new QHBoxLayout;
    boxLayout->setSpacing(1);

    k->valueA = new TDoubleComboBox(0, 255, this);
    k->valueA->setDecimals(0);

    QLabel * label = new QLabel("A");
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    boxLayout->addWidget(label);

    boxLayout->addWidget(k->valueA);

    for (int i = 0; i <= 100; i += 25)
         k->valueA->addPercent(i);

    connect(k->valueA, SIGNAL(editingFinished()), this, SLOT(syncValuesRgb()));
    connect(k->valueA, SIGNAL(activated(int)), this, SLOT(syncValuesRgb(int)));

    QCheckBox *show = new QCheckBox(tr("Percent"));
    show->setChecked(k->valueA->showAsPercent());

    gridLayout->addWidget(k->valueR, 0, 0,Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueG, 1, 0,Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueB, 2, 0,Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueH, 0, 1,Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueS, 1, 1,Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueV, 2, 1,Qt::AlignTop | Qt::AlignLeft);

    boxLayout->addSpacing(10);
    boxLayout->addWidget(show);

    static_cast<QHBoxLayout*>(layout())->addLayout(gridLayout);
    static_cast<QHBoxLayout*>(layout())->addLayout(boxLayout);

    connect(show, SIGNAL(toggled(bool)), k->valueA, SLOT(setShowAsPercent(bool)));
}

void TupColorValue::setColor(const QBrush &brush)
{
    QColor color = brush.color();
    k->ok = false;
    k->valueR->setValue(color.red());
    k->valueG->setValue(color.green());
    k->valueB->setValue(color.blue());
    k->valueH->setValue(color.hue());
    k->valueS->setValue(color.saturation());
    k->valueV->setValue(color.value());

    k->valueA->setValue(k->valueA->currentIndex(), color.alpha());

    k->ok = true;
}

void TupColorValue::syncValuesRgb(int)
{
    if (k->ok) {
        int r = k->valueR->value();
        int g = k->valueG->value();
        int b = k->valueB->value();
        int a = (int) ::ceil(k->valueA->value());

        QColor tmp = QColor::fromRgb(r, g, b, a);
        k->valueH->setValue(tmp.hue());
        k->valueS->setValue(tmp.saturation());
        k->valueV->setValue(tmp.value());

        emit brushChanged(QColor::fromRgb(r, g, b, a));
    }
}

int TupColorValue::hue()
{
    return k->valueH->value();
}

int TupColorValue::saturation()
{
    return k->valueS->value();
}

int TupColorValue::value()
{
    return k->valueV->value();
}

int TupColorValue::alpha()
{
    return (int) ::ceil(k->valueA->value());
}
