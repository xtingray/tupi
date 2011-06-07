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

#ifndef KDATETABLE_H
#define KDATETABLE_H

#include <QTableWidget>
#include <QDate>
#include "tglobal.h"

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_GUI_EXPORT KDateTable : public QTableWidget
{
    Q_OBJECT;
    public:
        KDateTable(QWidget *parent = 0);
        ~KDateTable();
        
        void setDate(const QDate &date);
        void setMonth(int month);
        
        int cellWidth() const;
        int cellHeight() const;
        
        QDate date() const;
        
    protected:
        void paintEvent(QPaintEvent *e);
        
    private:
        void setCellSize(int width, int height);
        QDate dateFromPosition(int position);
        
    private slots:
        QDate dateFromItem(QTableWidgetItem *item);
        
    signals:
        void dateChanged(const QDate &date);
        
    private:
        int m_cellWidth, m_cellHeight;
        QDate m_date;
};

#endif
