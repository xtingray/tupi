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

#include "erasertool.h"
#include "tglobal.h"
#include "tdebug.h"
#include "ktrectitem.h"
#include "ktellipseitem.h"
#include "ktlineitem.h"
#include "ktpathitem.h"
#include "ktitemconverter.h"
#include "ktscene.h"
#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktbrushmanager.h"

#include <QGraphicsView>
#include <QKeySequence>
#include <QDebug>
#include <QImage>
#include <QPaintDevice>

EraserTool::EraserTool()
{
    setupActions();
}

EraserTool::~EraserTool()
{
}

QStringList EraserTool::keys() const
{
    return QStringList() << tr("Eraser");
}

void EraserTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(THEME_DIR + "icons/eraser.png"), tr("Eraser"), this);
    action1->setShortcut(QKeySequence(tr("E")));
    action1->setCursor(QCursor(THEME_DIR + "cursors/eraser.png"));
    
    m_actions.insert(tr("Eraser"), action1);
   
    /* 
     TAction *action2 = new TAction(QIcon(THEME_DIR + "icons/ellipse.png"), tr("Ellipse"), this);
     action2->setShortcut(QKeySequence(tr("Ctrl+E")));
     action2->setCursor(QCursor(THEME_DIR + "cursors/circle.png"));
         
     m_actions.insert(tr("Ellipse"), action2);
         
         
     TAction *action3 = new TAction( QIcon(THEME_DIR+"/icons/line.png"), tr("Line"), this);
     action3->setShortcut( QKeySequence(tr("Ctrl+L")) );
     m_actions.insert(tr("Line"), action3);
    */
}

void EraserTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    if (input->buttons() == Qt::LeftButton) {
        QPointF pos = input->pos();
        
        if (name() == tr("Eraser")) {
            QList<QGraphicsItem *> items = scene->items(input->pos());
    
            if (items.count() > 0) {
                QGraphicsItem *itemPress = items[0];
                itemPressed(itemPress, brushManager, pos);
            }
        }
    }
}

void EraserTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
}

void EraserTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
}

QMap<QString, TAction *> EraserTool::actions() const
{
    return m_actions;
}

int EraserTool::toolType() const
{
    return Brush;
}
        
QWidget  *EraserTool::configurator()
{
    return  0;
}

void EraserTool::aboutToChangeScene(KTGraphicsScene *scene)
{
}

void EraserTool::aboutToChangeTool() 
{
}

void EraserTool::itemPressed(QGraphicsItem *item, const KTBrushManager *brush, const QPointF &pos)
{
    QList<QGraphicsItem *> collides = item->collidingItems();
    
    if (collides.count() == 0) {
        QRect intersectRect(pos.x() - (brush->pen().width()/2), pos.y() - (brush->pen().width())/2, 
                    brush->pen().width(), brush->pen().width());
        
        KTPathItem *path = qgraphicsitem_cast<KTPathItem*>(item);
        
        if (!path) {
            /*
             QString conv = "<convert type=\"2\" />"; // to path type
             KTProjectRequest *event = new KTProjectRequest(KTProjectRequest::Convert, 
             scene->currentSceneIndex(), scene->currentLayerIndex(), scene->currentFrameIndex(), 
             scene->currentFrame()->graphics().indexOf(item), conv);
             addProjectRequest(event);
            */
        } else {
            QPainterPath ppath = path->path();
        }
    }
}

void EraserTool::saveConfig()
{
}

void EraserTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        emit closeHugeCanvas();
}

Q_EXPORT_PLUGIN2(kt_eraser, EraserTool)

