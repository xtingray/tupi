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

#include "tupcamerawidget.h"

struct TupCameraWidget::Private
{
    QFrame *container;
    TupScreen *screen;
    TupCameraBar *cameraBar;
    QProgressBar *progressBar;
    TupCameraStatus *status;
    TupProject *project;
    int currentSceneIndex;
    QLabel *scaleLabel;
    QSize playerDimension;
    QSize screenDimension;
    bool isScaled;
};

TupCameraWidget::TupCameraWidget(TupProject *project, bool isNetworked, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCameraWidget()]";
        #else
            TINIT;
        #endif
    #endif

    QDesktopWidget desktop;
    int desktopWidth = (40 * desktop.screenGeometry().width()) / 100;
    int desktopHeight = (40 * desktop.screenGeometry().height()) / 100;
    k->screenDimension = QSize(desktopWidth, desktopHeight);

    k->project = project;
    setObjectName("TupCameraWidget_");

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->setAlignment(Qt::AlignCenter);
    labelLayout->setSpacing(0);

    QFont font = this->font();
    font.setBold(true);
    QLabel *name = new QLabel(k->project->projectName() + ": ");
    name->setFont(font);

    font.setBold(false);
    QLabel *description = new QLabel(k->project->description());
    description->setFont(font);

    labelLayout->addWidget(name); 
    labelLayout->addWidget(description);

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap(THEME_DIR + "icons/player.png"));
    QLabel *title = new QLabel(tr("Scene Preview"));
    font.setBold(true);
    title->setFont(font);
    font.setBold(false);
    k->scaleLabel = new QLabel;
    k->scaleLabel->setFont(font);

    setDimensionLabel(k->project->dimension());

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
    scaleLayout->addWidget(k->scaleLabel);

    layout->addWidget(titleWidget, 0, Qt::AlignCenter);
    layout->addWidget(scaleWidget, 0, Qt::AlignCenter);
    layout->addLayout(labelLayout, Qt::AlignCenter);

    k->progressBar = new QProgressBar(this); 
    QString style = "QProgressBar { background-color: #DDDDDD; text-align: center; color: #FFFFFF; border-radius: 2px; } QProgressBar::chunk { background-color: #009500; border-radius: 2px; }";
    k->progressBar->setStyleSheet(style);
    k->progressBar->setMaximumHeight(5);
    k->progressBar->setTextVisible(false);
    k->progressBar->setRange(1, 100);
    layout->addWidget(k->progressBar, 0, Qt::AlignCenter);

    k->screen = new TupScreen(k->project, k->playerDimension, k->isScaled);
    connect(k->screen, SIGNAL(isRendering(int)), this, SLOT(updateProgressBar(int)));

    layout->addWidget(k->screen, 0, Qt::AlignCenter);

    k->cameraBar = new TupCameraBar;
    layout->addWidget(k->cameraBar, 0, Qt::AlignCenter);

    connect(k->cameraBar, SIGNAL(play()), this, SLOT(doPlay()));
    connect(k->cameraBar, SIGNAL(playBack()), this, SLOT(doPlayBack()));
    connect(k->cameraBar, SIGNAL(stop()), k->screen, SLOT(stop()));
    connect(k->cameraBar, SIGNAL(ff()), k->screen, SLOT(nextFrame()));
    connect(k->cameraBar, SIGNAL(rew()), k->screen, SLOT(previousFrame()));

    k->status = new TupCameraStatus(this, isNetworked);
    k->status->setScenes(k->project); 
    connect(k->status, SIGNAL(sceneIndexChanged(int)), this, SLOT(selectScene(int)));

    updateFramesTotal(0);
    k->status->setFPS(k->project->fps());
    setLoop();

    layout->addWidget(k->status, 0, Qt::AlignCenter|Qt::AlignTop);
    setLayout(layout);
}

TupCameraWidget::~TupCameraWidget()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupCameraWidget()]";
        #else
            TEND;
        #endif
    #endif
}

