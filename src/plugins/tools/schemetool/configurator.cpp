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

    QTextEdit *textArea = new QTextEdit; 
    textArea->setFixedHeight(170);
    textArea->setHtml("<p>" + tr("This tool is just a <b>proof-of-concept</b> of the basic algorithm for the Tupi's free-tracing vectorial brushes") + "</p>"); 
    mainLayout->addWidget(textArea);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Parameters"));
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label);
    mainLayout->addLayout(layout);

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
    sizeBox->setSingleStep(50);
    sizeBox->setMinimum(0);
    sizeBox->setMaximum(500);
    sizeBox->setValue(50);
    sizeLayout->addWidget(sizeBox);

    connect(sizeBox, SIGNAL(valueChanged(int)), this, SIGNAL(updateSizeTolerance(int)));

    mainLayout->addLayout(sizeLayout);

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
    tError() << "Configurator::sizeToleranceValue() - Value: " << sizeBox->value();
    return sizeBox->value();
}
