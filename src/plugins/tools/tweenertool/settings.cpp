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
#include <QHeaderView>
#include <QGraphicsPathItem>

struct Settings::Private
{
    QBoxLayout *layout; 
    KRadioButtonGroup *options;
    QString tweenName;
    StepsViewer *stepViewer;
    QComboBox *combo;
    QLabel *totalLabel;
    bool selectionDone;
    Mode mode; 
    KImageButton *apply;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLabel *startingLabel = new QLabel(tr("Starting at frame") + ":"); 
    startingLabel->setAlignment(Qt::AlignHCenter);
    k->combo = new QComboBox();
    k->combo->setMaximumWidth(50);
    k->combo->setEditable(false);

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

    k->apply = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);

    connect(k->apply, SIGNAL(clicked()), this, SLOT(applyTween()));

    KImageButton *remove = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    remove->setToolTip(tr("Cancel Operation"));

    connect(remove, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->apply);
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

void Settings::setParameters(QString &name, Mode mode, int framesTotal, int startFrame)
{
    k->mode = mode;
    k->tweenName = name;

    if (k->mode == Add)
        k->combo->setEnabled(false);

    if (k->mode == Add)
        k->apply->setToolTip(tr("Save Tween"));
    else
        k->apply->setToolTip(tr("Update Tween"));

    initStartCombo(framesTotal, startFrame);
}

void Settings::setParameters(KTItemTweener *currentTween)
{
    k->mode = Edit;
    k->tweenName = currentTween->name();

    k->apply->setToolTip(tr("Update Tween"));

    initStartCombo(currentTween->frames(), currentTween->startFrame());

    k->stepViewer->setPath(currentTween->graphicsPath());
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void Settings::initStartCombo(int framesTotal, int currentIndex)
{
    kFatal() << "framesTotal: " << framesTotal;
    kFatal() << "currentFrame: " << currentIndex;

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

QString Settings::tweenToXml(int currentFrame, QString &path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", k->tweenName);
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

void Settings::applyTween()
{
    k->mode = Edit;
    k->apply->setToolTip(tr("Update Tween"));

    if (!k->combo->isEnabled())
        k->combo->setEnabled(true);

    emit clickedApplyTween();
}

