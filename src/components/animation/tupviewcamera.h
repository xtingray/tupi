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

#ifndef TUPVIEWCAMERA_H
#define TUPVIEWCAMERA_H

#include "tcirclebuttonbar.h"
#include "tvhbox.h"
#include "tupanimationarea.h"
#include "tupcamerabar.h"
#include "tupcamerastatus.h"

#include <QMainWindow>
#include <QFrame>

class TupProjectResponse;
class QCheckBox;
class TupCameraStatus;

/**
 * @author David Cuadrado 
*/

class TupViewCamera : public QFrame
{
    Q_OBJECT

    public:
        TupViewCamera(TupProject *work, bool isNetworked = false, QWidget *parent = 0);
        ~TupViewCamera();

        void updateFirstFrame();
        QSize sizeHint() const;
        void updateScenes(int sceneIndex);

    private slots:
        void setLoop();
        void selectScene(int index);

    public slots:
        bool handleProjectResponse(TupProjectResponse *event);
        void setFPS(int fps);
        void updateFramesTotal(int sceneIndex);
        void exportDialog();
        void postDialog();
        void doPlay();
        void doPlayBack();
        void doStop();
        void nextFrame();
        void previousFrame();

    signals:
        void requestTriggered(const TupProjectRequest *event);
        void requestForExportVideoToServer(const QString &title, const QString &topics, const QString &description, int fps, const QList<int> indexes);
        // void requestForExportStoryboardToServer(const QString &title, const QString &topics, const QString &description, const QList<int> indexes);

    private:
        struct Private;
        Private *const k;
};

#endif
