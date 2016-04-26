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

#include "tupscreen.h"

typedef QList<QImage> photoArray;

struct TupScreen::Private
{
    QWidget *container;
    QImage currentPhotogram;
    QPoint imagePos;
    bool firstShoot;
    bool isScaled;
    const TupProject *project;
    bool cyclicAnimation;
    int currentFramePosition;
    int currentSceneIndex;
    int fps;
    QTimer *timer;
    QTimer *playBackTimer;

    QList<TupSoundLayer *> sounds;
    QList<QImage> photograms; 
    QList<photoArray> animationList;
    QList<bool> renderControl;
    QSize screenDimension;

    TupLibrary *library;
    QList<QPair<int, QString> > lipSyncRecords;
    QMediaPlayer *soundPlayer;

    bool isPlaying;
};

TupScreen::TupScreen(TupProject *project, const QSize viewSize, bool isScaled, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen()]";
        #else
            TINIT;
        #endif
    #endif

    k->container = parent;
    k->project = project;
    k->library = project->library();
    k->isScaled = isScaled;
    k->screenDimension = viewSize;
    k->cyclicAnimation = false;
    k->fps = 24;
    k->currentSceneIndex = 0;
    k->currentFramePosition = 0;
    k->soundPlayer = new QMediaPlayer;
    k->isPlaying = false;

    k->timer = new QTimer(this);
    k->playBackTimer = new QTimer(this);

    connect(k->timer, SIGNAL(timeout()), this, SLOT(advance()));
    connect(k->playBackTimer, SIGNAL(timeout()), this, SLOT(back()));

    initPhotogramsArray();

    updateSceneIndex(0);
    updateFirstFrame();
}

TupScreen::~TupScreen()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupScreen()]";
        #else
            TEND;
        #endif
    #endif

    k->timer->stop();
    k->playBackTimer->stop();

    k->photograms.clear();
    k->animationList.clear();
    k->sounds.clear();
    k->renderControl.clear();

    delete k->soundPlayer;
    delete k->timer;
    delete k->playBackTimer;
    delete k;
}

/**
    Clean a photogram array if the scene has changed
**/

void TupScreen::resetPhotograms(int sceneIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::resetPhotograms()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (sceneIndex > -1) {
        if (k->renderControl.at(sceneIndex)) {
            k->renderControl.replace(sceneIndex, false);
            QList<QImage> photograms;
            k->animationList.replace(sceneIndex, photograms);
        }
    } else {
        initPhotogramsArray();
    }
}

void TupScreen::initPhotogramsArray()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::initPhotogramsArray()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->renderControl.clear();
    k->animationList.clear();
    for (int i=0; i < k->project->scenesCount(); i++) {
         k->renderControl.insert(i, false);
         QList<QImage> photograms;
         k->animationList.insert(i, photograms);
    }
}

void TupScreen::setFPS(int fps)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::setFPS()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->fps = fps;

    if (k->timer->isActive()) {
        k->timer->stop();
        play();
    }

    if (k->playBackTimer->isActive()) {
        k->playBackTimer->stop();
        playBack();
    }
}

void TupScreen::paintEvent(QPaintEvent *)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::paintEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->isPlaying)
        playLipSyncAt(k->currentFramePosition);

    if (!k->firstShoot) {
        if (k->currentFramePosition > -1 && k->currentFramePosition < k->photograms.count())
            k->currentPhotogram = k->photograms[k->currentFramePosition];
    } else {
        k->firstShoot = false;
    }

    QPainter painter;
    painter.begin(this);
    painter.drawImage(k->imagePos, k->currentPhotogram);

    // SQA: Border for the player. Useful for some tests
    // painter.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
    // painter.drawRect(x, y, k->currentPhotogram.size().width()-1, k->renderCamera.size().height()-1);
}

void TupScreen::play()
{
    #ifdef K_DEBUG
        QString msg = "TupScreen::play() - Playing at " + QString::number(k->fps) + " FPS";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("camera") << msg;
        #endif
    #endif

    if (k->photograms.count() == 1)
        return;

    k->isPlaying = true;

    if (k->playBackTimer->isActive())
        stop();

    k->currentFramePosition = 0;

    if (!k->timer->isActive()) {
        if (!k->renderControl.at(k->currentSceneIndex)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            render();
            QApplication::restoreOverrideCursor();
        }

        if (k->renderControl.at(k->currentSceneIndex))
            k->timer->start(1000/k->fps);
    }
}

void TupScreen::playBack()
{
    #ifdef K_DEBUG
        QString msg = "TupScreen::playBack() - Starting procedure...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("camera") << msg;
        #endif
    #endif

    if (k->timer->isActive())
        stop();

    k->currentFramePosition = k->photograms.count() - 1;

    if (!k->playBackTimer->isActive()) {
        if (!k->renderControl.at(k->currentSceneIndex))
            render();
        k->playBackTimer->start(1000 / k->fps);
    }
}

