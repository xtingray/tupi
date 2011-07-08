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

#ifndef AANIMATIONAREA_H
#define AANIMATIONAREA_H

#include "ktscene.h"
#include "ktglobal.h"
#include "ktabstractprojectresponsehandler.h"

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QFrame>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT KTAnimationArea : public QFrame, public KTAbstractProjectResponseHandler
{
    Q_OBJECT

    public:
        KTAnimationArea(const KTProject *project, QWidget *parent = 0);
        ~KTAnimationArea();

        QSize sizeHint() const;
        void setLoop(bool l);
        void setCurrentScene(int index);
        void updateSceneIndex(int index);
        KTScene *currentScene() const;
        void setFPS(int fps);
        void refreshAnimation(const KTProject *project);

    private:
        void initAnimationArea();

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
        void frameResponse(KTFrameResponse *event);
        void layerResponse(KTLayerResponse *event);
        void sceneResponse(KTSceneResponse *event);
        void projectResponse(KTProjectResponse *event);
        void itemResponse(KTItemResponse *event);
        void libraryResponse(KTLibraryResponse *request);

    signals:
        void progressStep(int, int);
        void toStatusBar(const QString &, int);
        void sceneChanged(const KTScene *newScene );

        void requestTriggered(const KTProjectRequest *event);

    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent * event);

    private:
        struct Private;
        Private *const k;
};

#endif
