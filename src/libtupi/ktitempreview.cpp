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

#include "ktitempreview.h"
#include "ktproxyitem.h"

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSvgItem>

#include "tdebug.h"

struct KTItemPreview::Private
{
    KTProxyItem *proxy;
};

KTItemPreview::KTItemPreview(QWidget *parent) : QWidget(parent), k(new Private)
{
    reset();
}

KTItemPreview::~KTItemPreview()
{
    delete k;
}

void KTItemPreview::reset()
{
    k->proxy = 0;
    QGraphicsTextItem *item = new QGraphicsTextItem(tr("Library is empty :("));
    render(item);
}

QSize KTItemPreview::sizeHint() const
{
    if (k->proxy) {
        int maxY = k->proxy->boundingRect().size().height();
        if (maxY < 100)
            return k->proxy->boundingRect().size().toSize() + QSize(10, 110 - maxY);
        else
            return k->proxy->boundingRect().size().toSize() + QSize(10,10);
    }
    
    return QWidget::sizeHint().expandedTo(QSize(100,100));
}

void KTItemPreview::render(QGraphicsItem *item)
{
    if (!k->proxy)
        k->proxy = new KTProxyItem(item);
    else
        k->proxy->setItem(item);
    
    update();
}

void KTItemPreview::paintEvent(QPaintEvent *)
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
        
        QTransform matrix = k->proxy->sceneTransform();
        
        opt.palette = palette();
        painter.setTransform(matrix);

        QRectF rectangle(QPointF(0,0), size()); 
        painter.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
        painter.drawRect(rectangle);

        // If preview is for a "path" object
        if (QGraphicsPathItem *path = qgraphicsitem_cast<QGraphicsPathItem *>(k->proxy->item())) {

            int pathWidth = path->path().boundingRect().width();
            int pathHeight = path->path().boundingRect().height();

            // If object is bigger than canvas, resize
            if (pathWidth > rect().width() || pathHeight > rect().height()) {
                float distance = 0;
                float base = 0;
                int newPosX = 0;
                int newPosY = 0;

                float limit = (float) rect().width() / (float) rect().height();
                float proportion = pathWidth / pathHeight;

                if (proportion <= limit) {
                    distance = pathHeight;
                    base = rect().height() - 10;
                } else {
                    distance = pathWidth;
                    base = rect().width();
                }

                float factor = base/distance;
                factor = factor - 0.05;
                float alterFactor = 1/factor;
                int widthRealLength = size().width()*alterFactor;
                int heightRealLength = size().height()*alterFactor;

                painter.scale(factor, factor);

                newPosX = (widthRealLength - pathWidth)/2;  
                newPosY = (heightRealLength - pathHeight)/2;
                painter.translate(newPosX, newPosY);

                newPosX = -path->path().boundingRect().topLeft().x();
                newPosY = -path->path().boundingRect().topLeft().y(); 
                painter.translate(newPosX, newPosY);

            } else { // if object is smaller than canvas, just show it

                painter.translate((rect().width() - pathWidth)/2, (rect().height() - pathHeight)/2);
                painter.translate(-path->path().boundingRect().topLeft().x(), -path->path().boundingRect().topLeft().y());

            }

        } else { // if preview is for images or svg objects 

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
