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

#include "tupanimationrenderer.h"
#include "tupgraphicsscene.h"
#include "tuplayer.h"
#include "tupframe.h"
#include "tupscene.h"
#include "tupgraphicobject.h"

struct TupAnimationRenderer::Private
{
    TupGraphicsScene *scene;
    int totalPhotograms;
    int currentPhotogram;
    QColor bgColor;

    Private() : scene(0), totalPhotograms(-1), currentPhotogram(0) {}

    ~Private() {
         delete scene;
    }

    int calculateTotalPhotograms(TupScene *scene);
};

TupAnimationRenderer::TupAnimationRenderer(const QColor color, TupLibrary *library) : k(new Private)
{
    k->bgColor = color;
    k->scene = new TupGraphicsScene;
    k->scene->setLibrary(library);
    k->scene->setBackgroundBrush(k->bgColor);
}

TupAnimationRenderer::~TupAnimationRenderer()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupAnimationRenderer()]";
        #else
            TEND;
        #endif
    #endif

    // SQA: Check why this instruction crashes the application 
    // delete k;
}

int TupAnimationRenderer::Private::calculateTotalPhotograms(TupScene *scene)
{
    Layers layers = scene->layers();

    int total = 0;

    int totalLayers = layers.size();
    for (int i = 0; i < totalLayers; i++) {
         TupLayer *layer = layers.at(i);
         if (layer)
             total = qMax(total, layer->frames().count());
    }

    return total;
}

void TupAnimationRenderer::setScene(TupScene *scene, QSize dimension)
{
    k->scene->setCurrentScene(scene);
    k->scene->setSceneRect(QRectF(QPointF(0,0), dimension));

    // k->scene->drawPhotogram(0, false); // ### SQA: Why whithout this doesn't work?
    k->currentPhotogram = -1;

    k->totalPhotograms = k->calculateTotalPhotograms(scene);
}

bool TupAnimationRenderer::nextPhotogram()
{
    if (k->totalPhotograms < 0) 
        return false;

    k->currentPhotogram++;

    if (k->currentPhotogram == k->totalPhotograms)
        return false;

    k->scene->drawPhotogram(k->currentPhotogram, false);

    return true;
}

void TupAnimationRenderer::renderPhotogram(int index) 
{
    k->scene->drawPhotogram(index, false);
}

void TupAnimationRenderer::render(QPainter *painter)
{
    k->scene->render(painter, k->scene->sceneRect().toRect(), k->scene->sceneRect().toRect(), Qt::IgnoreAspectRatio);
}

int TupAnimationRenderer::currentPhotogram() const
{
    return k->currentPhotogram;
}

int TupAnimationRenderer::totalPhotograms() const
{
    return k->totalPhotograms;
}

