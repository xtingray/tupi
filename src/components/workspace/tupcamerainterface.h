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

#ifndef TUPCAMERAINTERFACE_H
#define TUPCAMERAINTERFACE_H

#include "tglobal.h"
#include "tupcamerawindow.h"
#include "tupapplication.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "talgorithm.h"
#include "tosd.h"
#include "tupcolorwidget.h"

#include <QFrame>
#include <QCloseEvent>
#include <QComboBox>
#include <QBoxLayout>
#include <QIcon>
#include <QDir>
#include <QDesktopWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QCamera>
#include <QCameraImageCapture>

class TUPI_EXPORT TupCameraInterface : public QFrame
{
    Q_OBJECT

    public:
        TupCameraInterface(const QString &title, QList<QByteArray> cameraDevices, QComboBox *devicesCombo, int cameraIndex, 
                           const QSize cameraSize = QSize(), int counter = 1, QWidget *parent = 0);
        ~TupCameraInterface();

    protected:
        void closeEvent(QCloseEvent *event);

    signals:
        void pictureHasBeenSelected(int id, const QString path);

    private slots:
        void changeCameraDevice(int index);
        void takePicture();
        void drawGrid();
        void drawActionSafeArea();
        void showHistory();
        void updateImagesOpacity(double opacity);
        void updateImagesDepth(int depth);
        void updateGridSpacing(int space);
        void updateColour();

    private:
        QSize setBestResolution(QList<QSize> resolutions, QSize cameraSize);
        QString randomPath();
        struct Private;
        Private *const k;
};

#endif
