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
#include <QPushButton>
#include <QColorDialog>

struct Settings::Private
{
    QWidget *innerPanel;
    QBoxLayout *layout;
    Mode mode;

    QLineEdit *input;
    QComboBox *comboInit;
    QComboBox *comboEnd;
    KRadioButtonGroup *options;

    QPushButton *initColorButton;
    QColor initialColor;
    QPushButton *endColorButton;
    QColor endingColor;
    QComboBox *comboIterations;
    QCheckBox *loopBox;
    QCheckBox *reverseLoopBox;

    QLabel *totalLabel;
    int totalSteps;

    bool selectionDone;

    KImageButton *apply;
    KImageButton *remove;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
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

    k->initialColor = QColor("#fff");
    k->initColorButton = new QPushButton();
    k->initColorButton->setText(tr("White"));
    k->initColorButton->setPalette(QPalette(k->initialColor));
    k->initColorButton->setAutoFillBackground(true);
    connect(k->initColorButton, SIGNAL(clicked()), this, SLOT(setInitialColor()));

    /*
    k->initColorButton = new QComboBox();
    for (int i=0; i<=9; i++) {
         k->initColorButton->addItem("0." + QString::number(i));
         k->initColorButton->addItem("0." + QString::number(i) + "5");
    }
    k->initColorButton->addItem("1.0");
    k->initColorButton->setCurrentIndex(20);
    */

    QLabel *coloringInitLabel = new QLabel(tr("Initial Color") + ": ");
    coloringInitLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *coloringInitLayout = new QHBoxLayout;
    coloringInitLayout->setAlignment(Qt::AlignHCenter);
    coloringInitLayout->setMargin(0);
    coloringInitLayout->setSpacing(0);
    coloringInitLayout->addWidget(coloringInitLabel);
    coloringInitLayout->addWidget(k->initColorButton);

    /*
    k->endColorButton = new QComboBox();
    for (int i=0; i<=9; i++) {
         k->endColorButton->addItem("0." + QString::number(i));
         k->endColorButton->addItem("0." + QString::number(i) + "5");
    }
    k->endColorButton->addItem("1.0");
    */

    k->endingColor = QColor("#fff");
    k->endColorButton = new QPushButton();
    k->endColorButton->setText(tr("White"));
    k->endColorButton->setPalette(QPalette(k->endingColor));
    k->endColorButton->setAutoFillBackground(true);
    connect(k->endColorButton, SIGNAL(clicked()), this, SLOT(setEndingColor()));

    QLabel *coloringEndLabel = new QLabel(tr("Ending Color") + ": ");
    coloringEndLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *coloringEndLayout = new QHBoxLayout;
    coloringEndLayout->setAlignment(Qt::AlignHCenter);
    coloringEndLayout->setMargin(0);
    coloringEndLayout->setSpacing(0);
    coloringEndLayout->addWidget(coloringEndLabel);
    coloringEndLayout->addWidget(k->endColorButton);

    k->comboIterations = new QComboBox();
    k->comboIterations->setEditable(true);
    k->comboIterations->setValidator(new QIntValidator(k->comboIterations));
    for (int i=1; i<=100; i++)
         k->comboIterations->addItem(QString::number(i));

    QLabel *iterationsLabel = new QLabel(tr("Iterations") + ": ");
    iterationsLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QHBoxLayout *iterationsLayout = new QHBoxLayout;
    iterationsLayout->setAlignment(Qt::AlignHCenter);
    iterationsLayout->setMargin(0);
    iterationsLayout->setSpacing(0);
    iterationsLayout->addWidget(iterationsLabel);
    iterationsLayout->addWidget(k->comboIterations);

    k->loopBox = new QCheckBox(tr("Loop"), k->innerPanel);
    k->loopBox->setChecked(true);
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

    innerLayout->addSpacing(10);
    innerLayout->addWidget(new KSeparator(Qt::Horizontal));

    innerLayout->addLayout(coloringInitLayout);
    innerLayout->addLayout(coloringEndLayout);

