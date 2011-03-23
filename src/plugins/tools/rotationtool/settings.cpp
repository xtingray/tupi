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

struct Settings::Private
{
    QWidget *innerPanel;
    QWidget *rangePanel;
    QWidget *clockPanel;

    QBoxLayout *layout;
    Mode mode;
    EditMode editMode;

    QLineEdit *input;
    KRadioButtonGroup *options;
    QComboBox *comboInit;
    QComboBox *comboEnd;

    QComboBox *comboType;
    KTItemTweener::RotationType rotationType;

    QComboBox *comboStart;
    QComboBox *comboFinish;

    QComboBox *comboSpeed;
    QCheckBox *rangeLoopBox;
    QCheckBox *reverseLoopBox;
    QLabel *totalLabel;
    QComboBox *comboClock;
    int totalSteps;

    bool selectionDone;

    KImageButton *apply;
    KImageButton *remove;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;
    k->rotationType = KTItemTweener::Continuos;
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
    k->options->addItem(tr("Set Properties"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->apply = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    connect(k->apply, SIGNAL(clicked()), this, SLOT(applyTween()));

    k->remove = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    k->remove->setToolTip(tr("Cancel Tween"));
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

    activatePropertiesMode(Settings::Selection);
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
    k->comboInit = new QComboBox();
    k->comboInit->setMaximumWidth(50);
    k->comboInit->setEditable(false);
    k->comboInit->setValidator(new QIntValidator(k->comboInit));

    connect(k->comboInit, SIGNAL(currentIndexChanged(int)), this, SLOT(checkBottomLimit(int)));

    QLabel *endingLabel = new QLabel(tr("Ending at frame") + ": ");
    endingLabel->setAlignment(Qt::AlignVCenter);
    k->comboEnd = new QComboBox();
    k->comboEnd->setFixedWidth(60);
    k->comboEnd->setEditable(true);
    k->comboEnd->addItem(QString::number(1));
    k->comboEnd->setValidator(new QIntValidator(k->comboEnd));

    connect(k->comboEnd, SIGNAL(currentIndexChanged(int)), this, SLOT(checkTopLimit(int)));

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
    k->comboType->addItem(tr("Continous"));
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
    k->comboSpeed = new QComboBox();
    k->comboSpeed->setMaximumWidth(50);
    k->comboSpeed->setEditable(true);
    k->comboSpeed->setValidator(new QIntValidator(k->comboSpeed));
    for (int i=0; i<=359; i++)
         k->comboSpeed->addItem(QString::number(i));
    k->comboSpeed->setCurrentIndex(5);

    QVBoxLayout *speedLayout = new QVBoxLayout;
    speedLayout->setAlignment(Qt::AlignHCenter);
    speedLayout->setMargin(0);
    speedLayout->setSpacing(0);
    speedLayout->addWidget(speedLabel);

    QVBoxLayout *speedLayout2 = new QVBoxLayout;
    speedLayout2->setAlignment(Qt::AlignHCenter);
    speedLayout2->setMargin(0);
    speedLayout2->setSpacing(0);
    speedLayout2->addWidget(k->comboSpeed);

    innerLayout->addLayout(startLayout);
    innerLayout->addLayout(endLayout);
    innerLayout->addLayout(totalLayout);

    innerLayout->addSpacing(15);
    innerLayout->addWidget(new KSeparator(Qt::Horizontal));
    innerLayout->addLayout(typeLayout);

    innerLayout->addWidget(new KSeparator(Qt::Horizontal));

    setClockForm();
    innerLayout->addWidget(k->clockPanel);
    setRangeForm();
    innerLayout->addWidget(k->rangePanel);

    innerLayout->addLayout(speedLayout);
    innerLayout->addLayout(speedLayout2);

    innerLayout->addWidget(new KSeparator(Qt::Horizontal));

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

    k->comboStart = new QComboBox();
    k->comboStart->setEditable(true);
    k->comboStart->setValidator(new QIntValidator(k->comboStart));
    for (int i=0; i<=359; i++)
         k->comboStart->addItem(QString::number(i));

    connect(k->comboStart, SIGNAL(currentIndexChanged(int)), this, SLOT(checkRange(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(startLabel);
    startLayout->addWidget(k->comboStart);

    QLabel *endLabel = new QLabel(tr("Finish at") + ": ");
    endLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    k->comboFinish = new QComboBox();
    k->comboFinish->setEditable(true);
    k->comboFinish->setValidator(new QIntValidator(k->comboFinish));
    for (int i=0; i<=359; i++)
         k->comboFinish->addItem(QString::number(i));
    k->comboFinish->setCurrentIndex(5);

    connect(k->comboFinish, SIGNAL(currentIndexChanged(int)), this, SLOT(checkRange(int)));

    QHBoxLayout *endLayout = new QHBoxLayout;
    endLayout->setAlignment(Qt::AlignHCenter);
    endLayout->setMargin(0);
    endLayout->setSpacing(0);
    endLayout->addWidget(endLabel);
    endLayout->addWidget(k->comboFinish);

    k->rangeLoopBox = new QCheckBox(tr("Loop"), k->rangePanel);
    k->rangeLoopBox->setChecked(true);

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

void Settings::setParameters(const QString &name, int framesTotal, int startFrame)
{
    k->mode = Add;
    k->input->setText(name);

    activatePropertiesMode(Settings::Selection);
    k->apply->setToolTip(tr("Save Tween"));
}

// Editing new Tween

void Settings::setParameters(KTItemTweener *currentTween)
{
    kFatal() << "Settings::setParameters() - Editing tween!";
    setEditMode();
    activatePropertiesMode(Settings::Properties);

    k->input->setText(currentTween->name());

    k->comboType->setCurrentIndex(currentTween->tweenRotationType());
    k->comboSpeed->setItemText(0, QString::number(currentTween->tweenRotateSpeed()));
    k->comboSpeed->setCurrentIndex(0);

    if (currentTween->tweenRotationType() == KTItemTweener::Continuos) {
        k->comboClock->setCurrentIndex(currentTween->tweenRotateDirection());
    } else {
        k->comboStart->setItemText(0, QString::number(currentTween->tweenRotateStartDegree()));
        k->comboStart->setCurrentIndex(0);
        k->comboFinish->setItemText(0, QString::number(currentTween->tweenRotateEndDegree()));
        k->comboFinish->setCurrentIndex(0);
        k->rangeLoopBox->setChecked(currentTween->tweenRotateLoop());
        k->reverseLoopBox->setChecked(currentTween->tweenRotateReverseLoop());
    }
}

void Settings::initStartCombo(int framesTotal, int currentIndex)
{
    k->comboInit->clear();
    k->comboEnd->clear();

    for (int i=1; i<=framesTotal; i++) {
         k->comboInit->addItem(QString::number(i));
         k->comboEnd->addItem(QString::number(i));
    }

    k->comboInit->setCurrentIndex(currentIndex);
    k->comboEnd->setCurrentIndex(framesTotal - 1);
}

void Settings::setStartFrame(int currentIndex)
{
    k->comboInit->setCurrentIndex(currentIndex);
    int end = k->comboEnd->currentText().toInt();
    if (end < currentIndex+1)
        k->comboEnd->setItemText(0, QString::number(currentIndex + 1));
}

int Settings::startComboSize()
{
    return k->comboInit->count();
}

int Settings::totalSteps()
{
    return k->comboEnd->currentText().toInt() - k->comboInit->currentIndex();
}

void Settings::setEditMode()
{
    k->mode = Edit;
    k->apply->setToolTip(tr("Update Tween"));
    k->remove->setIcon(QPixmap(THEME_DIR + "icons/close_properties.png"));
    k->remove->setToolTip(tr("Close Tween properties"));
}

void Settings::applyTween()
{
    // SQA: Verify Tween is really well applied before call setEditMode!
    setEditMode();

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
                 if (k->editMode != Settings::Selection) {
                     kFatal() << "Settings::emitOptionChanged() - Rotation / Select Mode!";
                     activeInnerForm(false);
                     k->editMode = Settings::Selection;
                     emit clickedSelect();
                 }
             }
            break;
            case 1:
             {
                 if (k->editMode != Settings::Properties) {
                     kFatal() << "Settings::emitOptionChanged() - Rotation / Properties Mode!";
                     k->editMode = Settings::Properties;
                     if (k->selectionDone) {
                         activeInnerForm(true);
                         emit clickedDefineAngle();
                     } else {
                         k->options->setCurrentIndex(0);
                         KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);
                     }
                 }
             }
    }
}

QString Settings::tweenToXml(int currentFrame, QPointF point)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", KTItemTweener::Rotation);
    root.setAttribute("init", currentFrame);
    
    checkFramesRange();
    root.setAttribute("frames", k->totalSteps);

    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    root.setAttribute("rotationType", k->rotationType);
    int speed = k->comboSpeed->currentText().toInt();
    root.setAttribute("rotateSpeed", speed);

    if (k->rotationType == KTItemTweener::Continuos) {
        int direction = k->comboClock->currentIndex();
        root.setAttribute("rotateDirection", direction);

        int angle = 0;
        for (int i=0; i < k->totalSteps; i++) {
             KTTweenerStep *step = new KTTweenerStep(i);
             step->setRotation(angle);
             root.appendChild(step->toXml(doc));
             if (direction == KTItemTweener::Clockwise)
                 angle += speed;
             else
                 angle -= speed;
        }

    } else if (k->rotationType == KTItemTweener::Partial) {
               bool loop = k->rangeLoopBox->isChecked();
               if (loop)
                   root.setAttribute("rotateLoop", "1");
               else
                   root.setAttribute("rotateLoop", "0");

               int start = k->comboStart->currentText().toInt();
               root.setAttribute("rotateStartDegree", start);

               int end = k->comboFinish->currentText().toInt();
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
                        KTTweenerStep *step = new KTTweenerStep(i);
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
                        KTTweenerStep *step = new KTTweenerStep(i);
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

void Settings::activatePropertiesMode(Settings::EditMode mode)
{
    kFatal() << "Settings::activatePropertiesMode() - Rotation / Just tracing!";
    k->options->setCurrentIndex(mode);
}

void Settings::refreshForm(int type)
{
    if (type == 0) {
        k->rotationType = KTItemTweener::Continuos;
        activeClockForm(true);
        activeRangeForm(false);
    } else {
        k->rotationType = KTItemTweener::Partial;
        activeClockForm(false);
        activeRangeForm(true);
    }
}

void Settings::checkBottomLimit(int index)
{
    emit startingPointChanged(index);
    checkFramesRange();
}

void Settings::checkTopLimit(int index)
{
    Q_UNUSED(index);
    checkFramesRange();
}

void Settings::checkFramesRange()
{
    int begin = k->comboInit->currentText().toInt();
    int end = k->comboEnd->currentText().toInt();
        
    if (begin > end) {
        k->comboEnd->setCurrentIndex(k->comboEnd->count()-1);
        end = k->comboEnd->currentText().toInt();
    }

    k->totalSteps = end - begin + 1;
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->totalSteps));
}

void Settings::updateRangeCheckbox(int state)
{
    if (k->reverseLoopBox->isChecked() && k->rangeLoopBox->isChecked())
        k->rangeLoopBox->setChecked(false);
}

void Settings::updateReverseCheckbox(int state)
{
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

    int start = k->comboStart->currentText().toInt();
    int end = k->comboFinish->currentText().toInt();

    if (start == end) {
        if (k->comboFinish->currentIndex() == 359)
            k->comboStart->setCurrentIndex(k->comboStart->currentIndex() - 1);
        else
            k->comboFinish->setCurrentIndex(k->comboFinish->currentIndex() + 1);
    }
}
