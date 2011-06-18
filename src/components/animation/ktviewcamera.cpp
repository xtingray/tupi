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
#include "tdebug.h"
#include "tseparator.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

#include "ktprojectresponse.h"

KTViewCamera::KTViewCamera(KTProject *work, QWidget *parent) : QFrame(parent)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    project = work;

    setObjectName("KTViewCamera_");

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QFont font = this->font();
    font.setPointSize(10);
    QLabel *description = new QLabel(work->projectName() + ": " + work->description());
    description->setAlignment(Qt::AlignCenter);
    description->setFont(font);

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap(THEME_DIR + "icons/camera.png"));
    QLabel *title = new QLabel(tr("Render Camera Preview"));

    QWidget *titleWidget = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(icon);
    titleLayout->addWidget(title);

    layout->addWidget(titleWidget, Qt::AlignCenter);
    layout->addWidget(description, Qt::AlignCenter);

    m_animationArea = new KTAnimationArea(project);
    layout->addWidget(m_animationArea, 0, Qt::AlignCenter);

    KTCameraBar *m_bar = new KTCameraBar;
    layout->addWidget(m_bar, 0, Qt::AlignCenter);
    m_bar->show();

    connect(m_bar, SIGNAL(play()), this, SLOT(doPlay()));
    connect(m_bar, SIGNAL(playBack()), this, SLOT(doPlayBack()));
    connect(m_bar, SIGNAL(stop()), m_animationArea, SLOT(stop()));
    connect(m_bar, SIGNAL(ff()), m_animationArea, SLOT(nextFrame()));
    connect(m_bar, SIGNAL(rew()), m_animationArea, SLOT(previousFrame()));

    m_status = new KTCameraStatus(this);

    KTScene *scene = project->scene(0);
    if (scene)
        m_status->setSceneName(scene->sceneName());

    m_status->setFPS(project->fps());
    setLoop();

    layout->addWidget(m_status, 0, Qt::AlignCenter|Qt::AlignTop);

    setLayout(layout);
}

KTViewCamera::~KTViewCamera()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void KTViewCamera::showSceneInfo(const KTScene *scene)
{
    if (scene)
        m_status->setSceneName(scene->sceneName());
}

void KTViewCamera::setLoop()
{
    m_animationArea->setLoop(m_status->isLooping());
}

QSize KTViewCamera::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void KTViewCamera::doPlay()
{
    m_animationArea->play();
    updateSceneInfo();
}

void KTViewCamera::doPlayBack()
{
    m_animationArea->playBack();
    updateSceneInfo();
}

void KTViewCamera::doStop()
{
    m_animationArea->stop();
}

void KTViewCamera::nextFrame()
{
    m_animationArea->nextFrame();
}

void KTViewCamera::previousFrame()
{
    m_animationArea->previousFrame();
}

void KTViewCamera::updateSceneInfo()
{
    showSceneInfo(m_animationArea->currentScene());
}

bool KTViewCamera::handleProjectResponse(KTProjectResponse *response)
{
    if (response->part() == KTProjectRequest::Scene)
        updateSceneInfo();

    return m_animationArea->handleResponse(response);
}

void KTViewCamera::setFPS(int fps)
{
    fps++;
    project->setFPS(fps);
    //m_status->setFPS(fps);
    m_animationArea->setFPS(fps);
}

void KTViewCamera::updatePhotograms(KTProject *project)
{
    m_animationArea->refreshAnimation(project);
    KTScene *scene = project->scene(0);
    QString total = "";
    total = total.setNum(scene->framesTotal()); 
    m_status->setFramesTotal(total); 
}

void KTViewCamera::exportDialog()
{
    QDesktopWidget desktop;

    KTExportWidget *exportWidget = new KTExportWidget(project, this);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2, 
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();
}
