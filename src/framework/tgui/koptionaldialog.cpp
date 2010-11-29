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

#include "koptionaldialog.h"
#include "kseparator.h"

#include <QVBoxLayout>
#include <QLabel>

KOptionalDialog::KOptionalDialog(const QString &text,const QString &title,QWidget *parent) : QDialog(parent)
{
    setWindowTitle(title);
    m_layout = new QVBoxLayout;
    
    m_layout->addStretch(10);
    
    QLabel *label = new QLabel(text, this);
    
    m_layout->addWidget(label);
    
    m_layout->addStretch(10);
    
    m_layout->addWidget(new KSeparator);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    
    m_checkBox = new QCheckBox(tr("Don't show again"));
    buttonLayout->addWidget(m_checkBox);
    
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancelButton);
    
    QPushButton *okButton = new QPushButton(tr("Accept"));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okButton);
    
    m_layout->addLayout(buttonLayout);
    setLayout(m_layout);
}

KOptionalDialog::~KOptionalDialog()
{
}

bool KOptionalDialog::shownAgain()
{
    return m_checkBox->isChecked();
}
