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

#include <cmath>

TupBackground::TupBackground(TupScene *parent, const QSize size, const QColor color) : QObject(parent)
{
    dimension = size;
    bgColor = color;
    noDynamicRender = true;
    noStaticRender = true;

    dynamicBg = new TupFrame(this, "landscape_dynamic");
    dynamicBg->setDynamicDirection("0");
    dynamicBg->setDynamicShift("5");

    staticBg = new TupFrame(this, "landscape_static");
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
                   staticBg = new TupFrame(this, "landscape_static");

                   if (staticBg) {
                       QString newDoc;

                       {
                           QTextStream ts(&newDoc);
                           ts << n;
                       }

                       staticBg->fromXml(newDoc);
                   }
               } else if (type == "landscape_dynamic") {
                          dynamicBg = new TupFrame(this, "landscape_dynamic");

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
                       QString msg = "TupBackground::fromXml() - Error: The background input is invalid";
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
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

bool TupBackground::dynamicBgIsEmpty()
{
    return dynamicBg->isEmpty();
}

bool TupBackground::staticBgIsEmpty()
{
    return staticBg->isEmpty();
}

TupFrame *TupBackground::staticFrame()
{
    return staticBg;
}

TupFrame* TupBackground::dynamicFrame()
{
    return dynamicBg;
}

void TupBackground::setDynamicOpacity(double opacity)
{
    dynamicBg->setOpacity(opacity);
}

double TupBackground::dynamicOpacity()
{
    return dynamicBg->opacity();
}

void TupBackground::setStaticOpacity(double opacity)
{
    staticBg->setOpacity(opacity);
}

double TupBackground::staticOpacity()
{
    return staticBg->opacity();
}

void TupBackground::renderStaticView()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupBackground::renderStaticView()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupBackgroundScene bgScene(dimension, Qt::transparent, staticBg);
    QImage image(dimension, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    {
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        bgScene.renderView(&painter);
    }

    int width = dimension.width();
    int height = dimension.height();

    QImage background(width, height, QImage::Format_ARGB32);
    background.fill(Qt::transparent);
    QPainter canvas(&background);
    canvas.drawImage(0, 0, image);
    staticBgRaster = background;

    noStaticRender = false;
}

void TupBackground::renderDynamicView()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupBackground::renderDynamicView()]";
        #else
            T_FUNCINFO;
        #endif
    #endif 
	
    TupBackgroundScene bgScene(dimension, bgColor, dynamicBg);
    QImage image(dimension, QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        bgScene.renderView(&painter);
    }

    int width = dimension.width();
    int height = dimension.height();

    QImage background(width*2, height*2, QImage::Format_ARGB32);
    QPainter canvas(&background);
    canvas.drawImage(0, 0, image);
    canvas.drawImage(width, 0, image);
    canvas.drawImage(0, height, image);
    dynamicBgRaster = background;

    noDynamicRender = false;
}

QPixmap TupBackground::dynamicView(int frameIndex)
{
    int posX = 0;
    int posY = 0;
    int shift = dynamicShift();

    TupBackground::Direction direction = dynamicBg->dynamicDirection();
    switch (direction) {
            case TupBackground::Right:
            {
                int delta = dimension.width() / shift;
                if (delta > frameIndex) {
                    posX = dimension.width() - frameIndex*shift;
                } else {
                    int mod = fmod(frameIndex, delta);
                    posX = dimension.width() - (mod * shift);
                }
            }
            break;
            case TupBackground::Left:
            {
                int delta = dimension.width() / shift;
                if (delta > frameIndex) {
                    posX = frameIndex*shift;
                } else {
                    int mod = fmod(frameIndex, delta);
                    posX = mod * shift;
                }
            }
            break;
            case TupBackground::Top:
            {
                int delta = dimension.height() / shift;
                if (delta > frameIndex) {
                    posY = frameIndex*shift;
                } else {
                    int mod = fmod(frameIndex, delta);
                    posY = mod * shift;
                }
            }
            break;
            case TupBackground::Bottom:
            {
                int delta = dimension.height() / shift;
                if (delta > frameIndex) {
                    posY = dimension.height() - frameIndex*shift;
                } else {
                    int mod = fmod(frameIndex, delta);
                    posY = dimension.height() - (mod * shift);
                }
            }
            break;
    }

    QImage view = dynamicBgRaster.copy(posX, posY, dimension.width(), dimension.height()); 
    QPixmap pixmap = QPixmap::fromImage(view); 

    return pixmap;
}

bool TupBackground::dynamicRenderIsPending()
{
    return noDynamicRender;
}

bool TupBackground::staticRenderIsPending()
{
    return noStaticRender;
}

void TupBackground::updateDynamicRenderStatus(bool flag)
{
    noDynamicRender = flag;
}

void TupBackground::updateStaticRenderStatus(bool flag)
{
    noStaticRender = flag;
}

QImage TupBackground::dynamicRaster()
{
    return dynamicBgRaster;
}

QPixmap TupBackground::staticRaster()
{
    QPixmap pixmap = QPixmap::fromImage(staticBgRaster);
    return pixmap;
}

void TupBackground::setDynamicDirection(int direction)
{
    dynamicBg->setDynamicDirection(QString::number(direction));
}

void TupBackground::setDynamicShift(int shift)
{
    dynamicBg->setDynamicShift(QString::number(shift));
}

TupBackground::Direction TupBackground::dynamicDirection()
{
    return dynamicBg->dynamicDirection();
}

int TupBackground::dynamicShift()
{
    return dynamicBg->dynamicShift();
}

TupScene * TupBackground::scene()
{
    return static_cast<TupScene *>(parent());
}

TupProject * TupBackground::project()
{
    return scene()->project();
}

