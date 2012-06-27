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

#ifndef TUPPAINTAREA_H
#define TUPPAINTAREA_H

#include "tuppaintareabase.h"
#include "tupabstractprojectresponsehandler.h"
#include "tupgraphicsscene.h"

// #include <QTabletEvent>

class QGraphicsRectItem;
class TupBrushManager;
class TupInputDeviceInformation;
class TupProject;
class TupPaintAreaRotator;

/**
 * This class provides an area to make different kind of traces
 * @author Jorge Cuadrado - David Cuadrado
*/

class TupPaintArea : public TupPaintAreaBase, public TupAbstractProjectResponseHandler
{
    Q_OBJECT

    public:
        enum MoveItemType { MoveBack, MoveFront, MoveBackwards, MoveForwards };

        TupPaintArea(TupProject *project, QWidget * parent = 0);
        ~TupPaintArea();

        void setCurrentScene(int index);
        void setCurrentTool(QString tool);
        void updatePaintArea();
        void paintBackground();
        void updateSpaceContext();
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        void goOneFrameBack();
        void goOneFrameForward();
        void goToFrame(int frameIndex, int layerIndex, int sceneIndex);
        void goToScene(int sceneIndex);
        void copyCurrentFrame();
        void pasteDataOnCurrentFrame();
        void quickCopy();

    public slots:
        void setNextFramesOnionSkinCount(int n);
        void setPreviousFramesOnionSkinCount(int n);
        void setOnionFactor(double value);

    signals:
        void requestTriggered(const TupProjectRequest *event);
        void itemAddedOnSelection(TupGraphicsScene *);
        void frameChanged(int);
        void closePolyLine();
        void updateStatusBgColor(const QColor color);

    public slots:
        void deleteItems();
        void groupItems();
        void ungroupItems();
        void copyItems();
        void pasteItems();
        void cutItems();

        void addSelectedItemsToLibrary();
        void requestMoveSelectedItems(QAction *action);
        void goToFrame(int index);

    protected:
        void mousePressEvent(QMouseEvent *event);
        // void tabletEvent(QTabletEvent *event);

        void frameResponse(TupFrameResponse *event);
        void layerResponse(TupLayerResponse *event);
        void sceneResponse(TupSceneResponse *event);
        void itemResponse(TupItemResponse *event);
        void libraryResponse(TupLibraryResponse *request);
        void projectResponse(TupProjectResponse *projectResponse);

        bool canPaint() const;

    private:
        struct Private;
        Private *const k;
};

#endif
