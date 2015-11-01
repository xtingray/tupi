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

#ifndef FILLTOOLPLUGIN_H
#define FILLTOOLPLUGIN_H

#include "tglobal.h"
#include "tupsvgitem.h"
#include "tupserializer.h"
#include "tupitemconverter.h"
#include "tuprequestbuilder.h"
#include "tupscene.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupbrushmanager.h"
#include "tupgraphiclibraryitem.h"
#include "tuptoolplugin.h"
#include "tupitemgroup.h"
#include "tosd.h"

#include <QObject>
#include <QLabel>
#include <QKeySequence>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

// class TupPathItem;

/**
 * @author David Cuadrado
*/

class TUPI_PLUGIN FillTool : public TupToolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.maefloresta.tupi.TupToolInterface" FILE "filltool.json")
    
    public:
        FillTool();
        ~FillTool();
        
        virtual QStringList keys() const;

        void init(TupGraphicsScene *scene);
        
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, 
                    TupGraphicsScene *scene);

        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, 
                    TupGraphicsScene *scene);

        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, 
                    TupGraphicsScene *scene);
        
        TupPathItem *itemPressed(QGraphicsItem *item, const TupBrushManager *brush);
        
        virtual QMap<QString, TAction *> actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        void aboutToChangeScene(TupGraphicsScene *scene);
        virtual void aboutToChangeTool();
        
        QPainterPath mapPath(const QPainterPath &path, const QPointF &pos);
        QPainterPath mapPath(const QGraphicsPathItem *item);

        virtual void saveConfig();
        virtual void keyPressEvent(QKeyEvent *event);
        virtual QCursor cursor() const;

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);
 
    private:
        void setupActions();

    private:
        struct Private;
        Private *const k;
 
    /*       
    private:
        QMap<QString, TAction *> m_actions;
        TupGraphicsScene *m_scene;
    */
};

#endif
