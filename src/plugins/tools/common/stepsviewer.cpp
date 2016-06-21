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
#include <QLineEdit>

struct StepsViewer::Private
{
    QList<int> frames;
    QList<TPushButton*> *plusButton;
    QList<TPushButton*> *minusButton;
    QList<Segment> pointBlocks;
    QList<Segment> segments;
    int records;
    QPainterPath path;
    QList<QPointF> keys;
    QPolygonF points;
};

StepsViewer::StepsViewer(QWidget *parent) : QTableWidget(parent), k(new Private)
{
    verticalHeader()->hide();

    k->records = 0;
    setColumnCount(4);
    setColumnWidth(0, 70);
    setColumnWidth(1, 60);
    setColumnWidth(2, 20); 
    setColumnWidth(3, 20);

    setHorizontalHeaderLabels(QStringList() << tr("Interval") << tr("Frames") << tr("") << tr(""));

    setMinimumWidth(174);
    setMaximumHeight(800);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

void StepsViewer::loadPath(const QGraphicsPathItem *pathItem, QList<int> intervals)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[StepsViewer::loadPath()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->frames = intervals;
    k->records = k->frames.count();

    // Set of key points which define the path
    k->path = pathItem->path();
    k->points = k->path.toFillPolygon();
    k->points.removeLast();

    // This list contains the (green) key points of the path
    calculateKeys();
    calculateGroups();

    int total = k->frames.count();
    for (int row=0; row < total; row++) { // Processing every segment
         QList<QPointF> block = k->pointBlocks.at(row);
         int frames = k->frames.at(row);
         int size = block.size();
         QList<QPointF> segment;

         if (size > 2) {
             int delta = size/(frames-1);
             int pos = delta;
             if (row==0) {
                 frames--;
                 segment.append(block.at(0));
             } else {
                 delta = size/frames;
             }

             if (frames > 2) {
                 for (int i=1; i < frames; i++) { // calculating points set for the segment j
                      segment << block.at(pos);
                      pos += delta;
                 }
             } else {
                 if (row > 0)
                     segment << block.at(pos);
             }

             segment << k->keys.at(row);
         } else {
             QPointF init = block.at(0);
             int range = frames;
             if (row == 0) {
                 range--;
                 segment << init;
             } else {
                 init = k->keys.at(row-1);
             }

             if (row == 0 && range == 1)
                 segment << k->keys.at(row);
             else
                 segment.append(calculateSegmentPoints(init, k->keys.at(row), range));
         } 

         k->segments << segment;
         addTableRow(row, segment.count());
    }
}

void StepsViewer::setPath(const QGraphicsPathItem *pathItem)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[StepsViewer::setPath()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    // Set of key points which define the path 
    k->path = pathItem->path();
    k->points = k->path.toFillPolygon();
    k->points.removeLast();

    calculateKeys();

    if (k->records < k->keys.size()) { // A new table row must be added. Last segment must be calculated
        k->records = k->keys.size(); 
        int row = k->records - 1;

        QList<QPointF> segment;
        QPointF pInit;
        int frames = 4;
        int range = frames;
        if (row == 0) {
            frames++;
            pInit = k->points.at(0);
            segment << pInit;
        } else {
            pInit = k->keys.at(row-1);
        }

        k->frames << frames;
        addTableRow(row, frames);

        segment.append(calculateSegmentPoints(pInit, k->keys.at(row), range));
        k->segments << segment;
    } else { // A node was edited, segments must be recalculated
        // Recalculating segments
        calculateGroups();

        int total = k->frames.count();
        for (int row=0; row < total; row++) { // Processing every segment
             QList<QPointF> block = k->pointBlocks.at(row);
             int frames = k->frames.at(row);
             int size = block.size();
             QList<QPointF> segment; 

             if (size > 2) {
                 if (size < frames) {
                     segment = block;
                     k->frames.replace(row, segment.count());
                     QTableWidgetItem *cell = item(row, 1);
                     cell->setText(QString::number(segment.count()));
                 } else {
                     int delta = size/(frames-1);
                     int pos = delta;
                     if (row==0) {
                         frames--;
                         segment.append(block.at(0));
                     } else {
                         delta = size/frames;
                     }

                     if (frames > 2) {
                         for (int i=1; i < frames; i++) { // calculating points set for the segment j
                              segment << block.at(pos);
                              pos += delta;
                         }
                     } else {
                         if (row > 0)
                             segment << block.at(pos);
                     }

                     segment << k->keys.at(row);
                 }
             } else {
                 QPointF init = block.at(0);
                 if (row == 0) {
                     frames--;
                     segment << init;
                 } else {
                     init = k->keys.at(row-1);
                 }

                 if (row == 0 && frames == 1)
                     segment << k->keys.at(row);
                 else
                     segment.append(calculateSegmentPoints(init, k->keys.at(row), frames));
             }  

             k->segments.replace(row, segment);
        }
    }
}

void StepsViewer::updatePathSection(int column, int row)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[StepsViewer::updatePathSection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QTableWidgetItem *cell = item(row, 1);
    int frames = cell->text().toInt();

    calculateKeys();
    calculateGroups();

    QList<QPointF> block = k->pointBlocks.at(row);
    int range = block.size();

    if (column == 2)
        frames += 1;
    else
        frames -= 1;

    if (row == 0) {
        if (frames < 2)
            frames = 2;
    } else {
        if (frames < 1)
            frames = 1;
    }

    k->frames.replace(row, frames);
    QList<QPointF> segment;

    if (range > 2) {
        if (range < frames) {
            segment = block;
            k->frames.replace(row, segment.count());
        } else {
            int delta = range/(frames - 1);

            int pos = delta;
            if (row==0) {
                frames--;
                segment.append(block.at(0));
            } else {
                delta = range/frames;
            }

            if (frames > 2) {
                for (int i=1; i < frames; i++) { // calculating points set for the segment j
                     segment << block.at(pos);
                     pos += delta;
                }
            } else {
                if (row > 0)
                    segment << block.at(pos);
            }

            segment << k->keys.at(row);
        }
    } else {
        QPointF init = block.at(0);
        if (row == 0) {
            frames--;
            segment << init;
        } else {
            init = k->keys.at(row-1);
        }

        if (row == 0 && frames == 1)
            segment << k->keys.at(row);
        else 
            segment.append(calculateSegmentPoints(init, k->keys.at(row), frames));
    }

    cell->setText(QString::number(segment.count()));
    k->segments.replace(row, segment);

    emit totalHasChanged(totalSteps());
}

QVector<TupTweenerStep *> StepsViewer::steps()
{
    QVector<TupTweenerStep *> stepsVector;
    int i = 0;
    foreach (QList<QPointF> segment, k->segments) {
             foreach (QPointF point, segment) {
                      TupTweenerStep *step = new TupTweenerStep(i);
                      step->setPosition(point);
                      stepsVector << step;
                      i++;
             }
    }

    return stepsVector;
}

int StepsViewer::totalSteps()
{
    int total = 0;
    for (int i=0; i < k->frames.count(); i++)
         total += k->frames.at(i);

    return total;
}

QString StepsViewer::intervals()
{
    QString output = ""; 
    foreach(int interval, k->frames) 
            output += QString::number(interval) + ",";

    output.chop(1);
    return output;
}

void StepsViewer::clearInterface()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[StepsViewer::clearInterface()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    k->records = 0;
    k->frames.clear();
    k->segments.clear();

    int size = rowCount() - 1;
    for (int i=size; i >= 0; i--)
         removeRow(i);

    k->plusButton->clear();
    k->minusButton->clear();
}

QList<QPointF> StepsViewer::calculateSegmentPoints(QPointF begin, QPointF end, int total)
{
    QList<QPointF> pathPoints;

    qreal m = (end.y() - begin.y())/(end.x() - begin.x());
    qreal b = begin.y() - (m*begin.x());
    qreal delta = (end.x() - begin.x())/total; 
    qreal x = begin.x();

    QPointF dot;
    qreal y;
    for (int i=0; i<(total-1); i++) {
         x += delta;
         y = m*x + b;
         dot.setX(x);
         dot.setY(y);
         pathPoints.append(dot);
    }

    pathPoints.append(end);

    return pathPoints;
}

void StepsViewer::addTableRow(int row, int frames)  
{
    setRowCount(rowCount() + 1);

    QTableWidgetItem *intervalItem = new QTableWidgetItem();
    intervalItem->setTextAlignment(Qt::AlignCenter);
    intervalItem->setText(QString::number(row + 1));
    intervalItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem *framesItem = new QTableWidgetItem();
    framesItem->setTextAlignment(Qt::AlignCenter);
    framesItem->setText(QString::number(frames));
    // framesItem->setFlags(intervalItem->flags() & ~Qt::ItemIsEditable);

    k->plusButton->append(new TPushButton(this, "+", 2, row));
    connect(k->plusButton->at(row), SIGNAL(clicked(int, int)), this, SLOT(updatePathSection(int, int)));
    k->minusButton->append(new TPushButton(this, "-", 3, row));
    connect(k->minusButton->at(row), SIGNAL(clicked(int, int)), this, SLOT(updatePathSection(int, int)));

    setItem(row, 0, intervalItem);
    setItem(row, 1, framesItem);
    setCellWidget(row, 2, k->plusButton->at(row));
    setCellWidget(row, 3, k->minusButton->at(row));

    setRowHeight(row, 20);
}


void StepsViewer::calculateKeys()
{
    k->keys.clear();
    int total = k->path.elementCount();
    int count = 0;

    for (int i = 1; i < total; i++) {
         QPainterPath::Element e = k->path.elementAt(i);
         if (e.type != QPainterPath::CurveToElement) {
             if ((e.type == QPainterPath::CurveToDataElement) &&
                 (k->path.elementAt(i-1).type == QPainterPath::CurveToElement))
                 continue;

             QPointF point(e.x, e.y);
             k->keys.append(point);
             count++;
         }
    }
}

void StepsViewer::calculateGroups()
{
    k->pointBlocks.clear();

    int index = 0;
    int total = k->points.size();
    QList<QPointF> segment;

    for (int i=0; i < total; i++) { // Counting points between keys and saving key indexes
         QPointF point = k->points.at(i);
         if (point == k->keys.at(index)) {
             segment << point;
             k->pointBlocks << segment;
             index++;
             segment = QList<QPointF>();
        } else {
             segment << point;
        }
    }
}

void StepsViewer::commitData(QWidget *editor)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);

    if (lineEdit) {
        QString value = lineEdit->text();
        bool ok;
        int frames = value.toInt(&ok, 10);

        if (ok) {
            frames++;
            value = QString::number(frames);
            int row = currentRow();
            int column = currentColumn();
            QTableWidgetItem *cell = item(row, column);
            cell->setText(value);
            updatePathSection(column, row);
        }
    }
}
