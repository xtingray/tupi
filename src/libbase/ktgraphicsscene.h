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

#ifndef KTGRAPHICSSCENE_H
#define KTGRAPHICSSCENE_H

#include <QGraphicsScene>

#include "ktglobal.h"
#include "ktsvgitem.h"
#include "ktproject.h"

/**
 * @author David Cuadrado
*/

class KTFrame;
class KTGraphicObject;
class KTScene;
class KTToolPlugin;
class KTBrushManager;
class QMouseEvent;
class KTItemResponse;

class TUPI_EXPORT KTGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

    public:
        KTGraphicsScene();
        ~KTGraphicsScene();
        
        void setCurrentFrame(int layer, int frame);
        
        void setCurrentScene(KTScene *scene);
        void drawCurrentPhotogram();
        
        void drawPhotogram(int photogram);
        void drawBackground();

        void addFrame(KTFrame *frame, double opacity = 1.0);
        void addGraphicObject(KTGraphicObject *object, double opacity = 1.0);
        void addSvgObject(KTSvgItem *svgItem, double opacity = 1.0);
        
        void cleanWorkSpace();
        void removeScene();
        int currentFrameIndex() const;
        int currentLayerIndex() const;
        int currentSceneIndex() const;
        
        void setNextOnionSkinCount(int n);
        void setPreviousOnionSkinCount(int n);
        
        void setLayerVisible(int layerIndex, bool visible);
        
        KTScene *scene() const;
        
        KTFrame *currentFrame();
        
        void setTool(KTToolPlugin *tool);
        KTToolPlugin *currentTool() const;
        
        bool isDrawing() const;
        
        KTBrushManager *brushManager() const;
        
        void itemResponse(KTItemResponse *event);
        
        void mouseMoved(QGraphicsSceneMouseEvent *event);
        void mouseReleased(QGraphicsSceneMouseEvent *event);

        virtual void aboutToMousePress();

        void includeObject(QGraphicsItem *object);

        KTProject::Mode spaceMode();
        void setSpaceMode(KTProject::Mode mode);

    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        virtual void keyPressEvent(QKeyEvent *keyEvent);
       
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
