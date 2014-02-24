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
#include "tdebug.h"

#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QTimer>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPushButton>

struct TupCameraInterface::Private
{
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QSize projectSize;
    QSize cameraSize;
    QString dir;
    int counter;
};

TupCameraInterface::TupCameraInterface(QComboBox *devicesCombo, QList<QSize> resolutions, QCamera *camera, const QSize cameraSize, QCameraImageCapture *imageCapture, 
                                       const QSize projectSize, QWidget *parent) : QDialog(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setModal(false);

    k->projectSize = projectSize;
    k->cameraSize = cameraSize;
    k->counter = 1;

    k->dir = CACHE_DIR + QDir::separator() + TAlgorithm::randomString(8); 
    tError() << "TupCameraInterface() - k->dir: " << k->dir;

    QDir dir;
    if (dir.mkdir(k->dir))
        tError() << "TupCameraInterface() - Fatal Error: Can't create dir!";

    k->dir = k->dir + QDir::separator() + "pic";

    setWindowTitle(tr("Tupi Camera Manager"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));
    resize(cameraSize.width(), cameraSize.height());

    k->camera = camera;
    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));

    QCameraViewfinder *viewFinder = new QCameraViewfinder;
    k->camera->setViewfinder(viewFinder);
    viewFinder->show();

    k->imageCapture = imageCapture;
    connect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

    QWidget *cameraWidget = new QWidget;
    QBoxLayout *cameraLayout = new QBoxLayout(QBoxLayout::TopToBottom, cameraWidget);
    cameraLayout->addWidget(viewFinder);
    cameraWidget->resize(k->cameraSize.width(), k->cameraSize.height());

    QWidget *menuWidget = new QWidget;
    QBoxLayout *menuLayout = new QBoxLayout(QBoxLayout::TopToBottom, menuWidget);
    
    QLabel *modeLabel = new QLabel(tr("Mode"));
    modeLabel->setAlignment(Qt::AlignHCenter);
    QComboBox *modeCombo = new QComboBox();
    modeCombo->addItem(tr("Camera"));
    modeCombo->addItem(tr("History"));

    QBoxLayout *modeLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(modeCombo);

    QWidget *cameraOptions = new QWidget;
    QBoxLayout *cameraOptionsLayout = new QBoxLayout(QBoxLayout::TopToBottom, cameraOptions);

    QLabel *devicesLabel = new QLabel(tr("Devices"));
    devicesLabel->setAlignment(Qt::AlignHCenter);
    QLabel *resolutionLabel = new QLabel(tr("Resolutions"));
    resolutionLabel->setAlignment(Qt::AlignHCenter);
    QComboBox *resolutionCombo = new QComboBox();
    for (int i=0; i<resolutions.size(); i++) {
         QSize size = resolutions.at(i);
         QString label = QString::number(size.width()) + "x" + QString::number(size.height());
         resolutionCombo->addItem(label);
    }
    resolutionCombo->setCurrentIndex(resolutions.size() - 1);
    QPushButton *clickButton = new QPushButton(tr("Take Picture"));
    connect(clickButton, SIGNAL(clicked()), this, SLOT(takePicture()));

    cameraOptionsLayout->addWidget(devicesLabel);
    cameraOptionsLayout->addWidget(devicesCombo);
    cameraOptionsLayout->addWidget(resolutionLabel);
    cameraOptionsLayout->addWidget(resolutionCombo);
    cameraOptionsLayout->addWidget(clickButton);

    modeLayout->addWidget(cameraOptions);

    modeLayout->addStretch(2);
    menuLayout->addLayout(modeLayout);

    // menuLayout->addWidget(devicesCombo);

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

    delete k;
}

void TupCameraInterface::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    if (k->camera)
        k->camera->stop();
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

/*
QImageEncoderSettings imageSettings;
imageSettings.setCodec(“image/jpeg”);
imageSettings.setResolution(1600, 1200);
// Apply the encoding settings.
imageCapture->setEncodingSettings(imageSettings);
*/

void TupCameraInterface::checkSizeProject()
{
    if (k->projectSize != k->cameraSize) {
        QDesktopWidget desktop;
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Question"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("The camera resolution is different than your project size."));
        msgBox.setInformativeText(tr("Do you want to adjust your project size?"));

        msgBox.addButton(QString(tr("Adjust it")), QMessageBox::AcceptRole);
        msgBox.addButton(QString(tr("Cancel")), QMessageBox::DestructiveRole);

        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2 ,
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::AcceptRole:
                 emit projectSizeHasChanged(QSize(k->cameraSize.width(), k->cameraSize.height()));
                 tError() << "TupCameraInterface::checkSizeProject() - Calling signal projectSizeHasChanged()";
                 break;
        }
    }
}
