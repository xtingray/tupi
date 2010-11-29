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

#include "kdebug.h"
#include "kapplication.h"

#include <QBoxLayout>


KTGradientCreator::KTGradientCreator(QWidget *parent)
 : QFrame(parent)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);

    layout->setSpacing(2);
    layout->setMargin(2);
    setLayout(layout);
    
    QBoxLayout *selectorAndViewer = new QBoxLayout(QBoxLayout::TopToBottom);
    
    m_selector = new KTGradientSelector(this);
    m_viewer = new KTGradientViewer(this);
    
    connect(m_viewer, SIGNAL(gradientChanged()), this, SLOT(emitGradientChanged()));
    layout->addLayout(selectorAndViewer);
    
    selectorAndViewer->addWidget(m_viewer);
    selectorAndViewer->addWidget(m_selector);
    selectorAndViewer->addStretch(2);
    
    connect(m_selector, SIGNAL(gradientChanged(const QGradientStops&)),this, SLOT(changeGradientStops(const QGradientStops&)));
    connect(m_selector, SIGNAL(arrowAdded()), this, SIGNAL(controlArrowAdded()));
    
    QBoxLayout *subLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addLayout(subLayout);
    
    m_type = new QComboBox(this);
    QStringList list;
    list << tr("Linear") << tr("Radial") << tr("Conical");
    m_type->addItems(list);
    connect(m_type, SIGNAL(activated(int)),this, SLOT(changeType(int)));
    subLayout->addWidget(m_type);
    
    m_spread = new QComboBox(this);
    list.clear();
    list << tr("Pad") << tr("Reflect") << tr("Repeat");
    m_spread->addItems(list);
    connect(m_spread, SIGNAL(activated(int)),this, SLOT(changeSpread(int)));
    subLayout->addWidget(m_spread);
    
    m_spinControl = new SpinControl(this);
    
    connect(m_spinControl, SIGNAL(angleChanged(int)), m_viewer, SLOT(changeAngle(int)));
    connect(m_spinControl, SIGNAL(radiusChanged(int)), m_viewer, SLOT(changeRadius(int)));
    subLayout->addWidget(m_spinControl);
    
    subLayout->setSpacing(2);
    subLayout->setMargin(2);
    
    setFrameStyle(QFrame::StyledPanel);
    
    m_spinControl->setSpin(QGradient::Type(0));
    m_spinControl->setRadius(50);
    
    subLayout->addStretch(2);
}


KTGradientCreator::~KTGradientCreator()
{
    KEND;
}

void KTGradientCreator::setCurrentColor(const QColor &color)
{
    m_selector->setCurrentColor(color);
    m_viewer->createGradient();
    emit gradientChanged(QBrush(m_viewer->gradient()));
}

int KTGradientCreator::gradientType()
{
    return m_type->currentIndex();
}

void KTGradientCreator::changeType(int type)
{
    m_viewer->changeType(type);
    
    m_spinControl->setSpin( QGradient::Type(type));
    adjustSize();
    emitGradientChanged();
}

void KTGradientCreator::changeSpread(int spread)
{
    m_viewer->setSpread(spread);
    emitGradientChanged();
}

void KTGradientCreator::changeGradientStops(const QGradientStops& stops)
{
    m_viewer->changeGradientStops(stops);
    emit gradientChanged(m_viewer->gradient());
}

void KTGradientCreator::setGradient(const QBrush & gradient)
{
    const QGradient *gr = gradient.gradient();

    m_type->setCurrentIndex(gr->type());
    m_spread->setCurrentIndex(gr->spread());
    m_selector->setStops(gr->stops());
    m_viewer->setGradient(gr);
    m_spinControl->setSpin(gr->type());

    if(gr->type() == QGradient::RadialGradient) {
        m_spinControl->setRadius((int) static_cast<const QRadialGradient*>(gr)->radius());
    } else if (gr->type() == QGradient::ConicalGradient) {
        m_spinControl->setAngle((int) static_cast<const QConicalGradient*>(gr)->angle());
    }
}

void KTGradientCreator::emitGradientChanged()
{
    m_viewer->changeGradientStops(m_selector->gradientStops());
    emit gradientChanged(m_viewer->gradient());
}

QBrush KTGradientCreator::currentGradient()
{
    return QBrush(m_viewer->gradient());
}

QSize KTGradientCreator::sizeHint () const
{
    QSize size = QFrame::sizeHint();
    
    return size.expandedTo(QApplication::globalStrut());
}
