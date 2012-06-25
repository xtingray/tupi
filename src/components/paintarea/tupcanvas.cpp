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
 *   the Free Software Foundation; either version 3 of the License, or     *
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
#include "tupapplication.h"
#include "tapplicationproperties.h"
#include "tuptoolplugin.h"
#include "timagebutton.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"
#include "tuppendialog.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QGraphicsView>
#include <QColorDialog>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QFont>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QBuffer>

struct TupCanvas::Private
{
    QColor currentColor;
    TupBrushManager *brushManager;
    int frameIndex;
    int layerIndex;
    int sceneIndex;
    QSize size;
    TupGraphicsScene *scene;
};

TupCanvas::TupCanvas(QWidget *parent, Qt::WindowFlags flags, TupGraphicsScene *scene, 
                   const QPointF centerPoint, const QSize &screenSize, const QSize &projectSize, double scaleFactor,
                   int angle, const QColor &bg, TupBrushManager *brushManager) : QFrame(parent, flags), k(new Private)
{
    setWindowTitle(tr("Tupi: 2D Magic"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")));

    k->scene = scene;
    k->size = projectSize;
    k->currentColor = brushManager->penColor();
    k->brushManager = brushManager;

    graphicsView = new TupCanvasView(this, screenSize, projectSize, bg);

    graphicsView->setScene(scene);
    graphicsView->centerOn(centerPoint);
    graphicsView->scale(scaleFactor, scaleFactor);
    graphicsView->rotate(angle);

    TImageButton *pencil = new TImageButton(QPixmap(THEME_DIR + "icons/pencil_big.png"), 40, this, true);
    pencil->setToolTip(tr("Pencil"));
    connect(pencil, SIGNAL(clicked()), this, SLOT(wakeUpPencil()));

    TImageButton *ink = new TImageButton(QPixmap(THEME_DIR + "icons/ink_big.png"), 40, this, true);
    ink->setToolTip(tr("Ink"));
    connect(ink, SIGNAL(clicked()), this, SLOT(wakeUpInk()));

    /*
    TImageButton *polyline = new TImageButton(QPixmap(THEME_DIR + "icons/polyline_big.png"), 40, this, true);
    polyline->setToolTip(tr("Polyline"));
    connect(polyline, SIGNAL(clicked()), this, SLOT(wakeUpPolyline()));
    */

    TImageButton *ellipse = new TImageButton(QPixmap(THEME_DIR + "icons/ellipse_big.png"), 40, this, true);
    ellipse->setToolTip(tr("Ellipse"));
    connect(ellipse, SIGNAL(clicked()), this, SLOT(wakeUpEllipse()));

    TImageButton *rectangle = new TImageButton(QPixmap(THEME_DIR + "icons/square_big.png"), 40, this, true);
    rectangle->setToolTip(tr("Rectangle"));
    connect(rectangle, SIGNAL(clicked()), this, SLOT(wakeUpRectangle()));

    TImageButton *images = new TImageButton(QPixmap(THEME_DIR + "icons/bitmap_big.png"), 40, this, true);
    images->setToolTip(tr("Images"));
    connect(images, SIGNAL(clicked()), this, SLOT(wakeUpLibrary()));

    TImageButton *objects = new TImageButton(QPixmap(THEME_DIR + "icons/selection_big.png"), 40, this, true);
    objects->setToolTip(tr("Object Selection"));
    connect(objects, SIGNAL(clicked()), this, SLOT(wakeUpObjectSelection()));

    TImageButton *nodes = new TImageButton(QPixmap(THEME_DIR + "icons/nodes_big.png"), 40, this, true);
    nodes->setToolTip(tr("Line Selection"));
    connect(nodes, SIGNAL(clicked()), this, SLOT(wakeUpNodeSelection()));

    TImageButton *trash = new TImageButton(QPixmap(THEME_DIR + "icons/delete_big.png"), 40, this, true);
    trash->setToolTip(tr("Delete Selection"));
    connect(trash, SIGNAL(clicked()), this, SLOT(wakeUpDeleteSelection()));

    TImageButton *zoomIn = new TImageButton(QPixmap(THEME_DIR + "icons/zoom_in_big.png"), 40, this, true);
    zoomIn->setToolTip(tr("Zoom In"));
    connect(zoomIn, SIGNAL(clicked()), this, SLOT(wakeUpZoomIn()));

    TImageButton *zoomOut = new TImageButton(QPixmap(THEME_DIR + "icons/zoom_out_big.png"), 40, this, true);
    zoomOut->setToolTip(tr("Zoom Out"));
    connect(zoomOut, SIGNAL(clicked()), this, SLOT(wakeUpZoomOut()));

    TImageButton *hand = new TImageButton(QPixmap(THEME_DIR + "icons/hand_big.png"), 40, this, true);
    hand->setToolTip(tr("Hand"));
    connect(hand, SIGNAL(clicked()), this, SLOT(wakeUpHand()));

    TImageButton *undo = new TImageButton(QPixmap(THEME_DIR + "icons/undo_big.png"), 40, this, true);
    undo->setToolTip(tr("Undo"));
    connect(undo, SIGNAL(clicked()), this, SLOT(undo()));

    TImageButton *redo = new TImageButton(QPixmap(THEME_DIR + "icons/redo_big.png"), 40, this, true);
    redo->setToolTip(tr("Redo"));
    connect(redo, SIGNAL(clicked()), this, SLOT(redo()));

    TImageButton *colors = new TImageButton(QPixmap(THEME_DIR + "icons/color_palette_big.png"), 40, this, true);
    colors->setToolTip(tr("Color Palette"));
    connect(colors, SIGNAL(clicked()), this, SLOT(colorDialog()));

    TImageButton *pen = new TImageButton(QPixmap(THEME_DIR + "icons/pen_properties.png"), 40, this, true);
    pen->setToolTip(tr("Pen Size"));
    connect(pen, SIGNAL(clicked()), this, SLOT(penDialog()));

    TImageButton *exposure = new TImageButton(QPixmap(THEME_DIR + "icons/exposure_sheet_big.png"), 40, this, true);
    exposure->setToolTip(tr("Exposure Sheet"));
    connect(exposure, SIGNAL(clicked()), this, SLOT(penDialog()));

    QBoxLayout *controls = new QBoxLayout(QBoxLayout::TopToBottom);
    controls->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    controls->setContentsMargins(3, 3, 3, 3);
    controls->setSpacing(7);

    controls->addWidget(pencil);
    controls->addWidget(ink);
    // controls->addWidget(polyline);
    controls->addWidget(ellipse);
    controls->addWidget(rectangle);
    controls->addWidget(images);
    controls->addWidget(objects);
    controls->addWidget(nodes);
    controls->addWidget(trash);
    controls->addWidget(zoomIn);
    controls->addWidget(zoomOut);
    controls->addWidget(hand);

    controls->addWidget(undo);
    controls->addWidget(redo);
    controls->addWidget(colors);
    controls->addWidget(pen);
    controls->addWidget(exposure);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addLayout(controls);
    layout->addWidget(graphicsView);

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
    event->accept();
}

void TupCanvas::colorDialog(const QColor &current)
{
    QColor color = QColorDialog::getColor(current, this);
    k->currentColor = color;
    emit updateColorFromFullScreen(color);
}

void TupCanvas::colorDialog()
{
    QColor color = QColorDialog::getColor(k->currentColor, this);
    emit updateColorFromFullScreen(color);
}

void TupCanvas::penDialog()
{
    QDesktopWidget desktop;
    TupPenDialog *dialog = new TupPenDialog(k->brushManager, this);
    connect(dialog, SIGNAL(updatePen(int)), this, SLOT(updateThickness(int)));

    QApplication::restoreOverrideCursor();

    dialog->show();
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                        (int) (desktop.screenGeometry().height() - dialog->height())/2);
}

void TupCanvas::oneFrameBack()
{
    if (k->frameIndex > 0) {
        k->frameIndex--;
        // k->frame->setText(tr("F: ") + QString::number(k->frameIndex));
        emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameBack);
    }
}

