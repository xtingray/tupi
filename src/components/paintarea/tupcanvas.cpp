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

#include "tupcanvas.h"

struct TupCanvas::Private
{
    QColor currentColor;
    TupBrushManager *brushManager;
    int frameIndex;
    int layerIndex;
    int sceneIndex;
    QSize size;
    TupGraphicsScene *scene;
    TupProject *project;
    bool sketchMenuIsOpen;
    bool selectionMenuIsOpen;
    bool propertiesMenuIsOpen;
    bool exposureDialogIsOpen;
    UserHand hand;
    // TupInfoWidget *display;
    bool isNetworked;
    QStringList onLineUsers;
    TupExposureDialog *exposureDialog;
};

TupCanvas::TupCanvas(QWidget *parent, Qt::WindowFlags flags, TupGraphicsScene *scene, 
                   const QPointF centerPoint, const QSize &screenSize, TupProject *project, qreal scaleFactor,
                   int angle, TupBrushManager *brushManager, bool isNetworked, const QStringList &onLineUsers) : QFrame(parent, flags), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCanvas()]";
        #else
            TINIT;
        #endif
    #endif

    setWindowTitle(tr("Tupi: Open 2D Magic"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")));

    k->hand = Right;
    // k->hand = Left;
 
    k->scene = scene;
    // connect(k->scene, SIGNAL(showInfoWidget()), this, SLOT(showInfoWidget()));
  
    k->isNetworked = isNetworked;
    k->onLineUsers = onLineUsers;
    k->size = project->dimension();
    k->currentColor = brushManager->penColor();
    k->brushManager = brushManager;
    k->project = project;

    k->sketchMenuIsOpen = false;
    k->selectionMenuIsOpen = false;
    k->propertiesMenuIsOpen = false;
    k->exposureDialogIsOpen = false;

    graphicsView = new TupCanvasView(this, screenSize, k->size, project->bgColor());
    connect(graphicsView, SIGNAL(rightClick()), this, SIGNAL(rightClick()));

    graphicsView->setScene(scene);
    graphicsView->centerOn(centerPoint);
    graphicsView->scale(scaleFactor, scaleFactor);
    graphicsView->rotate(angle);

    TImageButton *sketchTools = new TImageButton(QPixmap(THEME_DIR + "icons/pencil_big.png"), 60, this, true);
    sketchTools->setToolTip(tr("Sketch Tools"));
    connect(sketchTools, SIGNAL(clicked()), this, SLOT(sketchTools()));

    TImageButton *images = new TImageButton(QPixmap(THEME_DIR + "icons/bitmap_big.png"), 60, this, true);
    images->setToolTip(tr("Images"));
    connect(images, SIGNAL(clicked()), this, SLOT(wakeUpLibrary()));

    TImageButton *selectionTools = new TImageButton(QPixmap(THEME_DIR + "icons/selection_big.png"), 60, this, true);
    selectionTools->setToolTip(tr("Selection Tools"));
    connect(selectionTools, SIGNAL(clicked()), this, SLOT(selectionTools()));

    TImageButton *undo = new TImageButton(QPixmap(THEME_DIR + "icons/undo_big.png"), 60, this, true);
    undo->setToolTip(tr("Undo"));
    connect(undo, SIGNAL(clicked()), this, SLOT(undo()));

    TImageButton *redo = new TImageButton(QPixmap(THEME_DIR + "icons/redo_big.png"), 60, this, true);
    redo->setToolTip(tr("Redo"));
    connect(redo, SIGNAL(clicked()), this, SLOT(redo()));

    TImageButton *trash = new TImageButton(QPixmap(THEME_DIR + "icons/delete_big.png"), 60, this, true);
    trash->setToolTip(tr("Delete Selection"));
    connect(trash, SIGNAL(clicked()), this, SLOT(wakeUpDeleteSelection()));

    TImageButton *zoomIn = new TImageButton(QPixmap(THEME_DIR + "icons/zoom_in_big.png"), 60, this, true);
    zoomIn->setToolTip(tr("Zoom In"));
    connect(zoomIn, SIGNAL(clicked()), this, SLOT(wakeUpZoomIn()));

    TImageButton *zoomOut = new TImageButton(QPixmap(THEME_DIR + "icons/zoom_out_big.png"), 60, this, true);
    zoomOut->setToolTip(tr("Zoom Out"));
    connect(zoomOut, SIGNAL(clicked()), this, SLOT(wakeUpZoomOut()));

    TImageButton *shift = new TImageButton(QPixmap(THEME_DIR + "icons/hand_big.png"), 60, this, true);
    shift->setToolTip(tr("Shift"));
    connect(shift, SIGNAL(clicked()), this, SLOT(wakeUpShift()));

    TImageButton *penProperties = new TImageButton(QPixmap(THEME_DIR + "icons/color_palette_big.png"), 60, this, true);
    penProperties->setToolTip(tr("Pen Properties"));
    connect(penProperties, SIGNAL(clicked()), this, SLOT(penProperties()));

    TImageButton *exposure = new TImageButton(QPixmap(THEME_DIR + "icons/exposure_sheet_big.png"), 60, this, true);
    exposure->setToolTip(tr("Exposure Sheet"));
    connect(exposure, SIGNAL(clicked()), this, SLOT(exposureDialog()));

    QBoxLayout *controls = new QBoxLayout(QBoxLayout::TopToBottom);
    controls->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    controls->setContentsMargins(3, 10, 3, 3);
    controls->setSpacing(7);

    controls->addWidget(sketchTools);
    controls->addWidget(images);
    controls->addWidget(selectionTools);
    controls->addWidget(undo);
    controls->addWidget(redo);
    controls->addWidget(trash);
    controls->addWidget(zoomIn);
    controls->addWidget(zoomOut);
    controls->addWidget(shift);
    controls->addWidget(penProperties);
    controls->addWidget(exposure);

    QBoxLayout *infoLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    infoLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);

    // SQA: Experimental code
    // k->display = new TupInfoWidget(this);
    // connect(k->display, SIGNAL(closePanel()), this, SLOT(hideInfoWidget()));
    // infoLayout->addWidget(k->display);
    // k->display->hide();

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    if (k->hand == Right)
        layout->addLayout(controls);
    else
        layout->addLayout(infoLayout);

    layout->addWidget(graphicsView);

    if (k->hand == Left)
        layout->addLayout(controls);
    else
        layout->addLayout(infoLayout);

    setLayout(layout);
}

