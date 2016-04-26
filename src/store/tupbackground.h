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

#ifndef TUPBACKGROUND_H
#define TUPBACKGROUND_H

#include "tglobal.h"
#include "tupabstractserializable.h"
#include "tupscene.h"

#include <QImage>
#include <QPixmap>

/**
 * @author Gustav Gonzalez
*/

class TUPI_EXPORT TupBackground : public QObject, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        enum Direction { Right = 0, Left = 1, Top, Bottom };
        TupBackground(TupScene *parent, const QSize dimension, const QColor bgColor);
        ~TupBackground();

        void setBgColor(const QColor color);
        TupFrame* staticFrame();
        TupFrame* dynamicFrame(); 
        void clear();
        void renderDynamicView();
        QPixmap dynamicView(int frameIndex);
        void setDynamicDirection(int direction);
        void setDynamicShift(int shift);
        Direction dyanmicDirection();
        int dyanmicShift();
        void setDynamicRaster(QImage bg);
        QImage dynamicRaster();
        bool dynamicBgIsEmpty();
        bool staticBgIsEmpty();
        bool rasterRenderIsPending();
        void setDynamicOpacity(double opacity); 
        double dynamicOpacity();
        void setStaticOpacity(double opacity);
        double staticOpacity();
        TupScene * scene();
        TupProject * project();

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        QSize dimension;
        QColor bgColor;
        TupFrame *staticBg;
        TupFrame *dynamicBg;
        QImage raster;
        bool noRender;
};

#endif
