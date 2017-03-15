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

#include "tupcamerabar.h"

TupCameraBar::TupCameraBar(QWidget *parent) : QFrame(parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCameraBar()]";
        #else
            TINIT;
        #endif
    #endif

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMidLineWidth(2);
    setLineWidth(1);

    QBoxLayout *m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, parent);
    m_mainLayout->addStretch(1);

    m_mainLayout->setSpacing(10);
    m_mainLayout->setMargin(3);

    m_rew = new TImageButton(QPixmap(THEME_DIR + "icons/rw.png"), 33, this, true);
    m_rew->setToolTip(tr("Rewind"));
    m_mainLayout->addWidget(m_rew);
    connect(m_rew, SIGNAL(clicked()), this, SIGNAL(rew()));

    m_play_back = new TImageButton(QPixmap(THEME_DIR + "icons/play_back.png"), 25, this, true);
    m_play_back->setToolTip(tr("Play in reverse"));
    m_mainLayout->addWidget(m_play_back);
    connect(m_play_back, SIGNAL(clicked()), this, SIGNAL(playBack()));

    m_play = new TImageButton(QPixmap(THEME_DIR + "icons/play.png"), 33, this, true);
    m_play->setToolTip(tr("Play"));
    m_mainLayout->addWidget(m_play);
    connect(m_play, SIGNAL(clicked()), this, SIGNAL(play()));

    m_pause = new TImageButton(QPixmap(THEME_DIR + "icons/pause.png"), 33, this, true);
    m_pause->setToolTip(tr("Pause"));
    m_mainLayout->addWidget(m_pause);
    connect(m_pause, SIGNAL(clicked()), this, SIGNAL(pause()));

    m_stop = new TImageButton(QPixmap(THEME_DIR + "icons/stop.png"), 25, this, true);
    m_stop->setToolTip(tr("Stop"));
    m_mainLayout->addWidget(m_stop);
    connect(m_stop, SIGNAL(clicked()), this, SIGNAL(stop()));

    m_ff = new TImageButton(QPixmap(THEME_DIR + "icons/ff.png"), 33, this, true);
    m_ff->setToolTip(tr("Forward"));
    m_mainLayout->addWidget(m_ff);
    connect(m_ff, SIGNAL(clicked()), this, SIGNAL(ff()));

    setLayout(m_mainLayout);
}

TupCameraBar::~TupCameraBar()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupCameraBar()]";
        #else
           TEND;
        #endif
    #endif
}

void TupCameraBar::setPalette(const QPalette &)
{
}