TupCanvas::~TupCanvas()
{
}

void TupCanvas::updateCursor(const QCursor &cursor)
{
    graphicsView->viewport()->setCursor(cursor);
}

void TupCanvas::closeEvent(QCloseEvent *event)
{
    delete graphicsView; 
    event->accept();
}

void TupCanvas::colorDialog(const QColor &current)
{
    emit closePenPropertiesMenu();
    k->propertiesMenuIsOpen = false;

    QColor color = QColorDialog::getColor(current, this);
    k->currentColor = color;
    emit updateColorFromFullScreen(color);
}

void TupCanvas::colorDialog()
{
    emit closePenPropertiesMenu();
    k->propertiesMenuIsOpen = false;

    QColor color = QColorDialog::getColor(k->currentColor, this);
    emit updateColorFromFullScreen(color);
}

void TupCanvas::sketchTools()
{
    if (k->selectionMenuIsOpen) {
        emit closeSelectionMenu();
        k->selectionMenuIsOpen = false;
    }

    if (k->propertiesMenuIsOpen) {
        emit closePenPropertiesMenu();
        k->propertiesMenuIsOpen = false;
    }

    if (!k->sketchMenuIsOpen) {
        QList<QString> toolsList;
        // toolsList << "PencilTool";
        toolsList << "PolyLineTool";
        toolsList << "EllipseTool";
        toolsList << "RectangleTool";

        TupToolsDialog *dialog = new TupToolsDialog(toolsList, this);
        connect(dialog, SIGNAL(callAction(int, int)), this, SIGNAL(callAction(int, int)));
        connect(dialog, SIGNAL(isClosed()), this, SLOT(updateSketchMenuState()));
        connect(this, SIGNAL(closeSketchMenu()), dialog, SLOT(close()));

        dialog->show();

        if (k->hand == Right)
            dialog->move(72, 0);
        else
            dialog->move(1072, 0);

        k->sketchMenuIsOpen = true;
    } else {
        emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PencilTool);
        emit closeSketchMenu();
        k->sketchMenuIsOpen = false;
    } 
}

