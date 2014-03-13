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
#include "tupcamerawindow.h"
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
#include <QSpinBox>
#include <QDoubleSpinBox>

struct TupCameraInterface::Private
{
    QStackedWidget *widgetStack;
    TupCameraWindow *currentCamera;
    QPushButton *gridButton; 
    QPushButton *safeAreaButton;
    QPushButton *historyButton;
    QWidget *gridWidget;
    QWidget *historyWidget;
    int counter;
};

TupCameraInterface::TupCameraInterface(const QString &title, QList<QByteArray> cameraDevices, QComboBox *devicesCombo, int cameraIndex, 
                                       const QSize cameraSize, const QString &path, int counter, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowTitle(tr("Tupi Camera Manager") + " | " + tr("Current resolution: ") + title);
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));

    k->counter = counter;

    k->widgetStack = new QStackedWidget();
    QSize displaySize = cameraSize;
    QDesktopWidget desktop;
    int desktopWidth = desktop.screenGeometry().width();
    int desktopHeight = desktop.screenGeometry().height();
    int maxWidth = 640;
    if (cameraDevices.count() == 1)
        maxWidth = 800;

    for (int i=0; i < cameraDevices.size(); i++) {
         QByteArray device = cameraDevices.at(i);
         QCamera *camera = new QCamera(device); 
         QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);

         QList<QSize> resolutions = imageCapture->supportedResolutions();
         QSize maxCameraSize = QSize(0, 0);
         for (int i=0; i < resolutions.size(); i++) {
              QSize resolution = resolutions.at(i);
              if (resolution.width() > maxCameraSize.width()) {
                  maxCameraSize.setWidth(resolution.width());
                  maxCameraSize.setHeight(resolution.height());
              }
         }

         if (cameraSize.width() > desktopWidth || cameraSize.height() > desktopHeight || cameraSize.width() > maxWidth) {
             int width = 0;
             int height = 0;
             if (cameraSize.width() > cameraSize.height()) {
                 width = desktopWidth/2;
                 height = width * cameraSize.height() / cameraSize.width();
             } else {
                 height = desktopHeight/2;
                 width = height * cameraSize.width() / cameraSize.height();
             }
             displaySize = QSize(width, height);
         }

         TupCameraWindow *cameraWindow = new TupCameraWindow(camera, maxCameraSize, displaySize, imageCapture, path);
         connect(cameraWindow, SIGNAL(pictureHasBeenSelected(int, const QString)), this, SIGNAL(pictureHasBeenSelected(int, const QString)));

         k->widgetStack->addWidget(cameraWindow);
    }

    k->widgetStack->setCurrentIndex(cameraIndex);
    k->currentCamera = (TupCameraWindow *) k->widgetStack->currentWidget();

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

    k->safeAreaButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "safe_area.png")), "");
    k->safeAreaButton->setIconSize(QSize(20, 20));
    k->safeAreaButton->setToolTip(tr("Show safe area"));
    k->safeAreaButton->setShortcut(QKeySequence(tr("+")));
    k->safeAreaButton->setCheckable(true);
    connect(k->safeAreaButton, SIGNAL(clicked()), this, SLOT(drawActionSafeArea()));

    k->gridButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "subgrid.png")), "");
    k->gridButton->setIconSize(QSize(20, 20));
    k->gridButton->setToolTip(tr("Show grid"));
    k->gridButton->setShortcut(QKeySequence(tr("#")));
    k->gridButton->setCheckable(true);
    connect(k->gridButton, SIGNAL(clicked()), this, SLOT(drawGrid()));

    k->gridWidget = new QWidget;
    QBoxLayout *gridLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->gridWidget);
    gridLayout->setContentsMargins(2, 2, 2, 2);

    QWidget *spacingWidget = new QWidget;
    QBoxLayout *spacingLayout = new QBoxLayout(QBoxLayout::LeftToRight, spacingWidget);
    spacingLayout->setContentsMargins(2, 2, 2, 2);

    QLabel *gridLabel = new QLabel;
    gridLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "grid_spacing.png"));
    gridLabel->setToolTip(tr("Grid spacing"));
    gridLabel->setMargin(2);

    QSpinBox *gridSpacing = new QSpinBox;
    gridSpacing->setSingleStep(10);
    gridSpacing->setRange(10, 100);
    gridSpacing->setValue(10);
    connect(gridSpacing, SIGNAL(valueChanged(int)), this, SLOT(updateGridSpacing(int)));

    spacingLayout->addWidget(gridLabel);
    spacingLayout->addWidget(gridSpacing);
    spacingLayout->addStretch(2);

    gridLayout->addWidget(spacingWidget, 1, Qt::AlignHCenter);
    gridLayout->addStretch(2);

    k->gridWidget->setVisible(false);

    k->historyButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "bitmap_array.png")), "");
    k->historyButton->setIconSize(QSize(20, 20));
    k->historyButton->setToolTip(tr("Show previous images"));
    k->historyButton->setShortcut(QKeySequence(tr("P")));
    k->historyButton->setCheckable(true);
    connect(k->historyButton, SIGNAL(clicked()), this, SLOT(showHistory()));

    k->historyWidget = new QWidget;
    QBoxLayout *historyLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->historyWidget);
    historyLayout->setContentsMargins(2, 2, 2, 2);

    QWidget *opacityWidget = new QWidget;
    QBoxLayout *opacityLayout = new QBoxLayout(QBoxLayout::LeftToRight, opacityWidget);
    opacityLayout->setContentsMargins(2, 2, 2, 2);

    QLabel *opacityLabel = new QLabel;
    opacityLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "onion.png"));
    opacityLabel->setToolTip(tr("Image opacity level")); 
    QDoubleSpinBox *opacitySpin = new QDoubleSpinBox;
    opacitySpin->setSingleStep(0.1);
    opacitySpin->setValue(0.5);
    opacitySpin->setRange(0.0, 1.0);
    opacitySpin->setDecimals(2);
    connect(opacitySpin, SIGNAL(valueChanged(double)), this, SLOT(updateImagesOpacity(double)));

    opacityLayout->addWidget(opacityLabel);
    opacityLayout->addWidget(opacitySpin); 
    opacityLayout->addStretch(2);

    QWidget *previousWidget = new QWidget;
    QBoxLayout *previousLayout = new QBoxLayout(QBoxLayout::LeftToRight, previousWidget);
    previousLayout->setContentsMargins(2, 2, 2, 2);

    QLabel *previousLabel = new QLabel;
    previousLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "layer.png"));
    previousLabel->setToolTip(tr("Amount of images to show"));
    QSpinBox *previousSpin = new QSpinBox;
    previousSpin->setValue(1);
    previousSpin->setRange(0, 5);
    connect(previousSpin, SIGNAL(valueChanged(int)), this, SLOT(updateImagesDepth(int)));

    previousLayout->addWidget(previousLabel);
    previousLayout->addWidget(previousSpin);
    previousLayout->addStretch(2);

    historyLayout->addWidget(opacityWidget, 1, Qt::AlignHCenter);
    historyLayout->addWidget(previousWidget, 1, Qt::AlignHCenter);
    historyLayout->addStretch(2);

    k->historyWidget->setVisible(false);

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
    menuLayout->addWidget(k->safeAreaButton);
    menuLayout->addWidget(k->gridButton);
    menuLayout->addWidget(k->gridWidget);
    menuLayout->addWidget(k->historyButton);
    menuLayout->addWidget(k->historyWidget);
    menuLayout->addStretch(2);

    connect(devicesCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCameraDevice(int)));

    QBoxLayout *dialogLayout = new QBoxLayout(QBoxLayout::LeftToRight, this); 
    dialogLayout->addWidget(k->widgetStack);
    dialogLayout->addWidget(menuWidget);

    k->currentCamera->startCamera();
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

    k->currentCamera->reset();
}

