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

#ifndef KDATEPICKER_H
#define KDATEPICKER_H

#include <QFrame>
#include <QToolButton>

#include "kdatetable.h"
#include "kglobal.h"

class QComboBox;
class QLabel;

/**
 * 
 * @author David Cuadrado <krawek@gmail.com>
 * 
*/

class K_GUI_EXPORT KDatePicker : public QFrame
{
    Q_OBJECT;
    public:
        KDatePicker(QWidget *parent = 0);
        ~KDatePicker();
        void setDate(const QDate &date);
        QDate date() const;
        
    private:
        void fillWeeks(const QDate &date);
        
    public slots:
        void setWeek(int week);
        void setYear(int year);
        
    protected slots:
        void previousYear();
        void nextYear();
        
        void previousMounth();
        void nextMounth();
        
    private slots:
        void mounthFromAction(QAction *act);
        
    signals:
        void dateChanged(const QDate &date);
        
    private:
        QComboBox *m_week;
        KDateTable *m_dateTable;
        
        class EditableButton;
        
         QToolButton *m_mounth;
         EditableButton *m_year;
};

class KDatePicker::EditableButton : public QToolButton
{
    Q_OBJECT
    public:
        EditableButton();
        ~EditableButton();
        
    public slots:
        void edit();
        
    private slots:
        void emitYearSelected();
        
    signals:
        void yearSelected(int year);
        
    private:
        QLineEdit *m_editor;
};

#endif