void TupScreen::stop()
{
    #ifdef K_DEBUG
        QString msg = "TupScreen::stop() - Stopping player!";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("camera") << msg;
        #endif
    #endif

    k->isPlaying = false;
    k->soundPlayer->stop();
   
    if (k->timer->isActive())
        k->timer->stop();

    if (k->playBackTimer->isActive())
        k->playBackTimer->stop();

    foreach (TupSoundLayer *sound, k->sounds)
             sound->stop();
	
    k->currentFramePosition = 0;
    repaint();
}

void TupScreen::nextFrame()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::nextFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition += 1;

    if (k->currentFramePosition == k->photograms.count())
        k->currentFramePosition = 0;

    repaint();
}

void TupScreen::previousFrame()
{
    /* 
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::previousFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition -= 1;

    if (k->currentFramePosition < 0)
        k->currentFramePosition = k->photograms.count() - 1;

    repaint();
}

void TupScreen::advance()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::advance()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (k->cyclicAnimation && k->currentFramePosition >= k->photograms.count())
        k->currentFramePosition = 0;

    if (k->currentFramePosition == 0) {
        foreach (TupSoundLayer *sound, k->sounds)
                 sound->play();
    }

    if (k->currentFramePosition < k->photograms.count()) {
        repaint();
        k->currentFramePosition++;
    } else if (!k->cyclicAnimation) {
               stop();
    }
}

void TupScreen::back()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::back()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->cyclicAnimation && k->currentFramePosition < 0)
        k->currentFramePosition = k->photograms.count() - 1;

    if (k->currentFramePosition >= 0) {
        repaint();
        k->currentFramePosition--;
    } else if (!k->cyclicAnimation) {
               stop();
    }
}

void TupScreen::frameResponse(TupFrameResponse *)
{
}

void TupScreen::layerResponse(TupLayerResponse *)
{
}

void TupScreen::sceneResponse(TupSceneResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::sceneResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int index = event->sceneIndex();

    switch (event->action()) {
            case TupProjectRequest::Add:
             {
                 addPhotogramsArray(index);
             }
            break;
            case TupProjectRequest::Remove:
             {
                 if (index < 0)
                     break;

                 k->renderControl.removeAt(index);
                 k->animationList.removeAt(index);

                 if (index == k->project->scenesCount())
                     index--;

                 updateSceneIndex(index);
             }
            break;
            case TupProjectRequest::Reset:
             {
                 k->renderControl.replace(index, false);
                 QList<QImage> photograms;
                 k->animationList.replace(index, photograms);
                 k->photograms = photograms;
             }
            break;
            case TupProjectRequest::Select:
             {
                 updateSceneIndex(index);
             }
            break;
            default: 
            break;
   }
}

void TupScreen::projectResponse(TupProjectResponse *)
{
}

void TupScreen::itemResponse(TupItemResponse *)
{
}

void TupScreen::libraryResponse(TupLibraryResponse *)
{
}

void TupScreen::render()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::render()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    emit isRendering(0);

    TupScene *scene = k->project->sceneAt(k->currentSceneIndex);
    if (!scene) {
        #ifdef K_DEBUG
            QString msg = "TupScreen::render() - [ Fatal Error ] - Scene is NULL! -> index: " + QString::number(k->currentSceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    k->sounds.clear();

    int soundLayersTotal = scene->soundLayers().size();
    for (int i = 0; i < soundLayersTotal; i++) {
         TupSoundLayer *layer = scene->soundLayers().at(i);
         k->sounds << layer;
    }

    TupAnimationRenderer renderer(k->project->bgColor(), k->library);
    renderer.setScene(scene, k->project->dimension());

    QFont font = this->font();
    font.setPointSize(8);

    QList<QImage> photogramList;
    int i = 1;

    while (renderer.nextPhotogram()) {
           QImage renderized = QImage(k->project->dimension(), QImage::Format_RGB32);

           QPainter painter(&renderized);
           painter.setRenderHint(QPainter::Antialiasing);
           renderer.render(&painter);

           if (k->isScaled) {
               QImage resized = renderized.scaledToWidth(k->screenDimension.width(), Qt::SmoothTransformation);
               photogramList << resized;
           } else {
               photogramList << renderized;
           }

           emit isRendering(i); 
           i++;
    }

    k->photograms = photogramList;
    k->animationList.replace(k->currentSceneIndex, photogramList);
    k->renderControl.replace(k->currentSceneIndex, true);

    emit isRendering(0); 
}

QSize TupScreen::sizeHint() const
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::sizeHint()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    return k->currentPhotogram.size();
}

void TupScreen::resizeEvent(QResizeEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::resizeEvent()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QFrame::resizeEvent(event);

    if (k->currentSceneIndex > -1) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
        /*
        if (k->timer->isActive() || k->playBackTimer->isActive()) {
            stop();
        }
        */
        // updateFirstFrame();
        // update();
    } else {
        #ifdef K_DEBUG
            QString msg = "TupScreen::resizeEvent() - [ Error ] - Current index is invalid -> " + QString::number(k->currentSceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupScreen::setLoop(bool loop)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::setLoop()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->cyclicAnimation = loop;
}

void TupScreen::updateSceneIndex(int index)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::updateSceneIndex()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->currentSceneIndex = index;
    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupScreen::updateSceneIndex() - [ Error ] - Can't set current photogram array -> " + QString::number(k->currentSceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

int TupScreen::currentSceneIndex()
{
    return k->currentSceneIndex;
}

TupScene *TupScreen::currentScene() const
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::currentScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->currentSceneIndex > -1) {
        return k->project->sceneAt(k->currentSceneIndex);
    } else {
        if (k->project->scenesCount() == 1) {
            k->currentSceneIndex = 0;
            return k->project->sceneAt(0);
        } 
    }

    return 0;
}

