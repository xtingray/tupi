/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#include "tupbackgroundscene.h"

struct TupBackgroundScene::Private
{
    TupFrame *bg;
};

TupBackgroundScene::TupBackgroundScene(const QSize dimension, const QColor color, TupFrame *background) : QGraphicsScene(), k(new Private)
{
    setSceneRect(QRectF(QPointF(0,0), dimension));
    setBackgroundBrush(color);
    k->bg = background;
    drawScene();
}

TupBackgroundScene::~TupBackgroundScene()
{
    clearFocus();
    clearSelection();

    foreach (QGraphicsView *view, this->views())
             view->setScene(0);

    foreach (QGraphicsItem *item, items())
             removeItem(item);

    delete k;
}

void TupBackgroundScene::drawScene()
{
    cleanWorkSpace();
    addFrame(k->bg);
    update();
}

void TupBackgroundScene::renderView(QPainter *painter)
{
    render(painter, QRect(0, 0, painter->device()->width(), painter->device()->height()),
           sceneRect().toRect(), Qt::IgnoreAspectRatio);
}

void TupBackgroundScene::cleanWorkSpace()
{
    foreach (QGraphicsItem *item, items()) {
             if (item->scene() == this)
                 removeItem(item);
    }
}

void TupBackgroundScene::addFrame(TupFrame *frame)
{
    if (frame) {
        for (int i = 0; i < frame->graphicItemsCount(); i++) {
             TupGraphicObject *object = frame->graphicAt(i);
             addGraphicObject(object);
        }

        for (int i = 0; i < frame->svgItemsCount(); i++) {
             TupSvgItem *object = frame->svgAt(i);
             addSvgObject(object);
        }
    }
}

void TupBackgroundScene::addGraphicObject(TupGraphicObject *object)
{
    if (object) {
        QGraphicsItem *item = object->item();
        item->setSelected(false);
        addItem(item);
    }
}

void TupBackgroundScene::addSvgObject(TupSvgItem *svgItem)
{
    if (svgItem) {
        svgItem->setSelected(false);
        addItem(svgItem);
    }
}