    innerLayout->addLayout(iterationsLayout);
    innerLayout->addLayout(loopLayout);
    innerLayout->addLayout(reverseLayout);

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

void Settings::setParameters(const QString &name, int framesTotal, int startFrame)
{
    k->mode = Add;
    k->input->setText(name);

    activatePropertiesMode(Settings::Selection);
    k->apply->setToolTip(tr("Save Tween"));
    k->remove->setIcon(QPixmap(THEME_DIR + "icons/close.png"));
    k->remove->setToolTip(tr("Cancel Tween"));

    k->comboInit->setCurrentIndex(startFrame);
    k->comboInit->setEditable(false);
    k->comboInit->setEnabled(false);

    k->initColorButton->setText(k->initialColor.name());
    k->initColorButton->setPalette(QPalette(k->initialColor));
    k->initColorButton->setAutoFillBackground(true);
}

void Settings::setParameters(KTItemTweener *currentTween)
{
    setEditMode();
    activatePropertiesMode(Settings::Properties);

    k->input->setText(currentTween->name());

    k->comboInit->setEnabled(true);
    k->comboInit->setEditable(true);
    k->comboInit->setCurrentIndex(currentTween->startFrame());
    k->comboEnd->setItemText(0, QString::number(currentTween->startFrame() + currentTween->frames()));
    k->comboEnd->setCurrentIndex(0);

    checkFramesRange();
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

void Settings::notifySelection(bool flag, QColor color)
{
    k->selectionDone = flag;
    k->initialColor = color;
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
    kFatal() << "Settings::emitOptionChanged() - Mode: " << option; 

    switch (option) {
            case 0:
             {
                 kFatal() << "Settings::emitOptionChanged() - Enabling selection mode!";
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
                     KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);
                 }
             }
    }
}

QString Settings::tweenToXml(int currentFrame)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", KTItemTweener::Coloring);
    root.setAttribute("init", currentFrame);
  
    checkFramesRange();
    root.setAttribute("frames", k->totalSteps);
    root.setAttribute("origin", "0,0");

    //double initFactor = k->initColorButton->currentText().toDouble();
    double initFactor = 1;
    root.setAttribute("initColoringFactor", initFactor);

    // double endFactor = k->endColorButton->currentText().toDouble();
    double endFactor = 2;
    root.setAttribute("endColoringFactor", endFactor);

    int iterations = k->comboIterations->currentText().toInt();
    root.setAttribute("coloringIterations", iterations);

    bool loop = k->loopBox->isChecked();
    if (loop)
        root.setAttribute("coloringLoop", "1");
    else
        root.setAttribute("coloringLoop", "0");

    bool reverse = k->reverseLoopBox->isChecked();
    if (reverse)
        root.setAttribute("coloringReverseLoop", "1");
    else
        root.setAttribute("coloringReverseLoop", "0");

    double delta = (initFactor - endFactor)/(double)iterations;

    kFatal() << "tweenToXml() - Delta: " << delta;
    kFatal() << "tweenToXml() - initFactor: " << initFactor;
    kFatal() << "tweenToXml() - endFactor: " << endFactor;

    KTTweenerStep *step = new KTTweenerStep(0);
    // step->setColor(initFactor);
    root.appendChild(step->toXml(doc));

    double reference = initFactor - delta; 
    int cycle = 2;
    bool token = true;

    for (int i=1; i < k->totalSteps; i++) {
         KTTweenerStep *step = new KTTweenerStep(i);
         // step->setColor(reference);
         root.appendChild(step->toXml(doc));

         if (cycle < iterations && token) {
             reference -= delta;
             cycle++;
         } else {
             if (loop) {
                 cycle = 1;
                 reference = initFactor;
             } else if (reverse && !token) {
                 if (cycle >= ((iterations*2)-2)) {
                     token = true;
                     cycle = 2;
                     reference -= delta;
                 } else {
                     reference += delta;
                     cycle++;
                 }
             } else {
                 token = false;
                 reference += delta;
             }
         }
    }

    doc.appendChild(root);

    return doc.toString();
}

void Settings::activatePropertiesMode(Settings::EditMode mode)
{
    kFatal() << "Settings::activatePropertiesMode() - Setting: " << mode;
    k->options->setCurrentIndex(mode);
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

void Settings::updateLoopCheckbox(int state)
{
    if (k->reverseLoopBox->isChecked() && k->loopBox->isChecked())
        k->loopBox->setChecked(false);
}

void Settings::updateReverseCheckbox(int state)
{
    if (k->reverseLoopBox->isChecked() && k->loopBox->isChecked())
        k->reverseLoopBox->setChecked(false);
}

void Settings::setInitialColor()
{
     k->initialColor = QColorDialog::getColor(k->initialColor, this);

     if (k->initialColor.isValid()) {
         k->initColorButton->setText(k->initialColor.name());
         k->initColorButton->setPalette(QPalette(k->initialColor));
         k->initColorButton->setAutoFillBackground(true);
     }
}

void Settings::setEndingColor()
{
     k->endingColor = QColorDialog::getColor(k->endingColor, this);

     if (k->endingColor.isValid()) {
         k->endColorButton->setText(k->endingColor.name());
         k->endColorButton->setPalette(QPalette(k->endingColor));
         k->endColorButton->setAutoFillBackground(true);
     }
}