void TupCameraInterface::takePicture()
{
    k->currentCamera->takePicture(k->counter);
    k->counter++;
}

void TupCameraInterface::changeCameraDevice(int index)
{
    TupCameraWindow *item = (TupCameraWindow *) k->widgetStack->currentWidget();
    item->stopCamera(); 

    k->widgetStack->setCurrentIndex(index);   
    k->currentCamera = (TupCameraWindow *) k->widgetStack->currentWidget();
    k->currentCamera->startCamera();
    drawGrid();
    drawActionSafeArea();
}

void TupCameraInterface::drawGrid()
{
    bool flag = k->gridButton->isChecked();
    k->gridWidget->setVisible(flag);
    k->currentCamera->drawGrid(flag);
}

void TupCameraInterface::drawActionSafeArea()
{
    k->currentCamera->drawActionSafeArea(k->safeAreaButton->isChecked());
}

void TupCameraInterface::showHistory()
{
    bool flag = k->historyButton->isChecked();
    k->historyWidget->setVisible(flag);
    k->currentCamera->showHistory(flag);
}

void TupCameraInterface::updateImagesOpacity(double opacity)
{
    k->currentCamera->updateImagesOpacity(opacity);
}

void TupCameraInterface::updateImagesDepth(int depth)
{
    k->currentCamera->updateImagesDepth(depth);
}

void TupCameraInterface::updateGridSpacing(int space)
{
    k->currentCamera->updateGridSpacing(space);
}
