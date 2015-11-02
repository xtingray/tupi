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
#include "tupitemtweener.h"
#include "tuptweenerstep.h"
#include "timagebutton.h"
#include "tseparator.h"
#include "tosd.h"

struct Settings::Private
{
    QWidget *innerPanel;
    QWidget *rangePanel;
    QWidget *clockPanel;

    QBoxLayout *layout;
    TupToolPlugin::Mode mode;
    QLineEdit *input;
    TRadioButtonGroup *options;

    QSpinBox *comboInit;
    QSpinBox *comboEnd;

    QLabel *totalLabel;
    int totalSteps;

    TupItemTweener::TransformAxes scaleAxes;
    QComboBox *comboAxes;
    QDoubleSpinBox *comboFactor;
    QSpinBox *comboIterations; 

    QCheckBox *loopBox;
    QCheckBox *reverseLoopBox;

    bool selectionDone;
    bool propertiesDone;

    TImageButton *apply;
    TImageButton *remove;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->scaleAxes = TupItemTweener::XY;
    k->selectionDone = false;
    k->totalSteps = 0;

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
    startingLabel->setAlignment(Qt::AlignVCenter);

    k->comboInit = new QSpinBox();
    k->comboInit->setEnabled(false);
    k->comboInit->setMaximum(999);
    connect(k->comboInit, SIGNAL(valueChanged(int)), this, SLOT(updateLastFrame()));

    QLabel *endingLabel = new QLabel(tr("Ending at frame") + ": ");
    endingLabel->setAlignment(Qt::AlignVCenter);

