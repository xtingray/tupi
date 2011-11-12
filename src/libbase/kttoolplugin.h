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

#ifndef KTTOOLPLUGIN_H
#define KTTOOLPLUGIN_H

#include "kttoolinterface.h"
#include "ktglobal.h"
#include "ktprojectresponse.h"

#include <QObject>

class KTGraphicsScene;
class QGraphicsView;
class KTInputDeviceInformation;
class KTBrushManager;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT KTToolPlugin : public QObject, public KTToolInterface
{
    Q_OBJECT
    Q_INTERFACES(KTToolInterface);
    
    public:

        enum MenuIndex { Brushes = 0, Selection, Fill, Zoom, Tweener };
        enum BrushesTool { Pencil = 0, Scheme, Ink, Eraser, PolyLine, Line, Rectangle, Ellipse, Text };

        explicit KTToolPlugin(QObject *parent = 0);
        ~KTToolPlugin();
        
        void setName(const QString &tool);
        QString name() const;
        
        virtual void init(KTGraphicsScene *scene);
        virtual void updateScene(KTGraphicsScene *scene);
        
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene) = 0;
        virtual void doubleClick(const KTInputDeviceInformation *input, KTGraphicsScene *scene);
        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene) = 0;
        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene) = 0;
        
        virtual QMap<QString, TAction *> actions() const = 0;
        
        virtual QWidget *configurator()  = 0;
        virtual void aboutToChangeTool() = 0;
        virtual void saveConfig() = 0;
        virtual void aboutToChangeScene(KTGraphicsScene *scene);

        virtual void itemResponse(const KTItemResponse *event);
        virtual void frameResponse(const KTFrameResponse *event);
        virtual void layerResponse(const KTLayerResponse *event);
        virtual void sceneResponse(const KTSceneResponse *event);

        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

        virtual QMenu *menu() const;
        virtual QCursor cursor() const;
        
    signals:
        void requested(const KTProjectRequest *request);
        
    public:
        // Events
        void begin();
        void end();
        
    private:
        struct Private;
        Private *const k;
};

#endif

