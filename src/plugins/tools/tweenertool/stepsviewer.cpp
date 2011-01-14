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

#include <QGraphicsPathItem>
#include <QDebug>
#include <cmath>

#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "spinboxdelegate.h"
#include "kdebug.h"

struct StepsViewer::Private
{
    QPolygonF points;
    QList<int> frames;
    QList<QPointF> *dots;
};

StepsViewer::StepsViewer(QWidget *parent) : QTableWidget(parent), k(new Private)
{
    setFont(QFont("Arial", 8, QFont::Normal, false));
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList() << tr("Interval") << tr("Frames"));

    /*
    SpinBoxDelegate *spin = new SpinBoxDelegate;
    setItemDelegate(spin);
    connect(spin, SIGNAL(nodeClicked()), this, SIGNAL(updateTable()));
    */

    //setMinimumWidth(142);
    setMaximumWidth(120);
}

StepsViewer::~StepsViewer()
{
    delete k;
}

void StepsViewer::setPath(const QGraphicsPathItem *path)
{
    QPolygonF points = path->path().toFillPolygon();

    if (points.count() > 2) {

        k->points.clear();
        k->frames.clear();
        
        //points.pop_back();
        //points.pop_front();

        //if (points.count() > 3)
        //    points.pop_back();

        k->points = points;

        kFatal() << "StepsViewer::setPath() - Dots: " << points.count();
        
        int count = 1;
        setRowCount(0);

        kFatal() << "StepsViewer::setPath() - Path elements: " << path->path().elementCount();

        QPointF basePoint;
        k->dots = new QList<QPointF>();

        for (int i = 1; i < path->path().elementCount(); i++) {

             QPainterPath::Element e  = path->path().elementAt(i);
            
             if (e.type != QPainterPath::CurveToElement) {

                 if ((e.type == QPainterPath::CurveToDataElement) && 
                     (path->path().elementAt(i-1).type == QPainterPath::CurveToElement)) 
                     continue;

                 // k->stops << e;
                
                 int frames = 1;
    
                 if (count == 1) {
                     frames = 10;
                     //frames = 3;
                     QPointF point1 = points.value(0);
                     basePoint = points.value(1);
                     k->dots->append(point1);
                     kFatal() << "Line #" << count;
                     kFatal() << "StepsViewer::setPath() - Point 1: (" << point1.x() << ", " << point1.y() << ")";
                     kFatal() << "StepsViewer::setPath() - Point 2: (" << basePoint.x() << ", " << basePoint.y() << ")";
                     k->dots->append(calculateDots(point1, basePoint, 9));
                     kFatal() << "StepsViewer::setPath() - Total Dots: " << k->dots->count();
                 } else {
                     frames = 9;
                     //frames = 2;
                     QPointF point2 = points.value(count);
                     kFatal() << "Line #" << count;
                     kFatal() << "StepsViewer::setPath() - Point 1: (" << basePoint.x() << ", " << basePoint.y() << ")";
                     kFatal() << "StepsViewer::setPath() - Point 2: (" << point2.x() << ", " << point2.y() << ")";
                     k->dots->append(calculateDots(basePoint, point2, 9));
                     basePoint = point2;
                     //if (points.count()-1 == count)
                     //    k->dots->append(point2);
                 }
               
                 /* 
                 QPolygonF::iterator it = points.begin();
                
                 while (it != points.end()) {
                        frames++;
                        if (e == (*it))
                            break;
                        else
                            it = points.erase(it);
                 }
                 */

                 k->frames << frames;
                 setRowCount(rowCount() + 1);

                 QTableWidgetItem *intervalItem = new QTableWidgetItem();
                 intervalItem->setTextAlignment(Qt::AlignCenter);
                 intervalItem->setText(QString::number(count));
                 intervalItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);

                 QTableWidgetItem *framesItem = new QTableWidgetItem();
                 framesItem->setTextAlignment(Qt::AlignCenter);
                 framesItem->setText(QString::number(frames));
                 framesItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);
                
                 setItem(count-1, 0, intervalItem);
                 setItem(count-1, 1, framesItem);
                 setRowHeight(count-1, 20);

                 count++;
            }
        }
    }
}

QVector<KTTweenerStep *> StepsViewer::steps()
{
    int count = 0;

    kFatal() << "StepsViewer::steps() - Dots total: " << k->dots->count();
    QVector<KTTweenerStep *> stepsVector;
    for (int i = 0; i < rowCount(); i++) {
        for (int j = 0; j < k->frames[i]; j++) {
             KTTweenerStep *step = new KTTweenerStep(count);
             QPointF pos = k->dots->at(count);
             kFatal() << "StepsViewer::steps() - Pos[" << count << "]: (" << pos.x() << ", " << pos.y() << ")";
             step->setPosition(pos);
             stepsVector << step;
             count++;
        }
    }

    return stepsVector;
}

int StepsViewer::totalSteps()
{
    int total = 0;
    for (int i = 0; i < rowCount(); i++)
         total += item(i,1)->text().toInt();

    return total;
}

void StepsViewer::cleanRows()
{
    int size = rowCount() - 1;
    for (int i = size ; i >= 0; i--)
         removeRow(i);
}

QList<QPointF> StepsViewer::calculateDots(QPointF dot1, QPointF dot2, int total)
{
    QList<QPointF> result;

    qreal m = (dot2.y() - dot1.y())/(dot2.x() - dot1.x());
    kFatal() << "StepsViewer::calculateDots() - M: " << m;

    qreal b = dot1.y() - m*dot1.x();

    qreal delta = (dot2.x() - dot1.x())/total; 
    //qreal delta = (dot2.x() - dot1.x())/2;

    kFatal() << "StepsViewer::calculateDots() - Delta: " << delta;

    qreal x = dot1.x();

    for (int i=1; i<total; i++) {
         x += delta;
         qreal y = m*x + b;
         kFatal() << "StepsViewer::calculateDots() - Calculating point: (" << x << ", " << y << ")";
         QPointF dot;
         dot.setX(x);
         dot.setY(y);
         result.append(dot);
    }

    kFatal() << "StepsViewer::calculateDots() - Calculating point: (" << dot2.x() << ", " << dot2.y() << ")";

    result.append(dot2);

    return result;
}