void TupCanvas::selectionTools()
{
    if (k->sketchMenuIsOpen) {
        emit closeSketchMenu();
        k->sketchMenuIsOpen = false;
    }

    if (k->propertiesMenuIsOpen) {
        emit closePenPropertiesMenu();
        k->propertiesMenuIsOpen = false;
    }

    if (!k->selectionMenuIsOpen) {
        QList<QString> toolsList;
        // toolsList << "ObjectsTool";
        toolsList << "NodesTool";

        TupToolsDialog *dialog = new TupToolsDialog(toolsList, this);
        connect(dialog, SIGNAL(callAction(int, int)), this, SIGNAL(callAction(int, int)));
        connect(dialog, SIGNAL(isClosed()), this, SLOT(updateSelectionMenuState()));
        connect(this, SIGNAL(closeSelectionMenu()), dialog, SLOT(close()));

        dialog->show();

        if (k->hand == Right)
            dialog->move(72, 132);
        else
            dialog->move(1212, 132);

        k->selectionMenuIsOpen = true;
    } else {
        emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::ObjectsTool);
        emit closeSelectionMenu();
        k->selectionMenuIsOpen = false;
    }
}

void TupCanvas::penProperties()
{
    if (k->sketchMenuIsOpen) {
        emit closeSketchMenu();
        k->sketchMenuIsOpen = false;
    }

    if (k->selectionMenuIsOpen) {
        emit closeSelectionMenu();
        k->selectionMenuIsOpen = false;
    }

    if (!k->propertiesMenuIsOpen) {
        QList<QString> toolsList;
        toolsList << "PenSize";
        toolsList << "Opacity";

        TupToolsDialog *dialog = new TupToolsDialog(toolsList, this);
        connect(dialog, SIGNAL(openColorDialog()), this, SLOT(colorDialog()));
        connect(dialog, SIGNAL(openPenDialog()), this, SLOT(penDialog()));
        connect(dialog, SIGNAL(openOpacityDialog()), this, SLOT(opacityDialog()));
        connect(this, SIGNAL(closePenPropertiesMenu()), dialog, SLOT(close()));

        dialog->show();

        if (k->hand == Right)
            dialog->move(72, 610);
        else
            dialog->move(1182, 610);

        k->propertiesMenuIsOpen = true;
    } else {
        colorDialog();
        emit closeSelectionMenu();
        k->propertiesMenuIsOpen = false;
    }
}

void TupCanvas::penDialog()
{
    emit closePenPropertiesMenu();
    k->propertiesMenuIsOpen = false;

    QDesktopWidget desktop;
    TupPenDialog *dialog = new TupPenDialog(k->brushManager, this);
    connect(dialog, SIGNAL(updatePen(int)), this, SIGNAL(updatePenThicknessFromFullScreen(int)));

    QApplication::restoreOverrideCursor();

    dialog->show();
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                        (int) (desktop.screenGeometry().height() - dialog->height())/2);
}

void TupCanvas::opacityDialog()
{
    emit closePenPropertiesMenu();
    k->propertiesMenuIsOpen = false;

    QDesktopWidget desktop;
    TupOnionOpacityDialog *dialog = new TupOnionOpacityDialog(k->brushManager->penColor(), k->scene->opacity(), this);
    connect(dialog, SIGNAL(updateOpacity(double)), this, SLOT(setOnionOpacity(double)));

    QApplication::restoreOverrideCursor();

    dialog->show();
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                        (int) (desktop.screenGeometry().height() - dialog->height())/2);
}

void TupCanvas::setOnionOpacity(double opacity)
{
    k->scene->setOnionFactor(opacity);
    emit updateOnionOpacityFromFullScreen(opacity); 
}

