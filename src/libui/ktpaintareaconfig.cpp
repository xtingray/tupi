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

#include "ktpaintareaconfig.h"
#include "kcolorbutton.h"
#include "tconfig.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

/**
 * This class handles the preferences dialog for the paint area settings.
 * Here is where the paint area parameters are set.
 * @author David Cuadrado
*/

KTPaintAreaConfig::KTPaintAreaConfig(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setupPage();
}

KTPaintAreaConfig::~KTPaintAreaConfig()
{
}

void KTPaintAreaConfig::setupPage()
{
    QFrame *page = new QFrame;
    QGridLayout *pageLayout = new QGridLayout(page);
    page->setFrameStyle(QFrame::Box | QFrame::Sunken);
    
    TCONFIG->beginGroup("PaintArea");
    
    pageLayout->addWidget(new QLabel(tr("Grid color")), 0, 0);
    m_gridColor = new KColorButton;
    pageLayout->addWidget(m_gridColor, 0, 1);
    
    m_gridColor->setColor(qvariant_cast<QColor>(TCONFIG->value("GridColor", QColor(Qt::gray))));
    
    pageLayout->addWidget(new QLabel(tr("Grid separation")), 1, 0);
    m_gridSeparation = new QSpinBox(this);
    
    pageLayout->addWidget(m_gridSeparation, 1, 1);
    
    m_gridSeparation->setMaximum(100);
    m_gridSeparation->setMinimum(10);
    m_gridSeparation->setValue(10);
    
    pageLayout->addWidget(new QLabel(tr("Background color")), 2, 0);
    m_backgroundColor = new KColorButton;
    pageLayout->addWidget(m_backgroundColor, 2, 1);
    
    m_backgroundColor->setColor(qvariant_cast<QColor>(TCONFIG->value("BackgroundColor", QColor(Qt::white))));
    
    pageLayout->addWidget(new QLabel(tr("Onion skin color")), 3, 0);
    m_onionSkinColor = new KColorButton;
    pageLayout->addWidget(m_onionSkinColor, 3, 1);
    
    m_onionSkinColor->setColor(qvariant_cast<QColor>(TCONFIG->value("OnionSkinColor", QColor(Qt::lightGray))));
    
    pageLayout->addWidget(new QLabel(tr("Onion skin background ")), 4, 0);
    m_onionSkinBackground = new KColorButton;
    pageLayout->addWidget(m_onionSkinBackground, 4, 1);
    
    m_onionSkinBackground->setColor(qvariant_cast<QColor>(TCONFIG->value("OnionSkinBackground", 
                    QColor(Qt::lightGray))));
    
    layout()->addWidget(page);
}

QColor KTPaintAreaConfig::gridColor() const
{
    return m_gridColor->color();
}

QColor KTPaintAreaConfig::backgroundColor() const
{
    return m_backgroundColor->color();
}

QColor KTPaintAreaConfig::onionSkinColor() const
{
    return m_onionSkinColor->color();
}

QColor KTPaintAreaConfig::onionSkinBackground() const
{
    return m_onionSkinBackground->color();
}

int KTPaintAreaConfig::gridSeparation() const
{
    return m_gridSeparation->value();
}
