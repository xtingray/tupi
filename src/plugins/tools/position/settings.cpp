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
#include "tradiobuttongroup.h"
#include "timagebutton.h"
#include "tupitemtweener.h"
#include "stepsviewer.h"
#include "tuptweenerstep.h"
#include "tosd.h"

struct Settings::Private
{
    QWidget *innerPanel;
    QBoxLayout *layout; 

    QLineEdit *input;
    TRadioButtonGroup *options;
    StepsViewer *stepViewer;
    QSpinBox *comboInit;
    QLabel *totalLabel;
    bool selectionDone;
    TupToolPlugin::Mode mode; 

    TImageButton *apply;
    TImageButton *remove;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QLabel *nameLabel = new QLabel(tr("Name") + ": ");
    k->input = new QLineEdit;

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    nameLayout->setMargin(0);
    nameLayout->setSpacing(0);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(k->input);

    k->options = new TRadioButtonGroup(tr("Options"), Qt::Vertical);
    k->options->addItem(tr("Select object"), 0);
    k->options->addItem(tr("Set Properties"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->apply = new TImageButton(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "save.png"), 22);
    connect(k->apply, SIGNAL(clicked()), this, SLOT(applyTween()));

    k->remove = new TImageButton(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close.png"), 22);
    connect(k->remove, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->apply);
    buttonsLayout->addWidget(k->remove);

    k->layout->addLayout(nameLayout);
    k->layout->addWidget(k->options);

    setInnerForm();

    k->layout->addSpacing(10);
    k->layout->addLayout(buttonsLayout);
    k->layout->setSpacing(5);

    activateMode(TupToolPlugin::Selection);
}

Settings::~Settings()
{
    delete k;
}

void Settings::setInnerForm()
{
    k->innerPanel = new QWidget; 

    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->innerPanel);
    innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QLabel *startingLabel = new QLabel(tr("Starting at frame") + ": ");
    startingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    k->comboInit = new QSpinBox();

    connect(k->comboInit, SIGNAL(valueChanged(int)), this, SIGNAL(startingFrameChanged(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(k->comboInit);

    k->stepViewer = new StepsViewer;
    k->stepViewer->verticalHeader()->hide();

    k->totalLabel = new QLabel(tr("Frames Total") + ": 0");
    k->totalLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    QHBoxLayout *totalLayout = new QHBoxLayout;
    totalLayout->setAlignment(Qt::AlignHCenter);
    totalLayout->setMargin(0);
    totalLayout->setSpacing(0);
    totalLayout->addWidget(k->totalLabel);

    innerLayout->addWidget(startingLabel);
    innerLayout->addLayout(startLayout);
    innerLayout->addWidget(k->stepViewer);

    innerLayout->addLayout(totalLayout);

    k->layout->addWidget(k->innerPanel);

    activeInnerForm(false);
}

void Settings::activeInnerForm(bool enable)
{
    if (enable && !k->innerPanel->isVisible())
        k->innerPanel->show();
    else
        k->innerPanel->hide();
}

// Adding new Tween

void Settings::setParameters(const QString &name, int framesCount, int startFrame)
{
    k->mode = TupToolPlugin::Add;
    k->input->setText(name);

    activateMode(TupToolPlugin::Selection);
    k->stepViewer->cleanRows();
    k->totalLabel->setText(tr("Frames Total") + ": 0");

    k->comboInit->setEnabled(false);
    k->apply->setToolTip(tr("Save Tween"));
    k->remove->setIcon(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close.png"));
    k->remove->setToolTip(tr("Cancel Tween"));

    initStartCombo(framesCount, startFrame);
}

// Editing new Tween

void Settings::setParameters(TupItemTweener *currentTween)
{
    setEditMode();

    notifySelection(true);
    activateMode(TupToolPlugin::Properties);

    k->input->setText(currentTween->name());
    k->comboInit->setEnabled(true);

    initStartCombo(currentTween->frames(), currentTween->initFrame());

    k->stepViewer->setPath(currentTween->graphicsPath());
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void Settings::initStartCombo(int framesCount, int currentIndex)
{
    k->comboInit->clear();
    k->comboInit->setMinimum(1);
    k->comboInit->setMaximum(framesCount);
    k->comboInit->setValue(currentIndex);
}

void Settings::setStartFrame(int currentIndex)
{
    k->comboInit->setValue(currentIndex);
}

int Settings::startFrame()
{
    return k->comboInit->value() - 1;
}

int Settings::startComboSize()
{
    return k->comboInit->maximum();
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
                 activeInnerForm(false);
                 emit clickedSelect();
             }
            break;
            case 1:
             {
                 if (k->selectionDone) {
                     activeInnerForm(true);
                     emit clickedCreatePath();
                 } else {
                     k->options->setCurrentIndex(0);
                     TOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), TOsd::Info);   
                 }
             }
    }
}

QString Settings::tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point, QString &path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", TupItemTweener::Position);
    root.setAttribute("initFrame", currentFrame);
    root.setAttribute("initLayer", currentLayer);
    root.setAttribute("initScene", currentScene);
    root.setAttribute("frames", k->stepViewer->totalSteps());
    // root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    root.setAttribute("origin", "0,0");
    root.setAttribute("coords", path);

    foreach (TupTweenerStep *step, k->stepViewer->steps())
             root.appendChild(step->toXml(doc));

    doc.appendChild(root);

    return doc.toString();
}

int Settings::totalSteps()
{
    return k->stepViewer->totalSteps();
}

/*
void Settings::activatePathMode()
{
    k->options->setCurrentIndex(1);
}

void Settings::activateSelectionMode()
{
    k->options->setCurrentIndex(0);
}
*/

void Settings::activateMode(TupToolPlugin::EditMode mode)
{
    k->options->setCurrentIndex(mode);
}

void Settings::cleanData()
{
    k->stepViewer->cleanRows();
}

void Settings::notifySelection(bool flag)
{
    k->selectionDone = flag;
}

void Settings::applyTween()
{
    if (!k->selectionDone) {
        k->options->setCurrentIndex(0);
        TOsd::self()->display(tr("Info"), tr("You must select at least one object!"), TOsd::Info);
        return;
    }

    if (totalSteps() <= 2) {
        TOsd::self()->display(tr("Info"), tr("You must define a path for this Tween!"), TOsd::Info);
        return;
    }

    // SQA: Verify Tween is really well applied before call setEditMode!
    setEditMode();

    if (!k->comboInit->isEnabled())
        k->comboInit->setEnabled(true);

    emit clickedApplyTween();
}

void Settings::setEditMode()
{
    k->mode = TupToolPlugin::Edit;
    k->apply->setToolTip(tr("Update Tween"));
    k->remove->setIcon(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close_properties.png"));
    k->remove->setToolTip(tr("Close Tween properties"));
}

QString Settings::currentTweenName() const
{
    QString tweenName = k->input->text();
    if (tweenName.length() > 0)
        k->input->setFocus();

    return tweenName;
}
