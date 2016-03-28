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

#include "tupcolorform.h"
#include "tupformitem.h"
#include "tseparator.h"
#include "tdoublecombobox.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
// #include <QSpinBox>
#include <QLineEdit>
#include <QSlider>
#include <cmath>

struct TupColorForm::Private
{
    TupFormItem *valueR; 
    TupFormItem *valueG;
    TupFormItem *valueB;
    TupFormItem *valueH;
    TupFormItem *valueS;
    TupFormItem *valueV; 

    QLabel *alphaCounter;
    QSlider *alphaSlider;
};

TupColorForm::TupColorForm(QWidget *parent) : QWidget(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupColorForm()]";
        #else
            TINIT;
        #endif
    #endif

    setupForm();
}

TupColorForm::~TupColorForm()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupColorForm()]";
        #else
            TEND;
        #endif
    #endif
}

void TupColorForm::setupForm()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QGridLayout *gridLayout = new QGridLayout;
    k->valueR = new TupFormItem("R");
    connect(k->valueR, SIGNAL(editingFinished()), this, SLOT(syncRgbValues()));

    k->valueG = new TupFormItem("G");
    connect(k->valueG, SIGNAL(editingFinished()), this, SLOT(syncRgbValues()));

    k->valueB = new TupFormItem("B");
    connect(k->valueB, SIGNAL(editingFinished()), this, SLOT(syncRgbValues()));

    k->valueH = new TupFormItem("H");
    k->valueH->setMax(359);
    connect(k->valueH, SIGNAL(editingFinished()), this, SLOT(syncHsvValues()));

    k->valueS = new TupFormItem("S");
    connect(k->valueS, SIGNAL(editingFinished()), this, SLOT(syncHsvValues()));

    k->valueV = new TupFormItem("V");
    connect(k->valueV, SIGNAL(editingFinished()), this, SLOT(syncHsvValues()));

    gridLayout->addWidget(k->valueR, 0, 0, Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueG, 1, 0, Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueB, 2, 0, Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueH, 0, 1, Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueS, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(k->valueV, 2, 1, Qt::AlignTop | Qt::AlignLeft);

    QLabel *alphaLabel = new QLabel(tr("Alpha (Transparency)"));
    alphaLabel->setAlignment(Qt::AlignHCenter);

    k->alphaSlider = new QSlider(Qt::Horizontal); 
    k->alphaSlider->setMinimum(0);
    k->alphaSlider->setMaximum(255);
    k->alphaSlider->setSingleStep(1);
    k->alphaSlider->setValue(255);
    connect(k->alphaSlider, SIGNAL(valueChanged(int)), this, SLOT(updateAlphaValue(int)));

    k->alphaCounter = new QLabel("255");
    k->alphaCounter->setAlignment(Qt::AlignHCenter);

    layout->addLayout(gridLayout);
    layout->addWidget(new TSeparator(Qt::Horizontal));
    layout->addWidget(alphaLabel);
    layout->addWidget(k->alphaSlider);
    layout->addWidget(k->alphaCounter);
}

void TupColorForm::setColor(const QBrush &brush)
{
    QColor color = brush.color();

    blockSignals(true);
    k->valueR->setValue(color.red());
    k->valueG->setValue(color.green());
    k->valueB->setValue(color.blue());
    k->valueH->setValue(color.hue());
    k->valueS->setValue(color.saturation());
    k->valueV->setValue(color.value());

    k->alphaCounter->setText(QString::number(color.alpha()));
    k->alphaSlider->setValue(color.alpha());
    blockSignals(false);
}

void TupColorForm::syncRgbValues()
{
    int r = k->valueR->value();
    int g = k->valueG->value();
    int b = k->valueB->value();
    int a = k->alphaSlider->value();

    QColor color = QColor::fromRgb(r, g, b, a);
    blockSignals(true);
    k->valueH->setValue(color.hue());
    k->valueS->setValue(color.saturation());
    k->valueV->setValue(color.value());
    blockSignals(false);

    emit brushChanged(color);
}

void TupColorForm::syncHsvValues()
{
    int h = k->valueH->value();
    int s = k->valueS->value();
    int v = k->valueV->value();
    int a = k->alphaSlider->value();

    QColor color = QColor::fromHsv(h, s, v, a);
    blockSignals(true);
    k->valueR->setValue(color.red());
    k->valueG->setValue(color.green());
    k->valueB->setValue(color.blue());
    blockSignals(false);

    emit brushChanged(color);
}

void TupColorForm::updateAlphaValue(int alpha)
{
    k->alphaCounter->setText(QString::number(alpha));
    syncRgbValues();
}
