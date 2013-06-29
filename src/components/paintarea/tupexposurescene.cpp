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
    int layerCounter;
    int frameCounter;

    QList<QHBoxLayout *> layerList;
    QList<TPushButton *> frameList;
    QVBoxLayout *sceneLayout;
};

TupExposureScene::TupExposureScene(const QString &title, TupScene *scene, int currentLayer, int currentFrame) : QGroupBox(title), k(new Private)
{
    k->scene = scene;
    k->currentLayer = currentLayer;
    k->currentFrame = currentFrame;

    k->sceneLayout = new QVBoxLayout;
    k->layerCounter = scene->layersTotal();
    k->frameCounter = k->scene->framesTotal();

    for (int j=0; j< scene->layersTotal(); j++) {

         if (j < 4) { // SQA: Temporary condition
             QGroupBox *layerGroup = new QGroupBox(tr("Layer") + " " + QString::number(j+1));
             QHBoxLayout *layerLayout = new QHBoxLayout;
             layerLayout->setSpacing(10);
             TupLayer *layer = scene->layer(j);

             for (int t=0; t < layer->framesTotal(); t++) {
                  if (t < 9) { // SQA: Temporary condition
                      TPushButton *frameButton = new TPushButton(this, tr("Frame") + " " + QString::number(t+1), t, j);
                      frameButton->setFixedSize(100, 70);
                      frameButton->setFont(QFont("Arial", 14, QFont::Bold));
                      frameButton->setCheckable(true);
                      connect(frameButton, SIGNAL(clicked(int, int)), this, SLOT(goToFrame(int, int)));
                      layerLayout->addWidget(frameButton);
                      if (j == currentLayer && t == currentFrame) {
                          frameButton->setChecked(true);
                          frameButton->setDisabled(true);
                      }
                      k->frameList << frameButton;
                  }
             }

             layerGroup->setLayout(layerLayout);
             k->layerList << layerLayout;
             k->sceneLayout->addWidget(layerGroup);
         }
    }

    setLayout(k->sceneLayout);
}

TupExposureScene::~TupExposureScene()
{
}

void TupExposureScene::addNewLayer()
{
    for(int i=0; i<k->frameList.size(); i++) {
        k->frameList.at(i)->setChecked(false);
        k->frameList.at(i)->setDisabled(false);
        k->frameList.at(i)->clearFocus();
    }

    k->currentLayer = k->layerCounter;
    k->layerCounter++;

    QGroupBox *layerGroup = new QGroupBox(tr("Layer") + " " + QString::number(k->layerCounter));
    QHBoxLayout *layerLayout = new QHBoxLayout;
    layerLayout->setSpacing(10);

    int oneRow = k->frameCounter;
    // k->scene->framesTotal();
    for(int i=0; i<oneRow; i++) {
        // TPushButton *frameButton = new TPushButton(this, tr("Frame") + " " + QString::number(i + 1), i, k->layerCounter - 1);
        TPushButton *frameButton = new TPushButton(this, tr("Frame") + " " + QString::number(i + 1), i, k->currentLayer);
        frameButton->setFixedSize(100, 70);
        frameButton->setFont(QFont("Arial", 14, QFont::Bold));
        frameButton->setCheckable(true);
        if (i == 0) {
            frameButton->setChecked(true);
            frameButton->setDisabled(true);
        }
        connect(frameButton, SIGNAL(clicked(int, int)), this, SLOT(goToFrame(int, int)));

        layerLayout->addWidget(frameButton);
        k->frameList << frameButton; 
    }

    layerGroup->setLayout(layerLayout);
    k->layerList << layerLayout;

    k->sceneLayout->addWidget(layerGroup);
}

void TupExposureScene::addNewFrame()
{
    tError() << "TupExposureScene::addNewFrame() - frameList size: " << k->frameList.size();
    tError() << "TupExposureScene::addNewFrame() - current layer: " << k->currentLayer;

    for(int j=0; j<k->frameList.size(); j++) {
        k->frameList.at(j)->setChecked(false);
        k->frameList.at(j)->setDisabled(false);
        k->frameList.at(j)->clearFocus();
    }

    // int frameIndex = k->layerList.at(0)->count();
    int frameIndex = k->frameCounter;
    k->frameCounter++;

    for(int i=0; i<k->layerList.size(); i++) {
        TPushButton *frameButton = new TPushButton(this, tr("Frame") + " " + QString::number(frameIndex + 1), frameIndex, i);
        frameButton->setFixedSize(100, 70);
        frameButton->setFont(QFont("Arial", 14, QFont::Bold));
        frameButton->setCheckable(true);
        if (i == k->currentLayer) {
            frameButton->setChecked(true);
            frameButton->setDisabled(true);
        }
        connect(frameButton, SIGNAL(clicked(int, int)), this, SLOT(goToFrame(int, int)));

        k->layerList.at(i)->addWidget(frameButton);

        // Find out the index for this ->  k->frameList << frameButton;
        // k->frameList.insert(k->frameCounter*i + 1, frameButton);
        int position = frameIndex;
        if (i > 0)
            position = k->frameCounter*i + frameIndex;

        k->frameList.insert(position, frameButton);
 
        tError() << "TupExposureScene::addNewFrame() -> inserting frame at position: " << position;
    }
}

void TupExposureScene::goToFrame(int frame, int layer)
{
    tError() << "TupExposureScene::goToFrame(int, int) - frame: " << frame << " - layer: " << layer;

    // int oneRow = k->scene->framesTotal();
    int index = frame + k->frameCounter*layer;

    tError() << "TupExposureScene::goToFrame(int, int) - index: " << index;

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
    // return k->scene->layersTotal();
    return k->layerCounter;
}
