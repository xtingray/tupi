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

#include "kradiobuttongroup.h"
#include "kimagebutton.h"
#include "kdebug.h"
 
#include "configurator.h"
#include "settings.h"
#include "ktitemtweener.h"
#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "kosd.h"

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

    //KRadioButtonGroup *options;
    //StepsViewer *stepViewer;
    //QComboBox *combo;
    //QLabel *totalLabel;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
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

void Configurator::emitOptionChanged(int option)
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
                     // k->options->setCurrentIndex(0);
                     KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);   
                 }
             }
    }
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
    QString name = k->input->text();

    if (name.length() > 0) {
        QListWidgetItem *tweenerItem = new QListWidgetItem(k->tweensList);
        tweenerItem->setFont(QFont("verdana", 8));
        tweenerItem->setText(name);
        tweenerItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        k->input->clear();
        k->tweensList->setCurrentItem(tweenerItem);
        k->addButton->setDisabled(true);

        k->settingsPanel = new Settings;
        connect(k->settingsPanel, SIGNAL(clickedRemoveTween()), this, SLOT(removeTween()));

        k->settingsLayout->addWidget(k->settingsPanel);
    }
}

void Configurator::removeTween()
{
    kFatal() << "Configurator::removeTween() - Tracing!";

    QListWidgetItem *item = k->tweensList->currentItem();
    emit clickedRemoveTween(item->text());
    k->tweensList->takeItem(k->tweensList->row(item));

    if (k->tweensList->count() == 0)
        k->addButton->setDisabled(false);

    if (k->settingsPanel)
        k->settingsPanel->hide();
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
    k->selectionDone = flag;
}

void Configurator::showMenu(const QPoint &point)
{
    kFatal() << "Configurator::showMenu() - Opening menu!";

    if (k->tweensList->count() > 0) {
        QAction *remove = new QAction(tr("Remove"), this);
        connect(remove, SIGNAL(triggered()), this, SLOT(removeTween()));

        QMenu *menu = new QMenu(tr("Options"));
        menu->addAction(remove);
        QPoint globalPos = k->tweensList->mapToGlobal(point);
        menu->exec(globalPos); 
    }
}


