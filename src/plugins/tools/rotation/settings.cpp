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

    QComboBox *comboType;
    TupItemTweener::RotationType rotationType;

    QSpinBox *comboStart;
    QSpinBox *comboFinish;

    QSpinBox *degreesSpinbox;
    QCheckBox *rangeLoopBox;
    QCheckBox *reverseLoopBox;
    QLabel *totalLabel;
    QComboBox *comboClock;
    int totalSteps;

    bool selectionDone;
    bool propertiesDone;

    TImageButton *apply;
    TImageButton *remove;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;
    k->propertiesDone = false;
    k->rotationType = TupItemTweener::Continuos;
    k->totalSteps = 0;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

#ifndef Q_OS_MAC 
    QFont font = this->font();
    font.setPointSize(8);
    setFont(font);
#endif

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

    k->comboType = new QComboBox();
    k->comboType->addItem(tr("Continuous"));
    k->comboType->addItem(tr("Partial"));

    connect(k->comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshForm(int)));

    QLabel *typeLabel = new QLabel(tr("Type") + ": ");
    typeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->setAlignment(Qt::AlignHCenter);
    typeLayout->setMargin(0);
    typeLayout->setSpacing(0);
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(k->comboType);

    QLabel *speedLabel = new QLabel(tr("Speed (Degrees/Frame)") + ": ");
    speedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    k->degreesSpinbox = new QSpinBox;
    k->degreesSpinbox->setEnabled(true);
    k->degreesSpinbox->setMinimum(0);
    k->degreesSpinbox->setMaximum(359);
    k->degreesSpinbox->setValue(5);

    QVBoxLayout *speedLayout = new QVBoxLayout;
    speedLayout->setAlignment(Qt::AlignHCenter);
    speedLayout->setMargin(0);
    speedLayout->setSpacing(0);
    speedLayout->addWidget(speedLabel);

    QVBoxLayout *speedLayout2 = new QVBoxLayout;
    speedLayout2->setAlignment(Qt::AlignHCenter);
    speedLayout2->setMargin(0);
    speedLayout2->setSpacing(0);
    speedLayout2->addWidget(k->degreesSpinbox);

    innerLayout->addLayout(startLayout);
    innerLayout->addLayout(endLayout);
    innerLayout->addLayout(totalLayout);

    innerLayout->addSpacing(15);
    innerLayout->addWidget(new TSeparator(Qt::Horizontal));
    innerLayout->addLayout(typeLayout);

    innerLayout->addWidget(new TSeparator(Qt::Horizontal));

    setClockForm();
    innerLayout->addWidget(k->clockPanel);
    setRangeForm();
    innerLayout->addWidget(k->rangePanel);

    innerLayout->addLayout(speedLayout);
    innerLayout->addLayout(speedLayout2);

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

void Settings::setClockForm()
{
    k->clockPanel = new QWidget;

    QBoxLayout *clockLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->clockPanel);
    clockLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    clockLayout->setMargin(0);
    clockLayout->setSpacing(0);

    QLabel *directionLabel = new QLabel(tr("Direction") + ": ");
    directionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    k->comboClock = new QComboBox();
    k->comboClock->addItem(tr("Clockwise"));
    k->comboClock->addItem(tr("Counterclockwise"));

    clockLayout->addWidget(directionLabel);    
    clockLayout->addWidget(k->comboClock);
    clockLayout->addSpacing(5);
    activeClockForm(true);
}

void Settings::activeClockForm(bool enable)
{
    if (enable && !k->clockPanel->isVisible())
        k->clockPanel->show();
    else
        k->clockPanel->hide();
}