void TupCameraWidget::setDimensionLabel(const QSize dimension)
{
    int screenWidth = k->screenDimension.width();
    int screenHeight = k->screenDimension.height();

    int projectWidth = dimension.width();
    int projectHeight = dimension.height();

    QString scale = "[ " + tr("Scale") + " ";
    k->isScaled = false;

    if (projectWidth <= screenWidth && projectHeight <= screenHeight) {
        k->playerDimension = k->project->dimension();
        scale += "1:1";
    } else {
        double proportion = 1;

        if (projectWidth > projectHeight) {
            int newH = (projectHeight*screenWidth)/projectWidth;
            k->playerDimension = QSize(screenWidth, newH);
            proportion = (double) projectWidth / (double) screenWidth;
        } else { // projectHeight > projectWidth
            int newW = (projectWidth*screenHeight)/projectHeight;
            k->playerDimension = QSize(newW, screenHeight);
            proportion = (double) projectHeight / (double) screenHeight;
        }

        scale += "1:" + QString::number(proportion, 'g', 2);
        k->isScaled = true;
    }

    scale += " | " + tr("Size") + ": ";
    scale += QString::number(projectWidth) + "x" + QString::number(projectHeight);
    scale += " px ]";

    k->scaleLabel->setText(scale);
}

void TupCameraWidget::setLoop()
{
    k->screen->setLoop(k->status->isLooping());
}

QSize TupCameraWidget::sizeHint() const
{
    QSize size = QWidget::sizeHint();
    return size.expandedTo(QApplication::globalStrut());
}

void TupCameraWidget::doPlay()
{
    k->screen->play();
}

void TupCameraWidget::doPlayBack()
{
    k->screen->playBack();
}

void TupCameraWidget::doStop()
{
    k->screen->stop();
}

void TupCameraWidget::nextFrame()
{
    k->screen->nextFrame();
}

void TupCameraWidget::previousFrame()
{
    k->screen->previousFrame();
}

bool TupCameraWidget::handleProjectResponse(TupProjectResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupCameraWidget::handleProjectResponse()]";
        #else
            T_FUNCINFO;
        #endif
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

                 if (index == k->project->scenesCount())
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
            default:
            {
                 #ifdef K_DEBUG
                     QString msg = "TupCameraWidget::handleProjectResponse() - Unknown/Unhandled project action: " + QString::number(sceneResponse->action());
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
            }
            break;
        }
    }

    return k->screen->handleResponse(response);
}

void TupCameraWidget::setFPS(int fps)
{
    fps++;
    k->project->setFPS(fps);
    k->screen->setFPS(fps);
}

void TupCameraWidget::setStatusFPS(int fps)
{
    k->status->blockSignals(true);
    k->status->setFPS(fps);
    k->status->blockSignals(false);

    k->project->setFPS(fps);
    k->screen->setFPS(fps);
}

void TupCameraWidget::updateFramesTotal(int sceneIndex)
{
    TupScene *scene = k->project->sceneAt(sceneIndex);
    if (scene) {
        int total = scene->framesCount();
        k->status->setFramesTotal(QString::number(total)); 
        k->progressBar->setRange(0, total);
    }
}

void TupCameraWidget::exportDialog()
{
    QDesktopWidget desktop;

    TupExportWidget *exportWidget = new TupExportWidget(k->project, this);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2, 
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();
}

void TupCameraWidget::postDialog()
{
    QDesktopWidget desktop;

    TupExportWidget *exportWidget = new TupExportWidget(k->project, this, false);
    exportWidget->show();
    exportWidget->move((int) (desktop.screenGeometry().width() - exportWidget->width())/2,
                       (int) (desktop.screenGeometry().height() - exportWidget->height())/2);
    exportWidget->exec();

    if (exportWidget->isComplete() != QDialog::Rejected) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        emit requestForExportVideoToServer(exportWidget->videoTitle(), exportWidget->videoTopics(), exportWidget->videoDescription(), k->status->getFPS(), exportWidget->videoScenes());
    }
}

void TupCameraWidget::selectScene(int index)
{
    if (index != k->screen->currentSceneIndex()) {
        TupProjectRequest event = TupRequestBuilder::createSceneRequest(index, TupProjectRequest::Select);
        emit requestTriggered(&event);

        doStop();
        k->screen->updateSceneIndex(index);
        k->screen->updateAnimationArea();
        doPlay();
    }
}

void TupCameraWidget::updateScenes(int sceneIndex)
{
    k->screen->resetPhotograms(sceneIndex);
}

void TupCameraWidget::updateFirstFrame()
{
    k->screen->updateAnimationArea();
}

void TupCameraWidget::updateProgressBar(int advance)
{
    k->progressBar->setValue(advance);
}
