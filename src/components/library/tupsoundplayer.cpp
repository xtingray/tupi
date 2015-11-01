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

#include "tupsoundplayer.h"

struct TupSoundPlayer::Private
{
    QMediaPlayer *player;
    QSlider *slider;
    QLabel *timer;
    TImageButton *playButton;
    bool playing;
    qint64 duration;
    QTime soundTotalTime;
    QString totalTime;
};

TupSoundPlayer::TupSoundPlayer(QWidget *parent) : QFrame(parent), k(new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMidLineWidth(2);
    setLineWidth(1);

    k->playing = false;
    k->player = new QMediaPlayer;
    connect(k->player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(k->player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(k->player, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QMediaPlayer::State)));

    k->timer = new QLabel("");
    QBoxLayout *timerLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    timerLayout->addStretch();
    timerLayout->addWidget(k->timer);
    timerLayout->addStretch();
    timerLayout->setContentsMargins(0, 0, 0, 0);

    k->slider = new QSlider(Qt::Horizontal, this);
    // k->slider->setRange(0, k->player->duration() / 1000);
    connect(k->slider, SIGNAL(sliderMoved(int)), this, SLOT(updateSoundPos(int)));

    QBoxLayout *sliderLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    sliderLayout->addWidget(k->slider);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    k->playButton = new TImageButton(QPixmap(THEME_DIR + "icons/play_small.png"), 33, this, true);
    k->playButton->setToolTip(tr("Play"));
    connect(k->playButton, SIGNAL(clicked()), this, SLOT(playFile()));

    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout->addStretch();
    buttonLayout->addWidget(k->playButton);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->addSpacing(5);
    layout->addLayout(timerLayout);
    layout->addLayout(sliderLayout);
    layout->addLayout(buttonLayout);
    layout->addSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);
}

TupSoundPlayer::~TupSoundPlayer()
{
    delete k;
}

QSize TupSoundPlayer::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(100, 100));
}

void TupSoundPlayer::setSoundObject(const QString &path)
{
    k->player->setMedia(QUrl::fromLocalFile(path)); 
}

void TupSoundPlayer::playFile()
{
    if (!k->playing) {
        k->playButton->setIcon(QIcon(QPixmap(THEME_DIR + "icons/pause.png")));
        k->playing = true;
        k->player->setVolume(60);

        QString initTime = "00:00";
        if (k->duration > 3600)
            initTime = "00:00:00";
        initTime = initTime + " / " + k->totalTime;
        k->timer->setText(initTime);

        k->player->play();
    } else {
        k->playButton->setIcon(QIcon(QPixmap(THEME_DIR + "icons/play_small.png")));
        k->playing = false;
        k->player->pause();
    }
}

void TupSoundPlayer::positionChanged(qint64 value)
{
    qint64 currentInfo = value / 1000;
    k->slider->setValue(currentInfo);
    QString time;

    if (currentInfo || k->duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QString format = "mm:ss";
        if (k->duration > 3600)
            format = "hh:mm:ss";
        time = currentTime.toString(format) + " / " + k->totalTime;
    }

    k->timer->setText(time);
}

void TupSoundPlayer::durationChanged(qint64 value)
{
    qDebug() << "TupSoundPlayer::durationChanged() - value: " << value;
    k->duration = value/1000;
    k->slider->setMinimum(0);
    k->slider->setMaximum(k->duration);

    k->soundTotalTime = QTime((k->duration/3600)%60, (k->duration/60)%60, k->duration%60, (k->duration*1000)%1000);
    QString format = "mm:ss";
    if (k->duration > 3600)
        format = "hh:mm:ss";
    k->totalTime = k->soundTotalTime.toString(format);
}

void TupSoundPlayer::stateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        k->slider->setValue(0);
        k->playButton->setIcon(QIcon(QPixmap(THEME_DIR + "icons/play_small.png")));
        k->playing = false;
        QString init = "00:00";
        if (k->duration > 3600)
            init = "00:00:00";
        k->timer->setText(init + " / " + k->totalTime);
    }
}

void TupSoundPlayer::updateSoundPos(int pos)
{
    k->player->setPosition(pos*1000);
}
