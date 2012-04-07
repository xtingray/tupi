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

#include "tupstatusbar.h"
#include "tdebug.h"

TupStatusBar::TupStatusBar(QWidget *parent)
 : QStatusBar(parent)
{
    setObjectName("TupStatusBar_");

    //m_progressBar = new QProgressBar(this);
    //m_progressBar->setMaximumSize(100, m_progressBar->height()/2);
    //m_progressBar->setMinimumWidth(100);

    //m_progressBar->setMaximum(10);

    m_status = new QLabel(this);
    m_status->setIndent(10);

    addPermanentWidget(m_status, 1);
    //addPermanentWidget(m_progressBar, 0);

    // m_timer = new QTimer(this);
    // connect(m_timer, SIGNAL(timeout()), this, SLOT(clear()));
}


TupStatusBar::~TupStatusBar()
{
}

/*
void TupStatusBar::clear()
{
    advance(0);
    m_timer->stop();
}
*/

//void TupStatusBar::setStatus(const QString &status, int ms)
void TupStatusBar::setStatus(const QString &status)
{
    m_status->setText(status);
    m_status->repaint();

    /*
    if (ms > 0) {
        if (m_timer->isActive())
            m_timer->stop();
        m_timer->start(ms);
    }
    */
}

/*

void TupStatusBar::addWidget(QWidget *widget, int stretch, bool permanent)
{
    if (permanent)
        QStatusBar::addPermanentWidget(widget,stretch);
    else
        QStatusBar::addWidget(widget,stretch);
}

void TupStatusBar::advance(int step, int totalSteps)
{
    if (totalSteps != -1)
        m_progressBar->setMaximum(totalSteps);

    m_progressBar->setValue(step);
}

*/
