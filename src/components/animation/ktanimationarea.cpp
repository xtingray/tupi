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

#include <QGraphicsItem>
#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>

#include "ktprojectresponse.h"
#include "ktgraphicobject.h"
#include "ktgraphicsscene.h"
#include "ktanimationrenderer.h"
#include "ktsoundlayer.h"

#include "kdebug.h"

struct KTAnimationArea::Private
{
    QWidget *container;
    QImage renderCamera;

    const KTProject *project;

    bool draw;
    bool cyclicAnimation;

    int currentFramePosition;
    int currentSceneIndex;
    int fps;

    QTimer *timer;
    QTimer *playBackTimer;

    QList<QImage> photograms;
    QList<KTSoundLayer *> sounds;

    bool isRendered;
};

KTAnimationArea::KTAnimationArea(const KTProject *project, QWidget *parent) : QFrame(parent), k(new Private)
{
    k->container = parent;

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    k->project = project;

    k->draw = false;
    k->cyclicAnimation = false;
    k->isRendered = false;

    k->currentSceneIndex = -1;
    k->currentFramePosition = -1;

    k->fps = 24;

    setAttribute(Qt::WA_StaticContents);

    k->renderCamera = QImage(size(), QImage::Format_RGB32);
    // k->renderCamera.fill(qRgb(255, 255, 255));
    // k->renderCamera.fill(k->project->bgColor().rgb());

    k->timer = new QTimer(this);
    k->playBackTimer = new QTimer(this);

    connect(k->timer, SIGNAL(timeout()), this, SLOT(advance()));
    connect(k->playBackTimer, SIGNAL(timeout()), this, SLOT(back()));

    setCurrentScene(0);

    /*
    KTScene *scene = project->scene(0);
    KTLayer *layer = scene->layer(0);
    if (layer->framesNumber() > 1)
        nextFrame();
    */
}


KTAnimationArea::~KTAnimationArea()
{
    delete k;
}

void KTAnimationArea::setFPS(int fps)
{
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
   if (k->currentFramePosition >= 0 && k->currentFramePosition < k->photograms.count())
       k->renderCamera = k->photograms[k->currentFramePosition];

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
          kDebug("camera") << "KTAnimationArea::play() - Playing at " << k->fps << " FPS";
   #endif

   if (k->playBackTimer->isActive()) 
       stop();

   k->draw = true;
   k->currentFramePosition = 0;

   if (k->project && !k->timer->isActive()) {
       if (!k->isRendered) {
           QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
           render();
           QApplication::restoreOverrideCursor();
       }

       if (k->isRendered)
           k->timer->start(1000 / k->fps);
   }
}

void KTAnimationArea::playBack()
{
   #ifdef K_DEBUG
          kDebug("camera") << "Playing back!";
   #endif

   if (k->timer->isActive())
       stop();

   k->draw = true;
   k->currentFramePosition = k->photograms.count() - 1;

   if (k->project && !k->playBackTimer->isActive()) {
       if (!k->isRendered)
           render();
       k->playBackTimer->start(1000 / k->fps);
   }
}

void KTAnimationArea::stop()
{
    #ifdef K_DEBUG
           kDebug("camera") << "Stopping";
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
    if (! k->isRendered)
        render();

    if (k->currentFramePosition >= k->photograms.count()) 
        return;

    k->currentFramePosition += 1;
    repaint();
}

void KTAnimationArea::previousFrame()
{
    if (! k->isRendered) 
        render();

    if (k->currentFramePosition < 1) 
        return;

    k->currentFramePosition -= 1;
    repaint();
}

void KTAnimationArea::advance()
{
    if (k->project) {
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
}

void KTAnimationArea::back()
{
    if (k->project) {
        if (k->cyclicAnimation && k->currentFramePosition < 0)
            k->currentFramePosition = k->photograms.count() - 1;

        if (k->currentFramePosition >= 0) {
            repaint();
            k->currentFramePosition--;
        } else if (!k->cyclicAnimation) {
                   stop();
        }
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
           K_FUNCINFOX("animation");
    #endif

    switch (event->action()) {
            case KTProjectRequest::Select:
             {
                 setCurrentScene(event->sceneIndex());
             }
            break;
            case KTProjectRequest::Remove:
             {
                 if (event->sceneIndex() == k->currentSceneIndex) {
                     if (k->currentSceneIndex != 0)
                         setCurrentScene(k->currentSceneIndex-1);
                 }
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

    if (!scene) 
        return;

    k->sounds.clear();

    foreach (KTSoundLayer *layer, scene->soundLayers().values())
             k->sounds << layer;

    k->photograms.clear();

    KTAnimationRenderer renderer(k->project->bgColor());
    renderer.setScene(scene);

    QFont font = this->font();
    font.setPointSize(8);

    QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog);
    progressDialog.setFont(font);
    progressDialog.setLabelText(tr("Rendering...")); 
    progressDialog.setCancelButton(0);
    progressDialog.setRange(1, renderer.totalPhotograms());
    progressDialog.show();
    int i = 1;

    QDesktopWidget desktop;
    progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2, 
                        (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

    while (renderer.nextPhotogram()) {
           QImage renderized = QImage(size(), QImage::Format_RGB32);
           //renderized.fill(qRgb(255, 255, 255));
           //renderized.fill(k->project->bgColor().rgb());

           QPainter painter(&renderized);
           painter.setRenderHint(QPainter::Antialiasing);
           renderer.render(&painter);

           k->photograms << renderized;
           progressDialog.setValue(i);
           i++;
    }

    k->isRendered = true;
}

void KTAnimationArea::initAnimationArea()
{
    KTScene *scene = k->project->scene(k->currentSceneIndex);
    k->currentFramePosition = 0;

    KTAnimationRenderer renderer(k->project->bgColor());
    renderer.setScene(scene);
    renderer.renderPhotogram(0);

    QImage renderized = QImage(size(), QImage::Format_RGB32);
    // renderized.fill(qRgb(255, 255, 255));
    // renderized.fill(k->project->bgColor().rgb());

    QPainter painter(&renderized);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter);

    k->photograms.insert(0, renderized);

    repaint();

    k->isRendered = false;
}

QSize KTAnimationArea::sizeHint() const
{
    return k->renderCamera.size();
}

void  KTAnimationArea::resizeEvent(QResizeEvent * e)
{
    QFrame::resizeEvent(e);

    stop();

    k->renderCamera = QImage(size(), QImage::Format_RGB32);
    // k->renderCamera.fill(qRgb(255, 255, 255));
    // k->renderCamera.fill(k->project->bgColor().rgb());
    k->isRendered = false;

    update();
}

void KTAnimationArea::setLoop(bool loop)
{
    k->cyclicAnimation = loop;
}

void KTAnimationArea::setCurrentScene(int index)
{
    k->currentSceneIndex = index;
}

KTScene *KTAnimationArea::currentScene() const
{
    return k->project->scene(k->currentSceneIndex);
}

void KTAnimationArea::refreshAnimation(const KTProject *project) 
{
    k->project = project;
    //render();
    //repaint();
    initAnimationArea();
}
