/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#ifndef AGEOMETRICTOOLPLUGIN_H
#define AGEOMETRICTOOLPLUGIN_H

#include "tuptoolplugin.h"
#include "tuprectitem.h"
#include "tupellipseitem.h"
#include "tuplineitem.h"
#include "infopanel.h"

#include <QObject>
#include <QLabel>
#include <QKeyEvent>

/**
 * @author David Cuadrado
*/

class GeometricTool : public TupToolPlugin
{
    Q_OBJECT
    
    public:
        GeometricTool();
        ~GeometricTool();
        
        virtual QStringList keys() const;
        virtual void init(TupGraphicsScene *scene);
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        virtual void doubleClick(const TupInputDeviceInformation *input, TupGraphicsScene *scene);

        virtual void sceneResponse(const TupSceneResponse *event);
        virtual void layerResponse(const TupLayerResponse *event);
        virtual void frameResponse(const TupFrameResponse *event);

        virtual QMap<QString, TAction *> actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        void aboutToChangeScene(TupGraphicsScene *scene);
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        virtual QCursor cursor() const;
        void updatePos(QPointF pos);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);
        
    private:
        void setupActions();

    private:
        void endItem();
        struct Private;
        Private *const k;

    /*
    private:
        TupRectItem *m_rect;
        TupEllipseItem *m_ellipse;
        TupLineItem *m_line;
        InfoPanel *m_configurator;
        bool added;
        QPointF firstPoint;
        QMap<QString, TAction *> m_actions;
        bool proportion;
        QGraphicsItem *m_item;
    */
};

#endif
