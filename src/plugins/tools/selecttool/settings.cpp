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
#include "timagebutton.h"
#include "tseparator.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QTextEdit>
#include <QSpinBox>
#include <QDir>

struct Settings::Private
{
    QWidget *tools;
    QWidget *help;
    QSpinBox *xPosField;
    QSpinBox *yPosField;
    QPushButton *tips;
};

Settings::Settings(QWidget *parent) : QWidget(parent), k(new Private)
{
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    k->tools = new QWidget(this);
    QBoxLayout *toolsLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->tools);

    QLabel *flips = new QLabel(tr("Flips"));
    flips->setFont(QFont("Arial", 8, QFont::Normal, false));
    flips->setAlignment(Qt::AlignHCenter);
    toolsLayout->addWidget(flips);

    QBoxLayout *buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(0);

    TImageButton *horizontalFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() 
                                                    + "horizontal_flip.png"), 22);
    horizontalFlip->setToolTip(tr("Horizontal Flip"));
    TImageButton *verticalFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() 
                                                  + "vertical_flip.png"), 22);
    verticalFlip->setToolTip(tr("Vertical Flip"));
    TImageButton *crossedFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() 
                                                 + "crossed_flip.png"), 22);
    crossedFlip->setToolTip(tr("Crossed Flip"));
    connect(horizontalFlip, SIGNAL(clicked()), this, SLOT(hFlip()));
    connect(verticalFlip, SIGNAL(clicked()), this, SLOT(vFlip()));
    connect(crossedFlip, SIGNAL(clicked()), this, SLOT(cFlip()));

    buttonsLayout->addWidget(horizontalFlip);
    buttonsLayout->addWidget(verticalFlip);
    buttonsLayout->addWidget(crossedFlip);

    toolsLayout->addLayout(buttonsLayout);

    toolsLayout->addWidget(new TSeparator(Qt::Horizontal));

    QLabel *order = new QLabel(tr("Order"));
    order->setFont(QFont("Arial", 8, QFont::Normal, false));
    order->setAlignment(Qt::AlignHCenter);
    toolsLayout->addWidget(order);

    QBoxLayout *orderButtonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    orderButtonsLayout->setMargin(0);
    orderButtonsLayout->setSpacing(0);

    TImageButton *toBack = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "to_back.png"), 22);
    toBack->setToolTip(tr("Send object to back"));

    TImageButton *toBackOneLevel = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "to_back_one.png"), 22);
    toBackOneLevel->setToolTip(tr("Send object to back one level"));

    TImageButton *toFront = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "to_front.png"), 22);
    toFront->setToolTip(tr("Send object to front"));

    TImageButton *toFrontOneLevel = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons" + QDir::separator() + "to_front_one.png"), 22);
    toFrontOneLevel->setToolTip(tr("Send object to front one level"));

    connect(toBack, SIGNAL(clicked()), this, SLOT(sendToBack()));
    connect(toBackOneLevel, SIGNAL(clicked()), this, SLOT(sendToBackOneLevel()));
    connect(toFront, SIGNAL(clicked()), this, SLOT(sendToFront()));
    connect(toFrontOneLevel, SIGNAL(clicked()), this, SLOT(sendToFrontOneLevel()));

    orderButtonsLayout->addWidget(toBack);
    orderButtonsLayout->addWidget(toBackOneLevel);
    orderButtonsLayout->addWidget(toFront);
    orderButtonsLayout->addWidget(toFrontOneLevel);

    toolsLayout->addLayout(orderButtonsLayout);

    toolsLayout->addWidget(new TSeparator(Qt::Horizontal));

    QLabel *position = new QLabel(tr("Position"));
    position->setFont(QFont("Arial", 8, QFont::Normal, false));
    position->setAlignment(Qt::AlignHCenter);
    toolsLayout->addWidget(position);

    k->xPosField = new QSpinBox;
    k->xPosField->setMinimum(-2000);
    k->xPosField->setMaximum(2000);
    k->yPosField = new QSpinBox;
    k->yPosField->setMinimum(-2000);
    k->yPosField->setMaximum(2000);

    k->xPosField->setEnabled(false);
    k->yPosField->setEnabled(false);

    toolsLayout->addWidget(k->xPosField);
    toolsLayout->addWidget(k->yPosField);

    toolsLayout->addWidget(new TSeparator(Qt::Horizontal));

    mainLayout->addWidget(k->tools);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->tips = new QPushButton(tr("Show Tips"));
    k->tips->setToolTip(tr("A little help for the Selection tool")); 
    k->tips->setFont(QFont("Arial", 8, QFont::Normal, false));
    layout->addWidget(k->tips);
    connect(k->tips, SIGNAL(clicked()), this, SLOT(openTipPanel())); 

    mainLayout->addLayout(layout);

    k->help = new QWidget(this);
    k->help->hide();
    QBoxLayout *helpLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->help);

    QTextEdit *textArea = new QTextEdit; 

    // SQA: Check this code with several screen resolutions. It must looks good with everyone! 
    textArea->setFont(QFont("Arial", 8, QFont::Normal, false));
    textArea->setHtml("<p><b>" + tr("Rotation mode") + ":</b> " + tr("Double click on any node or shortcut Alt + R") + "</p>"); 
    textArea->append("<p><b>" + tr("Arrows") + ":</b> " +  tr("Movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Shift + Arrows") + ":</b> " +  tr("Slow movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Ctrl + Arrows") + ":</b> " +  tr("Fast movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Shift + Left Mouse Button") + ":</b> " +  tr("Proportional scaling on selection") + "</p>");

    QString text = textArea->document()->toPlainText();
    int height = (text.length()*270)/200;
    textArea->setFixedHeight(height);

    helpLayout->addWidget(textArea); 

    mainLayout->addWidget(k->help);
    mainLayout->addStretch(2);
}

Settings::~Settings()
{
}

void Settings::hFlip()
{
    emit callFlip(Settings::Horizontal);
}

void Settings::vFlip()
{
    emit callFlip(Settings::Vertical);
}

void Settings::cFlip()
{
    emit callFlip(Settings::Crossed);
}

void Settings::sendToBack()
{
    emit callOrderAction(Settings::ToBack);
}

void Settings::sendToBackOneLevel()
{
    emit callOrderAction(Settings::ToBackOneLevel);
}

void Settings::sendToFront()
{
    emit callOrderAction(Settings::ToFront);
}

void Settings::sendToFrontOneLevel()
{
    emit callOrderAction(Settings::ToFrontOneLevel);
}

void Settings::openTipPanel() {
    if (k->tools->isVisible()) {
        k->tools->hide();
        k->tips->setText(tr("Show Tools"));
        k->tips->setToolTip(tr("Tools panel for the Selection tool"));
        k->help->show();
    } else {
        k->tools->show();
        k->tips->setText(tr("Show Tips"));
        k->tips->setToolTip(tr("A little help for the Selection tool"));
        k->help->hide();
    }
}

void Settings::setPos(int x, int y) {
    if (!k->xPosField->isEnabled())
        enablePositionControls(true);

    k->xPosField->setValue(x);
    k->yPosField->setValue(y);
}

void Settings::enablePositionControls(bool flag)
{
    if (!flag) {
        k->xPosField->setValue(0);
        k->yPosField->setValue(0);
    }

    k->xPosField->setEnabled(flag);
    k->yPosField->setEnabled(flag);
}
