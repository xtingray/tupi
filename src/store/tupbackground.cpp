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

#include "tupbackground.h"
#include "tupserializer.h"
#include "tupbackgroundscene.h"
#include "tdebug.h"

TupBackground::TupBackground(TupScene *parent, const QSize size, const QColor color) : QObject(parent)
{
    dimension = size;
    bgColor = color;
    dynamicBg = new TupFrame(this);
    dynamicBg->setFrameName("landscape_dynamic");
    dynamicBg->setDynamicFlag(true);
    dynamicBg->setDynamicDirection("0");
    dynamicBg->setDynamicShift("5");

    staticBg = new TupFrame(this);
    staticBg->setFrameName("landscape_static");
}

TupBackground::~TupBackground()
{
}

void TupBackground::setBgColor(const QColor color)
{
    bgColor = color;
}

void TupBackground::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();
    QDomNode n = root.firstChild();

    while (!n.isNull()) {

           QDomElement e = n.toElement();

           if (e.tagName() == "frame") {

               QString type = e.attribute("name", "none");

               if (type == "landscape_static") {
                   staticBg = new TupFrame(this);
                   staticBg->setFrameName("landscape_static");

                   if (staticBg) {
                       QString newDoc;

                       {
                           QTextStream ts(&newDoc);
                           ts << n;
                       }

                       staticBg->fromXml(newDoc);
                   }

               } else if (type == "landscape_dynamic") {
                          dynamicBg = new TupFrame(this);
                          dynamicBg->setFrameName("landscape_dynamic");
                          dynamicBg->setDynamicFlag(true);

                          if (dynamicBg) {
                              QString newDoc;

                              {
                                  QTextStream ts(&newDoc);
                                  ts << n;
                              }

                              dynamicBg->fromXml(newDoc);
                              if (!dynamicBg->isEmpty()) {
                                  renderDynamicView();
                              }
                          }
               } else {
                   #ifdef K_DEBUG
                          tError() << "TupBackground::fromXml() - Error: The background input is invalid";
                   #endif
               }
           }

           n = n.nextSibling();
    }
}

QDomElement TupBackground::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("background");
    doc.appendChild(root);

    root.appendChild(dynamicBg->toXml(doc));
    root.appendChild(staticBg->toXml(doc));

    return root;
}

TupFrame *TupBackground::staticFrame()
{
    return staticBg;
}

TupFrame* TupBackground::dynamicFrame()
{
    return dynamicBg;
}

void TupBackground::renderDynamicView()
{
    TupBackgroundScene bgScene(dimension, bgColor, dynamicBg);
    QImage image(dimension, QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        bgScene.renderView(&painter);
    }

    int width = dimension.width();
    int height = dimension.height();
    int posX = 0;
    int posY = 0;

    TupBackground::Direction direction = dynamicBg->dynamicDirection();
    switch (direction) {
            case TupBackground::Left2Right:
            case TupBackground::Right2Left:
            {
                 posX = width;
                 width *= 2;
            }
            break;
            case TupBackground::Top2Bottom:
            case TupBackground::Bottom2Top:
            {
                 posY = height;
                 height *= 2;
            }
            break;
    }

    QImage background(width, height, QImage::Format_ARGB32);
    QPainter canvas(&background);
    canvas.drawImage(0, 0, image);
    canvas.drawImage(posX, posY, image);
    setDynamicRaster(background);
}

QImage TupBackground::dynamicView(int frameIndex)
{
    QImage view = QImage(); 
    return view;
}

void TupBackground::setDynamicRaster(QImage bg)
{
    raster = bg;
}

QImage TupBackground::dynamicRaster()
{
    return raster;
}

void TupBackground::setDyanmicDirection(int direction)
{
    dynamicBg->setDynamicDirection(QString::number(direction));
}

void TupBackground::setDyanmicShift(int shift)
{
    dynamicBg->setDynamicShift(QString::number(shift));
}

TupBackground::Direction TupBackground::dyanmicDirection()
{
    return dynamicBg->dynamicDirection();
}

int TupBackground::dyanmicShift()
{
    return dynamicBg->dynamicShift();
}

