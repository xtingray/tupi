/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
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

#include "tuppaintareastatus.h"
#include "tseparator.h"
#include "tdebug.h"
#include "tupviewdocument.h"
#include "tupglobal.h"
#include "tupbrushmanager.h"
#include "tupcolorwidget.h"
#include "tupbrushstatus.h"
#include "tuptoolstatus.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QObject>
#include <QLineEdit>

////////////////

struct TupPaintAreaStatus::Private
{
    TupViewDocument *viewDocument;

    QLineEdit *frameField; 
    QComboBox *zoom;
    QComboBox *rotation;
    QCheckBox *antialiasHint;
    TupBrushStatus *brushStatus;
    TupBrushStatus *bgStatus;
    TupToolStatus *toolStatus;
    qreal scaleFactor;
    int currentFrame;
};

TupPaintAreaStatus::TupPaintAreaStatus(TupViewDocument *parent) : QStatusBar(parent), k( new Private)
{
    setSizeGripEnabled(false);
    k->viewDocument = parent;
    k->scaleFactor = 100;
    k->currentFrame = 1;

    QWidget *frameContainer = new QWidget;
    frameContainer->setFixedWidth(70);
    QHBoxLayout *frameLayout = new QHBoxLayout(frameContainer);
    frameLayout->setSpacing(3);
    frameLayout->setMargin(1);
    QLabel *frameLabel = new QLabel("");
    frameLabel->setToolTip(tr("Current Frame"));
    QPixmap framePix(THEME_DIR + "icons/frame_number.png");
    frameLabel->setPixmap(framePix);

    k->frameField = new QLineEdit(frameContainer);
    k->frameField->setFixedWidth(40);
    k->frameField->setAlignment(Qt::AlignRight);
    k->frameField->setText(tr("1"));
    connect(k->frameField, SIGNAL(editingFinished()), this, SLOT(updateFramePointer()));

    frameLayout->addWidget(frameLabel);
    frameLayout->addWidget(k->frameField);

    addPermanentWidget(frameContainer);

    QWidget *zoomContainer = new QWidget;
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomContainer);
    zoomLayout->setSpacing(3);
    zoomLayout->setMargin(1);

    QLabel *zoomTool = new QLabel("");
    zoomTool->setToolTip(tr("Zoom"));
    QPixmap pix(THEME_DIR + "icons/zoom_small.png");
    zoomTool->setPixmap(pix);
    // zoomTool->setMaximumSize(15, 15);

    zoomLayout->addWidget(zoomTool);

    k->zoom = new QComboBox();
    k->zoom->setDuplicatesEnabled(false);
    k->zoom->setEditable(true);
    //k->zoom->setFocusPolicy(Qt::NoFocus);
    //k->zoom->setInsertPolicy(QComboBox::InsertBeforeCurrent);

    for (int i = 500; i >= 250; i-=50)
         k->zoom->addItem(QString::number(i), i);

    for (int i = 200; i >= 25; i-=25)
         k->zoom->addItem(QString::number(i), i);

    k->zoom->setCurrentIndex(10);

    k->zoom->setValidator(new QIntValidator(10, 200, this));
    zoomLayout->addWidget(k->zoom);
    zoomLayout->addWidget(new QLabel(tr("%")));

    addPermanentWidget(zoomContainer);

    connect(k->zoom, SIGNAL(activated(const QString &)), this, SLOT(applyZoom(const QString &)));

    QWidget *rotContainer = new QWidget;
    QHBoxLayout *rotLayout = new QHBoxLayout(rotContainer);
    rotLayout->setSpacing(3);
    rotLayout->setMargin(1);

    // rotLayout->addWidget(new QLabel(tr("Rotate")));

    QLabel *rotateLabel = new QLabel("");
    rotateLabel->setToolTip(tr("Rotate Workspace"));
    QPixmap rotatePix(THEME_DIR + "icons/rotate_workspace.png");
    rotateLabel->setPixmap(rotatePix);

    rotLayout->addWidget(rotateLabel);

    k->rotation = new QComboBox();
    k->rotation->setDuplicatesEnabled(false);
    k->rotation->setEditable(true);

    for (int i = 0; i < 360; i+=30)
         k->rotation->addItem(QString::number(i), i);

    k->rotation->setValidator(new QIntValidator(-360, 360, this));

    rotLayout->addWidget(k->rotation);

    addPermanentWidget(rotContainer);

    connect(k->rotation, SIGNAL(activated(const QString &)), this, SLOT(applyRotation(const QString &)));

    ///////

    k->antialiasHint = new QCheckBox;
    k->antialiasHint->setIcon(QIcon(QPixmap(THEME_DIR + "icons/antialiasing.png")));
    k->antialiasHint->setToolTip(tr("Antialiasing"));
    // k->antialiasHint->setFocusPolicy(Qt::NoFocus);
    k->antialiasHint->setCheckable(true);
    k->antialiasHint->setChecked(true);
    k->antialiasHint->setFixedWidth(36);
    k->antialiasHint->setFixedHeight(23);
    addPermanentWidget(k->antialiasHint);

    connect(k->antialiasHint, SIGNAL(clicked()), this, SLOT(selectAntialiasingHint()));

    k->bgStatus = new TupBrushStatus(tr("Background Color"), QPixmap(THEME_DIR + "icons/background_color.png"), true);
    k->bgStatus->setTooltip(tr("Click here to change background color"));
    addPermanentWidget(k->bgStatus);
    k->bgStatus->setColor(k->viewDocument->project()->bgColor());

    connect(k->bgStatus, SIGNAL(colorUpdated(const QColor)), this, SIGNAL(colorUpdated(const QColor)));

    k->brushStatus = new TupBrushStatus(tr("Brush Color"), QPixmap(THEME_DIR + "icons/brush_color.png"), false);
    k->brushStatus->setTooltip(tr("Click here to change brush color"));
    addPermanentWidget(k->brushStatus);

    connect(k->brushStatus, SIGNAL(colorRequested()), this, SIGNAL(colorRequested())); 

    //connect(k->antialiasHint, SIGNAL(toggled(bool)), this, SLOT(selectAntialiasingHint(bool)));
    //connect(k->antialiasHint, SIGNAL(clicked()), this, SLOT(selectAntialiasingHint(bool)));

    k->brushStatus->setForeground(k->viewDocument->brushManager()->pen());

    k->toolStatus = new TupToolStatus;
    addPermanentWidget(k->toolStatus);
}

