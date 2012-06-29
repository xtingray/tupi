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

#include "tuppendialog.h"
#include "tdebug.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "tuppenthicknesswidget.h"
#include "timagebutton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

struct TupPenDialog::Private
{
    QVBoxLayout *innerLayout;
    TupPenThicknessWidget *thickPreview;
    TupBrushManager *brushManager;
    QLabel *sizeLabel;
    int currentSize;
};

TupPenDialog::TupPenDialog(TupBrushManager *brushManager, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    // setWindowTitle(tr("Pen Size"));
    // setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/brush.png")));
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);

    k->brushManager = brushManager;
    k->currentSize = k->brushManager->penWidth();

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setBrushCanvas();
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

TupPenDialog::~TupPenDialog()
{
}

QSize TupPenDialog::sizeHint() const
{
    return QSize(500, 250);
}

void TupPenDialog::setBrushCanvas()
{
    k->thickPreview = new TupPenThicknessWidget(this);
    k->thickPreview->setColor(k->brushManager->penColor());
    k->thickPreview->setBrush(k->brushManager->brush().style());
    k->thickPreview->render(k->currentSize);
    
    k->innerLayout->addWidget(k->thickPreview);
}

void TupPenDialog::setButtonsPanel()
{
    TImageButton *minus5 = new TImageButton(QPixmap(THEME_DIR + "icons/minus_sign_big.png"), 40, this, true);
    minus5->setToolTip(tr("-5"));
    connect(minus5, SIGNAL(clicked()), this, SLOT(fivePointsLess()));

    TImageButton *minus = new TImageButton(QPixmap(THEME_DIR + "icons/minus_sign_medium.png"), 40, this, true);
    minus->setToolTip(tr("-1"));
    connect(minus, SIGNAL(clicked()), this, SLOT(onePointLess()));

    k->sizeLabel = new QLabel(QString::number(k->currentSize));
    k->sizeLabel->setFont(QFont("Arial", 24, QFont::Bold));
    k->sizeLabel->setFixedWidth(40);

    TImageButton *plus = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign_medium.png"), 40, this, true);
    plus->setToolTip(tr("+1"));
    connect(plus, SIGNAL(clicked()), this, SLOT(onePointMore()));

    TImageButton *plus5 = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign_big.png"), 40, this, true);
    plus5->setToolTip(tr("+5"));
    connect(plus5, SIGNAL(clicked()), this, SLOT(fivePointsMore()));

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(minus5);
    layout->addWidget(minus);
    layout->addWidget(k->sizeLabel);
    layout->addWidget(plus);
    layout->addWidget(plus5);

    k->innerLayout->addLayout(layout);
}

void TupPenDialog::fivePointsLess()
{
    modifySize(-5);
}

void TupPenDialog::onePointLess()
{
    modifySize(-1);
}

void TupPenDialog::onePointMore()
{
    modifySize(1);
}

void TupPenDialog::fivePointsMore()
{
    modifySize(5);
}

void TupPenDialog::modifySize(int value)
{
    k->currentSize += value;
    if (k->currentSize > 100)
        k->currentSize = 100;

    if (k->currentSize < 1)
        k->currentSize = 1;

    k->thickPreview->render(k->currentSize);
    k->sizeLabel->setText(QString::number(k->currentSize));

    emit updatePen(k->currentSize);
}

