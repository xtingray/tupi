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

#include "settings.h"
#include "timagebutton.h"
#include "tconfig.h"

Settings::Settings(QWidget *parent) : QWidget(parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[Settings()]";
        #else
            TINIT;
        #endif
    #endif

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Smoothness"));
    label->setAlignment(Qt::AlignHCenter); 
    layout->addWidget(label);
    m_exactness = new QDoubleSpinBox();

    m_exactness->setDecimals(2);
    m_exactness->setSingleStep(0.1);
    m_exactness->setMaximum(100);
    layout->addWidget(m_exactness);

    mainLayout->addLayout(layout);

    mainLayout->addStretch(2);

    TCONFIG->beginGroup("BrushTool");
    double smoothness = TCONFIG->value("Smoothness", -1).toDouble();

    if (smoothness > 0) 
        m_exactness->setValue(smoothness);
    else
        m_exactness->setValue(4.0);
}

Settings::~Settings()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~Settings()]";
        #else
            TEND;
        #endif
    #endif
}

double Settings::exactness() const
{
    return m_exactness->value();
}
