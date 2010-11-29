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

#ifndef FILLTOOLPLUGIN_H
#define FILLTOOLPLUGIN_H

#include <QObject>
#include <QLabel>

#include <kttoolplugin.h>

class KTPathItem;

/**
 * @author David Cuadrado <krawek@toonka.com>
*/

class FillTool : public KTToolPlugin
{
    Q_OBJECT;
    
    public:
        FillTool();
        ~FillTool();
        
        virtual QStringList keys() const;

        void init(KTGraphicsScene *scene);
        
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, 
                    KTGraphicsScene *scene);

        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, 
                    KTGraphicsScene *scene);

        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, 
                    KTGraphicsScene *scene);
        
        KTPathItem *itemPressed(QGraphicsItem *item, const KTBrushManager *brush);
        
        virtual QMap<QString, KAction *> actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        void aboutToChangeScene(KTGraphicsScene *scene);
        virtual void aboutToChangeTool();
        
        QPainterPath mapPath(const QPainterPath &path, const QPointF &pos);
        QPainterPath mapPath(const QGraphicsPathItem *item);

        virtual void saveConfig();
 
    private:
        void setupActions();
        
    private:
        QMap<QString, KAction *> m_actions;
};

#endif
