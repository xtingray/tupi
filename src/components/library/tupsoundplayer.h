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

#ifndef TUPSOUNDPLAYER_H
#define TUPSOUNDPLAYER_H

#include "tglobal.h"
#include "timagebutton.h"
#include "tapplicationproperties.h"

#include <QFrame>
#include <QBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QMediaPlayer>
#include <QUrl>
#include <QTime>

/**
 * @author Gustav Gonzalez
**/

class TUPI_EXPORT TupSoundPlayer : public QFrame
{
    Q_OBJECT

    public:
        TupSoundPlayer(QWidget *parent);
        ~TupSoundPlayer();

        QSize sizeHint() const;
        void setSoundObject(const QString &path);

    private slots:
        void playFile();
        void positionChanged(qint64 value);
        void durationChanged(qint64 value);
        void stateChanged(QMediaPlayer::State state);
        void updateSoundPos(int pos);

    private:
        struct Private;
        Private *const k;
};

#endif


