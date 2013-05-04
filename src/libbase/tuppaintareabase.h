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

#ifndef TUPPAINTAREABASE_H
#define TUPPAINTAREABASE_H

#include "tuptoolplugin.h"
#include "tupglobal.h"

#include <QGraphicsView>

class QGraphicsRectItem;
class TupBrushManager;
class TupInputDeviceInformation;
class TupProject;
class TupPaintAreaRotator;

/**
 * This class provides an area to draw with some kind of brushes
 * @author Jorge Cuadrado - David Cuadrado
*/

class TUPI_EXPORT TupPaintAreaBase : public QGraphicsView
{
    Q_OBJECT

    public:
        TupPaintAreaBase(QWidget * parent = 0, QSize dimension = QSize(0, 0));
        ~TupPaintAreaBase();

        void setBgColor(const QColor color);
        void setAntialiasing(bool use);
        void setUseOpenGL(bool opengl);
        void setDrawGrid(bool draw);
        void setTool(TupToolPlugin *tool);

        bool drawGrid() const;

        void scaleView(qreal scaleFactor);
        void setRotationAngle(int angle);
        void setZoom(qreal factor);

        TupBrushManager *brushManager() const;

        QRectF drawingRect() const;
        TupGraphicsScene *graphicsScene() const;
        QPointF viewPosition();
        QPointF centerPoint() const;

    private:
        virtual void saveState();
        virtual void restoreState();
        void drawPadLock(QPainter *painter, const QRectF &rect, QString text);

    protected:
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void tabletEvent(QTabletEvent *event);
        virtual void enterEvent(QEvent *event);
        virtual void leaveEvent(QEvent *event);

        virtual void wheelEvent(QWheelEvent *event);
        virtual bool viewportEvent(QEvent *event);

    signals:
        void cursorPosition(const QPointF &pos);
        void requestTriggered(const TupProjectRequest *event);
        void changedZero(const QPointF &zero);
        void scaled(double scaleFactor);

    public slots:
        void centerDrawingArea();

    protected:
        virtual void drawBackground(QPainter *painter, const QRectF &rect);
        virtual void drawForeground(QPainter *painter, const QRectF &rect);
        virtual bool canPaint() const;

    private:
        struct Private;
        Private *const k;
};

#endif
