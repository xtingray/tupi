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

#ifndef Q_WS_MAC

#include "kflashwidget.h"

// #ifdef Q_WS_X11

#include <QtGlobal>
#include <QtDebug>
#include <QMouseEvent>
#include <QPainter>

struct KFlashWidget::Private
{
    QString movie;
    QProcess *process;
    bool isOk;
};

KFlashWidget::KFlashWidget(const QString &swf, QWidget *parent) : QX11EmbedContainer(parent), k(new Private)
{
    setWindowTitle(tr("Flashing"));
    
    k->movie = swf;
    k->isOk = true;
    
    // setAttribute(Qt::WA_OpaquePaintEvent, true);
    
    k->process = new QProcess(this);
    
    connect(k->process, SIGNAL(started ()), this, SLOT(updateSize()));
    // k->process->setEnvironment(QStringList()<<"SESSION_MANAGER=\"\"");
    
    resize(640, 480); // FIXME
}

KFlashWidget::~KFlashWidget()
{
    k->process->kill();
    k->process->waitForFinished();
    
    delete k;
}

void KFlashWidget::play()
{
    QStringList args;

    args << QStringList() << "-x" << QString::number(winId()) << "-s" << "1.5" << k->movie;

    k->process->start("gnash", args);
    
    k->process->waitForStarted();
    
    if (k->process->error() == QProcess::FailedToStart || k->process->state() == QProcess::NotRunning) {
        qWarning("Please install gnash");
        k->isOk = false;
    } else {
        k->isOk = true;
    }

    repaint();
}

void KFlashWidget::stop()
{
    k->process->terminate();
    k->process->waitForFinished();
}

void KFlashWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
        emit contextMenu(mapToGlobal(e->pos()));
    
    e->ignore();
}

void KFlashWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    stop();
    play();
    
    e->ignore();
}

void KFlashWidget::updateSize()
{
    // TODO: Update the widget size
}

void KFlashWidget::paintEvent(QPaintEvent *e)
{
    QX11EmbedContainer::paintEvent(e);
    
    if (!k->isOk) {

        QPainter painter(this);
        QString text = tr("Please install gnash from http://gnash.org");
        QFontMetrics fm(painter.font());
        QRect r = fm.boundingRect(text);
        QPoint pos = (rect().center() - r.topLeft())/2;
        
        painter.drawText(pos, text);

    }
}

#endif // Q_WS_MAC
