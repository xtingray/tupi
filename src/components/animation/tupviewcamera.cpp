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

#include "tupviewcamera.h"
#include "tupexportwidget.h"
#include "tdebug.h"
#include "tseparator.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"
#include "tuprequestbuilder.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

struct TupViewCamera::Private
{
    QFrame *container;
    TupAnimationArea *animationArea;
    TupCameraStatus *status;
    TupProject *project;
    int currentSceneIndex;
};

TupViewCamera::TupViewCamera(TupProject *project, bool isNetworked, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->project = project;

    setObjectName("TupViewCamera_");

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

    int width = size().width();
    int height = size().height();
    int pWidth = project->dimension().width();
    int pHeight = project->dimension().height();

    QString scale = "[ " + tr("Scale") + " ";

    if (pWidth < width && pHeight < height) {
        k->animationArea = new TupAnimationArea(k->project);
        scale += "1:1";
    } else {
        QSize dimension;
        double proportion = 1;
        if (pWidth > pHeight) {
            int newH = (pHeight*width)/pWidth;
            dimension = QSize(width, newH);
            proportion = (double) pWidth / (double) width; 
        } else {
            int newW = (pWidth*height)/pHeight;
            dimension = QSize(newW, height);
            proportion = (double) pHeight / (double) height;
        }

        scale += "1:" + QString::number(proportion, 'g', 2);
        k->animationArea = new TupAnimationArea(k->project, dimension, true);
    }

    scale += " ]";

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap(THEME_DIR + "icons/camera.png"));
    QLabel *title = new QLabel(tr("Render Camera Preview"));
    QLabel *scaleLabel = new QLabel(scale);
    scaleLabel->setFont(font);

    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);

    QWidget *scaleWidget = new QWidget();
    QHBoxLayout *scaleLayout = new QHBoxLayout(scaleWidget);
    scaleLayout->setContentsMargins(0, 0, 0, 0);
    scaleLayout->setAlignment(Qt::AlignCenter);
    scaleLayout->addWidget(scaleLabel);

    layout->addWidget(titleWidget, 0, Qt::AlignCenter);
    layout->addWidget(scaleWidget, 0, Qt::AlignCenter);
    layout->addLayout(labelLayout, Qt::AlignCenter);

    layout->addWidget(k->animationArea, 0, Qt::AlignCenter);

    TupCameraBar *m_bar = new TupCameraBar;
    layout->addWidget(m_bar, 0, Qt::AlignCenter);
    m_bar->show();

    connect(m_bar, SIGNAL(play()), this, SLOT(doPlay()));
    connect(m_bar, SIGNAL(playBack()), this, SLOT(doPlayBack()));
    connect(m_bar, SIGNAL(stop()), k->animationArea, SLOT(stop()));
    connect(m_bar, SIGNAL(ff()), k->animationArea, SLOT(nextFrame()));
    connect(m_bar, SIGNAL(rew()), k->animationArea, SLOT(previousFrame()));

    k->status = new TupCameraStatus(this, isNetworked);
    k->status->setScenes(k->project); 
    connect(k->status, SIGNAL(sceneIndexChanged(int)), this, SLOT(selectScene(int)));

    updateFramesTotal(0);
    k->status->setFPS(k->project->fps());
    setLoop();

    layout->addWidget(k->status, 0, Qt::AlignCenter|Qt::AlignTop);

    setLayout(layout);
}

TupViewCamera::~TupViewCamera()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupViewCamera::setLoop()
{
    k->animationArea->setLoop(k->status->isLooping());
}

QSize TupViewCamera::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void TupViewCamera::doPlay()
{
    k->animationArea->play();
}

void TupViewCamera::doPlayBack()
{
    k->animationArea->playBack();
}

void TupViewCamera::doStop()
{
    k->animationArea->stop();
}

void TupViewCamera::nextFrame()
{
    k->animationArea->nextFrame();
}

void TupViewCamera::previousFrame()
{
    k->animationArea->previousFrame();
}

bool TupViewCamera::handleProjectResponse(TupProjectResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (TupSceneResponse *sceneResponse = static_cast<TupSceneResponse *>(response)) {

        int index = sceneResponse->sceneIndex();

        switch (sceneResponse->action()) {
            case TupProjectRequest::Add:
            {
                 k->status->setScenes(k->project);
                 k->status->setCurrentScene(index);
            }
            break;
            case TupProjectRequest::Remove:
            {
                 if (index < 0)
                     break;

                 if (index == k->project->scenesTotal())
                     index--;

                 k->status->setScenes(k->project);
                 k->status->setCurrentScene(index);
            }
            break;
            case TupProjectRequest::Reset:
            {
                 k->status->setScenes(k->project);
            }
            break;
            case TupProjectRequest::Select:
            {
                 if (index >= 0) {
                     k->currentSceneIndex = index;
                     updateFramesTotal(index);
                     k->status->setCurrentScene(index);
                 }
            }
            break;
            case TupProjectRequest::Rename:
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

void TupViewCamera::setFPS(int fps)
{
    fps++;
    k->project->setFPS(fps);
    k->animationArea->setFPS(fps);
}

void TupViewCamera::updateFramesTotal(int sceneIndex)
{
    TupScene *scene = k->project->scene(sceneIndex);
    if (scene) {
        QString total = "";
        total = total.setNum(scene->framesTotal()); 
        k->status->setFramesTotal(total); 
    }
}

void TupViewCamera::exportDialog()
{
    QDesktopWidget desktop;

    TupExportWidget *exportWidget = new TupExportWidget(k->project, this);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2, 
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();
}

void TupViewCamera::postDialog()
{
    QDesktopWidget desktop;

    TupExportWidget *exportWidget = new TupExportWidget(k->project, this, false);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2,
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();

    if (exportWidget->isComplete() != QDialog::Rejected) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        if (exportWidget->workType() == TupExportWidget::Video)
            emit requestForExportVideoToServer(exportWidget->videoTitle(), exportWidget->videoTopics(), exportWidget->videoDescription(), k->status->getFPS(), exportWidget->videoScenes()); 
        else
            emit requestForExportStoryboardToServer(exportWidget->videoTitle(), exportWidget->videoTopics(), exportWidget->videoDescription(), exportWidget->videoScenes());
    }
}

void TupViewCamera::selectScene(int index)
{
    if (index != k->animationArea->currentSceneIndex()) {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit requestTriggered(&event);

        doStop();
        k->animationArea->updateSceneIndex(index);
        k->animationArea->updateAnimationArea();
        doPlay();
    }
}

void TupViewCamera::updateScenes(int sceneIndex)
{
    k->animationArea->resetPhotograms(sceneIndex);
}

void TupViewCamera::updateFirstFrame()
{
    k->animationArea->updateAnimationArea();
}


