/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tuprotationdial.h"

#include <QBoxLayout>
#include <QDial>
#include <QLabel>

struct TupRotationDial::Private
{
    QDial *dial;
    QLabel *label;
};

TupRotationDial::TupRotationDial(QWidget *parent) : QDialog(parent, Qt::CustomizeWindowHint), k(new Private)
{
    setModal(true);
    setAttribute(Qt::WA_TranslucentBackground);

    QBoxLayout *layout = new QVBoxLayout(this);
    k->dial = new QDial;
    connect(k->dial, SIGNAL(valueChanged(int)), this, SLOT(updateAngle(int)));
    k->dial->setRange(0, 360);
    layout->addWidget(k->dial);
    k->label = new QLabel;
    k->label->setAttribute(Qt::WA_TranslucentBackground);
    k->label->setAlignment(Qt::AlignHCenter);
    QFont f = font();
    f.setBold(true);
    f.setPointSize(f.pointSizeF() + 5);
    k->label->setFont(f);
    layout->addWidget(k->label);
}

TupRotationDial::~TupRotationDial()
{
}

void TupRotationDial::updateAngle(int angle)
{
    k->label->setText(QString::number(angle));
    emit valueChanged(angle);
}

void TupRotationDial::setAngle(int angle)
{
    k->dial->setSliderPosition(angle);
    k->label->setText(QString::number(angle)); 
}

void TupRotationDial::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    close();
}
