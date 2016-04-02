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

#ifndef SELECTIONTOOL_H
#define SELECTIONTOOL_H

#include "tglobal.h"
#include "tuptoolplugin.h"
#include "tuppathitem.h"
#include "tupproject.h"
#include "tupgraphicsscene.h"
#include "settings.h"
#include "tupprojectresponse.h"

#include <QObject>
#include <QPointF>
#include <QKeySequence>
#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <cmath>

class TupItemResponse;
class NodeManager;

/**
 * @author Jorge Cuadrado
*/

class TUPI_PLUGIN SelectionTool : public TupToolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.maefloresta.tupi.TupToolInterface" FILE "selectiontool.json")
    
    public:
        SelectionTool();
        virtual ~SelectionTool();
        
        virtual void init(TupGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

        virtual void sceneResponse(const TupSceneResponse *event);

        virtual QMap<QString, TAction *>actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        
        virtual void aboutToChangeTool();
        void aboutToChangeScene(TupGraphicsScene *scene);
        
        virtual void itemResponse(const TupItemResponse *event);
        virtual void saveConfig();
        QCursor cursor() const;

        void resizeNodes(qreal scaleFactor);
        void updateZoomFactor(qreal scaleFactor);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);

    private slots:
        void initItems(TupGraphicsScene *scene);
        void syncNodes();
        void applyFlip(Settings::Flip flip);
        void applyOrderAction(Settings::Order order);
        void applyGroupAction(Settings::Group action);
        void updateItemPosition(int x, int y);

    private:
        void setupActions();
        bool selectionIsActive();
        void updateItemPosition();
        void removeTarget();
        TupFrame* currentFrame();
        TupFrame* frameAt(int sceneIndex, int layerIndex, int frameIndex);
        void requestTransformation(QGraphicsItem *item, TupFrame *frame);

        struct Private;
        Private *const k;
        Settings *panel;

};

#endif
