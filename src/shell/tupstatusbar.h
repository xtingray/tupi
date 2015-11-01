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

#ifndef TUPSTATUSBAR_H
#define TUPSTATUSBAR_H

#ifdef K_DEBUG

#ifdef Q_OS_WIN32
#include <QDebug>
#else
#include "tdebug.h"
#endif

#endif

#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QStatusBar>

/**
 * @author David Cuadrado
*/

class TupStatusBar : public QStatusBar
{
    Q_OBJECT

    public:
        TupStatusBar(QWidget *parent);
        ~TupStatusBar();
        //void addWidget ( QWidget *widget, int stretch = 0, bool permanent = FALSE );

    public slots:
        // void setStatus(const QString &status, int ms = 0);
        void setStatus(const QString &status);
        //void advance(int step, int totalSteps = -1);
        //void clear();

    private:
        //QTimer *m_timer;
        QLabel *m_status;
        //QProgressBar *m_progressBar;

};

#endif
