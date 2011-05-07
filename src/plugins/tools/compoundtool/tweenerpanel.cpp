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

#include "tweenerpanel.h"
#include "positionsettings.h"
#include "kdebug.h"
#include "kradiobuttongroup.h"
// #include "ktitemtweener.h"
// #include "kttweenerstep.h"
#include "kimagebutton.h"
#include "kseparator.h"
#include "tweenertable.h"
#include "kosd.h"

#include <QLabel>
#include <QLineEdit>
#include <QBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHeaderView>

struct TweenerPanel::Private
{
    QWidget *tweenerTablePanel;
    QList<QWidget*> *panelList;

    QBoxLayout *layout;
    Mode mode;
    QLineEdit *input;
    KRadioButtonGroup *options;
    QComboBox *comboInit;
    QComboBox *comboEnd;

    TweenerTable *tweenerTable;

    QLabel *totalLabel;
    int totalSteps;

    bool selectionDone;
    bool propertiesDone;

    KImageButton *apply;
    KImageButton *remove;
};

TweenerPanel::TweenerPanel(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;
    k->propertiesDone = false;
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
    k->options->addItem(tr("Set Tweeners"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    k->apply = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    k->apply->setDisabled(true);
    // connect(k->apply, SIGNAL(clicked()), this, SLOT(applyTween()));

    k->remove = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    // connect(k->remove, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->apply);
    buttonsLayout->addWidget(k->remove);

    k->layout->addLayout(nameLayout);
    k->layout->addWidget(k->options);

    setTweenerTableForm();

    k->layout->addSpacing(10);
    k->layout->addLayout(buttonsLayout);
    k->layout->setSpacing(5);

    activateMode(TweenerPanel::Selection);
}

TweenerPanel::~TweenerPanel()
{
    delete k;
}

void TweenerPanel::setTweenerTableForm()
{
    k->tweenerTablePanel = new QWidget;

    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->tweenerTablePanel);
    innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QLabel *startingLabel = new QLabel(tr("Tweeners") + ": ");

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(startingLabel);

    k->tweenerTable = new TweenerTable;
    connect(k->tweenerTable, SIGNAL(callTweenerSettings(int)), this, SLOT(showTweenSettings(int)));

    innerLayout->addLayout(startLayout);
    innerLayout->addWidget(k->tweenerTable);

    k->layout->addWidget(k->tweenerTablePanel);
    activeTweenerTableForm(false);
}

void TweenerPanel::activeTweenerTableForm(bool enable)
{
    if (enable && !k->tweenerTablePanel->isVisible()) {
        k->propertiesDone = true;
        k->tweenerTablePanel->show();
    } else {
        k->propertiesDone = false;
        k->tweenerTablePanel->hide();
    }
}

void TweenerPanel::loadTweenComponents()
{
    QStringList labels;
    labels << tr("Position") << tr("Rotation") << tr("Scale") << tr("Shear") << tr("Opacity") << tr("Coloring");
    k->panelList = new QList<QWidget*>();

    for (int i = 0; i < labels.size(); ++i) {

         switch(i)  {
                case TweenerPanel::Position:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Position gui";
                     k->panelList->append(new PositionSettings());
                break;
                case TweenerPanel::Rotation:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Rotation gui";
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Scale:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Scale gui";
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Shear:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Shear gui";
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Opacity:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Opacity gui";
                     k->panelList->append(new QWidget); 
                break;
                case TweenerPanel::Coloring:
                     kFatal() << "TweenerPanel::showTweenSettings() - Opening Coloring gui";
                     k->panelList->append(new QWidget);
                break;
         }

         k->layout->addWidget(k->panelList->at(i));
         activeTweenComponent(i, false);
    }
}

void TweenerPanel::activeTweenComponent(int index, bool enable)
{
    if (enable && !k->panelList->at(index)->isVisible()) {
        k->panelList->at(index)->show();
    } else {
        k->panelList->at(index)->hide();
    }
}

// Adding new Tween

void TweenerPanel::setParameters(const QString &name, int framesTotal, int startFrame)
{
    k->mode = Add;
    k->input->setText(name);

    activateMode(TweenerPanel::Selection);
    k->apply->setToolTip(tr("Save Tween"));
    k->remove->setIcon(QPixmap(THEME_DIR + "icons/close.png"));
    k->remove->setToolTip(tr("Cancel Tween"));
}

void TweenerPanel::emitOptionChanged(int option)
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
                     activeTweenerTableForm(true);
                     emit clickedTweenProperties();
                 } else {
                     k->options->setCurrentIndex(0);
                     KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);
                 }
             }
    }
}

void TweenerPanel::activateMode(TweenerPanel::EditMode mode)
{
    k->options->setCurrentIndex(mode);
}

void TweenerPanel::notifySelection(bool flag)
{
    k->selectionDone = flag;
}

void TweenerPanel::showTweenSettings(int tweenType)
{
    switch(tweenType)  {
           case TweenerPanel::Position:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Position gui";
           break;
           case TweenerPanel::Rotation:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Rotation gui";
           break;
           case TweenerPanel::Scale:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Scale gui";
           break;
           case TweenerPanel::Shear:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Shear gui";
           break;
           case TweenerPanel::Opacity:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Opacity gui";
           break;
           case TweenerPanel::Coloring:
                kFatal() << "TweenerPanel::showTweenSettings() - Opening Coloring gui";
           break;
    }
}