void TupCanvas::oneFrameForward()
{
    k->frameIndex++;
    // k->frame->setText(tr("F: ") + QString::number(k->frameIndex));
    emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameForward);
}

void TupCanvas::updateThickness(int value)
{
    emit updatePenThicknessFromFullScreen(value);
}

void TupCanvas::wakeUpPencil()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PencilTool);
}

void TupCanvas::wakeUpInk()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::InkTool);
}

void TupCanvas::wakeUpEllipse()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::EllipseTool);
}

void TupCanvas::wakeUpRectangle()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::RectangleTool);
}

void TupCanvas::wakeUpLibrary()
{
    QString graphicPath = QFileDialog::getOpenFileName (this, tr("Import a SVG file..."), QDir::homePath(),
                                                    tr("Vectorial") + " (*.svg *.png *.jpg *.jpeg *.gif)");
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
                    QByteArray ba = extension.toAscii();
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

/*
void TupCanvas::wakeUpPolyline()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PolyLineTool);
}
*/

void TupCanvas::wakeUpObjectSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::ObjectsTool);
}

void TupCanvas::wakeUpNodeSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::NodesTool);
}

void TupCanvas::wakeUpDeleteSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::Delete);
}

void TupCanvas::wakeUpZoomIn()
{
    emit callAction(TupToolPlugin::ZoomMenu, TupToolPlugin::ZoomInTool);
}

void TupCanvas::wakeUpZoomOut()
{
    emit callAction(TupToolPlugin::ZoomMenu, TupToolPlugin::ZoomOutTool);
}

void TupCanvas::wakeUpHand()
{
    emit callAction(TupToolPlugin::ZoomMenu, TupToolPlugin::HandTool);
}

void TupCanvas::undo()
{
    QAction *undo = kApp->findGlobalAction("undo");
    if (undo) 
        undo->trigger();
}

void TupCanvas::redo()
{
    QAction *redo = kApp->findGlobalAction("redo");
    if (redo) 
        redo->trigger();
}

