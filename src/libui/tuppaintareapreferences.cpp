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

#include "tuppaintareapreferences.h"

#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

/**
 * This class handles the preferences dialog for the paint area settings.
 * Here is where the paint area parameters are set.
 * @author David Cuadrado
*/

struct TupPaintAreaPreferences::Private
{
    QColor gridColor; 
    QPushButton *gridColorButton;
    QSpinBox *gridSeparation;
};

TupPaintAreaPreferences::TupPaintAreaPreferences(QWidget *parent) : QWidget(parent),  k(new Private)
{
    setupPage();
}

TupPaintAreaPreferences::~TupPaintAreaPreferences()
{
}

void TupPaintAreaPreferences::setupPage()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QWidget *widget = new QWidget;
    QVBoxLayout *pageLayout = new QVBoxLayout;

    QLabel *generalLabel = new QLabel(tr("Workspace Preferences"));
    QFont labelFont = font();
    labelFont.setBold(true);
    labelFont.setPointSize(labelFont.pointSize() + 3);
    generalLabel->setFont(labelFont);
    pageLayout->addWidget(generalLabel);
    pageLayout->addSpacing(15);

    QLabel *startupLabel = new QLabel(tr("Grid Settings"));
    labelFont = font();
    labelFont.setBold(true);
    startupLabel->setFont(labelFont);
    pageLayout->addWidget(startupLabel);

    TCONFIG->beginGroup("PaintArea");
    QString colorName = TCONFIG->value("GridColor").toString();
    k->gridColor = QColor(colorName);
    int separation = TCONFIG->value("GridSeparation").toInt();

    QGridLayout *gridForm = new QGridLayout;

    gridForm->addWidget(new QLabel(tr("Grid color:")), 0, 0, Qt::AlignLeft);
    k->gridColorButton = new QPushButton;
    k->gridColorButton->setText(k->gridColor.name());
    k->gridColorButton->setStyleSheet("* { background-color: " + k->gridColor.name() + " }");
    connect(k->gridColorButton, SIGNAL(clicked()), this, SLOT(setGridColor()));
    gridForm->addWidget(k->gridColorButton, 0, 1, Qt::AlignLeft);

    gridForm->addWidget(new QLabel(tr("Grid separation:")), 1, 0, Qt::AlignLeft);
    k->gridSeparation = new QSpinBox(this);
    k->gridSeparation->setMinimum(5);
    k->gridSeparation->setMaximum(30);
    k->gridSeparation->setValue(separation);
    gridForm->addWidget(k->gridSeparation, 1, 1, Qt::AlignLeft);

    pageLayout->addLayout(gridForm);

    widget->setLayout(pageLayout);
    layout->addWidget(widget);
    layout->setAlignment(widget, Qt::AlignLeft);
    layout->addStretch(3);
}

void TupPaintAreaPreferences::saveValues()
{
    TCONFIG->beginGroup("PaintArea");
    TCONFIG->setValue("GridColor", k->gridColor.name());
    TCONFIG->setValue("GridSeparation", k->gridSeparation->value());
    TCONFIG->sync();
}

void TupPaintAreaPreferences::setGridColor()
{
     k->gridColor = QColorDialog::getColor(k->gridColor, this);

     if (k->gridColor.isValid()) {
         k->gridColorButton->setText(k->gridColor.name());
         k->gridColorButton->setStyleSheet("QPushButton { background-color: " + k->gridColor.name() + " }");
     } else {
         k->gridColor = QColor("#fff");
         k->gridColorButton->setText(tr("White"));
         k->gridColorButton->setStyleSheet("QPushButton { background-color: #fff }");
     }
}
