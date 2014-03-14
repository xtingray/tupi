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

#include "tupbasiccamerainterface.h"
#include "tupapplication.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "talgorithm.h"
#include "tosd.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QDesktopWidget>
#include <QPushButton>

struct TupBasicCameraInterface::Private
{
    QStackedWidget *widgetStack;
    QList <QCamera *> cameras;
    QCamera *currentCamera;
    QList <QCameraImageCapture *> imageCaptors;
    QCameraImageCapture *currentImageCaptor;

    QString path;
    int counter;
};

TupBasicCameraInterface::TupBasicCameraInterface(const QString &title, QList<QByteArray> cameraDevices, QComboBox *devicesCombo, int cameraIndex, 
                                       const QSize cameraSize, int counter, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowTitle(tr("Tupi Camera Manager") + " | " + tr("Current resolution:") + " " + title);
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));

    k->counter = counter;
    k->path = randomPath();
    QSize displaySize = cameraSize;
    k->widgetStack = new QStackedWidget();
    QDesktopWidget desktop;
    int desktopWidth = desktop.screenGeometry().width();

    if (cameraSize.width() > desktopWidth) {
        int width = desktopWidth/2;
        int height = width * cameraSize.height() / cameraSize.width();
        displaySize = QSize(width, height);
    } else {
        int maxWidth = 640;
        if (desktopWidth > 800)
            maxWidth = 800;

        if (cameraSize.width() > maxWidth) {
            int height = maxWidth * cameraSize.height() / cameraSize.width();
            displaySize = QSize(maxWidth, height);
        }
    }

    if (cameraDevices.count() == 1) {
        QByteArray device = cameraDevices.at(0);
        QCamera *camera = new QCamera(device);
        QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);
        connect(imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

        QCameraViewfinder *viewfinder = new QCameraViewfinder;
        viewfinder->setFixedSize(displaySize);
        camera->setViewfinder(viewfinder);
        k->cameras << camera;
        k->imageCaptors << imageCapture;

        k->widgetStack->addWidget(viewfinder);
    } else {
        for (int i=0; i < cameraDevices.size(); i++) {
             QByteArray device = cameraDevices.at(i);
             QCamera *camera = new QCamera(device); 
             QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);
             connect(imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

             QCameraViewfinder *viewfinder = new QCameraViewfinder;
             viewfinder->setFixedSize(displaySize);
             camera->setViewfinder(viewfinder);
             k->cameras << camera;
             k->imageCaptors << imageCapture;

             k->widgetStack->addWidget(viewfinder);
        }
    } 

    k->widgetStack->setCurrentIndex(cameraIndex);

    QWidget *menuWidget = new QWidget;
    QBoxLayout *menuLayout = new QBoxLayout(QBoxLayout::TopToBottom, menuWidget);

    QLabel *devicesLabel = new QLabel;
    QString deviceString = tr("Cameras");
    if (devicesCombo->count() == 1)
        deviceString = tr("Camera");
    devicesLabel->setText(deviceString);
    devicesLabel->setAlignment(Qt::AlignHCenter);

    QPushButton *clickButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "photo.png")), "");
    clickButton->setIconSize(QSize(20, 20));
    clickButton->setToolTip(tr("Take picture"));
    connect(clickButton, SIGNAL(clicked()), this, SLOT(takePicture()));

    menuLayout->addWidget(devicesLabel);
    if (devicesCombo->count() == 1) {
        QLabel *deviceDesc = new QLabel;
        QFont font = deviceDesc->font();
        font.setBold(true);
        deviceDesc->setFont(font);
        deviceDesc->setText(devicesCombo->itemText(0));
        menuLayout->addWidget(deviceDesc);
    } else {
        menuLayout->addWidget(devicesCombo);
    } 

    devicesCombo->setCurrentIndex(cameraIndex);
    menuLayout->addWidget(new TSeparator(Qt::Horizontal));
    menuLayout->addWidget(clickButton);
    menuLayout->addStretch(2);

    connect(devicesCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCameraDevice(int)));

    QBoxLayout *dialogLayout = new QBoxLayout(QBoxLayout::LeftToRight, this); 
    dialogLayout->addWidget(k->widgetStack);
    dialogLayout->addWidget(menuWidget);

    k->currentCamera = (QCamera *) k->cameras.at(cameraIndex);
    k->currentImageCaptor = (QCameraImageCapture *) k->imageCaptors.at(cameraIndex);
    k->currentCamera->start();
}

TupBasicCameraInterface::~TupBasicCameraInterface()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

void TupBasicCameraInterface::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    QDir dir(k->path);
    foreach (QString file, dir.entryList(QStringList() << "*.jpg")) {
             QString absolute = dir.absolutePath() + QDir::separator() + file;
             QFile::remove(absolute);
    }

    if (! dir.rmdir(dir.absolutePath())) {
        #ifdef K_DEBUG
               tError() << "TupBasicCameraInterface::closeEvent() - Fatal Error: Can't remove pictures directory -> " << dir.absolutePath();
        #endif
    }

    if (k->currentCamera->state() == QCamera::ActiveState)
        k->currentCamera->stop();
}

QString TupBasicCameraInterface::randomPath()
{
    QString path = CACHE_DIR + TAlgorithm::randomString(8);
    QDir dir;
    if (!dir.mkdir(path)) {
        #ifdef K_DEBUG
               tError() << "TupBasicCameraInterface::randomPath() - Fatal Error: Can't create pictures directory -> " << path;
        #endif
        path = "";
        TOsd::self()->display(tr("Error"), tr("Can't create pictures directory"), TOsd::Error);
    }

    return path;
}

void TupBasicCameraInterface::takePicture()
{
    QString prev = "pic";
    if (k->counter < 10)
        prev += "00";
    if (k->counter >= 10 && k->counter < 100)
        prev += "0";

    QString imagePath = k->path + QDir::separator() + prev + QString::number(k->counter) + ".jpg";

    //on half pressed shutter button
    k->currentCamera->searchAndLock();

    //on shutter button pressed
    k->currentImageCaptor->capture(imagePath);

    //on shutter button released
    k->currentCamera->unlock();
}

void TupBasicCameraInterface::changeCameraDevice(int index)
{
    k->currentCamera->stop();
    k->widgetStack->setCurrentIndex(index);   
    k->currentCamera = (QCamera *) k->cameras.at(index);
    k->currentImageCaptor = (QCameraImageCapture *) k->imageCaptors.at(index);

    k->currentCamera->start();
}

void TupBasicCameraInterface::imageSavedFromCamera(int id, const QString path)
{
    Q_UNUSED(id);

    if (path.isEmpty())
        return;

    emit pictureHasBeenSelected(k->counter, path);
    k->counter++;
}
