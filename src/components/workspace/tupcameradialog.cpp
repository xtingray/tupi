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

#include "tupcameradialog.h"

struct TupCameraDialog::Private
{
    QSize projectSize;
    QSize cameraSize;
    bool resizeProject;

    QCamera *camera;
    QComboBox *devicesCombo;
    QComboBox *resolutionCombo;
    QList<QSize> resolutions;
    QString cameraReference;
    int deviceIndex;
    bool useBasicInterface;
};

TupCameraDialog::TupCameraDialog(QComboBox *devicesCombo, const QSize projectSize, QList<QSize> resolutions, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Camera Settings"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/photo.png")));

    k->useBasicInterface = false;
    k->projectSize = projectSize;
    k->resolutions = resolutions;
    k->resizeProject = false;
    k->devicesCombo = devicesCombo;

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    if (devicesCombo->count() > 1) {
        k->cameraReference = k->devicesCombo->itemText(0);
        k->deviceIndex = 0; 
        setCamera(k->cameraReference);

        QLabel *cameraLabel = new QLabel(tr("Available Camera Devices:"));
        layout->addWidget(cameraLabel);
        layout->addWidget(k->devicesCombo);
        connect(devicesCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(changeCameraDevice(const QString &)));
    } else {
        QLabel *cameraLabel = new QLabel(tr("Camera Detected:"));
        QLabel *cameraReference = new QLabel;
        QFont font = cameraReference->font();
        font.setBold(true);
        cameraReference->setFont(font);
        cameraReference->setText(k->devicesCombo->itemText(0));
        layout->addWidget(cameraLabel);
        layout->addWidget(cameraReference);
    }

    k->deviceIndex = 0;

    QLabel *resolutionLabel = new QLabel(tr("Available Camera Resolutions:"));
    k->resolutionCombo = new QComboBox();
    for (int i=0; i<k->resolutions.size(); i++) {
         QSize size = k->resolutions.at(i);
         QString label = QString::number(size.width()) + "x" + QString::number(size.height());
         k->resolutionCombo->addItem(label);
    }
    k->resolutionCombo->setCurrentIndex(k->resolutions.size() - 1);
    k->cameraSize = k->resolutions.at(k->resolutions.size() - 1);

    connect(k->resolutionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCameraResolution(int)));

    QCheckBox *resizeCheck = new QCheckBox(tr("Resize my project to fit camera resolution"));
    connect(resizeCheck, SIGNAL(toggled(bool)), this, SLOT(projectSizeHasChanged(bool)));

    QCheckBox *lowCheck = new QCheckBox(tr("Use the basic camera interface (low resources)"));
    connect(lowCheck, SIGNAL(toggled(bool)), this, SLOT(enableBasicCamera(bool)));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);

    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancel);

    QPushButton *ok = new QPushButton(tr("Ok"));
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(ok);
    ok->setDefault(true);

    layout->addWidget(resolutionLabel);
    layout->addWidget(k->resolutionCombo);
    layout->addWidget(resizeCheck);
    layout->addWidget(lowCheck);

    layout->addLayout(buttonLayout);
}

TupCameraDialog::~TupCameraDialog()
{
}

void TupCameraDialog::changeCameraDevice(const QString &cameraReference)
{
    disconnect(k->resolutionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCameraResolution(int)));
    k->cameraReference = cameraReference;
    k->deviceIndex = k->devicesCombo->currentIndex();
    setCamera(k->cameraReference);

    // QCameraImageCapture *imageCapture = new QCameraImageCapture(k->camera);
    // k->resolutions = imageCapture->supportedResolutions();

    k->resolutions.clear();
    k->resolutions << QSize(1280, 1024);
    k->resolutions << QSize(1280, 960);
    k->resolutions << QSize(1224, 768);
    k->resolutions << QSize(800, 600);
    k->resolutions << QSize(640, 480);
    k->resolutions << QSize(352, 288);
    k->resolutions << QSize(320, 240);
    k->resolutions << QSize(176, 144);
    k->resolutions << QSize(160, 120);
	
    k->resolutionCombo->clear();

    for (int i=0; i<k->resolutions.size(); i++) {
         QSize size = k->resolutions.at(i);
         QString label = QString::number(size.width()) + "x" + QString::number(size.height());
         k->resolutionCombo->addItem(label);
    }
    k->resolutionCombo->setCurrentIndex(k->resolutions.size() - 1);
    k->cameraSize = k->resolutions.at(k->resolutions.size() - 1);

    connect(k->resolutionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCameraResolution(int)));
}

void TupCameraDialog::setCamera(const QString &cameraReference)
{
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
            QString description = k->camera->deviceDescription(deviceName);
            if (description.compare(cameraReference) == 0) {
                k->camera = new QCamera(deviceName);
                break;
            }
    }
}

void TupCameraDialog::setCameraResolution(int index)
{
    k->cameraSize = k->resolutions.at(index);
}

void TupCameraDialog::projectSizeHasChanged(bool flag)
{
    k->resizeProject = flag;
}

QSize TupCameraDialog::cameraResolution() const
{
    return k->cameraSize;
}

bool TupCameraDialog::changeProjectSize()
{
    return k->resizeProject;
}

int TupCameraDialog::cameraIndex()
{
    return k->deviceIndex;
}

void TupCameraDialog::enableBasicCamera(bool flag)
{
    k->useBasicInterface = flag;
}

bool TupCameraDialog::useBasicCamera()
{
    return k->useBasicInterface;
}
