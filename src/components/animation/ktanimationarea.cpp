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

#include "ktanimationarea.h"
#include "ktprojectresponse.h"
#include "ktgraphicobject.h"
#include "ktgraphicsscene.h"
#include "ktanimationrenderer.h"
#include "ktsoundlayer.h"
#include "tdebug.h"

#include <QGraphicsItem>
#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QTimer>

typedef QList<QImage> photoArray;

struct KTAnimationArea::Private
{
    QWidget *container;
    QImage renderCamera;
    bool firstShoot;
    const KTProject *project;
    bool cyclicAnimation;
    int currentFramePosition;
    int currentSceneIndex;
    int fps;
    QTimer *timer;
    QTimer *playBackTimer;

    QList<KTSoundLayer *> sounds;
  
    QList<QImage> photograms; 
    QList<photoArray> animationList;
    QList<bool> renderControl;
};

KTAnimationArea::KTAnimationArea(const KTProject *project, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->container = parent;
    k->project = project;
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

KTAnimationArea::~KTAnimationArea()
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

void KTAnimationArea::resetPhotograms(int sceneIndex)
{
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

void KTAnimationArea::initPhotogramsArray()
{
    k->renderControl.clear();
    k->animationList.clear();
    for (int i=0; i < k->project->scenesTotal(); i++) {
         k->renderControl.insert(i, false);
         QList<QImage> photograms;
         k->animationList.insert(i, photograms);
    }
}

void KTAnimationArea::setFPS(int fps)
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

void KTAnimationArea::paintEvent(QPaintEvent *)
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
   }

   QPainter painter;
   painter.begin(this);

   int x = (frameSize().width() - k->renderCamera.size().width()) / 2;
   int y = (frameSize().height() - k->renderCamera.size().height()) / 2;
   painter.drawImage(QPoint(x, y), k->renderCamera);

   painter.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine));
   painter.drawRect(x, y, k->renderCamera.size().width()-1, k->renderCamera.size().height()-1);
}

void KTAnimationArea::play()
{
   #ifdef K_DEBUG
          tWarning("camera") << "KTAnimationArea::play() - Playing at " << k->fps << " FPS";
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

void KTAnimationArea::playBack()
{
   #ifdef K_DEBUG
          tWarning("camera") << "KTAnimationArea::playBack() - Starting procedure...";
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

void KTAnimationArea::stop()
{
    #ifdef K_DEBUG
           tWarning("camera") << "KTAnimationArea::stop() - Stopping player!";
    #endif
   
    if (k->timer->isActive())
        k->timer->stop();

    if (k->playBackTimer->isActive())
        k->playBackTimer->stop();

    foreach (KTSoundLayer *sound, k->sounds)
             sound->stop();
	
    k->currentFramePosition = 0;
    repaint();
}

void KTAnimationArea::nextFrame()
{
    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition += 1;
    repaint();
}

void KTAnimationArea::previousFrame()
{
    if (!k->renderControl.at(k->currentSceneIndex))
        render();

    k->currentFramePosition -= 1;
    repaint();
}

void KTAnimationArea::advance()
{
    if (k->cyclicAnimation && k->currentFramePosition >= k->photograms.count())
        k->currentFramePosition = 0;

    if (k->currentFramePosition == 0) {
        foreach (KTSoundLayer *sound, k->sounds)
                 sound->play();
    }

    if (k->currentFramePosition < k->photograms.count()) {
        repaint();
        k->currentFramePosition++;
    } else if (!k->cyclicAnimation) {
               stop();
    }
}

void KTAnimationArea::back()
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

void KTAnimationArea::frameResponse(KTFrameResponse *)
{
}

void KTAnimationArea::layerResponse(KTLayerResponse *)
{
}

void KTAnimationArea::sceneResponse(KTSceneResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int index = event->sceneIndex();

    switch (event->action()) {
            case KTProjectRequest::Add:
             {
                 addPhotogramsArray(index);
             }
            break;
            case KTProjectRequest::Remove:
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
            case KTProjectRequest::Reset:
             {
                 k->renderControl.replace(index, false);
                 QList<QImage> photograms;
                 k->animationList.replace(index, photograms);
                 k->photograms = photograms;
             }
            break;
            case KTProjectRequest::Select:
             {
                 updateSceneIndex(index);
             }
            break;
            default: 
            break;
   }
}

void KTAnimationArea::projectResponse(KTProjectResponse *)
{
}

void KTAnimationArea::itemResponse(KTItemResponse *)
{
}

void KTAnimationArea::libraryResponse(KTLibraryResponse *)
{
}

void KTAnimationArea::render()
{
    KTScene *scene = k->project->scene(k->currentSceneIndex);

    if (!scene) {
        #ifdef K_DEBUG
               tError() << "KTAnimationArea::render() - [ Fatal Error ] - Scene is NULL! -> index: " << k->currentSceneIndex;
        #endif
        return;
    }

    k->sounds.clear();

    foreach (KTSoundLayer *layer, scene->soundLayers().values())
             k->sounds << layer;

    KTAnimationRenderer renderer(k->project->bgColor());
    renderer.setScene(scene, k->project->dimension());

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
           QImage renderized = QImage(k->project->dimension(), QImage::Format_RGB32);
           QPainter painter(&renderized);
           painter.setRenderHint(QPainter::Antialiasing);
           renderer.render(&painter);
           photogramList << renderized;
           progressDialog.setValue(i);
           i++;
    }

    k->photograms = photogramList;
    k->animationList.replace(k->currentSceneIndex, photogramList);
    k->renderControl.replace(k->currentSceneIndex, true);
}

QSize KTAnimationArea::sizeHint() const
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    return k->renderCamera.size();
}