void TupCanvas::exposureDialog()
{
    updateMenuStates();
    k->exposureDialogIsOpen = true;

    QDesktopWidget desktop;
    k->exposureDialog = new TupExposureDialog(k->project, k->scene->currentSceneIndex(), 
                                                      k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), 
                                                      k->isNetworked, k->onLineUsers, this);
    connect(k->exposureDialog, SIGNAL(goToFrame(int, int, int)), this, SIGNAL(goToFrame(int, int, int)));
    connect(k->exposureDialog, SIGNAL(goToScene(int)), this, SIGNAL(goToScene(int)));
    connect(k->exposureDialog, SIGNAL(callNewScene()), this, SLOT(createScene()));
    connect(k->exposureDialog, SIGNAL(callNewLayer(int, int)), this, SLOT(createLayer(int, int)));
    connect(k->exposureDialog, SIGNAL(callNewFrame(int, int, int, int)), this, SLOT(createFrame(int, int, int, int)));
    connect(k->exposureDialog, SIGNAL(windowHasBeenClosed()), this, SLOT(updateExposureDialogState()));

    QApplication::restoreOverrideCursor();

    k->exposureDialog->show();
    k->exposureDialog->move((int) (desktop.screenGeometry().width() - k->exposureDialog->width())/2 ,
                        (int) (desktop.screenGeometry().height() - k->exposureDialog->height())/2);
}

void TupCanvas::oneFrameBack()
{
    if (k->frameIndex > 0) {
        k->frameIndex--;
        emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameBack);
    }
}

void TupCanvas::oneFrameForward()
{
    k->frameIndex++;
    emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameForward);
}

/*
void TupCanvas::updateThickness(int value)
{
    emit updatePenThicknessFromFullScreen(value);
}
*/

void TupCanvas::wakeUpLibrary()
{
    updateMenuStates();

    QString graphicPath = QFileDialog::getOpenFileName (this, tr("Import a SVG file..."), QDir::homePath(),
                                                    tr("Vector") + " (*.svg *.png *.jpg *.jpeg *.gif)");
    if (graphicPath.isEmpty())
        return;

    QFile f(graphicPath);
    QFileInfo fileInfo(f);

    if (graphicPath.toLower().endsWith(".svg")) {
        QString tag = fileInfo.fileName();

        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            f.close();
            // int projectWidth = k->size.width();
            // int projectHeight = k->size.height();

            TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                        TupLibraryObject::Svg, TupProject::FRAMES_EDITION, data, QString(),
                                        k->scene->currentSceneIndex(), k->scene->currentLayerIndex(), k->scene->currentFrameIndex());
            emit requestTriggered(&request);
        }
    } else {
        QString symName = fileInfo.fileName();

        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            f.close();

            QPixmap *pixmap = new QPixmap(graphicPath);
            int picWidth = pixmap->width();
            int picHeight = pixmap->height();
            int projectWidth = k->size.width(); 
            int projectHeight = k->size.height();

            if (picWidth > projectWidth || picHeight > projectHeight) {
                QDesktopWidget desktop;
                QMessageBox msgBox;
                msgBox.setWindowTitle(tr("Information"));
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setText(tr("Image is bigger than workspace."));
                msgBox.setInformativeText(tr("Do you want to resize it?"));
                msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.show();
                msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2,
                            (int) (desktop.screenGeometry().height() - msgBox.height())/2);

                int answer = msgBox.exec();

                if (answer == QMessageBox::Yes) {
                    pixmap = new QPixmap();
                    QString extension = fileInfo.suffix().toUpper();
                    // QByteArray ba = extension.toAscii();
                    QByteArray ba = extension.toLatin1();
                    const char* ext = ba.data();
                    if (pixmap->loadFromData(data, ext)) {
                        QPixmap newpix;
                        if (picWidth > projectWidth)
                            newpix = QPixmap(pixmap->scaledToWidth(projectWidth, Qt::SmoothTransformation));
                        else
                            newpix = QPixmap(pixmap->scaledToHeight(projectHeight, Qt::SmoothTransformation));
                        QBuffer buffer(&data);
                        buffer.open(QIODevice::WriteOnly);
                        newpix.save(&buffer, ext);
                    }
                }
           }

           QString tag = symName;

           TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                                                               TupLibraryObject::Image, TupProject::FRAMES_EDITION, data, QString(),
                                                                               k->scene->currentSceneIndex(), k->scene->currentLayerIndex(), k->scene->currentFrameIndex());
           emit requestTriggered(&request);

           data.clear();
        }
    }
}

