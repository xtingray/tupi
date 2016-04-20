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

#include "tupcamerawindow.h"

#include <QCameraExposure>
#include <QCameraFocus>

struct TupCameraWindow::Private
{
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    TupVideoSurface *videoSurface;
    QString dir;
    int counter;
};

TupCameraWindow::TupCameraWindow(QCamera *input, const QSize &camSize, const QSize &displaySize, QCameraImageCapture *imageCapture, 
                                 const QString &path, QWidget *parent) : QWidget(parent), k(new Private)
{
    setFixedSize(displaySize + QSize(1, 1));

    k->dir = path;

    k->camera = input;
    k->imageCapture = imageCapture;
    k->camera->setCaptureMode(QCamera::CaptureStillImage);
    QCameraExposure *exposure = k->camera->exposure();
    exposure->setExposureMode(QCameraExposure::ExposureManual);
    QCameraFocus *focus = k->camera->focus();
    focus->setFocusMode(QCameraFocus::ManualFocus);
    focus->setFocusPointMode(QCameraFocus::FocusPointCenter);

    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(error(QCamera::Error)));
    connect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

    QMediaService *service = k->camera->service();

    // QVideoEncoderControl *encoderControl = service->requestControl<QVideoEncoderControl*>();
    // QVideoEncoderSettings settings = encoderControl->videoSettings();
    // settings.setResolution(camSize);
    // encoderControl->setVideoSettings(settings);

    QVideoRendererControl *rendererControl = service->requestControl<QVideoRendererControl*>();

    bool isScaled = false;

    if (camSize != displaySize)
        isScaled = true;

    k->videoSurface = new TupVideoSurface(this, this, displaySize, isScaled, this);
    rendererControl->setSurface(k->videoSurface);
}

TupCameraWindow::~TupCameraWindow()
{
}

void TupCameraWindow::startCamera()
{
    stopCamera();
    k->camera->start();
}

void TupCameraWindow::stopCamera()
{
    if (k->camera->state() == QCamera::ActiveState)
        k->camera->stop();
}

void TupCameraWindow::reset()
{
    QDir dir(k->dir);
    foreach (QString file, dir.entryList(QStringList() << "*.jpg")) {
             QString absolute = dir.absolutePath() + "/" + file;
             QFile::remove(absolute);
    }

    if (! dir.rmdir(dir.absolutePath())) {
        #ifdef K_DEBUG
            QString msg = "TupCameraInterface::closeEvent() - Fatal Error: Can't remove pictures directory -> " + dir.absolutePath();
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif 
    }

    if (k->videoSurface)
        k->videoSurface->stop();

    if (k->camera->state() == QCamera::ActiveState)
        k->camera->stop();
}

void TupCameraWindow::error(QCamera::Error error)
{
    switch (error) {
            case QCamera::NoError:
            {
                break;
            }
            case QCamera::CameraError:
            {
                QMessageBox::warning(this, "TupCameraWindow", tr("General Camera error"));
                break;
            }
            case QCamera::InvalidRequestError:
            {
                QMessageBox::warning(this, "TupCameraWindow", tr("Camera invalid request error"));
                break;
            }
            case QCamera::ServiceMissingError:
            {
                QMessageBox::warning(this, "TupCameraWindow", tr("Camera service missing error"));
                break;
            }
            case QCamera::NotSupportedFeatureError :
            {
                QMessageBox::warning(this, "TupCameraWindow", tr("Camera not supported error"));
                break;
            }
    };
}

void TupCameraWindow::updateVideo()
{
    repaint();
}

void TupCameraWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (k->videoSurface && k->videoSurface->isActive())
        k->videoSurface->paint(&painter);
}

void TupCameraWindow::takePicture(int counter)
{
    QString prev = "pic";
    if (counter < 10)
        prev += "00";
    if (counter >= 10 && counter < 100)
        prev += "0";

    QString imagePath = k->dir + "/" + prev + QString::number(counter) + ".jpg";

    //on half pressed shutter button
    k->camera->searchAndLock();

    //on shutter button pressed
    k->imageCapture->capture(imagePath);

    //on shutter button released
    k->camera->unlock();

    k->counter = counter;
}

void TupCameraWindow::imageSavedFromCamera(int id, const QString path)
{
    Q_UNUSED(id);

    if (path.isEmpty())
        return;

    emit pictureHasBeenSelected(k->counter, path);
    k->videoSurface->setLastImage(QImage(path));
}

void TupCameraWindow::drawGrid(bool flag)
{
    k->videoSurface->drawGrid(flag);
}

void TupCameraWindow::drawActionSafeArea(bool flag)
{
    k->videoSurface->drawActionSafeArea(flag);
}

void TupCameraWindow::showHistory(bool flag)
{
    k->videoSurface->showHistory(flag);
}

void TupCameraWindow::updateImagesOpacity(double opacity)
{
    k->videoSurface->updateImagesOpacity(opacity);
}

void TupCameraWindow::updateImagesDepth(int depth)
{
    k->videoSurface->updateImagesDepth(depth);
}

void TupCameraWindow::updateGridSpacing(int space)
{
    k->videoSurface->updateGridSpacing(space);
}

void TupCameraWindow::updateGridColor(const QColor color)
{
    k->videoSurface->updateGridColor(color);
}

