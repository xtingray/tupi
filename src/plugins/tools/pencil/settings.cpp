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
#include "tseparator.h"

Settings::Settings(QWidget *parent) : QWidget(parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[Settings()]";
        #else
            TINIT;
        #endif
    #endif

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    QLabel *toolTitle = new QLabel;
    toolTitle->setAlignment(Qt::AlignHCenter);
    QPixmap pic(THEME_DIR + "icons/pencil.png"); 
    toolTitle->setPixmap(pic.scaledToWidth(16, Qt::SmoothTransformation));
    toolTitle->setToolTip(tr("Pencil Properties"));
    layout->addWidget(toolTitle);
    layout->addWidget(new TSeparator(Qt::Horizontal));

    QLabel *label = new QLabel(tr("Smoothness"));
    label->setAlignment(Qt::AlignHCenter); 
    layout->addWidget(label);
    m_smoothness = new QDoubleSpinBox();

    m_smoothness->setDecimals(2);
    m_smoothness->setSingleStep(0.1);
    m_smoothness->setMinimum(0);
    m_smoothness->setMaximum(20);
    layout->addWidget(m_smoothness);

    mainLayout->addLayout(layout);

    mainLayout->addStretch(2);

    TCONFIG->beginGroup("PencilTool");
    double smoothness = TCONFIG->value("Smoothness", 4.0).toDouble();
    m_smoothness->setValue(smoothness);
}

Settings::~Settings()
{
}

double Settings::smoothness() const
{
    return m_smoothness->value();
}
