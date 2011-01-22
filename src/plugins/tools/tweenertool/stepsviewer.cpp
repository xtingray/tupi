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

#include "stepsviewer.h"
#include "kttweenerstep.h"
#include "spinboxdelegate.h"
#include "kdebug.h"

#include <cmath>
#include <QGraphicsPathItem>
#include <QDebug>

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

    // SQA: Disabled while plugin is fixed
    /*
    SpinBoxDelegate *spin = new SpinBoxDelegate;
    setItemDelegate(spin);
    connect(spin, SIGNAL(nodeClicked()), this, SIGNAL(updateTable()));
    */

    setMaximumWidth(120);
    setMaximumHeight(800);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

StepsViewer::~StepsViewer()
{
    delete k;
}

QSize StepsViewer::sizeHint() const
{
    return QSize(maximumWidth(), maximumHeight());
}

void StepsViewer::setPath(const QGraphicsPathItem *path)
{
    // Set of key points which define the path 
    QPolygonF points = path->path().toFillPolygon();

    if (points.count() > 2) {

        int count = 0; 

        k->points.clear();
        k->frames.clear();
        k->points = points;
        k->dots = new QList<QPointF>();

        setRowCount(0);

        // path().elementCount(): Returns the number of path elements in the painter path.

         QList<QPointF> *keys = new QList<QPointF>();

         for (int i = 1; i < path->path().elementCount(); i++) {
              QPainterPath::Element e  = path->path().elementAt(i);
              if (e.type != QPainterPath::CurveToElement) {
                  if ((e.type == QPainterPath::CurveToDataElement) &&
                      (path->path().elementAt(i-1).type == QPainterPath::CurveToElement))
                       continue;

                  QPointF point(e.x, e.y);
                  keys->append(point); 
                  count++;
              }
         }

         int control = 0;
         QPointF controlKey = keys->at(0);
         int frames = 0;

         for(int i=0; i < points.size()-1; i++) {
             QPointF point = points.at(i);
             if (point == controlKey) {
                 if (frames == 1) {
                     if (control == 0) {
                         k->dots->append(calculateDots(points.at(0), controlKey, 9));
                         frames = 10;
                     } else {
                         k->dots->append(calculateDots(keys->at(control-1), controlKey, 9));
                         frames = 9; 
                     }
                 } else {
                     if (control == 0) 
                         frames++;
                 } 

                 setRowCount(rowCount() + 1);

                 QTableWidgetItem *intervalItem = new QTableWidgetItem();
                 intervalItem->setTextAlignment(Qt::AlignCenter);
                 intervalItem->setText(QString::number(control+1));
                 intervalItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);

                 QTableWidgetItem *framesItem = new QTableWidgetItem();
                 framesItem->setTextAlignment(Qt::AlignCenter);
                 framesItem->setText(QString::number(frames));
                 framesItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);

                 setItem(control, 0, intervalItem);
                 setItem(control, 1, framesItem);
                 setRowHeight(control, 20);

                 if (point != keys->last()) {
                     control++;
                     controlKey = keys->at(control);
                 }
                 frames = 0;
             }

             k->dots->append(point);
             frames++;
         }
    }

    kFatal() << "StepsViewer::setPath() - Dots Size: " << k->dots->size();
}

QVector<KTTweenerStep *> StepsViewer::steps()
{
    QVector<KTTweenerStep *> stepsVector;
    int count = 0;

    for (int i=0; i < k->dots->size(); i++) {
         KTTweenerStep *step = new KTTweenerStep(count);
         step->setPosition(k->dots->at(i));
         stepsVector << step;
         count++;
    }
  
    kFatal() << "StepsViewer::steps() - Tracing Tweener size: " << stepsVector.size();

    return stepsVector;
}

int StepsViewer::totalSteps()
{
    return k->dots->count();
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
    qreal b = dot1.y() - m*dot1.x();
    qreal delta = (dot2.x() - dot1.x())/total; 
    qreal x = dot1.x();

    for (int i=1; i<total; i++) {
         x += delta;
         qreal y = m*x + b;
         QPointF dot;
         dot.setX(x);
         dot.setY(y);
         result.append(dot);
    }

    return result;
}
