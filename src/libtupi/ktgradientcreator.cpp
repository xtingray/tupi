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

#include "ktgradientcreator.h"
#include "tdebug.h"
#include "tapplication.h"

#include <QBoxLayout>

struct KTGradientCreator::Private
{
    KTGradientSelector *selector;
    KTGradientViewer *viewer;
    QComboBox *type;
    QComboBox *spread;
    QSpinBox *radius;
    QSpinBox *angle;
    SpinControl *spinControl;
};

KTGradientCreator::KTGradientCreator(QWidget *parent) : QFrame(parent), k(new Private)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->setSpacing(2);
    layout->setMargin(2);
    setLayout(layout);
    
    QBoxLayout *selectorAndViewer = new QBoxLayout(QBoxLayout::TopToBottom);
    
    k->selector = new KTGradientSelector(this);
    k->viewer = new KTGradientViewer(this);
    
    connect(k->viewer, SIGNAL(gradientChanged()), this, SLOT(emitGradientChanged()));
    layout->addLayout(selectorAndViewer);
    
    selectorAndViewer->addWidget(k->viewer);
    selectorAndViewer->addWidget(k->selector);
    selectorAndViewer->addStretch(2);
    
    connect(k->selector, SIGNAL(gradientChanged(const QGradientStops&)),this, SLOT(changeGradientStops(const QGradientStops&)));
    connect(k->selector, SIGNAL(arrowAdded()), this, SIGNAL(controlArrowAdded()));
    
    QBoxLayout *subLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addLayout(subLayout);
    
    k->type = new QComboBox(this);
    QStringList list;
    list << tr("Linear") << tr("Radial") << tr("Conical");
    k->type->addItems(list);
    connect(k->type, SIGNAL(activated(int)),this, SLOT(changeType(int)));
    subLayout->addWidget(k->type);
    
    k->spread = new QComboBox(this);
    list.clear();
    list << tr("Pad") << tr("Reflect") << tr("Repeat");
    k->spread->addItems(list);
    connect(k->spread, SIGNAL(activated(int)),this, SLOT(changeSpread(int)));
    subLayout->addWidget(k->spread);
    
    k->spinControl = new SpinControl(this);
    
    connect(k->spinControl, SIGNAL(angleChanged(int)), k->viewer, SLOT(changeAngle(int)));
    connect(k->spinControl, SIGNAL(radiusChanged(int)), k->viewer, SLOT(changeRadius(int)));
    subLayout->addWidget(k->spinControl);
    
    subLayout->setSpacing(2);
    subLayout->setMargin(2);
    
    setFrameStyle(QFrame::StyledPanel);
    
    k->spinControl->setSpin(QGradient::Type(0));
    k->spinControl->setRadius(50);
    
    subLayout->addStretch(2);
}

KTGradientCreator::~KTGradientCreator()
{
}

void KTGradientCreator::setCurrentColor(const QColor &color)
{
    k->selector->setCurrentColor(color);
    k->viewer->createGradient();

    emit gradientChanged(QBrush(k->viewer->gradient()));
}

int KTGradientCreator::gradientType()
{
    return k->type->currentIndex();
}

void KTGradientCreator::changeType(int type)
{
    k->viewer->changeType(type);
    
    k->spinControl->setSpin( QGradient::Type(type));
    adjustSize();

    emitGradientChanged();
}

void KTGradientCreator::changeSpread(int spread)
{
    k->viewer->setSpread(spread);

    emitGradientChanged();
}

void KTGradientCreator::changeGradientStops(const QGradientStops& stops)
{
    k->viewer->changeGradientStops(stops);

    emit gradientChanged(k->viewer->gradient());
}

void KTGradientCreator::setGradient(const QBrush & brush)
{
    const QGradient *gradient = brush.gradient();

    k->type->setCurrentIndex(gradient->type());
    k->spread->setCurrentIndex(gradient->spread());
    k->selector->setStops(gradient->stops());
    k->viewer->setGradient(gradient);
    k->spinControl->setSpin(gradient->type());

    if (gradient->type() == QGradient::RadialGradient) {
        k->spinControl->setRadius((int) static_cast<const QRadialGradient*>(gradient)->radius());
    } else if (gradient->type() == QGradient::ConicalGradient) {
               k->spinControl->setAngle((int) static_cast<const QConicalGradient*>(gradient)->angle());
    }
}

void KTGradientCreator::emitGradientChanged()
{
    k->viewer->changeGradientStops(k->selector->gradientStops());
    emit gradientChanged(k->viewer->gradient());
}

QBrush KTGradientCreator::currentGradient()
{
    return QBrush(k->viewer->gradient());
}

QSize KTGradientCreator::sizeHint () const
{
    QSize size = QFrame::sizeHint();
    
    return size.expandedTo(QApplication::globalStrut());
}
