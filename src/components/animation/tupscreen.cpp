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
#include "tupprojectresponse.h"
#include "tupgraphicobject.h"
#include "tupgraphicsscene.h"
#include "tupanimationrenderer.h"
#include "tupsoundlayer.h"
#include "tdebug.h"

#include <QGraphicsItem>
#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QTimer>

typedef QList<QImage> photoArray;

struct TupScreen::Private
{
    QWidget *container;
    QImage renderCamera;
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
    QSize dimension;
};

TupScreen::TupScreen(const TupProject *project, const QSize viewSize, bool isScaled, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->container = parent;
    k->project = project;
    k->isScaled = isScaled;

    if (k->isScaled)
        k->dimension = viewSize;
    else
        k->dimension = k->project->dimension();

    tError() << "TupScreen::TupScreen() - width: " << k->dimension.width();
    tError() << "TupScreen::TupScreen() - height: " << k->dimension.height();

    k->cyclicAnimation = false;
    k->fps = 24;
    k->currentSceneIndex = 0;
    k->currentFramePosition = 0;

    k->timer = new QTimer(this);
    k->playBackTimer = new QTimer(this);

    connect(k->timer, SIGNAL(timeout()), this, SLOT(advance()));
    connect(k->playBackTimer, SIGNAL(timeout()), this, SLOT(back()));

    initPhotogramsArray();
    updateFirstFrame();

    updateSceneIndex(0);
}

TupScreen::~TupScreen()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    k->timer->stop();
    k->playBackTimer->stop();

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
           T_FUNCINFO;
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
           T_FUNCINFO;
    #endif

    k->renderControl.clear();
    k->animationList.clear();
    for (int i=0; i < k->project->scenesTotal(); i++) {
         k->renderControl.insert(i, false);
         QList<QImage> photograms;
         k->animationList.insert(i, photograms);
    }
}

void TupScreen::setFPS(int fps)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
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
          T_FUNCINFO;
   #endif
   */

   if (!k->firstShoot) {
       if (k->currentFramePosition > -1 && k->currentFramePosition < k->photograms.count())
           k->renderCamera = k->photograms[k->currentFramePosition];
   } else {
       k->firstShoot = false;
       tError() << "TupScreen::paintEvent() - First render!";
   }

   QPainter painter;
   painter.begin(this);

   int x = (frameSize().width() - k->renderCamera.size().width()) / 2;
   int y = (frameSize().height() - k->renderCamera.size().height()) / 2;
   painter.drawImage(QPoint(x, y), k->renderCamera);

   // painter.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
   // painter.drawRect(x, y, k->renderCamera.size().width()-1, k->renderCamera.size().height()-1);
}

void TupScreen::play()
{
   #ifdef K_DEBUG
          tWarning("camera") << "TupScreen::play() - Playing at " << k->fps << " FPS";
   #endif

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
           k->timer->start(1000 / k->fps);
   }
}

void TupScreen::playBack()
{
   #ifdef K_DEBUG
          tWarning("camera") << "TupScreen::playBack() - Starting procedure...";
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
           tWarning("camera") << "TupScreen::stop() - Stopping player!";
    #endif
   
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
           T_FUNCINFO;
    #endif

    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition += 1;
    repaint();
}

void TupScreen::previousFrame()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition -= 1;
    repaint();
}

