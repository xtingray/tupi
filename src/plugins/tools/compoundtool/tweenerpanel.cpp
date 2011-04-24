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

#include "tweenerpanel.h"
#include "kdebug.h"
#include "kradiobuttongroup.h"
#include "ktitemtweener.h"
#include "kttweenerstep.h"
#include "kimagebutton.h"
#include "kseparator.h"
#include "kosd.h"

#include <QLabel>
#include <QLineEdit>
#include <QBoxLayout>
#include <QComboBox>
#include <QCheckBox>

struct TweenerPanel::Private
{
    QWidget *innerPanel;
    QWidget *rangePanel;
    QWidget *clockPanel;

    QBoxLayout *layout;
    Mode mode;
    QLineEdit *input;
    KRadioButtonGroup *options;
    QComboBox *comboInit;
    QComboBox *comboEnd;

    QLabel *totalLabel;
    int totalSteps;

    KTItemTweener::TransformAxes scaleAxes;
    QComboBox *comboAxes;
    QComboBox *comboFactor;
    QComboBox *comboIterations;
    QCheckBox *loopBox;
    QCheckBox *reverseLoopBox;

    bool selectionDone;
    bool propertiesDone;

    KImageButton *apply;
    KImageButton *remove;
};

TweenerPanel::TweenerPanel(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->scaleAxes = KTItemTweener::XY;
    k->selectionDone = false;
    k->totalSteps = 0;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLabel *nameLabel = new QLabel(tr("Name") + ": ");
    k->input = new QLineEdit;

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    nameLayout->setMargin(0);
    nameLayout->setSpacing(0);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(k->input);

    k->options = new KRadioButtonGroup(tr("Options"), Qt::Vertical);
    k->options->addItem(tr("Select object"), 0);
    k->options->addItem(tr("Set Tweeners"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->apply = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    // connect(k->apply, SIGNAL(clicked()), this, SLOT(applyTween()));

    k->remove = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    // connect(k->remove, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->apply);
    buttonsLayout->addWidget(k->remove);

    k->layout->addLayout(nameLayout);
    k->layout->addWidget(k->options);

    k->layout->addSpacing(10);
    k->layout->addLayout(buttonsLayout);
    k->layout->setSpacing(5);
}

TweenerPanel::~TweenerPanel()
{
    delete k;
}
