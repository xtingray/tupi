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

struct TupExposureDialog::Private
{
    QVBoxLayout *innerLayout;
    int currentScene;
    QList<TPushButton *> sceneList;
    TupProject *project;
    QList<TupExposureScene *> sceneGroupList;
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

    mainLayout->addLayout(sceneColumn);

    for(int i=0; i < k->sceneGroupList.size(); i++)
        mainLayout->addWidget(k->sceneGroupList.at(i));

    k->innerLayout->addLayout(mainLayout);
}

void TupExposureDialog::goToScene(int column, int sceneIndex)
{
    tError() << "TupExposureDialog::goToScene() - Coord: [ " << column << ", " << sceneIndex << " ]";
    k->sceneGroupList.at(k->currentScene)->hide();

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

    emit goToFrame(sceneTable->currentFrame(), sceneTable->currentLayer(), k->currentScene);
}

void TupExposureDialog::refreshUI(int frame, int layer)
{
    for(int i=0; i<k->sceneList.size(); i++) {
        k->sceneList.at(i)->clearFocus();
    }

    emit goToFrame(frame, layer, k->currentScene);
}
