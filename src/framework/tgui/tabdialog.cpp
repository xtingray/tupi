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

#include "tabdialog.h"

TabDialog::TabDialog(QWidget *parent, bool modal) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_tabWidget = new TTabWidget(this);
    
    mainLayout->addWidget(m_tabWidget);
    setLayout(mainLayout);
    
    setupButtons(Cancel|Ok);
    
    setModal(modal);
}

TabDialog::TabDialog(int buttons, QWidget *parent, bool modal) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    m_tabWidget = new TTabWidget(this);
    mainLayout->addWidget(m_tabWidget);
    setupButtons(buttons);
    setModal(modal);
}

void TabDialog::setupButtons(int buttons)
{
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    
    if (buttons & Custom1) {
        QPushButton *customButton = new QPushButton(tr(""));
        connect(customButton, SIGNAL(clicked()), this, SLOT(custom1()));
        buttonLayout->addWidget(customButton);
        m_buttons.insert(Custom1, customButton);
    }
    
    if (buttons & Custom2) {
        QPushButton *customButton = new QPushButton(tr(""));
        connect(customButton, SIGNAL(clicked()), this, SLOT(custom2()));
        buttonLayout->addWidget(customButton);
        m_buttons.insert( Custom2, customButton);
    }
    
    if (buttons & Custom3) {
        QPushButton *customButton = new QPushButton(tr(""));
        connect(customButton, SIGNAL(clicked()), this, SLOT(custom3()));
        buttonLayout->addWidget(customButton);
        m_buttons.insert( Custom3, customButton);
    }
    
    if (buttons & Help) {
        QPushButton *helpButton = new QPushButton(tr("Help"));
        connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
        buttonLayout->addWidget(helpButton);
        m_buttons.insert( Help, helpButton);
    }
    
    if (buttons & Apply) {
        QPushButton *applyButton = new QPushButton(tr("Apply"));
        connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
        buttonLayout->addWidget(applyButton);
        m_buttons.insert( Apply, applyButton);
    }
    
    if (buttons & Cancel) {
        QPushButton *cancelButton = new QPushButton(tr("Cancel"));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
        buttonLayout->addWidget(cancelButton);
        m_buttons.insert(Cancel, cancelButton);
    }

    if (buttons & Ok) {
        QPushButton *okButton = new QPushButton(tr("OK"));
        connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
        okButton->setDefault(true);
        buttonLayout->addWidget(okButton);
        m_buttons.insert(Ok, okButton);
    }
    
    static_cast<QVBoxLayout *>(layout())->addLayout(buttonLayout);
}

TabDialog::~TabDialog()
{
}

void TabDialog::addTab(QWidget * child, const QString & label)
{
    m_tabWidget->addTab(child, label);
}

void TabDialog::addTab(QWidget * child, const QIcon & iconset, const QString & label)
{
    m_tabWidget->addTab(child, iconset, label);
}

QWidget *TabDialog::currentTab()
{
    return m_tabWidget->currentWidget();
}

void TabDialog::ok()
{
    accept();
}

void TabDialog::cancel()
{
    reject();
}

void TabDialog::apply()
{
}

void TabDialog::setButtonText(Button b, const QString &text)
{
    QPushButton *button = m_buttons[b];
    if (button)
        button->setText(text);
}

QPushButton *TabDialog::button(Button b)
{
    return m_buttons[b];
}

TTabWidget *TabDialog::tabWidget() const
{
    return m_tabWidget;
}

void TabDialog::enableTab(int index, bool enable)
{
    m_tabWidget->setTabEnabled(index, enable);
}
