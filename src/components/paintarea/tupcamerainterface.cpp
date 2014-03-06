/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#include "tupcamerainterface.h"
#include "tupapplication.h"
#include "tapplicationproperties.h"
#include "toptionaldialog.h"
#include "tseparator.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QTimer>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPushButton>
// #include <QImageEncoderSettings>

struct TupCameraInterface::Private
{
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QCameraViewfinder *viewFinder;
    QSize cameraSize;
    QString dir;
    int counter;
};

TupCameraInterface::TupCameraInterface(const QString &title, QComboBox *devicesCombo, int cameraIndex, QCamera *camera, const QSize maxCameraSize, 
                                       QCameraImageCapture *imageCapture, const QString &path, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowTitle(tr("Tupi Camera Manager") + " | " + tr("Current resolution: ") + title);
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));
    k->cameraSize = maxCameraSize;
    k->dir = path;

    QDesktopWidget desktop;
    int desktopWidth = desktop.screenGeometry().width();
    int desktopHeight = desktop.screenGeometry().height();

    if (k->cameraSize.width() > desktopWidth || k->cameraSize.height() > desktopHeight || k->cameraSize.width() > 800) {
        int width = 0;
        int height = 0;
        if (k->cameraSize.width() > k->cameraSize.height()) {
            width = desktopWidth/2;
            height = width * k->cameraSize.height() / k->cameraSize.width();
        } else {
            height = desktopHeight/2;
            width = height * k->cameraSize.width() / k->cameraSize.height();
        }
        k->cameraSize = QSize(width, height);
    } 

    k->counter = 1;
    k->camera = camera;
    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));

    k->viewFinder = new QCameraViewfinder;
    k->viewFinder->setFixedSize(k->cameraSize);
    k->camera->setViewfinder(k->viewFinder);
    k->viewFinder->show();

    k->imageCapture = imageCapture;

    /*
    QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(k->cameraSize.width(), k->cameraSize.height());
    k->imageCapture->setEncodingSettings(imageSettings);
    */

    connect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

    QWidget *cameraWidget = new QWidget;
    QBoxLayout *cameraLayout = new QBoxLayout(QBoxLayout::TopToBottom, cameraWidget);
    cameraLayout->addWidget(k->viewFinder);

    QWidget *menuWidget = new QWidget;
    QBoxLayout *menuLayout = new QBoxLayout(QBoxLayout::TopToBottom, menuWidget);

    QLabel *devicesLabel = new QLabel(tr("Devices"));
    devicesLabel->setAlignment(Qt::AlignHCenter);

    QPushButton *clickButton = new QPushButton(tr("Take Picture"));
    connect(clickButton, SIGNAL(clicked()), this, SLOT(takePicture()));

    menuLayout->addWidget(devicesLabel);
    menuLayout->addWidget(devicesCombo);
    devicesCombo->setCurrentIndex(cameraIndex);
    menuLayout->addWidget(new TSeparator(Qt::Horizontal));
    menuLayout->addWidget(clickButton);
    menuLayout->addStretch(2);

    connect(devicesCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(changeCameraDevice(const QString &)));

    QBoxLayout *dialogLayout = new QBoxLayout(QBoxLayout::LeftToRight, this); 
    dialogLayout->addWidget(cameraWidget);
    dialogLayout->addWidget(menuWidget);

    k->camera->setCaptureMode(QCamera::CaptureStillImage);
    k->camera->start();
}

TupCameraInterface::~TupCameraInterface()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupCameraInterface::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    QDir dir(k->dir);
    foreach (QString file, dir.entryList(QStringList() << "*.jpg")) {
             QString absolute = dir.absolutePath() + QDir::separator() + file;
             QFile::remove(absolute);
    }

    if (! dir.rmdir(dir.absolutePath())) {
        #ifdef K_DEBUG
               tError() << "TupCameraInterface::closeEvent() - Fatal Error: Can't remove pictures directory -> " << dir.absolutePath();
        #endif
    }

    if (k->camera->state() == QCamera::ActiveState)
        k->camera->stop();

    delete k;
}

void TupCameraInterface::takePicture()
{
    QString prev = "pic";
    if (k->counter < 10)
        prev += "00";  
    if (k->counter > 10 && k->counter < 100)
        prev += "0";
 
    QString imagePath = k->dir + QDir::separator() + prev + QString::number(k->counter) + ".jpg";

    tError() << "TupCameraInterface::takePicture() - imagePath: " << imagePath;

    //on half pressed shutter button
    k->camera->searchAndLock();

    //on shutter button pressed
    k->imageCapture->capture(imagePath);

    //on shutter button released
    k->camera->unlock();
}

void TupCameraInterface::cameraError(QCamera::Error error)
{
    Q_UNUSED(error);
    #ifdef K_DEBUG
           tError() << "TupCameraInterface::cameraError() - Fatal Error: " << k->camera->errorString();
    #endif
}

void TupCameraInterface::imageSavedFromCamera(int id, const QString path)
{
    Q_UNUSED(id);

    tError() << "TupCameraInterface::imageSavedFromCamera() - ID: " << k->counter;  
    tError() << "TupCameraInterface::imageSavedFromCamera() - Image saved from Camera at: " << path;
    if (path.isEmpty())
        return;

    emit pictureHasBeenSelected(k->counter, path);

    k->counter++;
}

void TupCameraInterface::changeCameraDevice(const QString &cameraDesc)
{
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
            QCamera *device = new QCamera(deviceName);
            QString description = device->deviceDescription(deviceName);
            if (description.compare(cameraDesc) == 0) { 
                if (k->camera) {
                    if (k->camera->state() == QCamera::ActiveState)
                        k->camera->stop();

                    disconnect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));
                    k->camera = device;
                    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));

                    disconnect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));
                    k->imageCapture = new QCameraImageCapture(k->camera);
                    connect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

                    k->camera->setCaptureMode(QCamera::CaptureStillImage);
                    k->camera->start();
                    k->camera->setViewfinder(k->viewFinder);
                    k->viewFinder->show();

                    break;
                }
            }
    }
}