void KTAnimationArea::resizeEvent(QResizeEvent *event)
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
               tError() << "KTAnimationArea::resizeEvent() - [ Error ] - Current index is invalid -> " << k->currentSceneIndex;
        #endif
    }
}

void KTAnimationArea::setLoop(bool loop)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->cyclicAnimation = loop;
}

void KTAnimationArea::updateSceneIndex(int index)
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
               tError() << "KTAnimationArea::updateSceneIndex() - [ Error ] - Can't set current photogram array -> " << k->currentSceneIndex;
        #endif
    }
}

int KTAnimationArea::currentSceneIndex()
{
    return k->currentSceneIndex;
}

KTScene *KTAnimationArea::currentScene() const
{
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

void KTAnimationArea::updateAnimationArea()
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
               tError() << "KTAnimationArea::updateAnimationArea() - [ Fatal Error ] - Can't access to scene index: " << k->currentSceneIndex;
        #endif
    }
}

/**
    Prepare the first photogram of the current scene to be painted
**/

void KTAnimationArea::updateFirstFrame()
{
    if (k->currentSceneIndex > -1 && k->currentSceneIndex < k->animationList.count()) {
        KTScene *scene = k->project->scene(k->currentSceneIndex);
        if (scene) { 
            KTAnimationRenderer renderer(k->project->bgColor());
            renderer.setScene(scene, k->project->dimension());
            renderer.renderPhotogram(0);

            QImage firstFrame = QImage(k->project->dimension(), QImage::Format_RGB32);

            QPainter painter(&firstFrame);
            painter.setRenderHint(QPainter::Antialiasing);
            renderer.render(&painter);

            k->renderCamera = firstFrame;
            k->firstShoot = true;
        } else {
            #ifdef K_DEBUG
                   tError() << "KTAnimationArea::updateFirstFrame() - [ Fatal Error ] - Null scene at index: " << k->currentSceneIndex;
            #endif
        }
    } else {
        #ifdef K_DEBUG
               tError() << "KTAnimationArea::updateFirstFrame() - [ Fatal Error ] - Can't access to scene index: " << k->currentSceneIndex;
        #endif
    }
}

void KTAnimationArea::addPhotogramsArray(int sceneIndex)
{
    if (sceneIndex > -1) {
        k->renderControl.insert(sceneIndex, false);
        QList<QImage> photograms;
        k->animationList.insert(sceneIndex, photograms);
    }
}
