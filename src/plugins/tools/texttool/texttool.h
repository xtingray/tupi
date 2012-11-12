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

#ifndef TEXTTOOL_H
#define TEXTTOOL_H

#include "tuptoolplugin.h"
#include "tuptextitem.h"
#include "textconfigurator.h"
#include "tupbrushmanager.h"

/**
 * @author David Cuadrado
*/
class TextTool : public TupToolPlugin
{
    Q_OBJECT
    
    public:
        TextTool();
        virtual ~TextTool();
        
        virtual QStringList keys() const;
        
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void doubleClick(const TupInputDeviceInformation *input, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual bool itemPressed(QGraphicsItem *item);
        virtual QMap<QString, TAction *>actions() const;
        int toolType() const;
        virtual QWidget *configurator();
        void aboutToChangeScene(TupGraphicsScene *scene);
        void aboutToChangeTool();
        virtual void saveConfig();
        virtual void keyPressEvent(QKeyEvent *event);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);
 
    private:
        void setupActions();
        
    private:
        TupTextItem *m_item;
        TextConfigurator *m_configurator;
        QPoint m_position;
        QMap<QString, TAction *> m_actions;
};

#endif
