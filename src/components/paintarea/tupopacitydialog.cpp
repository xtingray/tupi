/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupopacitydialog.h"
#include "tdebug.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "tuppenthicknesswidget.h"
#include "timagebutton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <cmath>

struct TupOpacityDialog::Private
{
    QVBoxLayout *innerLayout;
    TupPenThicknessWidget *opacityPreview;
    QLabel *sizeLabel;
    QColor color;
    double currentOpacity;
};

TupOpacityDialog::TupOpacityDialog(const QColor &color, double opacity, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);

    k->color = color;
    k->currentOpacity = opacity;

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setOpacityCanvas();
    setButtonsPanel();

    TImageButton *closeButton = new TImageButton(QPixmap(THEME_DIR + "icons/close_big.png"), 60, this, true);
    closeButton->setToolTip(tr("Close"));
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    k->innerLayout->addWidget(new TSeparator());
    k->innerLayout->addWidget(buttonBox);

    layout->addLayout(k->innerLayout);
}

TupOpacityDialog::~TupOpacityDialog()
{
}

QSize TupOpacityDialog::sizeHint() const
{
    return QSize(500, 250);
}

void TupOpacityDialog::setOpacityCanvas()
{
    k->opacityPreview = new TupPenThicknessWidget(this);
    k->opacityPreview->setColor(k->color);
    k->opacityPreview->setBrush(Qt::SolidPattern);
    k->opacityPreview->render(k->currentOpacity);
    
    k->innerLayout->addWidget(k->opacityPreview);
}

void TupOpacityDialog::setButtonsPanel()
{
    TImageButton *minus5 = new TImageButton(QPixmap(THEME_DIR + "icons/minus_sign_big.png"), 40, this, true);
    minus5->setToolTip(tr("-0.05"));
    connect(minus5, SIGNAL(clicked()), this, SLOT(fivePointsLess()));

    TImageButton *minus = new TImageButton(QPixmap(THEME_DIR + "icons/minus_sign_medium.png"), 40, this, true);
    minus->setToolTip(tr("-0.01"));
    connect(minus, SIGNAL(clicked()), this, SLOT(onePointLess()));

    k->sizeLabel = new QLabel(QString::number(k->currentOpacity));
    k->sizeLabel->setFont(QFont("Arial", 24, QFont::Bold));
    k->sizeLabel->setFixedWidth(65);

    TImageButton *plus = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign_medium.png"), 40, this, true);
    plus->setToolTip(tr("+0.01"));
    connect(plus, SIGNAL(clicked()), this, SLOT(onePointMore()));

    TImageButton *plus5 = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign_big.png"), 40, this, true);
    plus5->setToolTip(tr("+0.05"));
    connect(plus5, SIGNAL(clicked()), this, SLOT(fivePointsMore()));

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(minus5);
    layout->addWidget(minus);
    layout->addWidget(k->sizeLabel);
    layout->addWidget(plus);
    layout->addWidget(plus5);

    k->innerLayout->addLayout(layout);
}

void TupOpacityDialog::fivePointsLess()
{
    modifySize(-0.05);
}

void TupOpacityDialog::onePointLess()
{
    modifySize(-0.01);
}

void TupOpacityDialog::onePointMore()
{
    modifySize(0.01);
}

void TupOpacityDialog::fivePointsMore()
{
    modifySize(0.05);
}

void TupOpacityDialog::modifySize(double value)
{
    k->currentOpacity += value;
    k->currentOpacity = floor(100 * k->currentOpacity) / 100;

    if (k->currentOpacity > 1)
        k->currentOpacity = 1;

    if (k->currentOpacity < 0)
        k->currentOpacity = 0;

    k->opacityPreview->render(k->currentOpacity);

    if (k->currentOpacity == 0) {
        k->sizeLabel->setText("0.00");
    } else if (k->currentOpacity == 1) {
        k->sizeLabel->setText("1.00");
    } else {
        k->sizeLabel->setText(QString::number(k->currentOpacity));
    }

    emit updateOpacity(k->currentOpacity);
}

