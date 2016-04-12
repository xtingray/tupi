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

#ifndef TUPGRAPHICSSCENE_H
#define TUPGRAPHICSSCENE_H

#include "tglobal.h"
#include "tupsvgitem.h"
#include "tupproject.h"
#include "tupprojectresponse.h"
#include "tupinputdeviceinformation.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QSvgRenderer>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QDesktopWidget>

/**
 * @author David Cuadrado
*/

class TupFrame;
class TupGraphicObject;
class TupScene;
class TupToolPlugin;
class TupBrushManager;
class QMouseEvent;
// class TupItemResponse;
// class TupLayerResponse;

class TUPI_EXPORT TupGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

    public:
        enum Context { Current = 1, Previous, Next };
        enum WorkSpace { Animation, Player };

        TupGraphicsScene();
        ~TupGraphicsScene();
        
        void setCurrentFrame(int layer, int frame);
        
        void setCurrentScene(TupScene *scene);
        void drawCurrentPhotogram();
        
        void drawPhotogram(int photogram, WorkSpace space);
        void drawSceneBackground(int photogram);

        void cleanWorkSpace();
        void removeScene();
        int currentFrameIndex() const;
        int currentLayerIndex() const;
        int currentSceneIndex() const;
        
        void setNextOnionSkinCount(int n);
        void setPreviousOnionSkinCount(int n);
        
        void updateLayerVisibility(int layerIndex, bool visible);
        
        TupScene *scene() const;
        
        TupFrame *currentFrame();
        
        void setTool(TupToolPlugin *tool);
        TupToolPlugin *currentTool() const;
        
        bool isDrawing() const;
        
        TupBrushManager *brushManager() const;
        
        void itemResponse(TupItemResponse *event);
        void frameResponse(TupFrameResponse *event);
        void layerResponse(TupLayerResponse *event);
        void sceneResponse(TupSceneResponse *event);
        
        void mouseMoved(QGraphicsSceneMouseEvent *event);
        void mouseReleased(QGraphicsSceneMouseEvent *event);

        void setSelectionRange();
        void enableItemsForSelection();

        void includeObject(QGraphicsItem *object, bool isPolyLine = false);

        TupProject::Mode spaceContext();
        void setSpaceMode(TupProject::Mode mode);

        void setOnionFactor(double opacity);
        double opacity();

        int framesCount();
        void setLibrary(TupLibrary *library);
        void resetCurrentTool(); 

        TupInputDeviceInformation * inputDeviceInformation();

    // private slots:
    //  void updateObjectInformation(const QString &value);
    //  void showInfoWidget();
    //  void updateAliveObject();

    signals:
        void showInfoWidget();

    private:
        void addBackgroundFrame(TupFrame *frame);
        void addBackgroundGraphicObject(TupGraphicObject *object, double opacity);
        void addBackgroundSvgObject(TupSvgItem *svgItem, double opacity); 

        void addFrame(TupFrame *frame);
        void addGraphicObject(TupGraphicObject *object, bool tweenInAdvance = false);
        void addSvgObject(TupSvgItem *svgItem, bool tweenInAdvance = false);

        void addTweeningObjects(int indexLayer, int photogram);
        void addSvgTweeningObjects(int indexLayer, int photogram);
        void addLipSyncObjects(TupLayer *layer, int photogram, int zLevel);

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void keyPressEvent(QKeyEvent *keyEvent);
        virtual void keyReleaseEvent(QKeyEvent *keyEvent);
       
        /* 
        virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
        virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
        virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
        virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
        */
        
        virtual bool event(QEvent *e);

    private:
        struct Private;
        Private *const k;
};

#endif
