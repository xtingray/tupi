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
#include <QPushButton>

struct TupCanvas::Private
{
    QColor currentColor;
    QLabel *penWidth;
    QLabel *frame;
    QLabel *layer;
    QLabel *scene;
    int frameIndex;
    int layerIndex;
    int sceneIndex;
};

TupCanvas::TupCanvas(QWidget *parent, Qt::WindowFlags flags, TupGraphicsScene *scene, 
                   const QPointF centerPoint, const QSize &screenSize, const QSize &projectSize, double scaleFactor,
                   int angle, const QColor &bg, TupBrushManager *brushManager) : QFrame(parent, flags), k(new Private)
{
    setWindowTitle(tr("Tupi: 2D Magic"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")));

    k->currentColor = brushManager->penColor();

    graphicsView = new TupCanvasView(this, screenSize, projectSize, bg);
    // graphicsView->setFixedSize(screenSize.width(), screenSize.height());

    graphicsView->setScene(scene);
    graphicsView->centerOn(centerPoint);
    graphicsView->scale(scaleFactor, scaleFactor);
    graphicsView->rotate(angle);

    TImageButton *pencil = new TImageButton(QPixmap(THEME_DIR + "icons/pencil_big.png"), 40, this, true);
    pencil->setToolTip(tr("Pencil"));
    connect(pencil, SIGNAL(clicked()), this, SLOT(wakeUpPencil()));

    QPushButton *circle = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/ellipse.png")), "");
    circle->setToolTip(tr("Ellipse"));
    connect(circle, SIGNAL(clicked()), this, SLOT(wakeUpCircle()));

    QPushButton *square = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/square.png")), "");
    square->setToolTip(tr("Rectangle"));
    connect(square, SIGNAL(clicked()), this, SLOT(wakeUpSquare()));

    QPushButton *polyline = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/polyline.png")), "");
    polyline->setToolTip(tr("PolyLine"));
    connect(polyline, SIGNAL(clicked()), this, SLOT(wakeUpPolyline()));

    TImageButton *objects = new TImageButton(QPixmap(THEME_DIR + "icons/selection_big.png"), 40, this, true);
    objects->setToolTip(tr("Object Selection"));
    connect(objects, SIGNAL(clicked()), this, SLOT(wakeUpObjectSelection()));

    TImageButton *nodes = new TImageButton(QPixmap(THEME_DIR + "icons/nodes_big.png"), 40, this, true);
    nodes->setToolTip(tr("Line Selection"));
    connect(nodes, SIGNAL(clicked()), this, SLOT(wakeUpNodeSelection()));

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

    QAction *colors = new QAction(QIcon(QPixmap(THEME_DIR + "icons/color_palette_big.png")), tr("Colors"), this);
    colors->setToolTip(tr("Color Palette"));
    connect(colors, SIGNAL(triggered()), this, SLOT(colorDialog()));

    QToolBar *toolbar = new QToolBar(tr("Tools"));
    toolbar->setOrientation(Qt::Vertical);
    toolbar->setIconSize(QSize(30, 30));
    toolbar->setFixedHeight(30);
    toolbar->addAction(colors);

    QSlider *thickness = new QSlider(Qt::Vertical);
    thickness->setFixedHeight(200);
    thickness->setRange(1, 100);
    thickness->setSingleStep(1);
    thickness->setValue(brushManager->penWidth());

    connect(thickness, SIGNAL(valueChanged(int)), this, SLOT(updateThickness(int)));

    QBoxLayout *slider = new QBoxLayout(QBoxLayout::TopToBottom);
    slider->setAlignment(Qt::AlignHCenter);
    slider->setContentsMargins(2, 2, 2, 2);
    slider->addWidget(thickness, Qt::AlignHCenter);

    k->penWidth = new QLabel(QString::number(brushManager->penWidth()));
    k->penWidth->setFont(QFont("Arial", 10, QFont::Bold));
    k->penWidth->setAlignment(Qt::AlignHCenter);
    k->penWidth->setFixedHeight(20);

    TImageButton *forward = new TImageButton(QPixmap(THEME_DIR + "icons/ff_big.png"), 40, this, true);
    forward->setToolTip(tr("One Frame Forward"));
    connect(forward, SIGNAL(clicked()), this, SLOT(oneFrameForward()));

    TImageButton *backward = new TImageButton(QPixmap(THEME_DIR + "icons/rw_big.png"), 40, this, true);
    backward->setToolTip(tr("One Frame Backward"));
    connect(backward, SIGNAL(clicked()), this, SLOT(oneFrameBack()));

    k->frameIndex = scene->currentFrameIndex();
    k->frame = new QLabel(tr("F: ") + QString::number(k->frameIndex));
    k->frame->setFont(QFont("Arial", 10, QFont::Bold));
    k->frame->setAlignment(Qt::AlignHCenter);
    k->frame->setFixedHeight(20);

    k->layerIndex = scene->currentLayerIndex();
    k->layer = new QLabel(tr("L: ") + QString::number(k->layerIndex));
    k->layer->setFont(QFont("Arial", 10, QFont::Bold));
    k->layer->setAlignment(Qt::AlignHCenter);
    k->layer->setFixedHeight(20);

    k->sceneIndex = scene->currentSceneIndex();
    k->scene = new QLabel(tr("S: ") + QString::number(k->sceneIndex));
    k->scene->setFont(QFont("Arial", 10, QFont::Bold));
    k->scene->setAlignment(Qt::AlignHCenter);
    k->scene->setFixedHeight(20);

    QBoxLayout *controls = new QBoxLayout(QBoxLayout::TopToBottom);
    controls->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    controls->setContentsMargins(10, 5, 1, 1);
    controls->setSpacing(2);

    controls->addWidget(pencil, Qt::AlignHCenter);
    //controls->addWidget(circle);
    //controls->addWidget(square);
    //controls->addWidget(polyline);
    controls->addWidget(objects, Qt::AlignHCenter);
    controls->addWidget(nodes, Qt::AlignHCenter);
    controls->addWidget(zoomIn, Qt::AlignHCenter);
    controls->addWidget(zoomOut, Qt::AlignHCenter);
    controls->addWidget(hand, Qt::AlignHCenter);
    controls->addWidget(undo, Qt::AlignHCenter);
    controls->addWidget(redo, Qt::AlignHCenter);

    controls->addSpacing(5);

    controls->addWidget(toolbar, Qt::AlignHCenter);
    controls->addLayout(slider, Qt::AlignHCenter);
    controls->addWidget(k->penWidth, Qt::AlignHCenter);

    controls->addSpacing(5);

    controls->addWidget(forward);
    controls->addWidget(backward);
    controls->addWidget(k->frame);
    controls->addWidget(k->layer);
    controls->addWidget(k->scene);

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

void TupCanvas::oneFrameBack()
{
    if (k->frameIndex > 0) {
        k->frameIndex--;
        k->frame->setText(tr("F: ") + QString::number(k->frameIndex));
        emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameBack);
    }
}

void TupCanvas::oneFrameForward()
{
    k->frameIndex++;
    k->frame->setText(tr("F: ") + QString::number(k->frameIndex));
    emit callAction(TupToolPlugin::Arrows, TupToolPlugin::FrameForward);
}

void TupCanvas::updateThickness(int value)
{
    k->penWidth->setText(QString::number(value));
    emit updatePenThicknessFromFullScreen(value);
}

void TupCanvas::wakeUpPencil()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PencilTool);
}

void TupCanvas::wakeUpCircle()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::EllipseTool);
}

void TupCanvas::wakeUpSquare()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::RectangleTool);
}

void TupCanvas::wakeUpPolyline()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PolyLineTool);
}

void TupCanvas::wakeUpObjectSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::ObjectsTool);
}

void TupCanvas::wakeUpNodeSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::NodesTool);
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