    k->comboEnd = new QSpinBox();
    k->comboEnd->setEnabled(true);
    k->comboEnd->setValue(1);
    k->comboEnd->setMaximum(999);
    connect(k->comboEnd, SIGNAL(valueChanged(int)), this, SLOT(checkTopLimit(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(startingLabel);
    startLayout->addWidget(k->comboInit);

    QHBoxLayout *endLayout = new QHBoxLayout;
    endLayout->setAlignment(Qt::AlignHCenter);
    endLayout->setMargin(0);
    endLayout->setSpacing(0);
    endLayout->addWidget(endingLabel);
    endLayout->addWidget(k->comboEnd);

    k->totalLabel = new QLabel(tr("Frames Total") + ": 1");
    k->totalLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    QHBoxLayout *totalLayout = new QHBoxLayout;
    totalLayout->setAlignment(Qt::AlignHCenter);
    totalLayout->setMargin(0);
    totalLayout->setSpacing(0);
    totalLayout->addWidget(k->totalLabel);

    k->comboAxes = new QComboBox();
    k->comboAxes->addItem(tr("Width & Height"));
    k->comboAxes->addItem(tr("Only Width"));
    k->comboAxes->addItem(tr("Only Height"));
    QLabel *axesLabel = new QLabel(tr("Scale in") + ": ");
    axesLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *axesLayout = new QHBoxLayout;
    axesLayout->setAlignment(Qt::AlignHCenter);
    axesLayout->setMargin(0);
    axesLayout->setSpacing(0);
    axesLayout->addWidget(axesLabel);
    axesLayout->addWidget(k->comboAxes);

    k->comboFactor = new QDoubleSpinBox;
    k->comboFactor->setDecimals(2);
    k->comboFactor->setMinimum(0.0);
    k->comboFactor->setMaximum(10);
    k->comboFactor->setSingleStep(0.05);
    k->comboFactor->setValue(1.10);

    QLabel *speedLabel = new QLabel(tr("Scaling Factor") + ": ");
    speedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *speedLayout = new QHBoxLayout;
    speedLayout->setAlignment(Qt::AlignHCenter);
    speedLayout->setMargin(0);
    speedLayout->setSpacing(0);
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(k->comboFactor);

    k->comboIterations = new QSpinBox;
    k->comboIterations->setEnabled(true);
    k->comboIterations->setMinimum(1);

    QLabel *iterationsLabel = new QLabel(tr("Iterations") + ": ");
    iterationsLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *iterationsLayout = new QHBoxLayout;
    iterationsLayout->setAlignment(Qt::AlignHCenter);
    iterationsLayout->setMargin(0);
    iterationsLayout->setSpacing(0);
    iterationsLayout->addWidget(iterationsLabel);
    iterationsLayout->addWidget(k->comboIterations);

    k->loopBox = new QCheckBox(tr("Loop"), k->innerPanel);
    connect(k->loopBox, SIGNAL(stateChanged(int)), this, SLOT(updateReverseCheckbox(int)));

    QVBoxLayout *loopLayout = new QVBoxLayout;
    loopLayout->setAlignment(Qt::AlignHCenter);
    loopLayout->setMargin(0);
    loopLayout->setSpacing(0);
    loopLayout->addWidget(k->loopBox);

    k->reverseLoopBox = new QCheckBox(tr("Loop with Reverse"), k->innerPanel);
    connect(k->reverseLoopBox, SIGNAL(stateChanged(int)), this, SLOT(updateLoopCheckbox(int)));

    QVBoxLayout *reverseLayout = new QVBoxLayout;
    reverseLayout->setAlignment(Qt::AlignHCenter);
    reverseLayout->setMargin(0);
    reverseLayout->setSpacing(0);
    reverseLayout->addWidget(k->reverseLoopBox);

    innerLayout->addLayout(startLayout);
    innerLayout->addLayout(endLayout);
    innerLayout->addLayout(totalLayout);

    innerLayout->addSpacing(15);
    innerLayout->addWidget(new TSeparator(Qt::Horizontal));

    innerLayout->addLayout(axesLayout);
    innerLayout->addLayout(speedLayout);
    innerLayout->addLayout(iterationsLayout);
    innerLayout->addLayout(loopLayout);
    innerLayout->addLayout(reverseLayout);

    innerLayout->addWidget(new TSeparator(Qt::Horizontal));

    k->layout->addWidget(k->innerPanel);

    activeInnerForm(false);
}

void Settings::activeInnerForm(bool enable)
{
    if (enable && !k->innerPanel->isVisible()) {
        k->propertiesDone = true;
        k->innerPanel->show();
    } else {
        k->propertiesDone = false;
        k->innerPanel->hide();
    }
}

// Adding new Tween

void Settings::setParameters(const QString &name, int framesCount, int initFrame)
{
    Q_UNUSED(framesCount);

    k->mode = TupToolPlugin::Add;
    k->input->setText(name);

    activateMode(TupToolPlugin::Selection);
    k->apply->setToolTip(tr("Save Tween"));
    k->remove->setIcon(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close.png"));
    k->remove->setToolTip(tr("Cancel Tween"));

    k->comboInit->setValue(initFrame + 1);
    k->comboInit->setEnabled(false);
}

// Editing new Tween

void Settings::setParameters(TupItemTweener *currentTween)
{
    setEditMode();
    activateMode(TupToolPlugin::Properties);

    k->input->setText(currentTween->name());

    k->comboInit->setEnabled(true); 
    k->comboInit->setValue(currentTween->initFrame() + 1);

    k->comboEnd->setValue(currentTween->initFrame() + currentTween->frames());

    checkFramesRange();

    k->comboAxes->setCurrentIndex(currentTween->tweenScaleAxes());
    k->comboFactor->setValue(currentTween->tweenScaleFactor());

    k->comboIterations->setValue(currentTween->tweenScaleIterations());

    k->loopBox->setChecked(currentTween->tweenScaleLoop());
    k->reverseLoopBox->setChecked(currentTween->tweenScaleReverseLoop());
}

void Settings::initStartCombo(int framesCount, int currentIndex)
{
    k->comboInit->clear();
    k->comboEnd->clear();

    k->comboInit->setMinimum(1);
    k->comboInit->setMaximum(framesCount);
    k->comboInit->setValue(currentIndex + 1);

    k->comboEnd->setMinimum(1);
    k->comboEnd->setValue(framesCount);
}

void Settings::setStartFrame(int currentIndex)
{
    k->comboInit->setValue(currentIndex + 1);
    int end = k->comboEnd->value();
    if (end < currentIndex+1)
        k->comboEnd->setValue(currentIndex + 1);
}

int Settings::startFrame()
{
    return k->comboInit->value() - 1;
}

int Settings::startComboSize()
{
    return k->comboInit->maximum();
}

int Settings::totalSteps()
{
    return k->comboEnd->value() - (k->comboInit->value() - 1);
}

void Settings::setEditMode()
{
    k->mode = TupToolPlugin::Edit;
    k->apply->setToolTip(tr("Update Tween"));
    k->remove->setIcon(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close_properties.png"));
    k->remove->setToolTip(tr("Close Tween properties"));
}

void Settings::applyTween()
{
    if (!k->selectionDone) {
        TOsd::self()->display(tr("Info"), tr("You must select at least one object!"), TOsd::Info);
        return;
    }

    if (!k->propertiesDone) {
        TOsd::self()->display(tr("Info"), tr("You must set Tween properties first!"), TOsd::Info);
        return;
    }

    // SQA: Verify Tween is really well applied before call setEditMode!
    setEditMode();

    if (!k->comboInit->isEnabled())
        k->comboInit->setEnabled(true);

    emit clickedApplyTween();
}

void Settings::notifySelection(bool flag)
{
    k->selectionDone = flag;
}

QString Settings::currentTweenName() const
{
    QString tweenName = k->input->text();
    if (tweenName.length() > 0)
        k->input->setFocus();

    return tweenName;
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
                     emit clickedDefineProperties();
                 } else {
                     k->options->setCurrentIndex(0);
                     TOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), TOsd::Info);
                 }
             }
    }
}

