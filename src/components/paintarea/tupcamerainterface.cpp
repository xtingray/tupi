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
#include "tdebug.h"

#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QTimer>
#include <QDesktopWidget>
#include <QMessageBox>

struct TupCameraInterface::Private
{
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QSize projectSize;
    QSize cameraSize;
};

TupCameraInterface::TupCameraInterface(const QSize projectSize, const QSize cameraSize, QWidget *parent) : QDialog(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->projectSize = projectSize;
    k->cameraSize = cameraSize;

    setWindowTitle(tr("Tupi Camera Manager"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));
    resize(cameraSize.width(), cameraSize.height());

    QByteArray cameraDevice = QCamera::availableDevices()[0];
    k->camera = new QCamera(cameraDevice);
    connect(k->camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));

    QCameraViewfinder *viewFinder = new QCameraViewfinder;
    k->camera->setViewfinder(viewFinder);
    viewFinder->show();

    k->imageCapture = new QCameraImageCapture(k->camera);
    connect(k->imageCapture, SIGNAL(imageSaved(int, const QString)), this, SLOT(imageSavedFromCamera(int, const QString)));

    QWidget *widget = new QWidget(this);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, widget);
    layout->addWidget(viewFinder);
    widget->resize(k->cameraSize.width(), k->cameraSize.height());

    k->camera->setCaptureMode(QCamera::CaptureStillImage);
    k->camera->start();

    // QTimer::singleShot(10000, this, SLOT(shotCamera()));
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

void TupCameraInterface::shotCamera()
{
    //on half pressed shutter button
    k->camera->searchAndLock();

    //on shutter button pressed
    k->imageCapture->capture("/tmp/test.png");

    //on shutter button released
    k->camera->unlock();
}

void TupCameraInterface::cameraError(QCamera::Error error)
{
    Q_UNUSED(error);

    tError() << "TupCameraInterface::cameraError(QCamera::Error) - Wow! :S";
}

void TupCameraInterface::imageSavedFromCamera(int id, const QString path)
{
    Q_UNUSED(id);

    tError() << "TupCameraInterface::imageSavedFromCamera() - Image saved from Camera at: " << path;
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
                 emit projectSizeHasChanged(k->cameraSize.width(), k->cameraSize.height());
                 tError() << "TupCameraInterface::checkSizeProject() - Calling signal projectSizeHasChanged()";
                 break;
        }
    }
}
