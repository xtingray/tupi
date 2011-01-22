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
#include <QPushButton>
#include <QHeaderView>
#include <QGraphicsPathItem>
#include <QMenu>
#include <QAction>

struct Configurator::Private
{
    QBoxLayout *layout; 
    QBoxLayout *settingsLayout;
    Settings *settingsPanel;

    QLineEdit *input;
    QListWidget *tweensList;
    bool selectionDone;

    KImageButton *addButton;
    KImageButton *removeButton;
    KImageButton *editButton;

    Settings::Mode mode; 
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->mode = Settings::View;
    k->selectionDone = false;

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

    listLayout->addWidget(k->tweensList);
    k->layout->addLayout(listLayout);

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    k->layout->addLayout(k->settingsLayout);
    k->layout->addStretch(2);
    k->layout->setSpacing(5);
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

void Configurator::addButtonsPanel()
{
    k->editButton = new KImageButton(QPixmap(THEME_DIR + "icons/tweener.png"), 22);
    k->removeButton = new KImageButton(QPixmap(THEME_DIR + "icons/minus_sign.png"), 22);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    controlLayout->setMargin(1);
    controlLayout->setSpacing(5);
    controlLayout->addWidget(k->editButton);
    controlLayout->addWidget(k->removeButton);

    k->settingsLayout->addLayout(controlLayout);
}

void Configurator::initStartCombo(int framesTotal, int currentIndex)
{
    /*
    k->combo->clear();
    for (int i=1; i<=framesTotal; i++)
         k->combo->addItem(QString::number(i));

    k->combo->setCurrentIndex(currentIndex);
    */
}

void Configurator::setStartFrame(int currentIndex)
{
    //k->combo->setCurrentIndex(currentIndex);
}

int Configurator::startFrame()
{
    //return k->combo->currentIndex();
    return 0;
}

int Configurator::startComboSize()
{
    //return k->combo->count();
    return 1;
}

void Configurator::updateSteps(const QGraphicsPathItem *path)
{
    //k->stepViewer->setPath(path);
    //k->totalLabel->setText(tr("Frames Total") + ": " + QString::number(k->stepViewer->totalSteps()));
}

QString Configurator::tweenToXml(int currentFrame, QString path)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", currentTweenName());
    root.setAttribute("init", currentFrame);
    //root.setAttribute("frames", k->stepViewer->totalSteps());
    root.setAttribute("frames", 0);
    root.setAttribute("coords", path);

    //foreach (KTTweenerStep *step, k->stepViewer->steps())
    //         root.appendChild(step->toXml(doc));

    doc.appendChild(root);

    return doc.toString();
}

int Configurator::totalSteps()
{
    //return k->stepViewer->totalSteps();
    return 0;
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
    //k->stepViewer->cleanRows();
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

                k->mode = Settings::Add;
                k->settingsPanel = new Settings(this, k->mode, 1, 0);
                connect(k->settingsPanel, SIGNAL(clickedSelect()), this, SIGNAL(clickedSelect()));
                connect(k->settingsPanel, SIGNAL(clickedCreatePath()), this, SIGNAL(clickedCreatePath()));
                connect(k->settingsPanel, SIGNAL(clickedResetTween()), this, SLOT(resetTween()));

                k->settingsLayout->addWidget(k->settingsPanel);
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
    k->mode = Settings::Edit;
}

void Configurator::resetTween()
{
    kFatal() << "Configurator::resetTween() - Tracing!";

    if (k->mode == Settings::Add)
        removeTween();

    closeSettingsPanel();
}

void Configurator::removeTween()
{
    QListWidgetItem *item = k->tweensList->currentItem();
    emit clickedRemoveTween(item->text());
    k->tweensList->takeItem(k->tweensList->row(item));

    if (k->tweensList->count() == 0)
        k->addButton->setDisabled(false);
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

    if (k->tweensList->count() > 0 && (k->mode == Settings::Edit)) {
        QAction *edit = new QAction(tr("Remove"), this);
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
    if (k->settingsPanel) {
        k->settingsPanel->hide();
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