void TupScreen::advance()
{
    /*
    #ifdef K_DEBUG
           T_FUNCINFO;
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
           T_FUNCINFO;
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
           T_FUNCINFO;
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

                 if (index == k->project->scenesTotal())
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
           T_FUNCINFO;
    #endif

    TupScene *scene = k->project->scene(k->currentSceneIndex);

    if (!scene) {
        #ifdef K_DEBUG
               tError() << "TupScreen::render() - [ Fatal Error ] - Scene is NULL! -> index: " << k->currentSceneIndex;
        #endif
        return;
    }

    k->sounds.clear();

    foreach (TupSoundLayer *layer, scene->soundLayers().values())
             k->sounds << layer;

    TupAnimationRenderer renderer(k->project->bgColor());
    renderer.setScene(scene, k->dimension);

    QFont font = this->font();
    font.setPointSize(8);

    QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog);
    progressDialog.setFont(font);
    progressDialog.setLabelText(tr("Rendering...")); 
    progressDialog.setCancelButton(0);
    progressDialog.setRange(1, renderer.totalPhotograms());

    QDesktopWidget desktop;
    progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2, 
                        (int) (desktop.screenGeometry().height() - progressDialog.height())/2);
    progressDialog.show();

    QList<QImage> photogramList;
    int i = 1;

    while (renderer.nextPhotogram()) {
           QImage renderized = QImage(k->dimension, QImage::Format_RGB32);

           QPainter painter(&renderized);
           painter.setRenderHint(QPainter::Antialiasing);
           renderer.render(&painter);

           if (k->isScaled) {
               tError() << "TupScreen::render() - Resizing photogram...";
               QImage resized = renderized.scaledToWidth(k->dimension.width(), Qt::SmoothTransformation);
               photogramList << resized;
           } else {
               tError() << "TupScreen::render() - Using NO resized photogram...";
               photogramList << renderized;
           }

           progressDialog.setValue(i);
           i++;
    }

    k->photograms = photogramList;
    k->animationList.replace(k->currentSceneIndex, photogramList);
    k->renderControl.replace(k->currentSceneIndex, true);
}

QSize TupScreen::sizeHint() const
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    return k->renderCamera.size();
}

void TupScreen::resizeEvent(QResizeEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QFrame::resizeEvent(event);

    if (k->currentSceneIndex > -1) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
        stop();
        updateFirstFrame();
        update();
    } else {
        #ifdef K_DEBUG
               tError() << "TupScreen::resizeEvent() - [ Error ] - Current index is invalid -> " << k->currentSceneIndex;
        #endif
    }
}

void TupScreen::setLoop(bool loop)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->cyclicAnimation = loop;
}

void TupScreen::updateSceneIndex(int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->currentSceneIndex = index;
    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
    } else {
        #ifdef K_DEBUG
               tError() << "TupScreen::updateSceneIndex() - [ Error ] - Can't set current photogram array -> " << k->currentSceneIndex;
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
           T_FUNCINFO;
    #endif

    if (k->currentSceneIndex > -1) {
        return k->project->scene(k->currentSceneIndex);
    } else {
        if (k->project->scenesTotal() == 1) {
            k->currentSceneIndex = 0;
            return k->project->scene(0);
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
           T_FUNCINFO;
    #endif

    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        k->currentFramePosition = 0;
        k->photograms = k->animationList.at(k->currentSceneIndex);
        updateFirstFrame();
        update();
    } else {
        #ifdef K_DEBUG
               tError() << "TupScreen::updateAnimationArea() - [ Fatal Error ] - Can't access to scene index: " << k->currentSceneIndex;
        #endif
    }
}

/**
    Prepare the first photogram of the current scene to be painted
**/

void TupScreen::updateFirstFrame()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        TupScene *scene = k->project->scene(k->currentSceneIndex);
        if (scene) { 
            TupAnimationRenderer renderer(k->project->bgColor());
            renderer.setScene(scene, k->dimension);
            renderer.renderPhotogram(0);

            QImage firstFrame = QImage(k->dimension, QImage::Format_RGB32);

            QPainter painter(&firstFrame);
            painter.setRenderHint(QPainter::Antialiasing);
            renderer.render(&painter);

            if (k->isScaled) {
                tError() << "TupScreen::updateFirstFrame() - Image was scaled to fit!";
                tError() << "TupScreen::updateFirstFrame() - firstFrame.width(): " <<  firstFrame.width();
                tError() << "TupScreen::updateFirstFrame() - k->dimension.width(): " << k->dimension.width(); 
                QImage resized = firstFrame.scaledToWidth(k->dimension.width(), Qt::SmoothTransformation);
                k->renderCamera = resized;
            } else {
                tError() << "TupScreen::updateFirstFrame() - Image was NOT scaled to fit!";
                k->renderCamera = firstFrame;
            }

            k->firstShoot = true;
        } else {
            #ifdef K_DEBUG
                   tError() << "TupScreen::updateFirstFrame() - [ Fatal Error ] - Null scene at index: " << k->currentSceneIndex;
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupScreen::updateFirstFrame() - [ Fatal Error ] - Can't access to scene index: " << k->currentSceneIndex;
        #endif
    }
}

void TupScreen::addPhotogramsArray(int sceneIndex)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (sceneIndex > -1) {
        k->renderControl.insert(sceneIndex, false);
        QList<QImage> photograms;
        k->animationList.insert(sceneIndex, photograms);
    }
}

void TupScreen::updateProjectDimension(const QSize dimension)
{
    k->dimension = dimension;
}

