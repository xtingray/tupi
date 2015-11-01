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

#include "tupcellscolor.h"

struct TupCellsColor::Private
{
    TupCellsColor::Type type;
    QString name;
    bool readOnly;
    QPoint startDragPosition;
};

TupCellsColor::TupCellsColor(QWidget *parent, Type type) : TCellView(16, parent), k(new Private)
{
    k->type = type;
    k->readOnly = false;
    setAcceptDrops(true);
}

TupCellsColor::~TupCellsColor()
{
    delete k;
}


void TupCellsColor::setReadOnly(bool enable)
{
    k->readOnly = enable;
}

bool TupCellsColor::isReadOnly()
{
    return k->readOnly;
}

void TupCellsColor::setType(Type type)
{
    k->type = type;
}

int TupCellsColor::type()
{
    return k->type;
}

QString TupCellsColor::name() const
{
    return k->name;
}

void TupCellsColor::setName(const QString& name)
{
    k->name = name;
}

void TupCellsColor::save( const QString &path)
{
    QFile save(path);
    TupPaletteDocument document(k->name, true);

    for (int i = 0; i < columnCount() ; i++) {
         for (int  j = 0; j < rowCount() ; j++) {
              QTableWidgetItem *tmpItem = itemAt(i*25, j*25);
              if (tmpItem) {
                  if (tmpItem->background().gradient())
                      document.addGradient(*tmpItem->background().gradient());
                  else if(tmpItem->background().color().isValid())
                      document.addColor(tmpItem->background().color());
              }
         }
    }

    if (save.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&save);
        out << document.toString();
        save.close();
    }
}

void TupCellsColor::dragEnterEvent(QDragEnterEvent *event)
{
    setFocus();

    if (event->mimeData()->hasColor()) {
        if (event->source() == this) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
        } else {
             event->acceptProposedAction();
        }
    } else {
          event->ignore();
    }
}

void TupCellsColor::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasColor()) {
        // QColor color = qvariant_cast<QColor>(event->mimeData()->colorData());

        // SQA: crear item in tupcellscolor.cpp

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
            event->ignore();
    }
}

void TupCellsColor::mousePressEvent(QMouseEvent* e)
{
    TCellView::mousePressEvent(e);
    k->startDragPosition = e->pos();
}

void TupCellsColor::mouseMoveEvent(QMouseEvent* e)
{
    TCellView::mouseMoveEvent(e);

    if ((e->pos() - k->startDragPosition).manhattanLength() <  QApplication::startDragDistance() || !currentItem())
        return;

    QDrag *drag = new QDrag(this);
    QPixmap pix( 25, 25 );
    QColor color =  currentItem()->background().color();
    pix.fill(color);

    QPainter painter(&pix);
    painter.drawRect( 0, 0, pix.width(), pix.height() );
    painter.end();

    QMimeData *mimeData = new QMimeData;
    mimeData->setColorData(currentItem()->background().color());

    drag->setMimeData(mimeData);
    drag->setPixmap(pix);
}

