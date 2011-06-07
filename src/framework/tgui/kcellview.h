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

#ifndef KCELLVIEW_H
#define KCELLVIEW_H
/**
 * @file kcellview.h
 * @brief Include this file if you need the class KCellView, KCellViewItem, KCellViewItemDelegate or KCellViewModel
 */

#include <QTableWidget>
#include <QStyleOptionViewItem>
#include "tglobal.h"

class KCellViewItemDelegate;

class KCellViewItem : public QTableWidgetItem
{
    public:
        KCellViewItem();
        ~KCellViewItem();

        QImage image() const;
        QBrush background() const;
};

/**
 * @author David Cuadrado <krawek@gmail.com>
 */
class KCellView : public QTableWidget
{
    Q_OBJECT;
    public:
        KCellView(int MAX_COLUMNS = 16, QWidget *parent = 0);
        KCellView(int rows, int columns, int MAX_COLUMNS = 16, QWidget *parent = 0);
        ~KCellView();

        void setItemSize(int w, int h);
        void addItem(KCellViewItem *item);
        void addItem(const QBrush &brush);
        void addItem(const QImage &image);

    private:
        void setup();

    private slots:
        void fixSize();

    protected:
        virtual QStyleOptionViewItem viewOptions() const;
        virtual void wheelEvent(QWheelEvent *e);

    private:
        int m_rectWidth, m_rectHeight;

    private:
        int m_countColor;
        int m_col, m_row;
        const int MAX_COLUMNS;
};

#endif
