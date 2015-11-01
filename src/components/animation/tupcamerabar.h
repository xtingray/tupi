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

#ifndef TUPCAMERABAR_H
#define TUPCAMERABAR_H

#include "tglobal.h"
#include "timagebutton.h"
#include "tapplicationproperties.h"

#include <QFrame>
#include <QPainter>
#include <QPushButton>
#include <QBoxLayout>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupCameraBar : public QFrame
{
    Q_OBJECT

    public:
        TupCameraBar(QWidget *parent = 0);
        ~TupCameraBar();
        void setPalette(const QPalette &);

    private:
        TImageButton *m_rew;
        TImageButton *m_play;
        TImageButton *m_play_back;
        TImageButton *m_stop;
        TImageButton *m_ff;

    signals:
        void playBack();
        void play();
        void stop();
        void ff();
        void rew();

    protected:
};

#endif
