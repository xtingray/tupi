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

#include "tupitempreview.h"
#include <QGraphicsItemGroup>

struct TupItemPreview::Private
{
    TupProxyItem *proxy;
};

TupItemPreview::TupItemPreview(QWidget *parent) : QWidget(parent), k(new Private)
{
    reset();
}

TupItemPreview::~TupItemPreview()
{
    delete k;
}

void TupItemPreview::reset()
{
    k->proxy = 0;
    QGraphicsTextItem *item = new QGraphicsTextItem(tr("Library is empty :("));
    render(item);
}

QSize TupItemPreview::sizeHint() const
{
    /*
    if (k->proxy) {
        int maxY = k->proxy->boundingRect().size().height();
        if (maxY < 100)
            return k->proxy->boundingRect().size().toSize() + QSize(10, 110 - maxY);
        else
            return k->proxy->boundingRect().size().toSize() + QSize(10, 10);
    }
    */
    
    return QWidget::sizeHint().expandedTo(QSize(100, 100));
}

void TupItemPreview::render(QGraphicsItem *item)
{
    if (!k->proxy)
        k->proxy = new TupProxyItem(item);
    else
        k->proxy->setItem(item);
    
    update();
}

void TupItemPreview::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    if (k->proxy) {
        QStyleOptionGraphicsItem opt;
        opt.state = QStyle::State_None;
        if (k->proxy->isEnabled())
            opt.state |= QStyle::State_Enabled;
        opt.exposedRect = QRectF(QPointF(0,0), k->proxy->boundingRect().size());
        opt.levelOfDetail = 1;
        opt.palette = palette();
        
        QTransform matrix = k->proxy->sceneTransform();
        painter.setTransform(matrix);

        QRectF rectangle(QPointF(0,0), size()); 
        painter.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
        painter.drawRect(rectangle);

        int itemWidth = 0;
        int itemHeight = 0;
        bool isNative = false;
        int newPosX = 0;
        int newPosY = 0;

        if (QGraphicsPathItem *path = qgraphicsitem_cast<QGraphicsPathItem *>(k->proxy->item())) {
            isNative = true;
            itemWidth = path->path().boundingRect().width();
            itemHeight = path->path().boundingRect().height();
            newPosX = -path->path().boundingRect().topLeft().x();
            newPosY = -path->path().boundingRect().topLeft().y();
        } else if (QGraphicsItemGroup *group = qgraphicsitem_cast<QGraphicsItemGroup *>(k->proxy->item())) {
                   isNative = true;
                   itemWidth = group->boundingRect().width();
                   itemHeight = group->boundingRect().height();
                   // SQA: These coords don't work if the group has been edited
                   // A new algorithm must be developed here
                   newPosX = -group->boundingRect().topLeft().x();
                   newPosY = -group->boundingRect().topLeft().y();
        }

        // If preview is for a native object (path or group)
        if (isNative) {
            // If object is bigger than canvas, resize
            if (itemWidth > rect().width() || itemHeight > rect().height()) {
                float distance = 0;
                float base = 0;
                float limit = (float) rect().width() / (float) rect().height();
                float proportion = itemWidth / itemHeight;

                if (proportion <= limit) {
                    distance = itemHeight;
                    base = rect().height() - 10;
                } else {
                    distance = itemWidth;
                    base = rect().width();
                }

                float factor = base/distance;
                factor = factor - 0.05;
                float alterFactor = 1/factor;
                int widthRealLength = size().width()*alterFactor;
                int heightRealLength = size().height()*alterFactor;

                painter.scale(factor, factor);

                int posX = (widthRealLength - itemWidth)/2;  
                int posY = (heightRealLength - itemHeight)/2;
                painter.translate(posX + newPosX, posY + newPosY);

                // painter.translate(newPosX, newPosY);
            } else { // if object is smaller than canvas, just show it
                painter.translate((rect().width() - itemWidth)/2, (rect().height() - itemHeight)/2);
                painter.translate(newPosX, newPosY);
            }
        } else { 
                // if preview is for images or svg objects 
                // if object is bigger than canvas, resize
                if (opt.exposedRect.width() > rect().width() || opt.exposedRect.height() > rect().height()) {
                    float distance = 0;
                    float base = 0;
                    int newPosX = 0;
                    int newPosY = 0;

                    // Let's calculate the proportion of the object and the preview area
                    float limit = (float) rect().width() / (float) rect().height();
                    float proportion = opt.exposedRect.width() / opt.exposedRect.height();

                    if (proportion <= limit) {
                        distance = opt.exposedRect.height();  
                        base = rect().height() - 10;
                    } else {
                        distance = opt.exposedRect.width();
                        base = rect().width();
                    }
 
                    float factor = base/distance;
                    float alterFactor = 1/factor;
                    int widthRealLength = rect().width()*alterFactor;
                    int heightRealLength = rect().height()*alterFactor;

                    if (widthRealLength > opt.exposedRect.width())
                        newPosX = (widthRealLength - opt.exposedRect.width())/2;

                    if (heightRealLength > opt.exposedRect.height()) 
                        newPosY = (heightRealLength - opt.exposedRect.height())/2;

                    painter.scale(factor, factor);
                    painter.translate(newPosX, newPosY);

                } else { // if object is smaller than canvas, just show it
                    painter.translate((rect().width() - opt.exposedRect.width())/2, (rect().height() - opt.exposedRect.height())/2);
                }
        }

        k->proxy->paint(&painter, &opt, this); // paint isn't const...
    }
}
