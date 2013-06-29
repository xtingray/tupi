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

#ifndef TABDIALOG_H
#define TABDIALOG_H

#include "ttabwidget.h"

#include <QDialog>
#include <QHash>

typedef QHash<int, QPushButton *> Buttons;

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT TabDialog : public QDialog
{
    Q_OBJECT

    public:
        enum Button
        {
            Help    = 1<<2,
            Ok      = 1<<3,
            Apply   = 1<<4,
            Cancel  = 1<<5,
            Custom1 = 1<<6,
            Custom2 = 1<<7,
            Custom3 = 1<<8
        };
        TabDialog(QWidget *parent = 0, bool modal = true);
        TabDialog(int buttons = Ok|Cancel, QWidget *parent = 0, bool modal = true);
        ~TabDialog();
        
        void addTab(QWidget * child, const QString & label);
        void addTab(QWidget * child, const QIcon & iconset, const QString & label);
        
        QWidget *currentTab();
        
        void setButtonText(Button b, const QString &text);
        QPushButton *button(Button b);
        
        TTabWidget *tabWidget() const;
        void enableTab(int index, bool enable);
        
    private:
        void setupButtons(int buttons);
        
    public slots:
        virtual void ok();
        virtual void cancel();
        virtual void apply();
        virtual void help(){};
        virtual void custom1() {};
        virtual void custom2() {};
        virtual void custom3() {};
        
    private:
        TTabWidget *m_tabWidget;
        Buttons m_buttons;
};

#endif
