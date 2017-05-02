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

#include "tupanimationspace.h"

/**
 * This class defines the space which contains the Animation Mode interface.
 * Here is where the visual space for Animation Mode is defined.
 * @author David Cuadrado
*/

struct TupAnimationspace::Private
{
    TupCameraWidget *playerInterface;
    QWidget *container;
    bool playOn;
};

TupAnimationspace::TupAnimationspace(TupCameraWidget *playerUI, QWidget *parent) : QMainWindow(parent), k(new Private)
{
    // TODO: Try a nice dark color for this window
    // setStyleSheet("QMainWindow { background-color: #d0d0d0; }");

    k->playerInterface = playerUI;
    k->playOn = false;
    setCameraWidget(k->playerInterface);
}

TupAnimationspace::~TupAnimationspace()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupCameraWidget()]";
        #else
            TEND;
        #endif
    #endif

    delete k->playerInterface;
    k->playerInterface = NULL;
    delete k->container;
    k->container = NULL;
    delete k;
}

void TupAnimationspace::setCameraWidget(TupCameraWidget *playerUI) 
{
    k->container = new QWidget();
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, k->container);
    k->playerInterface = playerUI;
    layout->addWidget(k->playerInterface, 0, Qt::AlignCenter);
    k->container->setLayout(layout);
    setCentralWidget(k->container);
}

void TupAnimationspace::mousePressEvent(QMouseEvent *event)
{
    if (event->button () == Qt::RightButton)
        emit contextMenu(mapToGlobal(event->pos()));
}

void TupAnimationspace::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void TupAnimationspace::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void TupAnimationspace::keyPressEvent(QKeyEvent *event) 
{
    switch (event->key()) {
            case Qt::Key_Space:
                  k->playerInterface->doPause();
            break;
            case Qt::Key_Escape:
                  k->playOn = false;
                  k->playerInterface->doStop();
            break;
            case Qt::Key_Right:
                  k->playerInterface->nextFrame(); 
            break;
            case Qt::Key_Left:
                  k->playerInterface->previousFrame();
            break;
            case Qt::Key_Up:

            break;
            case Qt::Key_Down:

            break;
            case Qt::Key_Return:
                  emit newPerspective(0);
                  k->playOn = false;
                  k->playerInterface->doStop();
            break;
            case Qt::Key_1:
                  if (event->modifiers() == Qt::ControlModifier)
                      emit newPerspective(0);
            break;
            case Qt::Key_3:
                  if (event->modifiers() == Qt::ControlModifier)
                      emit newPerspective(2);
            break;
    }
}