TupPaintAreaStatus::~TupPaintAreaStatus()
{
    delete k;
}

void TupPaintAreaStatus::selectAntialiasingHint()
{
    k->viewDocument->setAntialiasing(k->antialiasHint->isChecked()); 
}

void TupPaintAreaStatus::selectRenderer(int id)
{
  Q_UNUSED(id);

  /*
    Tupi::RenderType type = Tupi::RenderType(k->renderer->itemData(id ).toInt());

    if (type == Tupi::OpenGL)
        k->viewDocument->setOpenGL(true);
    else
        k->viewDocument->setOpenGL(false);
   */
}

void TupPaintAreaStatus::setPen(const QPen &pen)
{
    k->brushStatus->setForeground(pen);
}

void TupPaintAreaStatus::applyRotation(const QString & text)
{
    int angle = text.toInt();

    if (angle < 0)
        angle += 360;

    k->viewDocument->setRotationAngle(angle);
}

void TupPaintAreaStatus::applyZoom(const QString &text)
{
    int input = text.toInt();
    qreal factor = (qreal)input / (qreal)k->scaleFactor;

    k->viewDocument->setZoom(factor);
    k->scaleFactor = input;
}

void TupPaintAreaStatus::setZoomFactor(const QString &text)
{
    updateZoomField(text);
    applyZoom(text);
}

void TupPaintAreaStatus::updateZoomField(const QString &text)
{
    int index = k->zoom->findText(text);
    if (index != -1)
        k->zoom->setCurrentIndex(index);
    else
        k->zoom->setEditText(text);
}

qreal TupPaintAreaStatus::currentZoomFactor()
{
    return k->scaleFactor;
}

void TupPaintAreaStatus::updateZoomFactor(double factor) 
{
    double newFactor = k->scaleFactor * factor;
    QString number = QString::number(newFactor);
    int index = number.indexOf(".");
    if (index != -1)
        number = number.left(index + 2);

    updateZoomField(number);

    k->scaleFactor = newFactor;
}

void TupPaintAreaStatus::updateTool(const QString &label, const QPixmap &pixmap)
{
    k->toolStatus->updateTooltip(label);
    k->toolStatus->updatePixmap(pixmap);
}

void TupPaintAreaStatus::updateFrameIndex(int index)
{
    index++;
    k->currentFrame = index;
    QString text = QString::number(index); 
    k->frameField->setText(text);
}

void TupPaintAreaStatus::updateFramePointer()
{
    QString text = k->frameField->text();

    if (text.length() == 0) {
        k->frameField->setText(QString::number(k->currentFrame));
        return;
    }

    bool ok = false;
    int index = text.toInt(&ok); 
   
    if (ok) {

        if (index < 1 || index > 999) {
            k->frameField->setText(QString::number(k->currentFrame));
            return;
        }

        if (k->currentFrame != index) {
            if (index <= k->viewDocument->currentFramesTotal()) {
                k->currentFrame = index;
                index--;
                if (index >= 0)
                    emit newFramePointer(index);
            } else {
                k->frameField->setText(QString::number(k->currentFrame));
            }
        }
    } else {
        k->frameField->setText(QString::number(k->currentFrame));
    }
}

void TupPaintAreaStatus::setBgColor(QColor color)
{
    k->bgStatus->setColor(color);
}

