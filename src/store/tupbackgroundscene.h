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

#ifndef TUPBACKGROUNDSCENE_H
#define TUPBACKGROUNDSCENE_H

#include "tglobal.h"
#include "tupframe.h"
#include "tupgraphicobject.h"
#include "tupsvgitem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>

class TUPI_EXPORT TupBackgroundScene : public QGraphicsScene
{
    Q_OBJECT

    public:
        TupBackgroundScene(const QSize dimension, const QColor color, TupFrame *background);
        ~TupBackgroundScene();
        void renderView(QPainter *painter);

    private:
        void drawScene();
        void cleanWorkSpace();
        void addFrame(TupFrame *frame);
        void addGraphicObject(TupGraphicObject *object);
        void addSvgObject(TupSvgItem *svgItem);

        struct Private;
        Private *const k;
};

#endif
