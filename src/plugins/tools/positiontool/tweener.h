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

#ifndef POSITIONTWEENER_H
#define POSITIONTWEENER_H

#include "tuptoolplugin.h"
#include "settings.h"
#include "tupprojectresponse.h"

/**
 * @author Jorge Cuadrado
 * 
*/

class Tweener : public TupToolPlugin
{
    Q_OBJECT

    public:
        Tweener();
        virtual ~Tweener();
        virtual void init(TupGraphicsScene *scene);

        virtual QStringList keys() const;
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void updateScene(TupGraphicsScene *scene);
        virtual QMap<QString, TAction *>actions() const;
        virtual QWidget *configurator();
        virtual void aboutToChangeTool();
        virtual void saveConfig();

        int toolType() const;
        void aboutToChangeScene(TupGraphicsScene *scene);
        bool isComplete() const;

        virtual void sceneResponse(const TupSceneResponse *event);
        virtual void layerResponse(const TupLayerResponse *event);
        virtual void frameResponse(const TupFrameResponse *event);

        virtual TupToolPlugin::Mode currentMode();
        virtual TupToolPlugin::EditMode currentEditMode();

    private:
        int framesTotal();
        void setupActions();
        QString pathToCoords();
        int maxZValue();
        void clearSelection();
        void disableSelection();
        void removeTweenFromProject(const QString &name);

    private:
        struct Private;
        Private *const k;

    private slots:
        void applyReset();
        void applyTween();
        void removeTween(const QString &name);
        void setCreatePath();
        void setSelect();
        void setEditEnv();
        void updateMode(TupToolPlugin::Mode mode);
        void updateStartPoint(int index);
        void setCurrentTween(const QString &name);

    public slots:
        void updatePath();
};

#endif
