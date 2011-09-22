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

#include "kxyspinbox.h"
#include "tapplication.h"
#include "tdebug.h"

#include <QHBoxLayout>
#include <QSizePolicy>

KXYSpinBox::KXYSpinBox(const QString &title, QWidget *parent) : QGroupBox(title, parent), m_modifyTogether(false)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QGridLayout *internal = new QGridLayout;
    m_textX = new QLabel("X: ");
    internal->addWidget(m_textX, 0, 0, Qt::AlignRight);

    m_x = new QSpinBox;
    m_x->setMinimumWidth(60);
    internal->addWidget(m_x, 0, 1);
    m_textX->setBuddy(m_x);

    m_textY = new QLabel("Y: ");
    internal->addWidget(m_textY, 1, 0, Qt::AlignRight);

    m_y = new QSpinBox;
    m_y->setMinimumWidth(60);
    internal->addWidget(m_y, 1, 1);

    m_textY->setBuddy(m_y);
    layout->addLayout(internal);

    m_separator = new QPushButton;
    // m_separator->setFlat(true);
    m_separator->setMaximumWidth(20);
    m_separator->setIcon(QPixmap(THEME_DIR + "icons/svg.png"));

    // layout->addWidget(m_separator);

    connect(m_separator, SIGNAL(clicked()), this, SLOT(toggleModify()));
    setLayout(layout);

    connect(m_x, SIGNAL(editingFinished()), this, SLOT(updateYValue()));
    connect(m_y, SIGNAL(editingFinished()), this, SLOT(updateXValue()));

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

KXYSpinBox::~KXYSpinBox()
{
}

void KXYSpinBox::updateXValue()
{
     int value = (int) m_x->value()*380/520;
     m_y->setValue(value);

     /*
     if (m_modifyTogether) {
         int y = m_y->value();
         if (m_x->value() != y)
             m_x->setValue(y);
     }
     */
}

void KXYSpinBox::updateYValue()
{
     int value = (int) m_y->value()*520/380; 
     m_x->setValue(value);

     /*
     if (m_modifyTogether) {
         int x = m_x->value();
         if (m_y->value() != x)
             m_y->setValue(x);
     }
     */
}

void KXYSpinBox::setModifyTogether(bool enable)
{
    m_modifyTogether = enable;
    toggleModify();
}

void KXYSpinBox::toggleModify()
{
    if (!m_modifyTogether) {
        m_modifyTogether = true;
        m_separator->setIcon(QPixmap(THEME_DIR + "icons/padlock.png"));

        /*
        int x = m_x->value();
        if (m_y->value() != x)
            m_y->setValue(x);
        */

    } else {
        m_modifyTogether = false;
        m_separator->setIcon(QPixmap(THEME_DIR + "icons/svg.png")); // open padlock
    }
}

void KXYSpinBox::setSingleStep(int step)
{
    m_x->setSingleStep(step);
    m_y->setSingleStep(step);
}

void KXYSpinBox::setMinimum(int min)
{
    m_x->setMinimum(min);
    m_y->setMinimum(min);
}

void KXYSpinBox::setMaximum(int max)
{
    m_x->setMaximum(max);
    m_y->setMaximum(max);
}

void KXYSpinBox::setX(int x)
{
    m_x->setValue(x);
}

void KXYSpinBox::setY(int y)
{
    m_y->setValue(y);
}

int KXYSpinBox::x()
{
    return m_x->value();
}

int KXYSpinBox::y()
{
    return m_y->value();
}
