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

#include "configurator.h"
#include "buttonspanel.h"
#include "ktitemtweener.h"
#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "kosd.h"
#include "kradiobuttongroup.h"
#include "kimagebutton.h"
#include "kdebug.h"

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QBoxLayout>
#include <QHeaderView>
#include <QGraphicsPathItem>
#include <QMenu>
#include <QAction>

struct Configurator::Private
{
    QBoxLayout *layout; 
    QBoxLayout *settingsLayout;
    Settings *settingsPanel;
    ButtonsPanel *controlPanel;

    QLineEdit *input;
    QListWidget *tweensList;
    bool selectionDone;

    KImageButton *addButton;
    KImageButton *removeButton;
    KImageButton *editButton;

    Settings::Mode mode; 
    GuiState state;

    int framesTotal;
    int currentFrame;

    KTItemTweener *currentTween;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->framesTotal = 1;
    k->currentFrame = 0;

    k->mode = Settings::View;
    k->selectionDone = false;
    k->state = Clean;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QLabel *title = new QLabel(tr("Motion Tween"));
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(QFont("Arial", 8, QFont::Bold));

    k->layout->addWidget(title);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    k->input = new QLineEdit;
    k->addButton = new KImageButton(QPixmap(THEME_DIR + "icons/plus_sign.png"), 22);
    k->addButton->setToolTip(tr("Create a new Tween"));
    connect(k->input, SIGNAL(returnPressed()), this, SLOT(addTween()));
    connect(k->addButton, SIGNAL(clicked()), this, SLOT(addTween()));

    QHBoxLayout *lineLayout = new QHBoxLayout;
    lineLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lineLayout->setMargin(0);
    lineLayout->setSpacing(0);
    lineLayout->addWidget(k->input);
    lineLayout->addWidget(k->addButton);

    k->layout->addLayout(lineLayout);

    QBoxLayout *listLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    listLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    k->tweensList = new QListWidget;
    k->tweensList->setContextMenuPolicy(Qt::CustomContextMenu);
    k->tweensList->setViewMode(QListView::ListMode);
    k->tweensList->setFlow(QListView::TopToBottom);
    k->tweensList->setMovement(QListView::Static);
    k->tweensList->setFixedHeight(68);
    connect(k->tweensList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showMenu(const QPoint &)));
    connect(k->tweensList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(updateTweenData(QListWidgetItem *)));

    listLayout->addWidget(k->tweensList);
    k->layout->addLayout(listLayout);

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    k->settingsLayout->setMargin(0);
    k->settingsLayout->setSpacing(0);

    setPropertiesPanel();

    k->layout->addLayout(k->settingsLayout);
    k->layout->addStretch(2);
}

Configurator::~Configurator()
{
    delete k;
}

