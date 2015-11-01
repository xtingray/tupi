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
    QColor gridColor;
    TupColorWidget *colorCell;
};

TupCameraInterface::TupCameraInterface(const QString &title, QList<QByteArray> cameraDevices, QComboBox *devicesCombo, int cameraIndex, 
                                       const QSize cameraSize, int counter, QWidget *parent) : QFrame(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCameraInterface()]";
        #else
            TINIT;
        #endif
    #endif

    setWindowTitle(tr("Tupi Camera Manager") + " | " + tr("Current resolution:") + " " + title);
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png")));

    k->counter = counter;

    k->widgetStack = new QStackedWidget();
    QSize displaySize = cameraSize;
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

    QString path = randomPath();

    if (cameraDevices.count() == 1) {
        QByteArray device = cameraDevices.at(0);
        QCamera *camera = new QCamera(device);
        QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);

        TupCameraWindow *cameraWindow = new TupCameraWindow(camera, cameraSize, displaySize, imageCapture, path);
        connect(cameraWindow, SIGNAL(pictureHasBeenSelected(int, const QString)), this, SIGNAL(pictureHasBeenSelected(int, const QString)));

        k->widgetStack->addWidget(cameraWindow);
    } else {
        for (int i=0; i < cameraDevices.size(); i++) {
             QByteArray device = cameraDevices.at(i);
             QCamera *camera = new QCamera(device); 
             QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);

             // QSize camSize = setBestResolution(imageCapture->supportedResolutions(), cameraSize);

             QList<QSize> resolutions;
             resolutions << QSize(640, 480);
             QSize camSize = setBestResolution(resolutions, cameraSize);

             TupCameraWindow *cameraWindow = new TupCameraWindow(camera, camSize, displaySize, imageCapture, path);
             connect(cameraWindow, SIGNAL(pictureHasBeenSelected(int, const QString)), this, SIGNAL(pictureHasBeenSelected(int, const QString)));

             k->widgetStack->addWidget(cameraWindow);
        }
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
    QGridLayout *gridLayout = new QGridLayout(k->gridWidget);
    gridLayout->setHorizontalSpacing(2);

    QLabel *gridLabel = new QLabel;
    gridLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "grid_spacing.png"));
    gridLabel->setToolTip(tr("Grid spacing"));
    gridLabel->setMargin(2);

    QSpinBox *gridSpacing = new QSpinBox;
    gridSpacing->setSingleStep(10);
    gridSpacing->setRange(10, 100);
    gridSpacing->setValue(10);
    connect(gridSpacing, SIGNAL(valueChanged(int)), this, SLOT(updateGridSpacing(int)));

    QLabel *colorLabel = new QLabel;
    colorLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "color_palette.png"));
    colorLabel->setToolTip(tr("Grid color"));
    colorLabel->setMargin(2);

    k->gridColor = QColor(0, 0, 180, 50);
    k->colorCell = new TupColorWidget(k->gridColor);
    connect(k->colorCell, SIGNAL(clicked()), this, SLOT(updateColour()));

    gridLayout->addWidget(gridLabel, 0, 0, Qt::AlignHCenter);
    gridLayout->addWidget(gridSpacing, 0, 1, Qt::AlignHCenter);
    gridLayout->addWidget(colorLabel, 1, 0, Qt::AlignHCenter);
    gridLayout->addWidget(k->colorCell, 1, 1, Qt::AlignHCenter);

    k->gridWidget->setVisible(false);

    k->historyButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "bitmap_array.png")), "");
    k->historyButton->setIconSize(QSize(20, 20));
    k->historyButton->setToolTip(tr("Show previous images"));
    k->historyButton->setShortcut(QKeySequence(tr("P")));
    k->historyButton->setCheckable(true);
    connect(k->historyButton, SIGNAL(clicked()), this, SLOT(showHistory()));

    k->historyWidget = new QWidget;
    QGridLayout *historyLayout = new QGridLayout(k->historyWidget);
    historyLayout->setHorizontalSpacing(2);

    QLabel *opacityLabel = new QLabel;
    opacityLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "onion.png"));
    opacityLabel->setToolTip(tr("Image opacity level")); 
    QDoubleSpinBox *opacitySpin = new QDoubleSpinBox;
    opacitySpin->setSingleStep(0.1);
    opacitySpin->setValue(0.5);
    opacitySpin->setRange(0.0, 1.0);
    opacitySpin->setDecimals(2);
    connect(opacitySpin, SIGNAL(valueChanged(double)), this, SLOT(updateImagesOpacity(double)));

    QLabel *previousLabel = new QLabel;
    previousLabel->setPixmap(QPixmap(THEME_DIR + "icons" + QDir::separator() + "layer.png"));
    previousLabel->setToolTip(tr("Amount of images to show"));
    QSpinBox *previousSpin = new QSpinBox;
    previousSpin->setValue(1);
    previousSpin->setRange(0, 5);
    connect(previousSpin, SIGNAL(valueChanged(int)), this, SLOT(updateImagesDepth(int)));

    historyLayout->addWidget(opacityLabel, 0, 0, Qt::AlignHCenter);
    historyLayout->addWidget(opacitySpin, 0, 1, Qt::AlignHCenter);
    historyLayout->addWidget(previousLabel, 1, 0, Qt::AlignHCenter);
    historyLayout->addWidget(previousSpin, 1, 1, Qt::AlignHCenter);

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

    QPushButton *exitButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "exit.png")), "");
    exitButton->setIconSize(QSize(20, 20));
    exitButton->setToolTip(tr("Close manager"));
    exitButton->setShortcut(Qt::Key_Escape);
    connect(exitButton, SIGNAL(clicked()), this, SLOT(close()));

    devicesCombo->setCurrentIndex(cameraIndex);
    menuLayout->addWidget(new TSeparator(Qt::Horizontal));
    menuLayout->addWidget(clickButton);
    menuLayout->addWidget(k->safeAreaButton);
    menuLayout->addWidget(k->gridButton);
    menuLayout->addWidget(k->gridWidget);
    menuLayout->addWidget(k->historyButton);
    menuLayout->addWidget(k->historyWidget);
    menuLayout->addWidget(exitButton);
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
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupCameraInterface()]";
        #else
            TEND;
        #endif
    #endif
}

void TupCameraInterface::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    k->currentCamera->reset();
}

QSize TupCameraInterface::setBestResolution(QList<QSize> resolutions, QSize cameraSize)
{
    QSize maxCameraSize = QSize(0, 0);
    for (int i=0; i < resolutions.size(); i++) {
         QSize resolution = resolutions.at(i);
         if (cameraSize.width() == resolution.width() && cameraSize.height() == resolution.height())
             return cameraSize;
         if (resolution.width() > maxCameraSize.width())
             maxCameraSize = resolution;
    }

    return maxCameraSize;
}

QString TupCameraInterface::randomPath()
{
    QString path = CACHE_DIR + TAlgorithm::randomString(8);
    QDir dir;
    if (!dir.mkdir(path)) {
        #ifdef K_DEBUG
            QString msg = "TupCameraInterface::randomPath() - Fatal Error: Can't create pictures directory -> " + path;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        path = "";
        TOsd::self()->display(tr("Error"), tr("Can't create pictures directory"), TOsd::Error);
    }

    return path;
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
    showHistory();
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

void TupCameraInterface::updateColour()
{
    QColor color = QColorDialog::getColor(k->gridColor, this);
    if (color.isValid()) {
        k->currentCamera->updateGridColor(color);
        k->colorCell->setBrush(QBrush(color));
    }
}
