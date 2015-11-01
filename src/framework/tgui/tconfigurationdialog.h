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

#ifndef TCONFIGURATIONDIALOG_H
#define TCONFIGURATIONDIALOG_H

#include "tglobal.h"
#include "twidgetlistview.h"
#include "tflatbutton.h"
#include "tseparator.h"

#include <QDialog>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QListWidget>

class QStackedWidget;
class QListWidget;
class QListWidgetItem;

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT TConfigurationDialog : public QDialog
{
    Q_OBJECT

    public:
        TConfigurationDialog(QWidget *parent = 0);
        ~TConfigurationDialog();
        
        void addPage(QWidget *page, const QString &label, const QIcon &icon);
        QWidget *currentPage() const;
        void setCurrentItem(int row);
        
    public slots:
        virtual void ok();
        virtual void cancel();
        virtual void apply();
        
    private slots:
        void changePage(QListWidgetItem *, QListWidgetItem*);
        
    private:
        struct Private;
        Private *const k;
};

#endif
