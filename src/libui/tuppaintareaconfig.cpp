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

#include "tuppaintareaconfig.h"

#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

/**
 * This class handles the preferences dialog for the paint area settings.
 * Here is where the paint area parameters are set.
 * @author David Cuadrado
*/

struct TupPaintAreaConfig::Private
{
    TColorButton *gridColor;
    QSpinBox *gridSeparation;
};

TupPaintAreaConfig::TupPaintAreaConfig(QWidget *parent) : QWidget(parent),  k(new Private)
{
    setupPage();
}

TupPaintAreaConfig::~TupPaintAreaConfig()
{
}

void TupPaintAreaConfig::setupPage()
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
    // QString colorName = TCONFIG->value("GridColor", "#0000b4").toString();
    // QColor color = qvariant_cast<QColor>(TCONFIG->value("GridColor", QColor(0, 0, 180, 50)));
    QColor color(colorName);
    // int separation = TCONFIG->value("GridSeparation", 10).toInt();
    int separation = TCONFIG->value("GridSeparation").toInt();

    QGridLayout *gridForm = new QGridLayout;

    gridForm->addWidget(new QLabel(tr("Grid color:")), 0, 0, Qt::AlignLeft);
    k->gridColor = new TColorButton;
    k->gridColor->setColor(color);
    gridForm->addWidget(k->gridColor, 0, 1, Qt::AlignLeft);

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

void TupPaintAreaConfig::saveValues()
{
    TCONFIG->beginGroup("PaintArea");
    TCONFIG->setValue("GridColor", k->gridColor->color().name());
    TCONFIG->setValue("GridSeparation", k->gridSeparation->value());
    TCONFIG->sync();
}

/*
QColor TupPaintAreaConfig::gridColor() const
{
    return k->gridColor->color();
}

int TupPaintAreaConfig::gridSeparation() const
{
    return k->gridSeparation->value();
}
*/
