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

#ifndef TUPTOOLPLUGIN_H
#define TUPTOOLPLUGIN_H

#include "tuptoolinterface.h"
#include "tupglobal.h"
#include "tupprojectresponse.h"

#include <QObject>
#include <QSize>

class TupGraphicsScene;
class QGraphicsView;
class TupInputDeviceInformation;
class TupBrushManager;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupToolPlugin : public QObject, public TupToolInterface
{
    Q_OBJECT
    Q_INTERFACES(TupToolInterface);
    
    public:

        enum MenuIndex { InvalidMenu = -1, BrushesMenu = 0, SelectionMenu, FillMenu, ZoomMenu, TweenerMenu, Arrows, ColorMenu };
        enum BrushTools { InvalidBrush = -1, PencilTool = 0, InkTool, EraserTool, PolyLineTool, LineTool, RectangleTool, EllipseTool, TextTool, FrameBack, FrameForward, QuickCopy, Delete };
        enum SelectTools { InvalidSelection = -1, NodesTool = 0, ObjectsTool };
        enum FillTools { InvalidFill = -1, InsideTool = 0, ContourTool };
        enum ViewTools { InvalidView = -1, ZoomInTool = 0, ZoomOutTool, HandTool };
        enum ColorTools { InvalidColor = -1, ColorTool = 0 };

        enum Mode { Add = 1, Edit, View };
        enum EditMode { None, Selection = 0, Path, Properties };

        explicit TupToolPlugin(QObject *parent = 0);
        ~TupToolPlugin();
        
        void setName(const QString &tool);
        QString name() const;
        
        virtual void init(TupGraphicsScene *scene);
        virtual void updateScene(TupGraphicsScene *scene);
        
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene) = 0;
        virtual void doubleClick(const TupInputDeviceInformation *input, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene) = 0;
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene) = 0;
        
        virtual QMap<QString, TAction *> actions() const = 0;
        
        virtual QWidget *configurator()  = 0;
        virtual void aboutToChangeTool() = 0;
        virtual void saveConfig() = 0;
        virtual void aboutToChangeScene(TupGraphicsScene *scene);

        virtual void itemResponse(const TupItemResponse *event);
        virtual void frameResponse(const TupFrameResponse *event);
        virtual void layerResponse(const TupLayerResponse *event);
        virtual void sceneResponse(const TupSceneResponse *event);

        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

        virtual void updatePos(QPointF pos);

        virtual QMenu *menu() const;
        virtual QCursor cursor() const;
        QPair<int, int> setKeyAction(int key, Qt::KeyboardModifiers modifiers);

        virtual void resizeNodes(qreal factor);
        virtual void updateZoomFactor(qreal factor);

        virtual void autoZoom();
        virtual void setProjectSize(const QSize size);

        virtual TupToolPlugin::Mode currentMode();
        virtual TupToolPlugin::EditMode currentEditMode();
        
    signals:
        void requested(const TupProjectRequest *request);
        
    public:
        // Events
        void begin();
        void end();
        
    private:
        struct Private;
        Private *const k;
};

#endif

