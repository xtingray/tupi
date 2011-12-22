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

#ifndef DITEMSELECTOR_H
#define DITEMSELECTOR_H

#include "tapplicationproperties.h"
#include <QWidget>

/**
 * @author David Cuadrado
*/
class KItemSelector : public QWidget
{
    Q_OBJECT

    public:
        KItemSelector(QWidget *parent = 0);
        ~KItemSelector();
        
        void setItems(const QStringList &items);
        int addItem(const QString &item);
        void addItems(const QStringList &items);
        
        QStringList selectedItems() const;
        QList<int> selectedIndexes() const;
        
        void clear();
        void reset();
        void selectFirstItem();
        
    private slots:
        void addCurrent();
        void removeCurrent();
        void upCurrent();
        void downCurrent();
        
    signals:
        void changed();
        
    private:
        struct Private;
        Private *const k;
};

#endif
