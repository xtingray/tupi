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

#include "ktviewcamera.h"
#include "ktexportwidget.h"
// #include "postdialog.h"
#include "tdebug.h"
#include "tseparator.h"
#include "ktprojectrequest.h"
#include "ktprojectresponse.h"
#include "ktrequestbuilder.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

struct KTViewCamera::Private
{
    QFrame *container;
    KTAnimationArea *animationArea;
    KTCameraStatus *status;
    KTProject *project;
    int currentSceneIndex;
};

KTViewCamera::KTViewCamera(KTProject *project, bool isNetworked, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->project = project;

    setObjectName("KTViewCamera_");

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->setAlignment(Qt::AlignCenter);
    labelLayout->setSpacing(0);

    QFont font = this->font();
    font.setPointSize(10);
    font.setBold(true);
    QLabel *name = new QLabel(k->project->projectName() + ": ");
    name->setFont(font);

    font = this->font();
    font.setPointSize(10);
    QLabel *description = new QLabel(k->project->description());
    description->setFont(font);

    labelLayout->addWidget(name); 
    labelLayout->addWidget(description);

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap(THEME_DIR + "icons/camera.png"));
    QLabel *title = new QLabel(tr("Render Camera Preview"));

    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);

    layout->addWidget(titleWidget, Qt::AlignCenter);
    layout->addLayout(labelLayout, Qt::AlignCenter);

    k->animationArea = new KTAnimationArea(k->project);
    layout->addWidget(k->animationArea, 0, Qt::AlignCenter);

    KTCameraBar *m_bar = new KTCameraBar;
    layout->addWidget(m_bar, 0, Qt::AlignCenter);
    m_bar->show();

    connect(m_bar, SIGNAL(play()), this, SLOT(doPlay()));
    connect(m_bar, SIGNAL(playBack()), this, SLOT(doPlayBack()));
    connect(m_bar, SIGNAL(stop()), k->animationArea, SLOT(stop()));
    connect(m_bar, SIGNAL(ff()), k->animationArea, SLOT(nextFrame()));
    connect(m_bar, SIGNAL(rew()), k->animationArea, SLOT(previousFrame()));

    k->status = new KTCameraStatus(this, isNetworked);
    k->status->setScenes(k->project); 
    connect(k->status, SIGNAL(sceneIndexChanged(int)), this, SLOT(selectScene(int)));

    k->status->setFPS(k->project->fps());
    setLoop();

    layout->addWidget(k->status, 0, Qt::AlignCenter|Qt::AlignTop);

    setLayout(layout);
}

KTViewCamera::~KTViewCamera()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void KTViewCamera::setLoop()
{
    k->animationArea->setLoop(k->status->isLooping());
}

QSize KTViewCamera::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void KTViewCamera::doPlay()
{
    k->animationArea->play();
    // updateSceneInfo();
}

void KTViewCamera::doPlayBack()
{
    k->animationArea->playBack();
    // updateSceneInfo();
}

void KTViewCamera::doStop()
{
    k->animationArea->stop();
}

void KTViewCamera::nextFrame()
{
    k->animationArea->nextFrame();
}

void KTViewCamera::previousFrame()
{
    k->animationArea->previousFrame();
}

/*
void KTViewCamera::updateSceneInfo()
{
    KTScene *scene = k->animationArea->currentScene();
    if (scene)
        showSceneInfo(scene);
}
*/

bool KTViewCamera::handleProjectResponse(KTProjectResponse *response)
{
    if (KTSceneResponse *sceneResponse = static_cast<KTSceneResponse *>(response)) {

        int index = sceneResponse->sceneIndex();

        switch (sceneResponse->action()) {
            case KTProjectRequest::Add:
            {
                 k->status->setScenes(k->project);
                 k->status->setCurrentScene(index);
            }
            break;
            case KTProjectRequest::Remove:
            {
                 if (index < 0)
                     break;

                 k->status->setScenes(k->project);

                 if (index == k->project->scenesTotal())
                     index--;

                 k->animationArea->updateSceneIndex(index);
                 k->status->setCurrentScene(index);
            }
            break;
            case KTProjectRequest::Reset:
            {
                 k->status->setScenes(k->project);
            }
            break;
            case KTProjectRequest::Select:
            {
                 if (index >= 0) {
                     k->animationArea->updateSceneIndex(index);
                     k->status->setCurrentScene(index);
                 }
            }
            break;
            case KTProjectRequest::Rename:
            {
                 k->status->setScenes(k->project);
                 k->status->setCurrentScene(index);
            }
            break;
            /*
            default:
            {
            }
            break;
            */
        }
    }

    return k->animationArea->handleResponse(response);
}

void KTViewCamera::setFPS(int fps)
{
    fps++;
    k->project->setFPS(fps);
    //k->status->setFPS(fps);
    k->animationArea->setFPS(fps);
}

void KTViewCamera::updatePhotograms()
{
    k->animationArea->refreshAnimation();
    KTScene *scene = k->animationArea->currentScene(); 
    if (scene) {
        QString total = "";
        total = total.setNum(scene->framesTotal()); 
        k->status->setFramesTotal(total); 
    }
}

void KTViewCamera::exportDialog()
{
    QDesktopWidget desktop;

    KTExportWidget *exportWidget = new KTExportWidget(k->project, this);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2, 
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();
}

void KTViewCamera::postDialog()
{
    QDesktopWidget desktop;

    KTExportWidget *exportWidget = new KTExportWidget(k->project, this, false);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2,
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();

    if (exportWidget->isComplete() != QDialog::Rejected)
        emit requestForExportVideoToServer(exportWidget->videoTitle(), exportWidget->videoDescription(), exportWidget->videoScenes()); 
}

void KTViewCamera::selectScene(int index)
{
    if (index != k->animationArea->currentSceneIndex()) {
        k->animationArea->updateSceneIndex(index);
        updatePhotograms();

        KTProjectRequest event = KTRequestBuilder::createSceneRequest(index, KTProjectRequest::Select);
        emit requestTriggered(&event);
    }
}

