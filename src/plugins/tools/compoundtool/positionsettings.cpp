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
#include "kseparator.h"
#include "kimagebutton.h"
#include "tdebug.h"
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

    StepsViewer *stepViewer;
    QComboBox *comboInit;
    QLabel *totalLabel;
    TweenerPanel::Mode mode; 

    const QGraphicsPathItem *path;
    QPointF offset;

    KImageButton *applyButton;
    KImageButton *closeButton;
};

PositionSettings::PositionSettings(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLabel *componentLabel = new QLabel(tr("Component") + ": ");
    componentLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *componentName = new QLabel(tr("Position"));
    componentName->setAlignment(Qt::AlignHCenter);
    componentName->setFont(QFont("Arial", 8, QFont::Bold));

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    labelLayout->setMargin(0);
    labelLayout->setSpacing(0);
    labelLayout->addWidget(componentLabel);
    labelLayout->addWidget(componentName);

    QLabel *startingLabel = new QLabel(tr("Starting at frame") + ": ");
    startingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    k->comboInit = new QComboBox();
    k->comboInit->setFixedWidth(60);

    // SQA: Remove this instruction in the right moment
    // k->comboInit->setEnabled(false);

    connect(k->comboInit, SIGNAL(currentIndexChanged(int)), this, SIGNAL(startingPointChanged(int)));

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(k->comboInit);

    k->layout->addLayout(labelLayout);
    k->layout->addWidget(new KSeparator(Qt::Horizontal));

    k->layout->addWidget(startingLabel);
    k->layout->addLayout(startLayout);

    k->stepViewer = new StepsViewer;
    k->stepViewer->verticalHeader()->hide();

    k->layout->addWidget(k->stepViewer);

    k->totalLabel = new QLabel(tr("Frames Total") + ": 0");
    k->totalLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    QHBoxLayout *totalLayout = new QHBoxLayout;
    totalLayout->setAlignment(Qt::AlignHCenter);
    totalLayout->setMargin(0);
    totalLayout->setSpacing(0);
    totalLayout->addWidget(k->totalLabel);

    k->layout->addLayout(totalLayout);

    k->applyButton = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    connect(k->applyButton, SIGNAL(clicked()), this, SLOT(applyTween()));
    k->applyButton->setEnabled(false);

    k->closeButton = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    connect(k->closeButton, SIGNAL(clicked()), this, SLOT(closeTweenProperties()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->applyButton);
    buttonsLayout->addWidget(k->closeButton);

    k->layout->addSpacing(10);
    k->layout->addLayout(buttonsLayout);
    k->layout->setSpacing(5);
}

PositionSettings::~PositionSettings()
{
    delete k;
}

// Adding new Tween

void PositionSettings::setParameters(int framesTotal, int startFrame)
{
    k->mode = TweenerPanel::Add;

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

void PositionSettings::updateSteps(const QGraphicsPathItem *path, QPointF offset)
{
    k->path = path;
    k->offset = offset;
    k->stepViewer->setPath(path);
    k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));

    if (!k->applyButton->isEnabled())
        k->applyButton->setEnabled(true);
}

int PositionSettings::totalSteps()
{
    return k->stepViewer->totalSteps();
}

void PositionSettings::applyTween()
{
    if (totalSteps() <= 2) {
        KOsd::self()->display(tr("Info"), tr("You must define a path for this Tween!"), KOsd::Info);
        return;
    }

    setEditMode();

    if (!k->comboInit->isEnabled())
        k->comboInit->setEnabled(true);

    QString text(tr("Position Tween is set!")); 

    emit clickedApplyTween(TweenerPanel::Position, text);
}

void PositionSettings::resetTween()
{
    k->stepViewer->cleanRows();
    k->totalLabel->setText(tr("Frames Total") + ": 0");
}

void PositionSettings::closeTweenProperties()
{
    if (k->mode == TweenerPanel::Add)
        resetTween(); 

    tFatal() << "PositionSettings::closeTweenProperties() - Mode: " << k->mode;

    emit clickedCloseTweenProperties(k->mode);
}

void PositionSettings::setEditMode()
{
    k->mode = TweenerPanel::Edit;
    k->closeButton->setIcon(QPixmap(THEME_DIR + "icons/close_properties.png"));
    k->closeButton->setToolTip(tr("Close Tween properties"));
}

/* This method transforms the path created into a QString representation */

QString PositionSettings::pathToCoords(const QGraphicsPathItem *path, QPointF offset)
{
    QString strPath = "";
    QChar t;
    int offsetX = offset.x();
    int offsetY = offset.y();

    for (int i=0; i < path->path().elementCount(); i++) {
         QPainterPath::Element e = path->path().elementAt(i);
         switch (e.type) {
            case QPainterPath::MoveToElement:
            {
                if (t != 'M') {
                    t = 'M';
                    strPath += "M " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                } else {
                    strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                }
            }
            break;
            case QPainterPath::LineToElement:
            {
                if (t != 'L') {
                    t = 'L';
                    strPath += " L " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                } else {
                    strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                }
            }
            break;
            case QPainterPath::CurveToElement:
            {
                if (t != 'C') {
                    t = 'C';
                    strPath += " C " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                } else {
                    strPath += "  " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                }
            }
            break;
            case QPainterPath::CurveToDataElement:
            {
                if (t == 'C')
                    strPath +=  " " + QString::number(e.x + offsetX) + "  " + QString::number(e.y + offsetY) + " ";
            }
            break;
        }
    }

    return strPath.trimmed();
}

QString PositionSettings::pathString()
{
    return pathToCoords(k->path, k->offset);
}

QVector<KTTweenerStep *> PositionSettings::steps()
{
    return k->stepViewer->steps();
}
