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

#include "zoomconfigurator.h"
#include <QBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

#include "kimagebutton.h"
#include "kseparator.h"

#include "kglobal.h"
#include "kdebug.h"
#include "kconfig.h"

ZoomConfigurator::ZoomConfigurator(QWidget *parent) :QWidget(parent)
{
    KINIT;

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    scale = new QLabel(tr("Scale Factor"));
    scale->setFont(QFont("Arial", 8, QFont::Normal, false));
    scale->setAlignment(Qt::AlignHCenter);
    layout->addWidget(scale);

    factor = new QDoubleSpinBox();

    factor->setDecimals(1);
    factor->setSingleStep(0.1);
    factor->setMinimum(0.1);
    factor->setMaximum(0.9);
    layout->addWidget(factor);

    QLabel *label = new QLabel(tr("Tips"));
    label->setAlignment(Qt::AlignHCenter);
    label->setFont(QFont("Arial", 8, QFont::Normal, false));

    QTextEdit *textArea = new QTextEdit; 

    textArea->setFont(QFont("Arial", 8, QFont::Normal, false));
    textArea->setHtml("<p><b>" + tr("Zoom Square mode") + ":</b> " + tr("Press Ctrl key + Mouse left button") + "</p>"); 

    QString text = textArea->document()->toPlainText();
    int height = (text.length()*270)/207;

    textArea->setFixedHeight(height);

    mainLayout->addLayout(layout);
    mainLayout->addWidget(label);
    mainLayout->addWidget(textArea);
    mainLayout->addStretch(2);

    KCONFIG->beginGroup("ZoomTool");
    double value = KCONFIG->value("zoomFactor", -1).toDouble();

    if (value > 0) 
        factor->setValue(value);
    else 
        factor->setValue(0.5);
}

ZoomConfigurator::~ZoomConfigurator()
{
    KEND;
}

double ZoomConfigurator::getFactor() const
{
    return factor->value();
}

