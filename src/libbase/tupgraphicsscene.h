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

#include "tupglobal.h"
#include "tupsvgitem.h"
#include "tupproject.h"
#include "tupprojectresponse.h"

#include <QGraphicsScene>

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
        enum Context { Current = 1, Preview, Next };

        TupGraphicsScene();
        ~TupGraphicsScene();
        
        void setCurrentFrame(int layer, int frame);
        
        void setCurrentScene(TupScene *scene);
        void drawCurrentPhotogram();
        
        void drawPhotogram(int photogram, bool drawContext);
        void drawBackground();

        void cleanWorkSpace();
        void removeScene();
        int currentFrameIndex() const;
        int currentLayerIndex() const;
        int currentSceneIndex() const;
        
        void setNextOnionSkinCount(int n);
        void setPreviousOnionSkinCount(int n);
        
        void setLayerVisible(int layerIndex, bool visible);
        
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

        virtual void aboutToMousePress();

        void includeObject(QGraphicsItem *object);

        TupProject::Mode spaceMode();
        void setSpaceMode(TupProject::Mode mode);

        void setOnionFactor(double opacity);
        double opacity();

        int framesTotal();

    // private slots:
    //  void updateObjectInformation(const QString &value);
    //  void showInfoWidget();
    //  void updateAliveObject();

    signals:
        void showInfoWidget();

    private:
        void addFrame(TupFrame *frame, double opacity = 1.0, Context mode = Current);
        void addGraphicObject(TupGraphicObject *object, double opacity = 1.0);
        void addSvgObject(TupSvgItem *svgItem, double opacity = 1.0);
        void addTweeningObjects(int photogram);
        void addSvgTweeningObjects(int photogram);

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
