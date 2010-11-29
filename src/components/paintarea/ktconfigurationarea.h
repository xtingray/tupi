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

#ifndef KCONFIGURATIONAREA_H
#define KCONFIGURATIONAREA_H

#include <QDockWidget>

class QPushButton;

/**
 * @author David Cuadrado \<krawek@toonka.com\>
*/
class KTConfigurationArea : public QDockWidget
{
    Q_OBJECT
    public:
        KTConfigurationArea(QWidget *parent = 0);
        ~KTConfigurationArea();

        void setConfigurator(QWidget *widget, int maxWidth = 0);

        QSize sizeHint() const;

    public slots:
        void hideConfigurator();
        void showConfigurator();

   private:
        void shrink();

   private slots:
        void toggleLock();

   protected:
        void enterEvent(QEvent *e);
        void leaveEvent(QEvent *e);
        void paintEvent (QPaintEvent *e);

        private:
             struct Private;
             Private *const k;
};

#endif
