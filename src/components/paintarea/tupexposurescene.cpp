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

#include "tupexposurescene.h"
#include "tpushbutton.h"
#include "tuplayer.h"
#include "tdebug.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

struct TupExposureScene::Private
{
    TupScene *scene;
    int currentLayer;
    int currentFrame;
    QList<TPushButton *> frameList;
};

TupExposureScene::TupExposureScene(const QString &title, TupScene *scene, int currentLayer, int currentFrame) : QGroupBox(title), k(new Private)
{
    k->scene = scene;
    k->currentLayer = currentLayer;
    k->currentFrame = currentFrame;

    QVBoxLayout *sceneLayout = new QVBoxLayout;

    for (int j=0; j< scene->layersTotal(); j++) {
         QGroupBox *layerGroup = new QGroupBox(tr("Layer") + " " + QString::number(j+1));
         QBoxLayout *layerLayout = new QHBoxLayout;
         layerLayout->setSpacing(10);
         TupLayer *layer = scene->layer(j);

         for (int t=0; t < layer->framesTotal(); t++) {
              TPushButton *frameButton = new TPushButton(this, tr("Frame") + " " + QString::number(t+1), t, j);
              frameButton->setFixedSize(120, 80);
              frameButton->setFont(QFont("Arial", 20, QFont::Bold));
              frameButton->setCheckable(true);
              connect(frameButton, SIGNAL(clicked(int, int)), this, SLOT(goToFrame(int, int)));
              layerLayout->addWidget(frameButton);
              if (j == currentLayer && t == currentFrame) {
                  frameButton->setChecked(true);
                  frameButton->setDisabled(true);
              }
              k->frameList << frameButton;
         }

         layerGroup->setLayout(layerLayout);
         sceneLayout->addWidget(layerGroup);
    }

    setLayout(sceneLayout);
}

TupExposureScene::~TupExposureScene()
{
}

void TupExposureScene::goToFrame(int frame, int layer)
{
    int oneRow = k->scene->framesTotal();
    int index = frame + oneRow*layer;

    for(int i=0; i<k->frameList.size(); i++) {
        if (i == index) {
            k->frameList.at(i)->setChecked(true);
            k->frameList.at(i)->setDisabled(true);
            k->currentLayer = layer;
            k->currentFrame = frame;
        } else {
            k->frameList.at(i)->setChecked(false);
            k->frameList.at(i)->setDisabled(false);
            k->frameList.at(i)->clearFocus();
        }
    }

    emit updateUI(frame, layer);
}

int TupExposureScene::currentFrame()
{
    return k->currentFrame;
}

int TupExposureScene::currentLayer()
{
    return k->currentLayer;
}

int TupExposureScene::framesTotal()
{
    return k->scene->framesTotal();
}

int TupExposureScene::layersTotal()
{
    return k->scene->layersTotal();
}