void Settings::setRangeForm()
{
    k->rangePanel = new QWidget;
    QBoxLayout *rangeLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->rangePanel);
    rangeLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    rangeLayout->setMargin(0);
    rangeLayout->setSpacing(0);
 
    QLabel *rangeLabel = new QLabel(tr("Degrees Range") + ": ");
    rangeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *startLabel = new QLabel(tr("Start at") + ": ");
    startLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    k->comboStart = new QSpinBox;
    k->comboStart->setEnabled(true);
    k->comboStart->setMinimum(0);
    k->comboStart->setMaximum(359);
    connect(k->comboStart, SIGNAL(valueChanged(int)), this, SLOT(checkRange(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(startLabel);
    startLayout->addWidget(k->comboStart);

    QLabel *endLabel = new QLabel(tr("Finish at") + ": ");
    endLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    k->comboFinish = new QSpinBox;
    k->comboFinish->setEnabled(true);
    k->comboFinish->setMinimum(0);
    k->comboFinish->setMaximum(359);
    connect(k->comboFinish, SIGNAL(valueChanged(int)), this, SLOT(checkRange(int)));

    QHBoxLayout *endLayout = new QHBoxLayout;
    endLayout->setAlignment(Qt::AlignHCenter);
    endLayout->setMargin(0);
    endLayout->setSpacing(0);
    endLayout->addWidget(endLabel);
    endLayout->addWidget(k->comboFinish);

    k->rangeLoopBox = new QCheckBox(tr("Loop"), k->rangePanel);

    connect(k->rangeLoopBox, SIGNAL(stateChanged(int)), this, SLOT(updateReverseCheckbox(int)));

    QVBoxLayout *loopLayout = new QVBoxLayout;
    loopLayout->setAlignment(Qt::AlignHCenter);
    loopLayout->setMargin(0);
    loopLayout->setSpacing(0);
    loopLayout->addWidget(k->rangeLoopBox);
    k->reverseLoopBox = new QCheckBox(tr("Loop with Reverse"), k->rangePanel);

    connect(k->reverseLoopBox, SIGNAL(stateChanged(int)), this, SLOT(updateRangeCheckbox(int)));

    QVBoxLayout *reverseLayout = new QVBoxLayout;
    reverseLayout->setAlignment(Qt::AlignHCenter);
    reverseLayout->setMargin(0);
    reverseLayout->setSpacing(0);
    reverseLayout->addWidget(k->reverseLoopBox);

    rangeLayout->addWidget(rangeLabel);
    rangeLayout->addLayout(startLayout);
    rangeLayout->addLayout(endLayout);
    rangeLayout->addSpacing(5);
    rangeLayout->addLayout(loopLayout);
    rangeLayout->addLayout(reverseLayout);

    activeRangeForm(false);
}

void Settings::activeRangeForm(bool enable)
{
    if (enable && !k->rangePanel->isVisible())
        k->rangePanel->show();
    else
        k->rangePanel->hide();
}

// Adding new Tween

void Settings::setParameters(const QString &name, int framesCount, int initFrame)
{
    k->mode = TupToolPlugin::Add;
    k->input->setText(name);

    activateMode(TupToolPlugin::Selection);

    k->apply->setToolTip(tr("Save Tween"));
    k->remove->setIcon(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "close.png"));
    k->remove->setToolTip(tr("Cancel Tween"));

    initStartCombo(framesCount, initFrame);
}

// Editing new Tween

void Settings::setParameters(TupItemTweener *currentTween)
{
    setEditMode();
    activateMode(TupToolPlugin::Properties);

    k->input->setText(currentTween->name());

    k->comboInit->setEnabled(true);
    k->comboInit->setValue(currentTween->initFrame());

    k->comboEnd->setValue(currentTween->initFrame() + currentTween->frames());

    // tError() << "Settings::setParameters() - Tracing comboEnd - comboEnd: " << currentTween->initFrame() + currentTween->frames();

    checkFramesRange();

    k->comboType->setCurrentIndex(currentTween->tweenRotationType());
    k->degreesSpinbox->setValue(currentTween->tweenRotateSpeed());

    if (currentTween->tweenRotationType() == TupItemTweener::Continuos) {
        k->comboClock->setCurrentIndex(currentTween->tweenRotateDirection());
    } else {
        k->comboStart->setValue(currentTween->tweenRotateStartDegree());
        k->comboFinish->setValue(currentTween->tweenRotateEndDegree());

        k->rangeLoopBox->setChecked(currentTween->tweenRotateLoop());
        k->reverseLoopBox->setChecked(currentTween->tweenRotateReverseLoop());
    }
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
        #ifdef K_DEBUG
            QString msg = "Settings::applyTween() - You must select at least one object!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return;
    }

    if (!k->propertiesDone) {
        TOsd::self()->display(tr("Info"), tr("You must set Tween properties first!"), TOsd::Info);
        #ifdef K_DEBUG
            QString msg = "Settings::applyTween() - You must set Tween properties first!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
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
                    emit clickedDefineAngle();
                } else {
                    k->options->setCurrentIndex(0);
                    TOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), TOsd::Info);
                    #ifdef K_DEBUG
                        QString msg = "Settings::emitOptionChanged() - You must set Tween properties first!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            }
    }
}

