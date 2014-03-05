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

#include <QFrame>
#include <QCloseEvent>
#include <QComboBox>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

class TupCameraInterface : public QFrame
{
    Q_OBJECT

    public:
        TupCameraInterface(const QString &title, QComboBox *devicesCombo, int cameraIndex, QCamera *camera = 0, const QSize maxCameraSize = QSize(), 
                           QCameraImageCapture *imageCapture = 0, const QString &path = QString(), QWidget *parent = 0);
        ~TupCameraInterface();

    protected:
        void closeEvent(QCloseEvent *event);

    signals:
        void projectSizeHasChanged(const QSize size);
        void pictureHasBeenSelected(int id, const QString path);

    private slots:
        void takePicture();
        void cameraError(QCamera::Error error);
        void imageSavedFromCamera(int id, const QString path);
        void changeCameraDevice(const QString &cameraDesc);

    private:
        struct Private;
        Private *const k;
};

#endif
