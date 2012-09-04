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
#include "tupexposurescene.h"
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
#include <QListWidget>
#include <QDesktopWidget>

struct TupExposureDialog::Private
{
    QVBoxLayout *layout;
    int currentScene;
    int currentLayer;
    QList<TPushButton *> sceneList;
    TupProject *project;
    QList<TupExposureScene *> sceneGroupList;
    bool isNetworked;
    QListWidget *onlineList;
    QStringList onLineUsers;
    QBoxLayout *sceneColumn;
    QHBoxLayout *mainLayout;
};

TupExposureDialog::TupExposureDialog(TupProject *project, int scene, int layer, int frame, 
                                     bool isNetworked, const QStringList &onLineUsers, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/exposure_sheet.png")));

    k->project = project;
    k->isNetworked = isNetworked;
    k->onLineUsers = onLineUsers;

    k->layout = new QVBoxLayout(this);

    setButtonBar();
    setSheet(scene, layer, frame);

    TImageButton *closeButton = new TImageButton(QPixmap(THEME_DIR + "icons/close_big.png"), 60, this, true);
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    k->layout->addWidget(new TSeparator());
    k->layout->addWidget(buttonBox);
}

TupExposureDialog::~TupExposureDialog()
{
}

void TupExposureDialog::setButtonBar()
{
    QBoxLayout *buttonsLayout = new QHBoxLayout;

    TImageButton *sceneButton = new TImageButton(QPixmap(THEME_DIR + "icons/add_scene_big.png"), 60, this, true);
    connect(sceneButton, SIGNAL(clicked()), this, SLOT(createScene()));

    TImageButton *layerButton = new TImageButton(QPixmap(THEME_DIR + "icons/add_layer_big.png"), 60, this, true);
    connect(layerButton, SIGNAL(clicked()), this, SLOT(createLayer()));

    TImageButton *frameButton = new TImageButton(QPixmap(THEME_DIR + "icons/add_frame_big.png"), 60, this, true);
    connect(frameButton, SIGNAL(clicked()), this, SLOT(createFrame()));

    buttonsLayout->addWidget(sceneButton);
    buttonsLayout->addWidget(layerButton);
    buttonsLayout->addWidget(frameButton);

    k->layout->addLayout(buttonsLayout);
}

void TupExposureDialog::setSheet(int sceneIndex, int layerIndex, int frameIndex)
{
    k->mainLayout = new QHBoxLayout;
    k->mainLayout->setContentsMargins(5, 5, 5, 5);
    k->mainLayout->setSpacing(10);

    k->sceneColumn = new QVBoxLayout;

    for (int i=0; i < k->project->scenesTotal(); i++) {

         // List of scene buttons
         TPushButton *sceneButton = new TPushButton(this, tr("Scene") + " " + QString::number(i+1), 0, i);
         sceneButton->setFixedSize(100, 70);
         sceneButton->setFont(QFont("Arial", 14, QFont::Bold));
         sceneButton->setCheckable(true);
         connect(sceneButton, SIGNAL(clicked(int, int)), this, SLOT(goToScene(int, int)));
         if (i == sceneIndex) {
             sceneButton->setChecked(true);
             sceneButton->setDisabled(true);
             k->currentScene = i;
         }
         k->sceneColumn->addWidget(sceneButton);
         k->sceneList << sceneButton;

         // Exposure sheet interface for every Scene
         TupExposureScene *sceneGroup;

         if (i != sceneIndex) {
             sceneGroup = new TupExposureScene(tr("Scene") + " " + QString::number(i + 1), k->project->scene(i), 0, 0);
             sceneGroup->hide();
         } else {
             sceneGroup = new TupExposureScene(tr("Scene") + " " + QString::number(i + 1), k->project->scene(i), layerIndex, frameIndex);
             k->currentLayer = layerIndex;
         }

         connect(sceneGroup, SIGNAL(updateUI(int, int)), this, SLOT(refreshUI(int, int)));

         k->sceneGroupList << sceneGroup;
    }

    if (k->isNetworked) {
        QGroupBox *users= new QGroupBox(tr("Work Team"));
        QVBoxLayout *layout = new QVBoxLayout; 

        k->onlineList = new QListWidget(this);
        k->onlineList->setFixedWidth(100);

        for (int j=0; j < k->onLineUsers.size(); j++)
             new QListWidgetItem(k->onLineUsers.at(j), k->onlineList);

        layout->addWidget(k->onlineList);
        users->setLayout(layout);
        k->mainLayout->addWidget(users);
    }

    k->mainLayout->addLayout(k->sceneColumn);

    for(int i=0; i < k->sceneGroupList.size(); i++)
        k->mainLayout->addWidget(k->sceneGroupList.at(i));

    k->layout->addLayout(k->mainLayout);
}

