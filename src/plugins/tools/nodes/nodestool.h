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

#ifndef NODESTOOL_H
#define NODESTOOL_H

#include "tglobal.h"
#include "tuppathitem.h"
#include "tcontrolnode.h"
#include "tnodegroup.h"
#include "tuptoolplugin.h"
#include "taction.h"
#include "talgorithm.h"
#include "tnodegroup.h"
#include "tupgraphicalgorithm.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupserializer.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"
#include "tuprequestbuilder.h"
#include "tupproxyitem.h"
#include "tuplibraryobject.h"
#include "tupitemgroup.h"
#include "tosd.h"
#include "tupgraphiclibraryitem.h"
#include "tuppixmapitem.h"

#include <QObject>
#include <QSpinBox>
#include <QGraphicsView>
#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QDir>

class TupItemResponse;
class TupGraphicsScene;

/**
 * @author Jorge Cuadrado
 */

class TUPI_PLUGIN NodesTool : public TupToolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.maefloresta.tupi.TupToolInterface" FILE "nodestool.json")
    
    public:
        NodesTool();
        virtual ~NodesTool();
        
        virtual void init(TupGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        
        virtual QMap<QString, TAction *>actions() const;
        int toolType() const;
        virtual QWidget *configurator();
        void aboutToChangeScene(TupGraphicsScene *scene);
        virtual void aboutToChangeTool();
        
        virtual void itemResponse(const TupItemResponse *event);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void saveConfig();
        virtual QCursor cursor() const;

        void resizeNodes(qreal scaleFactor);
        void updateZoomFactor(qreal scaleFactor);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);
 
    private:
        // void reset(TupGraphicsScene *scene);
        void setupActions();
        TupFrame* currentFrame();
        
    private:
        struct Private;
        Private *const k;
};

#endif

