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
#include "tapplicationproperties.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"

#include <QLabel>
#include <QComboBox>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QDir>

struct TupCameraDialog::Private
{
    QSize projectSize;
    QSize cameraSize;
    bool resizeProject;
    QList<QSize> resolutions;
};

TupCameraDialog::TupCameraDialog(const QSize projectSize, QList<QSize> resolutions, QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Camera Resolution"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "photo.png")));

    k->projectSize = projectSize;
    k->resolutions = resolutions;
    k->resizeProject = false;

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QLabel *resolutionLabel = new QLabel(tr("Please, choose the resolution you want to work with:"));
    QComboBox *resolutionCombo = new QComboBox();
    for (int i=0; i<k->resolutions.size(); i++) {
         QSize size = k->resolutions.at(i);
         QString label = QString::number(size.width()) + "x" + QString::number(size.height());
         resolutionCombo->addItem(label);
    }
    resolutionCombo->setCurrentIndex(k->resolutions.size() - 1);
    k->cameraSize = k->resolutions.at(k->resolutions.size() - 1);

    connect(resolutionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setCameraResolution(int)));

    QCheckBox *resizeCheck = new QCheckBox(tr("Resize my project to fit camera resolution"));
    connect(resizeCheck, SIGNAL(toggled(bool)), this, SLOT(projectSizeHasChanged(bool)));

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
    layout->addWidget(resolutionCombo);
    layout->addWidget(resizeCheck);

    layout->addLayout(buttonLayout);
}

TupCameraDialog::~TupCameraDialog()
{
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