void TupExposureDialog::goToScene(int column, int sceneIndex)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif
    Q_UNUSED(column);

    tError() << "TupExposureDialog::goToScene() - sceneIndex: " << sceneIndex;

    TupExposureScene *oldScene = k->sceneGroupList.at(k->currentScene);
    oldScene->hide();
    int oldFramesTotal = oldScene->framesTotal(); 
    int oldLayersTotal = oldScene->layersTotal();

    for(int i=0; i<k->sceneList.size(); i++) {
        if (i == sceneIndex) {
            k->sceneList.at(i)->setChecked(true);
            k->sceneList.at(i)->setDisabled(true);
            k->currentScene = i;
        } else {
            k->sceneList.at(i)->setChecked(false);
            k->sceneList.at(i)->setDisabled(false);
        }
    } 

    TupExposureScene *sceneTable = k->sceneGroupList.at(sceneIndex);
    sceneTable->show();
    int newFramesTotal = sceneTable->framesTotal();
    int newLayersTotal = sceneTable->layersTotal();

    emit goToScene(k->currentScene);
    emit goToFrame(sceneTable->currentFrame(), sceneTable->currentLayer(), k->currentScene);

    k->currentLayer = sceneTable->currentLayer();

    if (newFramesTotal != oldFramesTotal || newLayersTotal != oldLayersTotal) {
        QDesktopWidget desktop;
        move((int) (desktop.screenGeometry().width() - width())/2 ,
             (int) (desktop.screenGeometry().height() - height())/2);
    }
}

void TupExposureDialog::refreshUI(int frame, int layer)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    for(int i=0; i<k->sceneList.size(); i++) {
        k->sceneList.at(i)->clearFocus();
    }

    k->currentLayer = layer;

    tError() << "TupExposureDialog::refreshUI() - Going to frame -> " << frame;

    emit goToFrame(frame, layer, k->currentScene);
}

void TupExposureDialog::updateUsersList(const QStringList &onLineUsers)
{
    k->onlineList->clear();
    k->onlineList->addItems(onLineUsers);    
}

void TupExposureDialog::closeDialog()
{
    emit windowHasBeenClosed();
    close();    
}

void TupExposureDialog::createScene()
{
    int scene = k->project->scenesTotal();

    if (scene > 5) // Temporary condition
        return;

    scene++;

    emit callNewScene();

    int oldIndex = k->currentScene;

    for(int i=0; i<k->sceneList.size(); i++) {
        k->sceneList.at(i)->setChecked(false);
        k->sceneList.at(i)->setDisabled(false);
    }

    TPushButton *sceneButton = new TPushButton(this, tr("Scene") + " " + QString::number(scene), 0, scene-1);
    sceneButton->setFixedSize(100, 70);
    sceneButton->setFont(QFont("Arial", 14, QFont::Bold));
    sceneButton->setCheckable(true);
    sceneButton->setChecked(true);
    sceneButton->setDisabled(true);
    connect(sceneButton, SIGNAL(clicked(int, int)), this, SLOT(goToScene(int, int)));

    k->currentScene = scene-1;
    k->sceneColumn->addWidget(sceneButton);
    k->sceneList << sceneButton;

    tError() << "TupExposureDialog::createScene() - Scenes Total: " << scene;
    tError() << "TupExposureDialog::createScene() - Last index: " << oldIndex;

    TupExposureScene *sceneGroup = new TupExposureScene(tr("Scene") + " " + QString::number(scene), k->project->scene(scene-1), 0, 0);
    connect(sceneGroup, SIGNAL(updateUI(int, int)), this, SLOT(refreshUI(int, int)));
    k->sceneGroupList << sceneGroup;
    k->mainLayout->addWidget(sceneGroup);

    TupExposureScene *oldScene = k->sceneGroupList.at(oldIndex);
    oldScene->hide();

    TupExposureScene *sceneTable = k->sceneGroupList.at(scene-1);
    sceneTable->show();

    QDesktopWidget desktop;
    move((int) (desktop.screenGeometry().width() - width())/2,
         (int) (desktop.screenGeometry().height() - (height() + sceneButton->height()))/2);
}

void TupExposureDialog::createLayer()
{
    TupExposureScene *sceneTable = k->sceneGroupList.at(k->currentScene);

    int layersTotal = sceneTable->layersTotal();
    if (layersTotal > 2) // Temporary condition
        return;

    sceneTable->addNewLayer();

    emit callNewLayer(k->currentScene, layersTotal);

    QDesktopWidget desktop;
    move((int) (desktop.screenGeometry().width() - width())/2,
         (int) (desktop.screenGeometry().height() - (height() + 100))/2);
}

void TupExposureDialog::createFrame()
{
    TupExposureScene *sceneTable = k->sceneGroupList.at(k->currentScene);
    sceneTable->addNewFrame();

    int layersTotal = sceneTable->layersTotal();
    int framesTotal = sceneTable->framesTotal();
    // emit callNewFrame(k->currentScene, k->currentLayer, 0);
    emit callNewFrame(k->currentScene, k->currentLayer, layersTotal, framesTotal);

    QDesktopWidget desktop;
    move((int) (desktop.screenGeometry().width() - (width() + 50))/2,
         (int) (desktop.screenGeometry().height() - (height() + 100))/2);
}
