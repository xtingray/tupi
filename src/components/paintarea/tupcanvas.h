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
#include <QStringList>

class TupGraphicsScene;

class TupCanvas : public QFrame
{
    Q_OBJECT

    public:
        TupCanvas(QWidget *parent=0, Qt::WindowFlags f=0, TupGraphicsScene *scene=0, 
                  const QPointF centerPoint = QPoint(0, 0) , const QSize &size = QSize(0, 0), 
                  TupProject *project = 0, double scaleFactor = 1, int angle=0, 
                  TupBrushManager *brushManager = 0, bool isNetworked = false, const QStringList &onLineUsers = QStringList());
        ~TupCanvas();
        void updateCursor(const QCursor &cursor);
        void updateOnLineUsers(const QStringList &onLineUsers);

   protected:
        void closeEvent(QCloseEvent *event);

   private:
        enum UserHand { Right = 0, Left };
        TupCanvasView *graphicsView;
        struct Private;
        Private *const k;

   public slots:
        void colorDialog(const QColor &current);

   private slots:
        void sketchTools();
        void selectionTools();

        void colorDialog();
        void penDialog();
        void penProperties();
        void opacityDialog();
        void setOnionOpacity(double opacity);
        void exposureDialog();

        void oneFrameBack();
        void oneFrameForward();
        void wakeUpLibrary();

        void wakeUpDeleteSelection();
        void wakeUpZoomIn();
        void wakeUpZoomOut();
        void wakeUpHand();

        void undo();
        void redo();

        void updateSketchMenuState();
        void updateSelectionMenuState();
        void updateMenuStates();
        void showInfoWidget();
        void hideInfoWidget();

        void updateExposureDialogState();
        void createScene();
        void createLayer(int sceneIndex, int layerIndex);
        void createFrame(int sceneIndex, int layerIndex, int layersTotal, int frameIndex);

   signals:
        void requestTriggered(const TupProjectRequest *event);
        void updateColorFromFullScreen(const QColor &color);
        void updatePenThicknessFromFullScreen(int size);
        void updateOnionOpacityFromFullScreen(double opacity);
        void callAction(int menu, int index);
        void goToFrame(int frame, int layer, int scene);
        void goToScene(int scene);
        void closeSketchMenu();
        void closeSelectionMenu();
        void closePenPropertiesMenu();
};

#endif
