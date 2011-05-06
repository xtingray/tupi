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

#include "tweenertable.h"
#include "kpushbutton.h"
#include "kseparator.h"
#include "kdebug.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>

struct TweenerTable::Private
{
    QList<QCheckBox*> *checkList;
    QList<KPushButton*> *buttonList;
};

TweenerTable::TweenerTable(QWidget *parent) : QWidget(parent), k(new Private)
{
    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QStringList labels;
    labels << tr("Position") << tr("Rotation") << tr("Scale") << tr("Shear") << tr("Opacity") << tr("Coloring"); 

    k->checkList = new QList<QCheckBox*>();
    k->buttonList = new QList<KPushButton*>();

    innerLayout->addWidget(new KSeparator(Qt::Horizontal));

    for (int i = 0; i < labels.size(); ++i) {
         QHBoxLayout *itemLayout = new QHBoxLayout;
         itemLayout->setAlignment(Qt::AlignHCenter);
         itemLayout->setMargin(0);
         itemLayout->setSpacing(5);

         k->checkList->append(new QCheckBox(this));
         k->buttonList->append(new KPushButton(this, labels.at(i), 1, i));

         itemLayout->addWidget(k->checkList->at(i));
         connect(k->checkList->at(i), SIGNAL(stateChanged(int)), this, SLOT(enableTween(int)));

         itemLayout->addWidget(k->buttonList->at(i));
         connect(k->buttonList->at(i), SIGNAL(clicked(int, int)), this, SLOT(showTweenSettings(int, int)));

         k->buttonList->at(i)->setDisabled(true);
         innerLayout->addLayout(itemLayout);
    }

    innerLayout->addWidget(new KSeparator(Qt::Horizontal));
}

TweenerTable::~TweenerTable()
{
    delete k;
}

void TweenerTable::showTweenSettings(int column, int row)
{
    kFatal() << "TweenerTable::updatePath() - Row: " << row;
    emit callTweenerSettings(row);
}

void TweenerTable::enableTween(int state)
{
    Q_UNUSED(state);

    for (int i=0; i < k->checkList->size(); i++) {
         if (k->checkList->at(i)->isChecked()) {
             if (!k->buttonList->at(i)->isEnabled())
                 k->buttonList->at(i)->setEnabled(true);
         } else {
             if (k->buttonList->at(i)->isEnabled())
                 k->buttonList->at(i)->setEnabled(false);
         }
    }
}
