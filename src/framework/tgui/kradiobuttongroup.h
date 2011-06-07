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

#ifndef KRADIOBUTTONGROUP_H
#define KRADIOBUTTONGROUP_H

#include <QGroupBox>
#include <QRadioButton>
#include <QBoxLayout>
#include <QButtonGroup>
#include "tglobal.h"

/**
 * @author David Cuadrado
*/

class K_GUI_EXPORT KRadioButtonGroup : public QGroupBox
{
    Q_OBJECT

    public:
        KRadioButtonGroup(const QString &title, Qt::Orientation orientation, QWidget *parent = 0);
        ~KRadioButtonGroup();
        
        void addItem(const QString & text, int index = -1);
        void addItems(const QStringList &texts);
        
        int currentIndex() const;
        void setCurrentIndex(int index);
        
    private slots:
        void emitButtonId(QAbstractButton *button);
        
    signals:
        void clicked(int index);
        
    private:
        Qt::Orientation m_orientation;
        QBoxLayout *m_layout;
        QButtonGroup *m_buttonGroup;
        int currentButtonIndex;
};

#endif
