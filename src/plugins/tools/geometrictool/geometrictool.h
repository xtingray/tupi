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

#ifndef AGEOMETRICTOOLPLUGIN_H
#define AGEOMETRICTOOLPLUGIN_H

#include <QObject>
#include <QLabel>
#include <QKeyEvent>

#include <kttoolplugin.h>
#include <ktrectitem.h>
#include <ktellipseitem.h>
#include <ktlineitem.h>

#include "infopanel.h"

/**
 * @author David Cuadrado
*/

class GeometricTool : public KTToolPlugin
{
    Q_OBJECT
    
    public:
        GeometricTool();
        ~GeometricTool();
        
        virtual QStringList keys() const;
        virtual void init(KTGraphicsScene *scene);
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);

        virtual QMap<QString, TAction *> actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        void aboutToChangeScene(KTGraphicsScene *scene);
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        
    private:
        void setupActions();
        
    private:
        KTRectItem *m_rect;
        KTEllipseItem *m_ellipse;
        KTLineItem *m_line;

        InfoPanel *m_configurator;

        bool added;
        QPointF firstPoint;

        QMap<QString, TAction *> m_actions;
        bool proportion;
        
        QGraphicsItem *m_item;
};

#endif
