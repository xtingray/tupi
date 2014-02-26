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
#include "talgorithm.h"
#include "tseparator.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QTimer>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QImageEncoderSettings>

struct TupCameraInterface::Private
{
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QSize cameraSize;
    QString dir;
    int counter;
};

TupCameraInterface::TupCameraInterface(QComboBox *devicesCombo, QCamera *camera, const QSize cameraSize, QCameraImageCapture *imageCapture, 
                                       QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    QString res = QString::number(cameraSize.width()) + "x" + QString::number(cameraSize.height());
    setWindowTitle(tr("Tupi Camera Manager") + " | " + tr("Current resolution: ") + res);
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));

    k->cameraSize = cameraSize;
    QDesktopWidget desktop;
    int desktopWidth = desktop.screenGeometry().width();
    int desktopHeight = desktop.screenGeometry().height();
    if (k->cameraSize.width() > desktopWidth || k->cameraSize.height() > desktopHeight) {
        int width = 0;
        int height = 0;
        if (k->cameraSize.width() > k->cameraSize.height()) {
            width = desktopWidth/2;
            height = width * k->cameraSize.height() / k->cameraSize.width();
        } else {
            height = desktopHeight/2;
            width = height * k->cameraSize.width() / k->cameraSize.height();
        }

        resize(width, height);
    } else {
        resize(k->cameraSize + QSize(100, 100));
    }

    tError() << "TupCameraInterface() - camera w: " << k->cameraSize.width();
    tError() << "TupCameraInterface() - camera h: " << k->cameraSize.height();

    k->counter = 1;

    k->dir = CACHE_DIR + TAlgorithm::randomString(8); 
    tError() << "TupCameraInterface() - k->dir: " << k->dir;

    QDir dir;
    if (!dir.mkdir(k->dir))
        tError() << "TupCameraInterface() - Fatal Error: Can't create dir!";

    k->dir = k->dir + QDir::separator() + "pic";

    k->camera = camera;
    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));

    QCameraViewfinder *viewFinder = new QCameraViewfinder;
    k->camera->setViewfinder(viewFinder);
    viewFinder->show();

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
    cameraLayout->addWidget(viewFinder);

    QWidget *menuWidget = new QWidget;
    QBoxLayout *menuLayout = new QBoxLayout(QBoxLayout::TopToBottom, menuWidget);

    QLabel *devicesLabel = new QLabel(tr("Devices"));
    devicesLabel->setAlignment(Qt::AlignHCenter);

    QPushButton *clickButton = new QPushButton(tr("Take Picture"));
    connect(clickButton, SIGNAL(clicked()), this, SLOT(takePicture()));

    menuLayout->addWidget(devicesLabel);
    menuLayout->addWidget(devicesCombo);
    menuLayout->addWidget(new TSeparator(Qt::Horizontal));
    menuLayout->addWidget(clickButton);
    menuLayout->addStretch(2);

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

    tError() << "TupCameraInterface::closeEvent() - Stopping camera!";

    if (k->camera)
        k->camera->stop();

    delete k;
}

void TupCameraInterface::takePicture()
{
    QString imagePath = k->dir + QString::number(k->counter) + ".jpg";

    tError() << "TupCameraInterface::takePicture() - imagePath: " << imagePath;

    //on half pressed shutter button
    k->camera->searchAndLock();

    //on shutter button pressed
    k->imageCapture->capture(imagePath);

    //on shutter button released
    k->camera->unlock();

    k->counter++;
}

void TupCameraInterface::cameraError(QCamera::Error error)
{
    Q_UNUSED(error);

    tError() << "TupCameraInterface::cameraError(QCamera::Error) - Wow! :S";
}

void TupCameraInterface::imageSavedFromCamera(int id, const QString path)
{
    tError() << "TupCameraInterface::imageSavedFromCamera() - ID: " << id;  
    tError() << "TupCameraInterface::imageSavedFromCamera() - Image saved from Camera at: " << path;
    if (path.isEmpty())
        return;

    emit pictureHasBeenSelected(id, path);
}
