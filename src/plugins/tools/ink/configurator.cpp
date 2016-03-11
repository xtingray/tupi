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

#include "configurator.h"
#include "tconfig.h"

#include <QLabel>
#include <QBoxLayout>

Configurator::Configurator(QWidget *parent) :QWidget(parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Configurator()]";
        #else
            TINIT;
        #endif
    #endif

    QFont titleFont = font(); 
    titleFont.setBold(true);

    TCONFIG->beginGroup("InkTool");
    double spacing = TCONFIG->value("DotsSpacing", 5).toInt();
    double tolerance = TCONFIG->value("Tolerance", 5).toInt();
    double smoothness = TCONFIG->value("Smoothness", 4.0).toDouble();
    bool border = TCONFIG->value("ShowBorder", true).toBool();
    bool borderSize = TCONFIG->value("BorderSize", 1).toInt();

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QBoxLayout *borderLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *borderLabel = new QLabel(tr("Border"));
    borderLabel->setFont(titleFont);
    borderLabel->setAlignment(Qt::AlignHCenter);
    borderLayout->addWidget(borderLabel);
    mainLayout->addLayout(borderLayout);

    borderOption = new QCheckBox(tr("Show border line"));
    borderOption->setChecked(border);
    mainLayout->addWidget(borderOption);

    QBoxLayout *borderSizeLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *borderSizeLabel = new QLabel(tr("Border Size"));
    borderSizeLabel->setAlignment(Qt::AlignHCenter);
    borderSizeLayout->addWidget(borderSizeLabel);

    borderSizeBox = new QSpinBox();
    borderSizeBox->setSingleStep(1);
    borderSizeBox->setMinimum(1);
    borderSizeBox->setMaximum(10);
    borderSizeBox->setValue(borderSize);
    borderSizeLayout->addWidget(borderSizeBox);

    mainLayout->addLayout(borderSizeLayout);

    QBoxLayout *paramsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *paramsLabel = new QLabel(tr("Parameters"));
    paramsLabel->setFont(titleFont);
    paramsLabel->setAlignment(Qt::AlignHCenter);
    paramsLayout->addWidget(paramsLabel);
    mainLayout->addLayout(paramsLayout);

    QBoxLayout *spaceLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *spaceLabel = new QLabel(tr("Dot Spacing"));
    spaceLabel->setAlignment(Qt::AlignHCenter);
    spaceLayout->addWidget(spaceLabel);

    spacingBox = new QSpinBox();
    spacingBox->setSingleStep(1);
    spacingBox->setMinimum(1);
    spacingBox->setMaximum(10);
    spacingBox->setValue(spacing);
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
    sizeBox->setValue(tolerance);
    sizeLayout->addWidget(sizeBox);

    connect(sizeBox, SIGNAL(valueChanged(int)), this, SIGNAL(updateSizeTolerance(int)));

    mainLayout->addLayout(sizeLayout);

    QBoxLayout *smoothLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *smoothLabel = new QLabel(tr("Smoothness"));
    smoothLabel->setAlignment(Qt::AlignHCenter);
    smoothLayout->addWidget(smoothLabel);
    smoothBox = new QDoubleSpinBox();

    smoothBox->setValue(smoothness);
    smoothBox->setDecimals(2);
    smoothBox->setSingleStep(0.1);
    smoothBox->setMinimum(0);
    smoothBox->setMaximum(20);
    smoothLayout->addWidget(smoothBox);

    mainLayout->addLayout(smoothLayout);
    mainLayout->addStretch(2);
}

Configurator::~Configurator()
{
}

int Configurator::spacingValue()
{
    return spacingBox->value();
}

qreal Configurator::sizeToleranceValue()
{
    return sizeBox->value();
}

double Configurator::smoothness() const
{
    return smoothBox->value();
}

bool Configurator::showBorder()
{
    return borderOption->isChecked();
}

int Configurator::borderSizeValue()
{
    return borderSizeBox->value();
}
