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

#include "positionsettings.h"
#include "kradiobuttongroup.h"
// #include "kimagebutton.h"
#include "kdebug.h"
#include "ktitemtweener.h"
#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "kosd.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QHeaderView>
#include <QGraphicsPathItem>

struct PositionSettings::Private
{
    QBoxLayout *layout; 

    QLineEdit *input;
    KRadioButtonGroup *options;
    StepsViewer *stepViewer;
    QComboBox *comboInit;
    QLabel *totalLabel;
    bool selectionDone;
    Mode mode; 

    QPushButton *closeButton;
};

PositionSettings::PositionSettings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    k->stepViewer = new StepsViewer;
    k->stepViewer->verticalHeader()->hide();

    k->layout->addWidget(k->stepViewer);

    k->closeButton = new QPushButton(tr("Back to Tweens"), this);
    connect(k->closeButton, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->closeButton);

    k->layout->addSpacing(10);
    k->layout->addLayout(buttonsLayout);
    k->layout->setSpacing(5);

    // activateSelectionMode();
}

PositionSettings::~PositionSettings()
{
    delete k;
}

// Adding new Tween

void PositionSettings::setParameters(const QString &name, int framesTotal, int startFrame)
{
    k->mode = Add;
    k->input->setText(name);

    activateSelectionMode();
    k->stepViewer->cleanRows();
    k->totalLabel->setText(tr("Frames Total") + ": 0");

    k->comboInit->setEnabled(false);
    k->closeButton->setIcon(QPixmap(THEME_DIR + "icons/close.png"));
    k->closeButton->setToolTip(tr("Cancel Tween"));

    initStartCombo(framesTotal, startFrame);
}

// Editing new Tween

void PositionSettings::setParameters(KTItemTweener *currentTween)
{
    setEditMode();

    notifySelection(true);
    activatePathMode();

    k->input->setText(currentTween->name());
    k->comboInit->setEnabled(true);

    initStartCombo(currentTween->frames(), currentTween->startFrame());

    k->stepViewer->setPath(currentTween->graphicsPath());
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void PositionSettings::initStartCombo(int framesTotal, int currentIndex)
{
    k->comboInit->clear();
    for (int i=1; i<=framesTotal; i++)
         k->comboInit->addItem(QString::number(i));

    k->comboInit->setCurrentIndex(currentIndex);
}

void PositionSettings::setStartFrame(int currentIndex)
{
    k->comboInit->setCurrentIndex(currentIndex);
}

int PositionSettings::startFrame()
{
    return k->comboInit->currentIndex();
}

int PositionSettings::startComboSize()
{
    return k->comboInit->count();
}

void PositionSettings::updateSteps(const QGraphicsPathItem *path)
{
    k->stepViewer->setPath(path);
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

void PositionSettings::emitOptionChanged(int option)
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

QString PositionSettings::tweenToXml(int currentFrame, QPointF point, QString &path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("type", KTItemTweener::Position);
    root.setAttribute("init", currentFrame);
    root.setAttribute("frames", k->stepViewer->totalSteps());
    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    root.setAttribute("coords", path);

    foreach (KTTweenerStep *step, k->stepViewer->steps())
             root.appendChild(step->toXml(doc));

    doc.appendChild(root);

    return doc.toString();
}

int PositionSettings::totalSteps()
{
    return k->stepViewer->totalSteps();
}

void PositionSettings::activatePathMode()
{
    k->options->setCurrentIndex(1);
}

void PositionSettings::activateSelectionMode()
{
    k->options->setCurrentIndex(0);
}

void PositionSettings::cleanData()
{
    k->stepViewer->cleanRows();
}

void PositionSettings::notifySelection(bool flag)
{
    k->selectionDone = flag;
}

void PositionSettings::applyTween()
{
    if (!k->selectionDone) {
        k->options->setCurrentIndex(0);
        KOsd::self()->display(tr("Info"), tr("You must select at least one object!"), KOsd::Info);
        return;
    }

    if (totalSteps() <= 2) {
        KOsd::self()->display(tr("Info"), tr("You must define a path for this Tween!"), KOsd::Info);
        return;
    }

    // SQA: Verify Tween is really well applied before call setEditMode!
    setEditMode();

    if (!k->comboInit->isEnabled())
        k->comboInit->setEnabled(true);

    emit clickedApplyTween();
}

void PositionSettings::setEditMode()
{
    k->mode = Edit;
    k->closeButton->setIcon(QPixmap(THEME_DIR + "icons/close_properties.png"));
    k->closeButton->setToolTip(tr("Close Tween properties"));
}

QString PositionSettings::currentTweenName() const
{
    QString tweenName = k->input->text();
    if (tweenName.length() > 0)
        k->input->setFocus();

    return tweenName;
}
