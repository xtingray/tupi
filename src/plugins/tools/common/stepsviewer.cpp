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
 *   the Free Software Foundation; either version 2 of the License, or     *
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

struct StepsViewer::Private
{
    QPolygonF points;
    QList<int> frames;
    QList<QPointF> *dots;
    QList<TPushButton*> *plusButton;
    QList<TPushButton*> *minusButton;
};

StepsViewer::StepsViewer(QWidget *parent) : QTableWidget(parent), k(new Private)
{
    setColumnCount(4);
    setColumnWidth(0, 70);
    setColumnWidth(1, 60);
    setColumnWidth(2, 20); 
    setColumnWidth(3, 20);

    setHorizontalHeaderLabels(QStringList() << tr("Interval") << tr("Frames") << tr("") << tr(""));

    // SQA: Disabled while plugin is fixed
    /*
    SpinBoxDelegate *spin = new SpinBoxDelegate;
    setItemDelegate(spin);
    connect(spin, SIGNAL(nodeReleased()), this, SIGNAL(updateTable()));
    */

    setMinimumWidth(174);
    setMaximumHeight(800);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    k->dots = new QList<QPointF>();
    k->plusButton = new QList<TPushButton*>();
    k->minusButton = new QList<TPushButton*>();
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[StepsViewer::setPath()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    // Set of key points which define the path 
    QPolygonF points = path->path().toFillPolygon();

    if (points.count() > 2) {
        int count = 0; 

        k->points.clear();
        k->frames.clear();
        k->points = points;
        k->dots->clear();

        k->plusButton->clear();
        k->minusButton->clear();

        setRowCount(0);

        // This list contains the (green) key points of the path 
        QList<QPointF> *keys = new QList<QPointF>();

        int total = path->path().elementCount();
        // tError() << "Elements Count: " << total; // Returns the number of path elements in the painter path.

        for (int i = 1; i < total; i++) {
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

        // tError() << "points.size(): " << points.size();

        // Grouping dots by key points
        for (int i=0; i < points.size()-1; i++) {
             QPointF point = points.at(i);
             // tError() << "Point: [" << point.x() << ", " << point.y() << "]";

             if (point == controlKey) {
                 if (frames == 1) {
                     if (control == 0) {
                         k->dots->append(calculateDots(points.at(0), controlKey, 29));
                         frames = 30;
                     } else {
                         k->dots->append(calculateDots(keys->at(control-1), controlKey, 29));
                         frames = 29; 
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

                 k->plusButton->append(new TPushButton(this, "+", 2, control)); 
                 connect(k->plusButton->at(control), SIGNAL(clicked(int, int)), this, SLOT(updatePath(int, int)));
                 k->minusButton->append(new TPushButton(this, "-", 3, control));
                 connect(k->minusButton->at(control), SIGNAL(clicked(int, int)), this, SLOT(updatePath(int, int)));

                 setItem(control, 0, intervalItem);
                 setItem(control, 1, framesItem);
                 setCellWidget(control, 2, k->plusButton->at(control));
                 setCellWidget(control, 3, k->minusButton->at(control));

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

    /*
    for (int i=0; i<100; i++) {
         qreal t = (qreal)i/(qreal)100; 
         QPointF point = path->path().pointAtPercent(t);
         tError() << "Point: [" << point.x() << ", " << point.y() << "]";
    }
    */
}

void StepsViewer::updatePath(int column, int row)
{
    QTableWidgetItem *cell = item(row, 1);
    int value = cell->text().toInt();

    if (column == 2)
        value += 1;
    else
        value -= 1;

    if (value < 0)
        value = 1;
    
    cell->setText(QString::number(value));

    // SQA: Make the points calculation right here!
}

QVector<TupTweenerStep *> StepsViewer::steps()
{
    QVector<TupTweenerStep *> stepsVector;

    for (int i=0; i < k->dots->size(); i++) {
         TupTweenerStep *step = new TupTweenerStep(i);
         step->setPosition(k->dots->at(i));
         stepsVector << step;
    }
  
    return stepsVector;
}

int StepsViewer::totalSteps()
{
    return k->dots->count();
}

void StepsViewer::cleanRows()
{
    k->points.clear();
    k->frames.clear();
    k->dots->clear();

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
