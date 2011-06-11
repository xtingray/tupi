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

#if defined(Q_WS_X11) && defined(Q_OS_LINUX) 

#ifndef KFLASHWIDGET_H
#define KFLASHWIDGET_H

#include <QWidget>
#include <QProcess>
#include <QX11EmbedContainer>

#include "tglobal.h"

/**
 * @author David Cuadrado 
*/

class K_GUI_EXPORT KFlashWidget : public QX11EmbedContainer
{
    Q_OBJECT

    public:
        KFlashWidget(const QString &swf, QWidget *parent = 0);
        ~KFlashWidget();
        
    public slots:
        void play();
        void stop();
        
    private slots:
        void updateSize();
        
    signals:
        void contextMenu(const QPoint &p);
        
    protected:
        void mousePressEvent(QMouseEvent *e);
        void mouseDoubleClickEvent(QMouseEvent *);
        void paintEvent(QPaintEvent *e);
        
    private:
        struct Private;
        Private *const k;

};

#endif
#endif
