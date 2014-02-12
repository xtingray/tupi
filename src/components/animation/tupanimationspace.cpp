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
#include "tdebug.h"

#include <QMouseEvent>
#include <QDropEvent>
#include <QLinearGradient>
#include <QBoxLayout>

/**
 * This class defines the space which contains the Animation Mode interface.
 * Here is where the visual space for Animation Mode is defined.
 * @author David Cuadrado
*/

TupAnimationspace::TupAnimationspace(TupViewCamera *internal, QWidget *parent) : QMainWindow(parent)
{
    // TODO: Try a nice dark color for this window
    // setStyleSheet("QMainWindow { background-color: #d0d0d0; }");
    camera = internal;
    playOn = false;
    QWidget *widget = new QWidget();
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
    layout->addWidget(camera, 0, Qt::AlignCenter);
    widget->setLayout(layout);
    setCentralWidget(widget);
}

TupAnimationspace::~TupAnimationspace()
{
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

void TupAnimationspace::keyPressEvent(QKeyEvent *event) {

    switch (event->key()) {
            case Qt::Key_Space:
                  if (event->modifiers()==Qt::ShiftModifier) {
                      camera->doPlayBack();
                  } else {
                      if (!playOn) {
                          camera->doPlay();
                          playOn = true;
                      } else {
                          camera->doStop();
                          playOn = false;
                      }
                  }
            break;
            case Qt::Key_Escape:
                  camera->doStop();
            break;
            case Qt::Key_Right:
                  camera->nextFrame(); 
            break;
            case Qt::Key_Left:
                  camera->previousFrame();
            break;
            case Qt::Key_Up:

            break;
            case Qt::Key_Down:

            break;
    }
}

/*
void TupAnimationspace::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        QColor color = qvariant_cast<QColor>(event->mimeData()->colorData());
        QPalette pal = palette();
        pal.setColor(QPalette::Dark, color);
 
        setPalette(pal);
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
 
void TupAnimationspace::dragEnterEvent(QDragEnterEvent *event)
{
    setFocus();

    if (event->mimeData()->hasColor()) 
        event->acceptProposedAction();
    else 
        event->ignore();
 }
*/
