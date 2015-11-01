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

#ifndef TVHBOX_H
#define TVHBOX_H

#include "tglobal.h"

#include <QFrame>
#include <QLayout>
#include <QBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPoint>

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT TVHBox : public QFrame
{
    Q_OBJECT

    public:
        TVHBox(QWidget *parent, Qt::Orientation o);
        TVHBox(QWidget *parent = 0, bool isVertical = true);
        ~TVHBox();
        void moveWidgetUp(QWidget *);
        void moveWidgetDown(QWidget *);
        void switchWidgetsPosition(QWidget *first, QWidget *second);
        virtual QSize sizeHint() const;
        void setSpacing(int space);
        void setStretchFactor(QWidget* w, int stretch);
        void addWidget(QWidget *child, Qt::Alignment alignment = 0);
        QBoxLayout *boxLayout();
        
    signals:
        void mouseAt(const QPoint &);
        
    protected:
        virtual bool event(QEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        
    private:
        QBoxLayout *m_pLayout;

};

#endif