void TupCanvas::wakeUpDeleteSelection()
{
    updateMenuStates();
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::Delete);
}

void TupCanvas::wakeUpZoomIn()
{
    updateMenuStates();
    graphicsView->scale(1.3, 1.3);

    emit updateZoomFactorFromFullScreen(1.3);
}

void TupCanvas::wakeUpZoomOut()
{
    updateMenuStates();
    graphicsView->scale(0.7, 0.7);

    emit updateZoomFactorFromFullScreen(0.7);
}

void TupCanvas::wakeUpShift()
{
    updateMenuStates();
    emit callAction(TupToolPlugin::ZoomMenu, TupToolPlugin::ShiftTool);
}

void TupCanvas::undo()
{
    updateMenuStates();

    QAction *undo = kApp->findGlobalAction("undo");
    if (undo) 
        undo->trigger();
}

void TupCanvas::redo()
{
    updateMenuStates();

    QAction *redo = kApp->findGlobalAction("redo");
    if (redo) 
        redo->trigger();
}

void TupCanvas::updateSketchMenuState()
{
    k->sketchMenuIsOpen = false;
}

void TupCanvas::updateSelectionMenuState()
{
    k->selectionMenuIsOpen = false; 
}

void TupCanvas::updateMenuStates()
{
    if (k->sketchMenuIsOpen) {
        emit closeSketchMenu();
        k->sketchMenuIsOpen = false;
        return;
    }

    if (k->selectionMenuIsOpen) {
        emit closeSelectionMenu();
        k->selectionMenuIsOpen = false;
        return;
    }

    if (k->propertiesMenuIsOpen) {
        emit closePenPropertiesMenu();
        k->propertiesMenuIsOpen = false;
        return;
    }
}

void TupCanvas::showInfoWidget()
{
    // k->display->show();
}

void TupCanvas::hideInfoWidget()
{
    // k->display->hide();
}

void TupCanvas::updateOnLineUsers(const QStringList &onLineUsers)
{
    k->onLineUsers = onLineUsers;
    if (k->exposureDialogIsOpen)
        k->exposureDialog->updateUsersList(onLineUsers);
}

void TupCanvas::updateExposureDialogState()
{
    k->exposureDialogIsOpen = false;
}

void TupCanvas::createScene()
{
    int sceneIndex = k->project->scenesCount();

    TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Add, tr("Scene %1").arg(sceneIndex + 1));
    emit requestTriggered(&request);

    request = TupRequestBuilder::createLayerRequest(sceneIndex, 0, TupProjectRequest::Add, tr("Layer 1"));
    emit requestTriggered(&request);

    request = TupRequestBuilder::createFrameRequest(sceneIndex, 0, 0, TupProjectRequest::Add, tr("Frame 1"));
    emit requestTriggered(&request);

    request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
    emit localRequestTriggered(&request);
}

void TupCanvas::createLayer(int sceneIndex, int layerIndex)
{
    TupProjectRequest request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::Add, tr("Layer %1").arg(layerIndex + 1));
    emit requestTriggered(&request);

    // tError() << "TupCanvas::createLayer() - Creating layer at [ " << sceneIndex << ", " << layerIndex << " ]";

    int oneRow = k->scene->framesCount();
    for(int i=0; i<oneRow; i++) {
        request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, i, TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
        emit requestTriggered(&request);
    }

    request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, 0, TupProjectRequest::Select);
    // emit requestTriggered(&request);
    emit localRequestTriggered(&request);
}

void TupCanvas::createFrame(int sceneIndex, int layerIndex, int layersCount, int frameIndex)
{
    for(int i=0; i<layersCount; i++) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, i, frameIndex, TupProjectRequest::Add, tr("Frame %1").arg(frameIndex + 1));
        emit requestTriggered(&request);
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex, TupProjectRequest::Select);
    // emit requestTriggered(&request);
    emit localRequestTriggered(&request);
}
