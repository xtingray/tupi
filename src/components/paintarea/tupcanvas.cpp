/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupcanvas.h"
#include "tapplicationproperties.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QGraphicsView>

TupCanvas::TupCanvas(QWidget *parent, Qt::WindowFlags flags, TupGraphicsScene *scene, 
                   const QPointF centerPoint, const QSize &screenSize, const QSize &projectSize, double scaleFactor,
                   int angle, const QColor &bg) : QFrame(parent, flags)
{
    setWindowTitle(tr("Tupi: 2D Magic"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")));

    graphicsView = new TupCanvasView(this, screenSize, projectSize, bg);
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    graphicsView->setFixedSize(screenSize.width(), screenSize.height());

    graphicsView->setScene(scene);
    graphicsView->centerOn(centerPoint);
    graphicsView->scale(scaleFactor, scaleFactor);
    graphicsView->rotate(angle);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(graphicsView);
    setLayout(layout);
}

TupCanvas::~TupCanvas()
{
}

void TupCanvas::updateCursor(const QCursor &cursor)
{
    graphicsView->viewport()->setCursor(cursor);
}

void TupCanvas::closeEvent(QCloseEvent *event)
{
    graphicsView->stopUpdateTimer();
    event->accept();
}
