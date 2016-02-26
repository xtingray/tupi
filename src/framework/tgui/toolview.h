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

#ifndef TOOLVIEW_H
#define TOOLVIEW_H

#include "tglobal.h"
#include "tviewbutton.h"
#include "tmainwindow.h"

#include <QDockWidget>
#include <QIcon>
#include <QKeySequence>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QLayout>
#include <QEvent>

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT ToolView : public QDockWidget
{
    Q_OBJECT

    public:
        ToolView(const QString &title, const QIcon &icon = QIcon(), const QString &code = QString(), QWidget * parent = 0);
        virtual ~ToolView();

        void setDescription(const QString &description);
        TViewButton *button() const;
        void expandDock(bool flag);
        bool isExpanded();
        void setExpandingFlag();
        void setSizeHint();
        QSize sizeHint() const;
        void setPerspective(int wsp);
        int perspective() const;
        void setFixedSize(int s);
        int fixedSize() const;
        void enableButton(bool flag);
        QString getObjectID();
        bool isChecked();
        void setShortcut(QKeySequence shortcut);

    public slots:
        void saveSize(bool checked);

    private:
        void setup(const QString &label);

    protected:
        virtual void showEvent(QShowEvent *event);

    private:
        TViewButton *m_button;
        int m_size;
        int m_perspective;
        bool expanded;
};

#endif
