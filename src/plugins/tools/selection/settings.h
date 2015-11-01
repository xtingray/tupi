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
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QLabel>
#include <QBoxLayout>
#include <QTextEdit>
#include <QSpinBox>
#include <QDir>

/**
 * @author Gustav Gonzalez 
*/

class TUPI_PLUGIN Settings : public QWidget
{
    Q_OBJECT

    public:
        enum Flip { Vertical = 1, Horizontal, Crossed };
        enum Order { ToBack = 0, ToFront, ToBackOneLevel, ToFrontOneLevel };
        enum Group { GroupItems = 0, UngroupItems };

        Settings(QWidget *parent = 0);
        ~Settings();
        void setPos(int x, int y);
        void enablePositionControls(bool flag);

     signals:
        void callFlip(Settings::Flip flip);
        void callOrderAction(Settings::Order action);
        void callGroupAction(Settings::Group action);
        void updateItemPosition(int x, int y);

     private slots:
        void vFlip();
        void hFlip();
        void cFlip();
        void sendToBack();
        void sendToBackOneLevel();
        void sendToFront();
        void sendToFrontOneLevel();
        void openTipPanel();
        void notifyXMovement(int x);
        void notifyYMovement(int y);
        void groupItems();
        void ungroupItems();

    private:
        struct Private;
        Private *const k;
};

#endif
