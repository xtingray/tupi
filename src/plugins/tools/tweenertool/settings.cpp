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

#include "settings.h"
#include "kradiobuttongroup.h"
#include "kimagebutton.h"
#include "kdebug.h"
#include "ktitemtweener.h"
#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "kosd.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QGraphicsPathItem>

struct Settings::Private
{
    QBoxLayout *layout; 
    KRadioButtonGroup *options;
    StepsViewer *stepViewer;
    QListWidget *tweensList;
    QComboBox *combo;
    QLabel *totalLabel;
    bool selectionDone;
    Mode mode; 
};

Settings::Settings(QWidget *parent, Mode mode, int framesTotal, int startFrame) : QWidget(parent), k(new Private)
{
    k->mode = mode;
    k->selectionDone = false;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLabel *startingLabel = new QLabel(tr("Starting at frame") + ":"); 
    startingLabel->setAlignment(Qt::AlignHCenter);
    k->combo = new QComboBox();
    k->combo->setMaximumWidth(50);
    k->combo->setEditable(false);
    initStartCombo(framesTotal, startFrame);

    if (k->mode == Settings::Add)
        k->combo->setEnabled(false);

    connect(k->combo, SIGNAL(currentIndexChanged(int)), this, SIGNAL(startingPointChanged(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(k->combo);

    k->options = new KRadioButtonGroup(tr("Options"), Qt::Vertical);
    k->options->addItem(tr("Select object"), 0);
    k->options->addItem(tr("Create path"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->totalLabel = new QLabel(tr("Frames Total") + ": 0");
    k->totalLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    QHBoxLayout *totalLayout = new QHBoxLayout;
    totalLayout->setMargin(0);
    totalLayout->setSpacing(0);
    totalLayout->addWidget(k->totalLabel);

    KImageButton *apply = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    if (k->mode == Settings::Add)
        apply->setToolTip(tr("Save Tween"));
    else
        apply->setToolTip(tr("Update Tween"));
    connect(apply, SIGNAL(clicked()), this, SIGNAL(clickedApplyTween()));

    KImageButton *remove = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    remove->setToolTip(tr("Cancel Operation"));
    connect(remove, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(apply);
    //buttonsLayout->addWidget(reset);
    buttonsLayout->addWidget(remove);

    k->stepViewer = new StepsViewer;
    k->stepViewer->verticalHeader()->hide();
    k->stepViewer->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    k->layout->addWidget(startingLabel);
    k->layout->addLayout(startLayout);

    k->layout->addWidget(k->options);
    k->layout->addWidget(k->stepViewer);

    k->layout->addLayout(totalLayout);

    k->layout->addSpacing(10);

    k->layout->addLayout(buttonsLayout);

    k->layout->setSpacing(5);

    activateSelectionMode();
}

Settings::~Settings()
{
    delete k;
}

void Settings::initStartCombo(int framesTotal, int currentIndex)
{
    k->combo->clear();
    for (int i=1; i<=framesTotal; i++)
         k->combo->addItem(QString::number(i));

    k->combo->setCurrentIndex(currentIndex);
}

void Settings::setStartFrame(int currentIndex)
{
    k->combo->setCurrentIndex(currentIndex);
}

int Settings::startFrame()
{
    return k->combo->currentIndex();
}

int Settings::startComboSize()
{
    return k->combo->count();
}

void Settings::updateSteps(const QGraphicsPathItem *path)
{
    k->stepViewer->setPath(path);
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void Settings::emitOptionChanged(int option)
{
    switch (option) {
            case 0:
             {
                 emit clickedSelect();
             }
            break;
            case 1:
             {
                 if (k->selectionDone) {
                     emit clickedCreatePath();
                 } else {
                     k->options->setCurrentIndex(0);
                     KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);   
                 }
             }
    }
}

QString Settings::tweenToXml(int currentFrame, QString path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", "Tween00");
    root.setAttribute("init", currentFrame);
    root.setAttribute("frames", k->stepViewer->totalSteps());
    root.setAttribute("coords", path);

    kFatal() << "Settings::tweenToXml() - Total Steps: " << k->stepViewer->totalSteps();

    foreach (KTTweenerStep *step, k->stepViewer->steps())
             root.appendChild(step->toXml(doc));

    doc.appendChild(root);

    return doc.toString();
}

int Settings::totalSteps()
{
    return k->stepViewer->totalSteps();
}

void Settings::activatePathMode()
{
    k->options->setCurrentIndex(1);
    k->stepViewer->cleanRows();
}

void Settings::activateSelectionMode()
{
    k->options->setCurrentIndex(0);
}

void Settings::cleanData()
{
    k->stepViewer->cleanRows();
}

void Settings::notifySelection(bool flag)
{
    kFatal() << "Settings::notifySelection() - Updating selection flag: " << flag; 
    k->selectionDone = flag;
}