QString Settings::tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", TupItemTweener::Scale);
    root.setAttribute("initFrame", currentFrame);
    root.setAttribute("initLayer", currentLayer);
    root.setAttribute("initScene", currentScene);

    checkFramesRange();
    root.setAttribute("frames", k->totalSteps);
    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    k->scaleAxes = TupItemTweener::TransformAxes(k->comboAxes->currentIndex());
    root.setAttribute("scaleAxes", k->scaleAxes);

    double factor = k->comboFactor->value();
    root.setAttribute("scaleFactor", QString::number(factor));

    int iterations = k->comboIterations->value();
    if (iterations == 0) {
        iterations = 1;
        k->comboIterations->setValue(iterations);
    }
    root.setAttribute("scaleIterations", iterations);

    bool loop = k->loopBox->isChecked();
    if (loop)
        root.setAttribute("scaleLoop", "1");
    else
        root.setAttribute("scaleLoop", "0");

    bool reverse = k->reverseLoopBox->isChecked();
    if (reverse)
        root.setAttribute("scaleReverseLoop", "1");
    else
        root.setAttribute("scaleReverseLoop", "0");

    double factorX = 1.0;
    double factorY = 1.0;
    double scaleX = 1.0;
    double scaleY = 1.0;

    if (k->scaleAxes == TupItemTweener::XY) {
        factorX = factor;
        factorY = factor;
    } else if (k->scaleAxes == TupItemTweener::X) {
               factorX = factor;
    } else {
        factorY = factor;
    }

    int cycle = 1;
    int reverseTop = (iterations*2)-2;

    for (int i=0; i < k->totalSteps; i++) {
         if (cycle <= iterations) {
             if (cycle == 1) {
                 scaleX = 1.0;
                 scaleY = 1.0;
             } else {
                 scaleX *= factorX;
                 scaleY *= factorY;
             }
             cycle++;
         } else {
             // if repeat option is enabled
             if (loop) {
                 cycle = 2;
                 scaleX = 1.0;
                 scaleY = 1.0;
             } else if (reverse) { // if reverse option is enabled
                        scaleX /= factorX;
                        scaleY /= factorY;

                        if (cycle < reverseTop)
                            cycle++;
                        else
                            cycle = 1;

             } else { // If cycle is done and no loop and no reverse
                 scaleX = 1.0;
                 scaleY = 1.0;
             }
         }

         TupTweenerStep *step = new TupTweenerStep(i);
         step->setScale(scaleX, scaleY);
         root.appendChild(step->toXml(doc));
    }

    doc.appendChild(root);

    return doc.toString();
}

void Settings::activateMode(TupToolPlugin::EditMode mode)
{
    k->options->setCurrentIndex(mode);
}

void Settings::updateLastFrame()
{
    int end = k->comboInit->value() + k->totalSteps - 1;
    k->comboEnd->setValue(end);
}

void Settings::checkTopLimit(int index)
{
    Q_UNUSED(index);
    checkFramesRange();
}

void Settings::checkFramesRange()
{
    int begin = k->comboInit->value();
    int end = k->comboEnd->value();

    if (begin > end) {
        k->comboEnd->setValue(k->comboEnd->maximum() - 1);
        end = k->comboEnd->value();
    }

    k->totalSteps = end - begin + 1;
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->totalSteps));
}

void Settings::updateLoopCheckbox(int state)
{
    Q_UNUSED(state);

    if (k->reverseLoopBox->isChecked() && k->loopBox->isChecked())
        k->loopBox->setChecked(false);
}

void Settings::updateReverseCheckbox(int state)
{
    Q_UNUSED(state);

    if (k->reverseLoopBox->isChecked() && k->loopBox->isChecked())
        k->reverseLoopBox->setChecked(false);
}

void Settings::updateTotalSteps(const QString &text)
{
    Q_UNUSED(text);
    checkFramesRange();
}
