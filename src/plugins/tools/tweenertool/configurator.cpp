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

#include "kradiobuttongroup.h"
#include "kimagebutton.h"
#include "kdebug.h"
 
#include "configurator.h"
#include "ktitemtweener.h"
#include "stepsviewer.h"
#include "kttweenerstep.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QGraphicsPathItem>

struct Configurator::Private
{
    KRadioButtonGroup *options;
    StepsViewer *stepViewer;
    QListWidget *tweensList;
    QComboBox *combo;
    QLabel *totalLabel;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLineEdit *input = new QLineEdit(this);
    KImageButton *addButton = new KImageButton(QPixmap(THEME_DIR + "icons/plus_sign.png"), 22);
    addButton->setToolTip(tr("Create a new Tween"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(addTween()));
    // SQA: Temporary code
    addButton->setDisabled(true);

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->setMargin(0);
    lineLayout->setSpacing(0);
    lineLayout->addWidget(input);
    lineLayout->addWidget(addButton);

    k->tweensList = new QListWidget(this);
    k->tweensList->setViewMode(QListView::ListMode);
    k->tweensList->setFlow(QListView::TopToBottom);
    k->tweensList->setMovement(QListView::Static);

    QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
    tweenerItem->setFont(QFont("verdana", 8));
    tweenerItem->setText("tween00");
    tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    k->tweensList->setFixedHeight(68);
    k->tweensList->setCurrentRow(0);

    QLabel *startingLabel = new QLabel(tr("Starting at frame") + ":"); 
    startingLabel->setAlignment(Qt::AlignHCenter);
    k->combo = new QComboBox();
    k->combo->setMaximumWidth(50);
    k->combo->setEditable(false);

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(k->combo);

    k->options = new KRadioButtonGroup(tr("Options"), Qt::Vertical);
    k->options->addItem(tr("Select object"), 0);
    k->options->addItem(tr("Create path"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->totalLabel = new QLabel(tr("Frames Total") + ": 0");
    k->totalLabel->setAlignment(Qt::AlignHCenter);
    QHBoxLayout *totalLayout = new QHBoxLayout;
    totalLayout->setMargin(0);
    totalLayout->setSpacing(0);
    totalLayout->addWidget(k->totalLabel);

    QPushButton *reset = new QPushButton(tr("Reset Tween"));
    connect(reset, SIGNAL(clicked()), this, SIGNAL(clickedResetTweener()));

    QPushButton *applyButton = new QPushButton(tr("Apply"));
    connect(applyButton, SIGNAL(clicked()), this, SIGNAL(clickedApplyTweener()));

    k->stepViewer = new StepsViewer;
    k->stepViewer->verticalHeader()->hide();
    k->stepViewer->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignHCenter);
    layout->addLayout(lineLayout);
    layout->addWidget(k->tweensList);
    layout->addWidget(startingLabel);
    layout->addLayout(startLayout);
    layout->addWidget(k->options);
    layout->addWidget(k->stepViewer);
    layout->addLayout(totalLayout);
    layout->addWidget(reset);
    layout->addWidget(applyButton);
    setLayout(layout);
}

Configurator::~Configurator()
{
    delete k;
}

void Configurator::initCombo(int framesTotal, int currentIndex)
{
    k->combo->clear();
    for (int i=1; i<=framesTotal; i++)
         k->combo->addItem(QString::number(i));

    k->combo->setCurrentIndex(currentIndex);
}

void Configurator::setComboIndex(int currentIndex)
{
    k->combo->setCurrentIndex(currentIndex);
}

void Configurator::updateSteps(const QGraphicsPathItem *path)
{
    kFatal() << "Configurator::updateSteps() - Just tracing!";
    k->stepViewer->setPath(path);
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void Configurator::emitOptionChanged(int option)
{
    switch (option) {
            case 0:
             {
                 emit clickedSelect();
             }
            break;
            case 1:
             {
                 emit clickedCreatePath();
             }
    }
}

QString Configurator::tweenToXml(int currentFrame, QString path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("init", currentFrame);
    root.setAttribute("frames", k->stepViewer->totalSteps());
    root.setAttribute("coords", path);

    foreach (KTTweenerStep *step, k->stepViewer->steps())
             root.appendChild(step->toXml(doc));

    doc.appendChild(root);

    return doc.toString();
}

int Configurator::totalSteps()
{
    return k->stepViewer->totalSteps();
}

void Configurator::activatePathMode()
{
    k->options->setCurrentIndex(1);
    k->stepViewer->cleanRows();
}

void Configurator::activateSelectionMode()
{
    k->options->setCurrentIndex(0);
}

void Configurator::cleanData()
{
    k->stepViewer->cleanRows();
}

void Configurator::addTween()
{
    kFatal() << "Configurator::addTween() - Adding new Tween!";
}

QString Configurator::currentTweenName() const
{
    QListWidgetItem *item = k->tweensList->currentItem();
    if (item)
        return item->text();
    else
        return "";
}
