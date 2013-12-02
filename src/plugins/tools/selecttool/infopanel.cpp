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

#include "infopanel.h"
#include <QBoxLayout>
#include <QTextEdit>
#include <QDir>

#include "timagebutton.h"
#include "tseparator.h"
#include "tglobal.h"
#include "tdebug.h"

InfoPanel::InfoPanel(QWidget *parent) :QWidget(parent)
{
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QBoxLayout *flipLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *flips = new QLabel(tr("Flips"));
    flips->setAlignment(Qt::AlignHCenter);
    flipLayout->addWidget(flips);
    mainLayout->addLayout(flipLayout);

    QBoxLayout *buttonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(0);

    TImageButton *horizontalFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/horizontal_flip.png"), 22);
    horizontalFlip->setToolTip(tr("Horizontal Flip"));
    TImageButton *verticalFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/vertical_flip.png"), 22);
    verticalFlip->setToolTip(tr("Vertical Flip"));
    TImageButton *crossedFlip = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/crossed_flip.png"), 22);
    crossedFlip->setToolTip(tr("Crossed Flip"));
    connect(horizontalFlip, SIGNAL(clicked()), this, SLOT(hFlip()));
    connect(verticalFlip, SIGNAL(clicked()), this, SLOT(vFlip()));
    connect(crossedFlip, SIGNAL(clicked()), this, SLOT(cFlip()));

    buttonsLayout->addWidget(horizontalFlip);
    buttonsLayout->addWidget(verticalFlip);
    buttonsLayout->addWidget(crossedFlip);

    mainLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(new TSeparator(Qt::Horizontal));

    QBoxLayout *orderLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *order = new QLabel(tr("Order"));
    order->setAlignment(Qt::AlignHCenter);
    orderLayout->addWidget(order);
    mainLayout->addLayout(orderLayout);

    QBoxLayout *orderButtonsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    orderButtonsLayout->setMargin(0);
    orderButtonsLayout->setSpacing(0);

    TImageButton *toBack = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/to_back.png"), 22);
    toBack->setToolTip(tr("Send object to back"));

    TImageButton *toBackOneLevel = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/to_back_one.png"), 22);
    toBackOneLevel->setToolTip(tr("Send object to back one level"));

    TImageButton *toFront = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/to_front.png"), 22);
    toFront->setToolTip(tr("Send object to front"));

    TImageButton *toFrontOneLevel = new TImageButton(QPixmap(kAppProp->themeDir() + QDir::separator() + "icons/to_front_one.png"), 22);
    toFrontOneLevel->setToolTip(tr("Send object to front one level"));

    connect(toBack, SIGNAL(clicked()), this, SLOT(sendToBack()));
    connect(toBackOneLevel, SIGNAL(clicked()), this, SLOT(sendToBackOneLevel()));
    connect(toFront, SIGNAL(clicked()), this, SLOT(sendToFront()));
    connect(toFrontOneLevel, SIGNAL(clicked()), this, SLOT(sendToFrontOneLevel()));

    orderButtonsLayout->addWidget(toBack);
    orderButtonsLayout->addWidget(toBackOneLevel);
    orderButtonsLayout->addWidget(toFront);
    orderButtonsLayout->addWidget(toFrontOneLevel);

    mainLayout->addLayout(orderButtonsLayout);

    mainLayout->addWidget(new TSeparator(Qt::Horizontal));

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Tips"));
    label->setAlignment(Qt::AlignHCenter); 
    layout->addWidget(label);

    mainLayout->addLayout(layout);

    QTextEdit *textArea = new QTextEdit; 

    // SQA: Check this code with several screen resolutions. It must looks good with everyone! 
    textArea->setFont(QFont("Arial", 8, QFont::Normal, false));
    textArea->setHtml("<p><b>" + tr("Rotation mode") + ":</b> " + tr("Double click on any node or shortcut Alt + R") + "</p>"); 
    textArea->append("<p><b>" + tr("Arrows") + ":</b> " +  tr("Movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Shift + Arrows") + ":</b> " +  tr("Slow movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Ctrl + Arrows") + ":</b> " +  tr("Fast movement on selection") + "</p>");
    textArea->append("<p><b>" + tr("Shift + Left Mouse Button") + ":</b> " +  tr("Proportional scaling on selection") + "</p>");

    QString text = textArea->document()->toPlainText();
    int height = (text.length()*270)/230;

    textArea->setFixedHeight(height);

    mainLayout->addWidget(textArea);
    mainLayout->addStretch(2);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::hFlip()
{
    emit callFlip(InfoPanel::Horizontal);
}

void InfoPanel::vFlip()
{
    emit callFlip(InfoPanel::Vertical);
}

void InfoPanel::cFlip()
{
    emit callFlip(InfoPanel::Crossed);
}

void InfoPanel::sendToBack()
{
    emit callOrderAction(InfoPanel::ToBack);
}

void InfoPanel::sendToBackOneLevel()
{
    emit callOrderAction(InfoPanel::ToBackOneLevel);
}

void InfoPanel::sendToFront()
{
    emit callOrderAction(InfoPanel::ToFront);
}

void InfoPanel::sendToFrontOneLevel()
{
    emit callOrderAction(InfoPanel::ToFrontOneLevel);
}

