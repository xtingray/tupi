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

#include "tuptoolsdialog.h"
#include "tapplicationproperties.h"
#include "tuptoolplugin.h"
#include "timagebutton.h"
#include "tdebug.h"

#include <QHBoxLayout>
#include <QPixmap>

struct TupToolsDialog::Private
{
    QHBoxLayout *layout;
};

TupToolsDialog::TupToolsDialog(QList<QString> tools, QWidget *parent) : QDialog(parent), k(new Private)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);
    k->layout = new QHBoxLayout(this);
    setToolsPanel(tools);
}

TupToolsDialog::~TupToolsDialog()
{
}

void TupToolsDialog::setToolsPanel(QList<QString> tools)
{
    for(int i=0; i < tools.size(); i++) {
        if (tools.at(i).compare("PencilTool") == 0) {
            TImageButton *pencil = new TImageButton(QPixmap(THEME_DIR + "icons/pencil_big.png"), 60, this, true);
            pencil->setToolTip(tr("Pencil"));
            connect(pencil, SIGNAL(clicked()), this, SLOT(wakeUpPencil()));

            k->layout->addWidget(pencil);
        }

        if (tools.at(i).compare("InkTool") == 0) {
            TImageButton *ink = new TImageButton(QPixmap(THEME_DIR + "icons/ink_big.png"), 60, this, true);
            ink->setToolTip(tr("Ink"));
            connect(ink, SIGNAL(clicked()), this, SLOT(wakeUpInk()));

            k->layout->addWidget(ink);
        }

        if (tools.at(i).compare("PolyLineTool") == 0) {
            TImageButton *polyline = new TImageButton(QPixmap(THEME_DIR + "icons/polyline_big.png"), 60, this, true);
            polyline->setToolTip(tr("Polyline"));
            connect(polyline, SIGNAL(clicked()), this, SLOT(wakeUpPolyline()));

            k->layout->addWidget(polyline);
        }

        if (tools.at(i).compare("EllipseTool") == 0) {
            TImageButton *ellipse = new TImageButton(QPixmap(THEME_DIR + "icons/ellipse_big.png"), 60, this, true);
            ellipse->setToolTip(tr("Ellipse"));
            connect(ellipse, SIGNAL(clicked()), this, SLOT(wakeUpEllipse()));

            k->layout->addWidget(ellipse);
        }

        if (tools.at(i).compare("RectangleTool") == 0) {
            TImageButton *rectangle = new TImageButton(QPixmap(THEME_DIR + "icons/square_big.png"), 60, this, true);
            rectangle->setToolTip(tr("Rectangle"));
            connect(rectangle, SIGNAL(clicked()), this, SLOT(wakeUpRectangle()));

            k->layout->addWidget(rectangle);
        }

        if (tools.at(i).compare("ObjectsTool") == 0) {
            TImageButton *objects = new TImageButton(QPixmap(THEME_DIR + "icons/selection_big.png"), 60, this, true);
            objects->setToolTip(tr("Object Selection"));
            connect(objects, SIGNAL(clicked()), this, SLOT(wakeUpObjectSelection()));

            k->layout->addWidget(objects);
        }


        if (tools.at(i).compare("NodesTool") == 0) {
            TImageButton *nodes = new TImageButton(QPixmap(THEME_DIR + "icons/nodes_big.png"), 60, this, true);
            nodes->setToolTip(tr("Line Selection"));
            connect(nodes, SIGNAL(clicked()), this, SLOT(wakeUpNodeSelection()));

            k->layout->addWidget(nodes);
        }

        if (tools.at(i).compare("ColorTool") == 0) {
            TImageButton *colors = new TImageButton(QPixmap(THEME_DIR + "icons/color_palette_big.png"), 60, this, true);
            colors->setToolTip(tr("Color Palette"));
            connect(colors, SIGNAL(clicked()), this, SIGNAL(openColorDialog()));

            k->layout->addWidget(colors);
        }

        if (tools.at(i).compare("PenSize") == 0) {
            TImageButton *pen = new TImageButton(QPixmap(THEME_DIR + "icons/pen_properties.png"), 40, this, true);
            pen->setToolTip(tr("Pen Size"));
            connect(pen, SIGNAL(clicked()), this, SIGNAL(openPenDialog()));

            k->layout->addWidget(pen);
        }

        if (tools.at(i).compare("Opacity") == 0) {
            TImageButton *opacity = new TImageButton(QPixmap(THEME_DIR + "icons/onion_big.png"), 40, this, true);
            opacity->setToolTip(tr("Opacity Value"));
            connect(opacity, SIGNAL(clicked()), this, SIGNAL(openOpacityDialog()));

            k->layout->addWidget(opacity);
        }
    }
}

void TupToolsDialog::wakeUpPencil()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PencilTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpInk()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::InkTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpEllipse()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::EllipseTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpRectangle()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::RectangleTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpPolyline()
{
    emit callAction(TupToolPlugin::BrushesMenu, TupToolPlugin::PolyLineTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpObjectSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::ObjectsTool);
    emit isClosed();
    close();
}

void TupToolsDialog::wakeUpNodeSelection()
{
    emit callAction(TupToolPlugin::SelectionMenu, TupToolPlugin::NodesTool);
    emit isClosed();
    close();
}



