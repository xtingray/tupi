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

#ifndef KXYSPINBOX_H
#define KXYSPINBOX_H

#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include "kglobal.h"

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_GUI_EXPORT KXYSpinBox : public QGroupBox
{
    Q_OBJECT
    public:
        KXYSpinBox(const QString &title, QWidget *parent = 0);
        ~KXYSpinBox();
        void setSingleStep(int step);
        void setMinimum(int min);
        void setMaximum(int max);
        void setX(int x);
        void setY(int y);
        int x();
        int y();
        void setModifyTogether(bool enable);

    private slots:
        void updateXValue();
        void updateYValue();
        void toggleModify();

    private:
        QLabel *m_textX;
        QLabel *m_textY;
        QSpinBox *m_x;
        QSpinBox *m_y;
        QPushButton *m_separator;
        bool m_modifyTogether;

    /*
    signals:
        void valueXChanged(double);
        void valueYChanged(double);
        void valueXYChanged(double, double);
    */
};

#endif
