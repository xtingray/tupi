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

#include "kttoolplugin.h"
#include "configurator.h"
#include "tweenerpanel.h"
#include "ktprojectresponse.h"

/**
 * @author Gustav Gonzalez 
 *
*/

class Tweener : public KTToolPlugin
{
    Q_OBJECT

    public:
        Tweener();
        virtual ~Tweener();
        virtual void init(KTGraphicsScene *scene);

        virtual QStringList keys() const;
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void updateScene(KTGraphicsScene *scene);
        virtual QMap<QString, TAction *>actions() const;
        virtual QWidget *configurator();
        virtual void aboutToChangeTool();
        virtual void saveConfig();

        int toolType() const;
        void aboutToChangeScene(KTGraphicsScene *scene);
        // bool isComplete() const;

        virtual void sceneResponse(const KTSceneResponse *event);
        virtual void layerResponse(const KTLayerResponse *event);
        virtual void frameResponse(const KTFrameResponse *event);

    private:
        int framesTotal();
        void setupActions();
        int maxZValue();
        void clearSelection();
        void removeTweenFromProject(const QString &name);
        void disableSelection();

    private:
        struct Private;
        Private *const k;

    private slots:
        void updateCurrentTweenerType(TweenerPanel::TweenerType type);
        void applyReset();
        void applyTween();
        void removeTween(const QString &name);
        void setCreatePath();
        void setSelect();
        void setEditEnv();
        void updateMode(TweenerPanel::Mode mode);
        void updateStartPoint(int index);
        void setCurrentTween(const QString &name);
        void setPath(bool isEnabled, bool reset);
        void tweenListMode();

    public slots:
        void updatePath();
};

#endif
