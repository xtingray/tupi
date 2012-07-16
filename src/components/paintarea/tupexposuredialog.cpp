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
    QVBoxLayout *innerLayout;
    int currentScene;
    QList<TPushButton *> sceneList;
    TupProject *project;
    QList<TupExposureScene *> sceneGroupList;
    bool isNetworked;
    QListWidget *onlineList;
    QStringList usersOnLine;
};

TupExposureDialog::TupExposureDialog(TupProject *project, int scene, int layer, int frame, 
                                     bool isNetworked, const QStringList &usersOnLine, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    // setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/exposure_sheet.png")));

    k->project = project;
    k->isNetworked = isNetworked;
    k->usersOnLine = usersOnLine;

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setSheet(scene, layer, frame);

    TImageButton *closeButton = new TImageButton(QPixmap(THEME_DIR + "icons/close_big.png"), 60, this, true);
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

void TupExposureDialog::setSheet(int sceneIndex, int layerIndex, int frameIndex)
{
    QBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(10);

    QBoxLayout *sceneColumn = new QVBoxLayout;

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
         sceneColumn->addWidget(sceneButton);
         k->sceneList << sceneButton;

         // Exposure sheet interface for every Scene
         TupExposureScene *sceneGroup;

         if (i != sceneIndex) {
             sceneGroup = new TupExposureScene(tr("Scene") + " " + QString::number(i + 1), k->project->scene(i), 0, 0);
             sceneGroup->hide();
         } else {
             sceneGroup = new TupExposureScene(tr("Scene") + " " + QString::number(i + 1), k->project->scene(i), layerIndex, frameIndex);
         }

         connect(sceneGroup, SIGNAL(updateUI(int, int)), this, SLOT(refreshUI(int, int)));

         k->sceneGroupList << sceneGroup;
    }

    if (k->isNetworked) {
        QGroupBox *users= new QGroupBox(tr("Work Team"));
        QVBoxLayout *layout = new QVBoxLayout; 

        k->onlineList = new QListWidget(this);
        k->onlineList->setFixedWidth(120);
        if (k->usersOnLine.size() > 0) {
            for (int j=0; j < k->usersOnLine.size(); j++) {
                 QListWidgetItem *item = new QListWidgetItem(k->usersOnLine.at(j), k->onlineList); 
            }
        }

        layout->addWidget(k->onlineList);
        users->setLayout(layout);
        mainLayout->addWidget(users);
    }

    mainLayout->addLayout(sceneColumn);

    for(int i=0; i < k->sceneGroupList.size(); i++)
        mainLayout->addWidget(k->sceneGroupList.at(i));

    k->innerLayout->addLayout(mainLayout);
}

void TupExposureDialog::goToScene(int column, int sceneIndex)
{
    Q_UNUSED(column);
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

    if (newFramesTotal != oldFramesTotal || newLayersTotal != oldLayersTotal) {
        QDesktopWidget desktop;
        move((int) (desktop.screenGeometry().width() - width())/2 ,
             (int) (desktop.screenGeometry().height() - height())/2);
    }
}

void TupExposureDialog::refreshUI(int frame, int layer)
{
    for(int i=0; i<k->sceneList.size(); i++) {
        k->sceneList.at(i)->clearFocus();
    }

    emit goToFrame(frame, layer, k->currentScene);
}
