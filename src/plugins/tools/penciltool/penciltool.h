/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "kttoolplugin.h"
#include "exactnessconfigurator.h"
#include "ktpathitem.h"

#include <QObject>
#include <QSpinBox>
#include <QTimer>

class QKeySequence;

/**
 * @author David Cuadrado
*/

class PencilTool : public KTToolPlugin
{
    Q_OBJECT
    
    public:
        PencilTool();
        virtual ~PencilTool();
        
        virtual void init(KTGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual QMap<QString, TAction *>actions() const;
        int toolType() const;
        virtual QWidget *configurator();
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        virtual void keyPressEvent(QKeyEvent *event);
        virtual QCursor cursor() const;
        
    private:
        void setupActions();
        void smoothPath(QPainterPath &path, double smoothness, int from = 0, int to = -1);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);

    private:
        struct Private;
        Private *const k;

    /*
    private:
        QPointF m_firstPoint;
        QPointF m_oldPos;
        QPainterPath m_path;
        ExactnessConfigurator * m_configurator;
        QMap<QString, TAction *> m_actions;
        KTPathItem *m_item;
        QCursor m_cursor;
    */
};

#endif
