/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#ifndef TABBEDMAINWINDOW_H
#define TABBEDMAINWINDOW_H

#include "tmainwindow.h"

class QTabWidget;

/**
 * A tabbed main window.
 * @author David Cuadrado
*/

class T_IDEAL_EXPORT TabbedMainWindow : public TMainWindow
{
    Q_OBJECT

    public:

        enum Perspective {
             Animation = 0x01,
             Player = 0x02,
             Help = 0x04,
             News = 0x08,
             All = Animation | Player | Help | News
        };

        TabbedMainWindow(QWidget *parent = 0);
        ~TabbedMainWindow();

        void addWidget(QWidget *widget, bool persistant = true, int perspective = All);
        void removeWidget(QWidget *widget, bool force = false);
        void setTabWidget(QTabWidget *w);
        QTabWidget *tabWidget() const;
        void setCurrentTab(int index);

    protected:
        virtual void setupTabWidget(QTabWidget *w);

    protected slots:
        void closeCurrentTab();
        //virtual void setupPerspective(int wps);

    signals:
        void widgetChanged(QWidget *widget);
        void tabHasChanged(int);
		
    private slots:
        void emitWidgetChanged(int index);

    private:
        QTabWidget *m_tabWidget;
        QWidgetList m_persistantWidgets;
        QHash<QWidget *, int> m_tabs;
        QWidgetList m_pages;
};

#endif
