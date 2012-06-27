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

#ifndef TUPCANVAS_H
#define TUPCANVAS_H

#include "tupgraphicsscene.h"
#include "tupcanvasview.h"
#include "tupbrushmanager.h"
#include "tupprojectrequest.h"
#include "tuprequestbuilder.h"
#include "tupproject.h"

#include <QFrame>
#include <QPointF>
#include <QSize>
#include <QCloseEvent>
#include <QColor>
#include <QPen>

class TupGraphicsScene;

class TupCanvas : public QFrame
{
    Q_OBJECT

    public:
        TupCanvas(QWidget *parent=0, Qt::WindowFlags f=0, TupGraphicsScene *scene=0, 
                  const QPointF centerPoint = QPoint(0, 0) , const QSize &size = QSize(0, 0), 
                  TupProject *project = 0, double scaleFactor = 1, int angle=0, 
                  TupBrushManager *brushManager = 0);
        ~TupCanvas();
        void updateCursor(const QCursor &cursor);

   protected:
        void closeEvent(QCloseEvent *event);

   private:
        TupCanvasView *graphicsView;
        struct Private;
        Private *const k;

   public slots:
        void colorDialog(const QColor &current);

   private slots:
        void colorDialog();
        void penDialog();
        void exposureDialog();

        void oneFrameBack();
        void oneFrameForward();
        void updateThickness(int value);
        void wakeUpPencil();
        void wakeUpInk();
        void wakeUpEllipse();
        void wakeUpRectangle();
        // void wakeUpPolyline();
        void wakeUpLibrary();
        void wakeUpObjectSelection();
        void wakeUpNodeSelection();
        void wakeUpDeleteSelection();
        void wakeUpZoomIn();
        void wakeUpZoomOut();
        void wakeUpHand();

        void undo();
        void redo();

   signals:
        void requestTriggered(const TupProjectRequest *event);
        void updateColorFromFullScreen(const QColor &color);
        void updatePenThicknessFromFullScreen(int size);
        void callAction(int menu, int index);
        void goToFrame(int frame, int layer, int scene);
        void goToScene(int scene);
};

#endif
