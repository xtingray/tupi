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

#ifndef SCHEMETOOL_H
#define SCHEMETOOL_H

#include "tuptoolplugin.h"
#include "configurator.h"
#include "tuppathitem.h"

#include <QObject>
#include <QSpinBox>
#include <QTimer>

class QKeySequence;

/**
 * @author David Cuadrado
*/

class SchemeTool : public TupToolPlugin
{
    Q_OBJECT
    
    public:
        SchemeTool();
        virtual ~SchemeTool();
        
        virtual void init(TupGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual QMap<QString, TAction *>actions() const;
        int toolType() const;
        virtual QWidget *configurator();
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        virtual void keyPressEvent(QKeyEvent *event);
        virtual QCursor cursor() const;

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);
        
    private:
        void setupActions();
        void smoothPath(QPainterPath &path, double smoothness, int from = 0, int to = -1);

    private slots:
        void updateSpacingVar(int value);
        void updateSizeToleranceVar(int value);

    private:
        struct Private;
        Private *const k;

    /*        
    private:
        QPointF m_firstPoint;
        QPointF m_oldPos;
        QPointF previewPoint;
        QPointF oldPosRight;
        QPointF oldPosLeft;
        QPointF connector;
        QPainterPath m_path;
        QPainterPath pathRight; 
        QPainterPath pathLeft;
        Configurator *m_configurator;
        QMap<QString, TAction *> m_actions;
        TupPathItem *m_item;
        TupPathItem *itemRight;
        TupPathItem *itemLeft;
        int dotsCounter;
        qreal penWidth;
        qreal oldSlope;
        int spacing;
        qreal tolerance;
        qreal widthVar; 
        int arrowSize;
        int firstArrow;
    */
};

#endif