void Configurator::loadTweenList(QList<QString> tweenList)
{
    for (int i=0; i < tweenList.size(); i++) {
        QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
        tweenerItem->setFont(QFont("verdana", 8));
        tweenerItem->setText(tweenList.at(i));
        tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
}

void Configurator::setPropertiesPanel()
{
    k->settingsPanel = new Settings(this);

    connect(k->settingsPanel, SIGNAL(startingPointChanged(int)), this, SIGNAL(startingPointChanged(int)));

    connect(k->settingsPanel, SIGNAL(clickedSelect()), this, SIGNAL(clickedSelect()));
    connect(k->settingsPanel, SIGNAL(clickedCreatePath()), this, SIGNAL(clickedCreatePath()));

    connect(k->settingsPanel, SIGNAL(clickedApplyTween()), this, SLOT(applyItem()));
    connect(k->settingsPanel, SIGNAL(clickedResetTween()), this, SLOT(closeTweenProperties()));

    k->settingsLayout->addWidget(k->settingsPanel);

    activePropertiesPanel(false);
}

void Configurator::activePropertiesPanel(bool enable)
{
    if (enable)
        k->settingsPanel->show();
    else
        k->settingsPanel->hide();
}

void Configurator::setButtonsPanel()
{
    kFatal() << "Configurator::setButtonsPanel() - Setting panel!";

    k->controlPanel = new ButtonsPanel(this);
    connect(k->controlPanel, SIGNAL(clickedEditTween()), this, SLOT(editTween()));
    connect(k->controlPanel, SIGNAL(clickedRemoveTween()), this, SLOT(removeTween()));

    k->settingsLayout->addWidget(k->controlPanel);

    k->state = Buttons; 
}

void Configurator::activeButtonsPanel(bool enable)
{
    if (enable)
        k->controlPanel->show();
    else
        k->controlPanel->hide();
}

void Configurator::initStartCombo(int framesTotal, int currentFrame)
{
    k->framesTotal = framesTotal;
    k->currentFrame = currentFrame;
    k->settingsPanel->initStartCombo(framesTotal, currentFrame);
}

void Configurator::setStartFrame(int currentIndex)
{
    k->settingsPanel->setStartFrame(currentIndex);
}

int Configurator::startFrame()
{
    return k->settingsPanel->startFrame();
}

int Configurator::startComboSize()
{
    return k->settingsPanel->startComboSize();
}

void Configurator::updateSteps(const QGraphicsPathItem *path)
{
    k->settingsPanel->updateSteps(path);
}

QString Configurator::tweenToXml(int currentFrame, QString &path)
{
    return k->settingsPanel->tweenToXml(currentFrame, path);
}

int Configurator::totalSteps()
{
    return k->settingsPanel->totalSteps();
}

void Configurator::activatePathMode()
{
    //k->options->setCurrentIndex(1);
    //k->stepViewer->cleanRows();
}

void Configurator::activateSelectionMode()
{
    //k->options->setCurrentIndex(0);
}

void Configurator::cleanData()
{
    k->settingsPanel->cleanData();
}

void Configurator::addTween()
{
    kFatal() << "Configurator::addTween() - Adding new Tween!";

    if (k->mode == Settings::View) {

        QString name = k->input->text();

        if (name.length() > 0) {

            if (!itemExists(name)) {

                QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
                tweenerItem->setFont(QFont("verdana", 8));
                tweenerItem->setText(name);
                tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                k->input->clear();
                k->tweensList->setCurrentItem(tweenerItem);
                k->addButton->setDisabled(true);

                if (k->state == Buttons)
                    activeButtonsPanel(false);

                k->mode = Settings::Add;

                activePropertiesPanel(true);
                k->settingsPanel->setParameters(name, k->mode, k->framesTotal, k->currentFrame);

                k->state = Properties;

                emit selectionModeOn();

            } else {
                KOsd::self()->display(tr("Error"), tr("Tween name already exists!"), KOsd::Error);        
            }
        }

    } else {
        KOsd::self()->display(tr("Info"), tr("Save or Cancel current Tween before adding a new one"), KOsd::Info);
        k->input->clear();
    }
}

void Configurator::editTween()
{
    kFatal() << "Configurator::editTween() - Editing the current Tween!";

    k->addButton->setDisabled(true);

    if (k->state == Buttons)
        activeButtonsPanel(false);

    k->mode = Settings::Edit;
    k->state = Properties;

    k->settingsPanel->setParameters(k->currentTween);
    k->settingsPanel->show();
}

void Configurator::closeTweenProperties()
{
    kFatal() << "Configurator::closeTweenProperties() - Tracing mode: " << k->mode;

    if (k->mode == Settings::Add) {
        kFatal() << "Configurator::closeTweenProperties() - removing Tween!";
        removeTween();
    } else if (k->mode == Settings::Edit) {
        kFatal() << "Configurator::closeTweenProperties() - edit Mode!";
        closeSettingsPanel();
        setButtonsPanel();
        k->addButton->setDisabled(false);
        k->mode = Settings::View;
        emit clickedResetInterface();
    }

    closeSettingsPanel();
}

void Configurator::removeTween()
{
    kFatal() << "Configurator::removeTween() - Removing Item!";

    QListWidgetItem *item = k->tweensList->currentItem();
    k->tweensList->takeItem(k->tweensList->row(item));

    if (k->tweensList->count() == 0) {
        k->addButton->setDisabled(false);
        if (k->state == Buttons) {
            activeButtonsPanel(false);
        }
    }

    emit clickedRemoveTween(item->text());
}

QString Configurator::currentTweenName() const
{
    QListWidgetItem *item = k->tweensList->currentItem();
    if (item)
        return item->text();
    else
        return "";
}

void Configurator::notifySelection(bool flag)
{
    kFatal() << "Configurator::notifySelection() - Updating selection flag: " << flag;

    if (k->mode != Settings::View)
        k->settingsPanel->notifySelection(flag); 
}

void Configurator::showMenu(const QPoint &point)
{
    kFatal() << "Configurator::showMenu() - Opening menu!";

    if (k->tweensList->count() > 0 && (k->mode == Settings::View)) {
        QAction *edit = new QAction(tr("Edit"), this);
        connect(edit, SIGNAL(triggered()), this, SLOT(editTween()));
        QAction *remove = new QAction(tr("Remove"), this);
        connect(remove, SIGNAL(triggered()), this, SLOT(removeTween()));

        QMenu *menu = new QMenu(tr("Options"));
        menu->addAction(edit);
        menu->addAction(remove);
        QPoint globalPos = k->tweensList->mapToGlobal(point);
        menu->exec(globalPos); 
    }
}

void Configurator::closeSettingsPanel()
{
    if (k->state == Properties) {
        activePropertiesPanel(false);
        k->mode = Settings::View;
    }
}

Settings::Mode Configurator::mode()
{
    return k->mode;
}

bool Configurator::itemExists(const QString &name)
{
     for (int i=0; i < k->tweensList->count(); i++) {
          QListWidgetItem *item = k->tweensList->item(i);
          if (name.compare(item->text()) == 0)
              return true;
     }

     return false;
}

void Configurator::applyItem()
{
     k->mode = Settings::Edit;
     emit clickedApplyTween();
}

void Configurator::resetUI()
{
    if (k->tweensList->count() > 0)
        k->tweensList->clear();

    if (!k->addButton->isEnabled())
        k->addButton->setDisabled(false);

    closeSettingsPanel();

    k->state = Clean;
}

void Configurator::updateTweenData(QListWidgetItem *item)
{
    if (k->mode == Settings::View) {
        kFatal() << "Configurator::updateTweenData() - Item: " << item->text();
        emit getTweenData(item->text());
    }
}

void Configurator::setCurretTween(KTItemTweener *currentTween)
{
    kFatal() << "Configurator::setCurretTween() - Tracing VAR";

    k->currentTween = currentTween;
}
