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

#include "configurator.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QTextEdit>

Configurator::Configurator(QWidget *parent) :QWidget(parent)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    /*
    QTextEdit *textArea = new QTextEdit; 
    textArea->setFixedHeight(170);
    textArea->setHtml("<p>" + tr("This tool is just a <b>proof-of-concept</b> of the basic algorithm for the Tupi's free-tracing vectorial brushes") + "</p>"); 
    mainLayout->addWidget(textArea);
    */

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Parameters"));
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label);
    mainLayout->addLayout(layout);

    /*
    QBoxLayout *structureLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *structureLabel = new QLabel(tr("Structure"));
    structureLabel->setAlignment(Qt::AlignHCenter);
    structureLayout->addWidget(structureLabel);

    structureCombo = new QComboBox();
    structureCombo->addItem(tr("Basic"));
    structureCombo->addItem(tr("Axial"));
    structureCombo->addItem(tr("Organic"));
    structureCombo->setCurrentIndex(2);
    structureLayout->addWidget(structureCombo);

    mainLayout->addLayout(structureLayout);
    */

    QBoxLayout *spaceLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *spaceLabel = new QLabel(tr("Dot Spacing"));
    spaceLabel->setAlignment(Qt::AlignHCenter);
    spaceLayout->addWidget(spaceLabel);

    spacingBox = new QSpinBox();
    spacingBox->setSingleStep(1);
    spacingBox->setMinimum(1);
    spacingBox->setMaximum(10);
    spacingBox->setValue(2);
    spaceLayout->addWidget(spacingBox);

    connect(spacingBox, SIGNAL(valueChanged(int)), this, SIGNAL(updateSpacing(int)));

    mainLayout->addLayout(spaceLayout);

    QBoxLayout *sizeLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *sizeLabel = new QLabel(tr("Size Tolerance"));
    sizeLabel->setAlignment(Qt::AlignHCenter);
    sizeLayout->addWidget(sizeLabel);

    sizeBox = new QSpinBox();
    sizeBox->setSingleStep(10);
    sizeBox->setMinimum(0);
    sizeBox->setMaximum(200);
    sizeBox->setValue(50);
    sizeLayout->addWidget(sizeBox);

    connect(sizeBox, SIGNAL(valueChanged(int)), this, SIGNAL(updateSizeTolerance(int)));

    mainLayout->addLayout(sizeLayout);

    /*
    QBoxLayout *checkLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    checkBox = new QCheckBox(tr("Run simulation"));
    checkBox->setChecked(true);
    checkLayout->addWidget(checkBox);
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(updateInterface(int)));
    mainLayout->addLayout(checkLayout);
    */

    QBoxLayout *smoothLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *smoothLabel = new QLabel(tr("Smoothness"));
    smoothLabel->setAlignment(Qt::AlignHCenter);
    smoothLayout->addWidget(smoothLabel);
    smoothBox = new QDoubleSpinBox();

    smoothBox->setValue(2.0);
    smoothBox->setDecimals(2);
    smoothBox->setSingleStep(0.1);
    smoothBox->setMaximum(100);
    smoothLayout->addWidget(smoothBox);

    mainLayout->addLayout(smoothLayout);
    // smoothBox->setDisabled(true);

    mainLayout->addStretch(2);
}

Configurator::~Configurator()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

int Configurator::spacingValue()
{
    return spacingBox->value();
}

qreal Configurator::sizeToleranceValue()
{
    return sizeBox->value();
}

/*
bool Configurator::runSimulation()
{
    return checkBox->isChecked();
}
*/

/*
void Configurator::updateInterface(int state)
{ 
    if (state == 2)
        smoothBox->setDisabled(false);
    else
        smoothBox->setDisabled(true);
}
*/

double Configurator::smoothness() const
{
    return smoothBox->value();
}

/*
Configurator::Structure Configurator::structureType()
{
    int index = structureCombo->currentIndex();
    return Structure(index);
}
*/
