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

#ifndef KTVIEWCAMERA_H
#define KTVIEWCAMERA_H

#include "kcirclebuttonbar.h"
#include "kvhbox.h"
#include "ktanimationarea.h"
#include "ktcamerabar.h"
#include "ktcamerastatus.h"

#include <QMainWindow>
#include <QFrame>

class KTProjectResponse;
class QCheckBox;
class KTCameraStatus;

/**
 * @author David Cuadrado 
*/

class KTViewCamera : public QFrame
{
    Q_OBJECT

    public:
        KTViewCamera(KTProject *work, bool isNetworked = false, QWidget *parent = 0);
        ~KTViewCamera();

        void updateFirstFrame();
        QSize sizeHint() const;
        void updateScenes(int sceneIndex);

    private slots:
        void setLoop();
        void selectScene(int index);

    public slots:
        bool handleProjectResponse(KTProjectResponse *event);
        void setFPS(int fps);
        void updateFramesTotal();
        void exportDialog();
        void postDialog();
        void doPlay();
        void doPlayBack();
        void doStop();
        void nextFrame();
        void previousFrame();

    signals:
        void requestTriggered(const KTProjectRequest *event);
        void requestForExportVideoToServer(const QString &title, const QString &description, int fps, const QList<int> indexes);

    private:
        struct Private;
        Private *const k;
};

#endif
