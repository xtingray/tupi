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

#ifndef POLYLINE_H
#define POLYLINE_H

#include "tuptoolplugin.h"
#include "tuppathitem.h"
#include "infopanel.h"
#include "tcontrolnode.h"
#include "tnodegroup.h"

/**
 * @author Jorge Cuadrado
 */

class PolyLine : public TupToolPlugin
{    
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.maefloresta.tupi.TupToolInterface" FILE "polylinetool.json")
    
    public:
        PolyLine();
        virtual ~PolyLine();
        
        virtual void init(TupGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual QMap<QString, TAction *>actions() const;
        int toolType() const;

        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        
        virtual void keyPressEvent(QKeyEvent *event);

        virtual QWidget *configurator();
        void aboutToChangeScene(TupGraphicsScene *scene);
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        virtual QCursor cursor() const;

    protected:
        virtual void itemResponse(const TupItemResponse *response);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);

    private slots:
        void nodeChanged();
        void initEnv();
        
    private:
        void setupActions();
        struct Private;
        Private * const k;
};

#endif
