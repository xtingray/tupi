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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>

struct TupExposureDialog::Private
{
    QVBoxLayout *innerLayout;
    QLabel *sizeLabel;
};

TupExposureDialog::TupExposureDialog(TupProject *project, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Exposure Sheet"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/exposure_sheet.png")));

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setSheet(project);

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

void TupExposureDialog::setSheet(TupProject *project)
{
    QBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(10);

    QBoxLayout *sceneColumn = new QVBoxLayout;
    for (int i=0; i<project->scenesTotal(); i++) {
         QPushButton *scene = new QPushButton(tr("Scene") + " " + QString::number(i+1));
         scene->setFixedSize(120, 80);
         scene->setFont(QFont("Arial", 20, QFont::Bold));
         scene->setCheckable(true);
         connect(scene, SIGNAL(clicked()), this, SLOT(fivePointsLess()));
         sceneColumn->addWidget(scene);
    }

    mainLayout->addLayout(sceneColumn);
    TupScene *scene = project->scene(0);

    QBoxLayout *sceneLayout = new QVBoxLayout;
    for (int j=0; j< scene->layersTotal(); j++) {
         QGroupBox *layerGroup = new QGroupBox(tr("Layer") + " " + QString::number(j+1));
         QBoxLayout *layerLayout = new QHBoxLayout;
         layerLayout->setSpacing(10);
         TupLayer *layer = scene->layer(j);
         for (int i=0; i<layer->framesTotal(); i++) {
              QPushButton *frame = new QPushButton(tr("Frame") + " " + QString::number(i+1));
              frame->setFixedSize(120, 80);
              frame->setFont(QFont("Arial", 20, QFont::Bold));
              frame->setCheckable(true);
              connect(frame, SIGNAL(clicked()), this, SLOT(fivePointsLess()));
              layerLayout->addWidget(frame);
         }
         layerGroup->setLayout(layerLayout);
         sceneLayout->addWidget(layerGroup);
    }

    mainLayout->addLayout(sceneLayout);
   
    k->innerLayout->addLayout(mainLayout);
}