/**
    Update and paint the first image of the current scene
**/

void TupScreen::updateAnimationArea()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::updateAnimationArea()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
        updateFirstFrame();
        update();
    } else {
        #ifdef K_DEBUG
            QString msg = "TupScreen::updateAnimationArea() - [ Fatal Error ] - Can't access to scene index: " + QString::number(k->currentSceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

/**
    Prepare the first photogram of the current scene to be painted
**/

void TupScreen::updateFirstFrame()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::updateFirstFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        TupScene *scene = k->project->sceneAt(k->currentSceneIndex);
        if (scene) { 
            setLipSyncSettings();

            TupAnimationRenderer renderer(k->project->bgColor(), k->library);
            renderer.setScene(scene, k->project->dimension());
            renderer.renderPhotogram(0);

            QImage firstFrame = QImage(k->project->dimension(), QImage::Format_RGB32);

            QPainter painter(&firstFrame);
            painter.setRenderHint(QPainter::Antialiasing);
            renderer.render(&painter);

            if (k->isScaled) {
                QImage resized = firstFrame.scaledToWidth(k->screenDimension.width(), Qt::SmoothTransformation);
                k->currentPhotogram = resized;
            } else {
                k->currentPhotogram = firstFrame;
            }

            int x = (frameSize().width() - k->currentPhotogram.size().width()) / 2;
            int y = (frameSize().height() - k->currentPhotogram.size().height()) / 2;
            k->imagePos = QPoint(x, y);

            k->firstShoot = true;
        } else {
            #ifdef K_DEBUG
                QString msg = "TupScreen::updateFirstFrame() - [ Fatal Error ] - Null scene at index: " + QString::number(k->currentSceneIndex);
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupScreen::updateFirstFrame() - [ Fatal Error ] - Can't access to scene index: " + QString::number(k->currentSceneIndex);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupScreen::addPhotogramsArray(int sceneIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupScreen::addPhotogramsArray()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (sceneIndex > -1) {
        k->renderControl.insert(sceneIndex, false);
        QList<QImage> photograms;
        k->animationList.insert(sceneIndex, photograms);
    }
}

void TupScreen::setLipSyncSettings()
{
    TupScene *scene = k->project->sceneAt(k->currentSceneIndex);
    if (scene) {
        if (scene->lipSyncTotal() > 0) {
            k->lipSyncRecords.clear();
            Mouths mouths = scene->getLipSyncList();
            foreach(TupLipSync *lipsync, mouths) {
                    TupLibraryFolder *folder = k->library->getFolder(lipsync->name());
                    if (folder) {
                        TupLibraryObject *sound = folder->getObject(lipsync->soundFile());
                        if (sound) {
                            QPair<int, QString> soundRecord;
                            soundRecord.first = lipsync->initFrame();
                            soundRecord.second = sound->dataPath();
                            k->lipSyncRecords << soundRecord;
                        }
                    }
            }
        }
    }
}

void TupScreen::playLipSyncAt(int frame)
{
    int size = k->lipSyncRecords.count();
    for(int i=0; i<size; i++) {
             QPair<int, QString> soundRecord = k->lipSyncRecords.at(i);
             if (frame == soundRecord.first) {
                 QString path = soundRecord.second;
                 k->soundPlayer->setMedia(QUrl::fromLocalFile(soundRecord.second));
                 k->soundPlayer->play();
             }
    }
}

