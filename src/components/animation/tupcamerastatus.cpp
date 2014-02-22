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

#include "tupcamerastatus.h"
#include "tupexportwidget.h"
#include "tdebug.h"
#include "tconfig.h"
#include "tseparator.h"

#include <QStatusBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>

struct TupCameraStatus::Private
{
    // QComboBox *fps;
    QSpinBox *fps;
    QComboBox *scenes;
    QLabel *framesTotal;
    QCheckBox *loopBox;
    bool loop;
};

TupCameraStatus::TupCameraStatus(TupCameraWidget *camera, bool isNetworked, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMidLineWidth(2);
    setLineWidth(1);

    QBoxLayout *sceneInfoLayout = new QBoxLayout(QBoxLayout::LeftToRight, parent);
    sceneInfoLayout->addStretch(1);

    sceneInfoLayout->setSpacing(0);
    sceneInfoLayout->setMargin(3);

    QFont font = this->font();
    font.setPointSize(8);

    QLabel *sceneNameText = new QLabel("<B>" + tr("Scene name") + ":</B> ");
    sceneNameText->setFont(font);

    k->scenes = new QComboBox();
    k->scenes->setIconSize(QSize(15, 15));
    k->scenes->setMaximumWidth(120);
    k->scenes->setFont(font);
    connect(k->scenes, SIGNAL(activated(int)), this, SIGNAL(sceneIndexChanged(int)));

    sceneInfoLayout->addWidget(sceneNameText, 1);
    sceneInfoLayout->addWidget(k->scenes, 1);
    sceneInfoLayout->addSpacing(20);

    QLabel *label = new QLabel("<B>" + tr("Frames total") + ":</B> ");
    label->setFont(font);

    k->framesTotal = new QLabel;
    k->framesTotal->setFont(font);

    sceneInfoLayout->addWidget(label, 1);
    sceneInfoLayout->addWidget(k->framesTotal, 1);

    sceneInfoLayout->addSpacing(20);

    QLabel *fpsText = new QLabel("<B>" + tr("FPS") + ":</B> ");
    fpsText->setFont(font);

    k->fps = new QSpinBox();
    k->fps->setMinimum(1);
    k->fps->setMaximum(100);
    k->fps->setValue(24);

    connect(k->fps, SIGNAL(valueChanged(int)), camera, SLOT(setFPS(int)));

    sceneInfoLayout->addWidget(fpsText, 1);
    sceneInfoLayout->addWidget(k->fps, 1);

    sceneInfoLayout->addSpacing(20);

    k->loopBox = new QCheckBox();
    QPixmap pix(THEME_DIR + "icons/loop.png");
    k->loopBox->setToolTip(tr("Loop"));
    k->loopBox->setIcon(pix); 
    k->loopBox->setFocusPolicy(Qt::NoFocus);

    k->loopBox->setShortcut(QKeySequence(tr("Ctrl+L")));

    connect(k->loopBox, SIGNAL(clicked()), camera, SLOT(setLoop()));

    TCONFIG->beginGroup("AnimationParameters");
    k->loop = TCONFIG->value("Loop").toBool();
    if (k->loop)
        k->loopBox->setChecked(true);

    sceneInfoLayout->addWidget(k->loopBox, 1);

    sceneInfoLayout->addSpacing(20);

    QPushButton *exportButton = new QPushButton(tr("Export"));
    exportButton->setIcon(QIcon(THEME_DIR + "icons/export_button.png"));
    exportButton->setFont(font);
    exportButton->setFocusPolicy(Qt::NoFocus);

    connect(exportButton, SIGNAL(pressed()), camera, SLOT(exportDialog()));
    sceneInfoLayout->addWidget(exportButton, 1);

    if (isNetworked) {
        sceneInfoLayout->addSpacing(5);
        QPushButton *postButton = new QPushButton(tr("Post"));
        postButton->setIcon(QIcon(THEME_DIR + "icons/import_project.png"));
        postButton->setFont(font);
        postButton->setFocusPolicy(Qt::NoFocus);

        connect(postButton, SIGNAL(pressed()), camera, SLOT(postDialog()));
        sceneInfoLayout->addWidget(postButton, 1);
    }

    setLayout(sceneInfoLayout);
}

TupCameraStatus::~TupCameraStatus()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupCameraStatus::setFPS(int frames)
{
    if (frames > 0 && frames < 101)
        k->fps->setValue(frames);
    else
        k->fps->setValue(24);
}

int TupCameraStatus::getFPS()
{
    return k->fps->value();
}

void TupCameraStatus::setCurrentScene(int index)
{
    if (k->scenes->currentIndex() != index)
        k->scenes->setCurrentIndex(index);
}

void TupCameraStatus::setScenes(TupProject *project)
{
    if (k->scenes->count())
        k->scenes->clear(); 

    foreach (TupScene *scene, project->scenes().values()) { 
             if (scene)
                 k->scenes->addItem(scene->sceneName());
    }
}

void TupCameraStatus::setFramesTotal(const QString &frames)
{
    k->framesTotal->setText(frames);
}

bool TupCameraStatus::isLooping()
{
    k->loop = k->loopBox->isChecked();
    TCONFIG->beginGroup("AnimationParameters");
    TCONFIG->setValue("Loop", k->loop);

    return k->loop;
}