QString Settings::tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", TupItemTweener::Rotation);
    root.setAttribute("initFrame", currentFrame);
    root.setAttribute("initLayer", currentLayer);
    root.setAttribute("initScene", currentScene);
    
    checkFramesRange();
    root.setAttribute("frames", k->totalSteps);

    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    root.setAttribute("rotationType", k->rotationType);
    int speed = k->degreesSpinbox->value();
    root.setAttribute("rotateSpeed", speed);

    if (k->rotationType == TupItemTweener::Continuos) {
        int direction = k->comboClock->currentIndex();
        root.setAttribute("rotateDirection", direction);

        int angle = 0;
        for (int i=0; i < k->totalSteps; i++) {
             TupTweenerStep *step = new TupTweenerStep(i);
             step->setRotation(angle);
             root.appendChild(step->toXml(doc));
             if (direction == TupItemTweener::Clockwise)
                 angle += speed;
             else
                 angle -= speed;
        }

    } else if (k->rotationType == TupItemTweener::Partial) {
               bool loop = k->rangeLoopBox->isChecked();
               if (loop)
                   root.setAttribute("rotateLoop", "1");
               else
                   root.setAttribute("rotateLoop", "0");

               int start = k->comboStart->value();
               root.setAttribute("rotateStartDegree", start);

               int end = k->comboFinish->value();
               root.setAttribute("rotateEndDegree", end);

               bool reverse = k->reverseLoopBox->isChecked();
               if (reverse)
                   root.setAttribute("reverseLoop", "1");
               else
                   root.setAttribute("reverseLoop", "0");

               double angle = start;
               bool token = false;

               if (start < end) {
                   for (int i=0; i < k->totalSteps; i++) {
                        TupTweenerStep *step = new TupTweenerStep(i);
                        step->setRotation(angle);
                        root.appendChild(step->toXml(doc));

                        if (!token) {
                            if (angle < end)
                                angle += speed;
                        } else {
                            angle -= speed;
                        }

                        if (reverse) {
                            if (angle >= end)
                                token = true;
                            else if (angle < start) 
                                     token = false;
                        } else if (loop && angle >= end) {
                                   angle = start;
                        } 
                   }
               } else {
                   for (int i=0; i < k->totalSteps; i++) {
                        TupTweenerStep *step = new TupTweenerStep(i);
                        step->setRotation(angle);
                        root.appendChild(step->toXml(doc));

                        if (!token) {
                            if (angle > end)
                                angle -= speed;
                        } else {
                            angle += speed;
                        }

                        if (reverse) {
                            if (angle <= end)
                                token = true;
                            else if (angle > start)
                                     token = false;
                        } else if (loop && angle <= end) {
                                   angle = start;
                        }
                   }
               }
    }

    doc.appendChild(root);

    return doc.toString();
}

void Settings::activateMode(TupToolPlugin::EditMode mode)
{
    k->options->setCurrentIndex(mode);
}

void Settings::refreshForm(int type)
{
    if (type == 0) {
        k->rotationType = TupItemTweener::Continuos;
        activeClockForm(true);
        activeRangeForm(false);
    } else {
        k->rotationType = TupItemTweener::Partial;
        activeClockForm(false);
        activeRangeForm(true);
    }
}

void Settings::checkTopLimit(int index)
{
    Q_UNUSED(index);
    checkFramesRange();
}

void Settings::updateLastFrame()
{
    int end = k->comboInit->value() + k->totalSteps - 1;
    k->comboEnd->setValue(end);
}

void Settings::checkFramesRange()
{
    int begin = k->comboInit->value();
    int end = k->comboEnd->value();
       
    if (begin > end) {
        // tError() << "Settings::checkFramesRange() - begin: " << begin;
        // tError() << "Settings::checkFramesRange() - end: " << end;
        // tError() << "Settings::checkFramesRange() - Updating comboEnd value...";
        k->comboEnd->setValue(k->comboEnd->maximum() - 1);
        end = k->comboEnd->value();
    }

    k->totalSteps = end - begin + 1;
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->totalSteps));
}

void Settings::updateRangeCheckbox(int state)
{
    Q_UNUSED(state);

    if (k->reverseLoopBox->isChecked() && k->rangeLoopBox->isChecked())
        k->rangeLoopBox->setChecked(false);
}

void Settings::updateReverseCheckbox(int state)
{
    Q_UNUSED(state);

    if (k->reverseLoopBox->isChecked() && k->rangeLoopBox->isChecked())
        k->reverseLoopBox->setChecked(false);
}

void Settings::updateTotalSteps(const QString &text)
{
    Q_UNUSED(text);
    checkFramesRange();
}

void Settings::checkRange(int index)
{
    Q_UNUSED(index);

    int start = k->comboStart->value();
    int end = k->comboFinish->value();

    if (start == end) {
        if (k->comboFinish->value() == 359)
            k->comboStart->setValue(k->comboStart->value() - 1);
        else
            k->comboFinish->setValue(k->comboFinish->value() + 1);
    }
}
