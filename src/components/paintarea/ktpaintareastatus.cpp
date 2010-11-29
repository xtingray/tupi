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

#include "ktpaintareastatus.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QObject>

#include "kseparator.h"
#include "kdebug.h"

#include "ktviewdocument.h"
#include "ktglobal.h"
#include "ktbrushmanager.h"
#include "ktcolorwidget.h"
#include "ktbrushstatus.h"
#include "kttoolstatus.h"

////////////////

struct KTPaintAreaStatus::Private
{
    KTViewDocument *viewDocument;
    QComboBox *zoom;
    QComboBox *rotation;
    QCheckBox *antialiasHint;
    KTBrushStatus *brushStatus;
    KTToolStatus *toolStatus;
    qreal scaleFactor;
};

KTPaintAreaStatus::KTPaintAreaStatus(KTViewDocument *parent) : QStatusBar(parent), k( new Private)
{
    setSizeGripEnabled(false);
    k->viewDocument = parent;
    k->scaleFactor = 100;

    QWidget *zoomContainer = new QWidget;
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomContainer);
    zoomLayout->setSpacing(3);
    zoomLayout->setMargin(1);

    zoomLayout->addWidget(new QLabel(tr("Zoom")));

    k->zoom = new QComboBox();
    k->zoom->setDuplicatesEnabled(false);
    k->zoom->setEditable(true);
    k->zoom->setFocusPolicy(Qt::NoFocus);
    //k->zoom->setInsertPolicy(QComboBox::InsertBeforeCurrent);

    for (int i = 200; i >= 25; i-=25)
         k->zoom->addItem(QString::number(i), i);

    k->zoom->setCurrentIndex(4);

    k->zoom->setValidator(new QIntValidator(10, 200,this));
    zoomLayout->addWidget(k->zoom);
    zoomLayout->addWidget(new QLabel(tr("%")));

    addPermanentWidget(zoomContainer);

    connect(k->zoom, SIGNAL(activated(const QString &)), this, SLOT(applyZoom(const QString &)));

    QWidget *rotContainer = new QWidget;
    QHBoxLayout *rotLayout = new QHBoxLayout(rotContainer);
    rotLayout->setSpacing(3);
    rotLayout->setMargin(1);

    rotLayout->addWidget(new QLabel(tr("Rotate")));

    k->rotation = new QComboBox();
    k->rotation->setDuplicatesEnabled(false);
    k->rotation->setEditable(true);

    for (int i = 0; i < 360; i+=30)
         k->rotation->addItem(QString::number(i), i);

    k->rotation->setValidator(new QIntValidator(-360, 360,this));

    rotLayout->addWidget(k->rotation);

    addPermanentWidget(rotContainer);

    connect(k->rotation, SIGNAL(activated(const QString &)), this, SLOT(applyRotation(const QString &)));

    ///////

    k->antialiasHint = new QCheckBox(tr("Antialiasing"));
    k->antialiasHint->setFocusPolicy(Qt::NoFocus);
    k->antialiasHint->setCheckable(true);
    k->antialiasHint->setChecked(true);
    k->antialiasHint->setFixedHeight(22);
    addPermanentWidget(k->antialiasHint);

    connect(k->antialiasHint, SIGNAL(clicked()), this, SLOT(selectAntialiasingHint()));

    k->brushStatus = new KTBrushStatus;
    addPermanentWidget(k->brushStatus);

    //connect(k->antialiasHint, SIGNAL(toggled(bool)), this, SLOT(selectAntialiasingHint(bool)));
    //connect(k->antialiasHint, SIGNAL(clicked()), this, SLOT(selectAntialiasingHint(bool)));

    k->brushStatus->setForeground(k->viewDocument->brushManager()->pen());

    k->toolStatus = new KTToolStatus;
    addPermanentWidget(k->toolStatus);
}

KTPaintAreaStatus::~KTPaintAreaStatus()
{
    delete k;
}

void KTPaintAreaStatus::selectAntialiasingHint()
{
    k->viewDocument->setAntialiasing(k->antialiasHint->isChecked()); 
}

void KTPaintAreaStatus::selectRenderer(int id)
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

void KTPaintAreaStatus::setPen(const QPen &pen)
{
    k->brushStatus->setForeground(pen);
}

void KTPaintAreaStatus::applyRotation(const QString & text)
{
    int angle = text.toInt();

    if (angle < 0)
        angle += 360;

    k->viewDocument->setRotationAngle(angle);
}

void KTPaintAreaStatus::applyZoom(const QString & text)
{
    int input = text.toInt();

    qreal factor = (qreal)input / (qreal)k->scaleFactor;

    k->viewDocument->setZoom(factor);
    k->scaleFactor = input;
}

void KTPaintAreaStatus::updateTool(const QString &label, const QPixmap &pixmap)
{
    k->toolStatus->updateTooltip(label);
    k->toolStatus->updatePixmap(pixmap);
}

