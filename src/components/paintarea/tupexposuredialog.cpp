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

#include "tupexposuredialog.h"
#include "tdebug.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "timagebutton.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tpushbutton.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>

struct TupExposureDialog::Private
{
    QVBoxLayout *innerLayout;
    QBoxLayout *sceneLayout;
    QLabel *sizeLabel;
    int currentScene;
    QList<int> currentLayer;
    QList<int> currentFrame;
    QList<TPushButton *> sceneList;
    QList<TPushButton *> frameList;
    TupProject *project;
    QList<QGroupBox *> sceneGroupList;
};

TupExposureDialog::TupExposureDialog(TupProject *project, int scene, int layer, int frame, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/exposure_sheet.png")));

    k->project = project;

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setSheet(scene, layer, frame);

    TImageButton *closeButton = new TImageButton(QPixmap(THEME_DIR + "icons/close_big.png"), 40, this, true);
    closeButton->setToolTip(tr("Close"));
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    k->innerLayout->addWidget(new TSeparator());
    k->innerLayout->addWidget(buttonBox);

    layout->addLayout(k->innerLayout);
}

TupExposureDialog::~TupExposureDialog()
{
}

/*
QSize TupExposureDialog::sizeHint() const
{
    return QSize(700, 250);
}
*/

void TupExposureDialog::setSheet(int sceneIndex, int layerIndex, int frameIndex)
{
    QBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(10);

    QBoxLayout *sceneColumn = new QVBoxLayout;

    for (int i=0; i < k->project->scenesTotal(); i++) {

         // List of scene buttons
         TPushButton *sceneButton = new TPushButton(this, tr("Scene") + " " + QString::number(i+1), 0, i);
         sceneButton->setFixedSize(120, 80);
         sceneButton->setFont(QFont("Arial", 20, QFont::Bold));
         sceneButton->setCheckable(true);
         connect(sceneButton, SIGNAL(clicked(int, int)), this, SLOT(goToScene(int, int)));
         if (i == sceneIndex) {
             sceneButton->setChecked(true);
             sceneButton->setDisabled(true);
             k->currentScene = i;
         }
         sceneColumn->addWidget(sceneButton);
         k->sceneList << sceneButton;

         // Exposure sheet interface for every Scene

         TupScene *scene = k->project->scene(i);
         QGroupBox *sceneGroup = new QGroupBox(tr("Scene") + " " + QString::number(i + 1));
         k->sceneLayout = new QVBoxLayout;

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
                   if (j == layerIndex && t == frameIndex) {
                       frameButton->setChecked(true);
                       frameButton->setDisabled(true);
                       k->currentLayer << j;
                       k->currentFrame << t;
                   }
                   k->frameList << frameButton;
              }

              layerGroup->setLayout(layerLayout);
              k->sceneLayout->addWidget(layerGroup);
              sceneGroup->setLayout(k->sceneLayout);
         }

         if (i != sceneIndex)
             sceneGroup->hide();

         k->sceneGroupList << sceneGroup;
    }

    mainLayout->addLayout(sceneColumn);

    for(int i=0; i < k->sceneGroupList.size(); i++)
        mainLayout->addWidget(k->sceneGroupList.at(i));

    k->innerLayout->addLayout(mainLayout);
}

void TupExposureDialog::goToScene(int column, int row)
{
    tError() << "TupExposureDialog::goToScene() - Coord: [ " << column << ", " << row << " ]";
    k->sceneGroupList.at(k->currentScene)->hide();

    for(int i=0; i<k->sceneList.size(); i++) {
        if (i == row) {
            k->sceneList.at(i)->setChecked(true);
            k->sceneList.at(i)->setDisabled(true);
            k->currentScene = i;
        } else {
            k->sceneList.at(i)->setChecked(false);
            k->sceneList.at(i)->setDisabled(false);
            // k->sceneList.at(i)->clearFocus();
        }
    } 

    k->sceneGroupList.at(row)->show();
    // k->sceneGroupList.at(row)->clearFocus();
 
    goToFrame(k->currentFrame.at(row), k->currentLayer.at(row));

    tError() << "TupExposureDialog::goToScene() - Current Scene: " << row;
    tError() << "TupExposureDialog::goToScene() - Current Layer: " << k->currentLayer.at(row);
    tError() << "TupExposureDialog::goToScene() - Current Frame: " << k->currentFrame.at(row);
}

void TupExposureDialog::goToFrame(int column, int row)
{
    tError() << "TupExposureDialog::goToFrame() - Coord: [ " << column << ", " << row << " ]";
    TupScene *scene = k->project->scene(k->currentScene);
    int oneRow = scene->framesTotal();
    int index = column + oneRow*row;

    for(int i=0; i<k->frameList.size(); i++) {
        if (i == index) {
            k->frameList.at(i)->setChecked(true);
            k->frameList.at(i)->setDisabled(true);
            k->currentLayer.replace(k->currentScene, row);
            k->currentFrame.replace(k->currentScene, column);
        } else {
            k->frameList.at(i)->setChecked(false);
            k->frameList.at(i)->setDisabled(false);
            k->frameList.at(i)->clearFocus();
        }
    }

    for(int i=0; i<k->sceneList.size(); i++) {
        k->sceneList.at(i)->clearFocus();
    }
}
