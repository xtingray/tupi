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

#ifndef TUPSCREEN_H
#define TUPSCREEN_H

#include "tupscene.h"
#include "tupglobal.h"
#include "tupabstractprojectresponsehandler.h"

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QFrame>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupScreen : public QFrame, public TupAbstractProjectResponseHandler
{
    Q_OBJECT

    public:
        TupScreen(const TupProject *project, const QSize viewSize = QSize(), bool isScaled = false, QWidget *parent = 0);
        ~TupScreen();

        QSize sizeHint() const;
        void setLoop(bool l);
        void updateSceneIndex(int index);
        TupScene *currentScene() const;
        int currentSceneIndex();
        void setFPS(int fps);
        void resetPhotograms(int sceneIndex);
        void updateAnimationArea();
        void updateProjectDimension(const QSize dimension);
        // void updatePhotograms(int sceneIndex);

    public slots:
        virtual void render();
        virtual void play();
        virtual void playBack();
        virtual void stop();
        virtual void nextFrame();
        virtual void previousFrame();

    private slots:
        void advance();
        void back();

    protected:
        void frameResponse(TupFrameResponse *event);
        void layerResponse(TupLayerResponse *event);
        void sceneResponse(TupSceneResponse *event);
        void projectResponse(TupProjectResponse *event);
        void itemResponse(TupItemResponse *event);
        void libraryResponse(TupLibraryResponse *request);

    signals:
        void progressStep(int, int);
        void toStatusBar(const QString &, int);
        void sceneChanged(const TupScene *newScene);
        void requestTriggered(const TupProjectRequest *event);

    protected:
        void paintEvent(QPaintEvent *event);
        void resizeEvent(QResizeEvent *event);

    private:
        void initPhotogramsArray();
        void addPhotogramsArray(int index);
        void updateFirstFrame();
        struct Private;
        Private *const k;
};

#endif
