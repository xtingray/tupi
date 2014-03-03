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

#ifndef TUPDOCUMENTVIEW_H
#define TUPDOCUMENTVIEW_H

#include "tupdocumentruler.h"
#include "tactionmanager.h"
#include "tosd.h"
#include "tupfilterinterface.h"
#include "tuptoolinterface.h"
#include "tupconfigurationarea.h"
#include "tupglobal.h"
#include "tupstoryboard.h"

#include <QMainWindow>

class TupProjectRequest;
class TupProject;
class TupBrushManager;
class TupPaintAreaCommand;
class TupPaintAreaEvent;
class TupProjectResponse;

/**
 * @author Jorge Cuadrado
*/

class TupDocumentView : public QMainWindow
{
    Q_OBJECT

    public:
        TupDocumentView(TupProject *project, QWidget *parent = 0, bool isNetworked = true, const QStringList &users = QStringList());
        ~TupDocumentView();
        void setWorkSpaceSize(int width, int height);
        void closeArea();
        QSize sizeHint() const;

        void setAntialiasing(bool useIt);
        void setOpenGL(bool useIt);

        QPainter::RenderHints renderHints() const;
        void setZoom(qreal factor);

        TupBrushManager *brushManager() const;
        TupPaintAreaCommand *createCommand(const TupPaintAreaEvent *event);
        void updatePaintArea();
        TupProject::Mode spaceContext();
        TupProject *project();
        int currentFramesTotal();
        int currentSceneIndex();
        void setZoomView(const QString &percent);
        void setRotationAngle(int angle);
        QSize workSpaceSize() const;
        void updateUsersOnLine(const QString &login, int state);
        void resizeProjectDimension(const QSize dimension);

    private slots:
        void setNextOnionSkin(int n);
        void setPreviousOnionSkin(int n);
        // void setZoomFactor(int porcent);
        void updateScaleVars(double factor);
        void updateRotationVars(int angle);
        void changeRulerOrigin(const QPointF &zero);
        void saveTimer();
        void showFullScreen();
        void closeFullScreen();
        void loadPlugin(int menu, int index);
        void updateStatusBgColor(const QColor color);
        void updatePenThickness(int size);
        void updateOnionOpacity(double opacity);
        void setBackgroundDirection(int direction);
        void updateBackgroundShiftProperty(int shift);
        void renderDynamicBackground();
        void fullScreenRightClick();
        void cameraInterface();
        void insertPictureInFrame(int id, const QString path);

    private:
        struct Private;
        Private *const k;
        void setupDrawActions();
        void createToolBar();
        void createMenu();
        void createTools();
        void updateRotationAngleFromRulers(int angle);

    private slots: 
        // Plugins
        void loadPlugins();
        void setSpaceContext();
        void updateBgColor(const QColor color);
        void enableOnionFeature();
        void setDefaultOnionFactor();
        void setOnionFactor(double value);
        void exportImage();
        void postImage();
        void storyboardSettings();
        void selectFrame(int frame, int layer, int scene);
        void selectScene(int scene);

    private slots:
        void showPos(const QPointF &point);	
        void setCursor(const QCursor &cursor);
        void updateZoomFactor(double factor);
        void selectToolFromMenu(QAction *action);
        void callAutoSave();
        void sendStoryboard(TupStoryboard *storyboard, int sceneIndex);

    public slots:
        void undo();
        void redo();
        void selectTool();
        void applyFilter();
        void drawGrid();
        void drawActionSafeArea();
        bool handleProjectResponse(TupProjectResponse *event);

    signals:
        void sendToStatus(const QString &msg);
        void requestTriggered(const TupProjectRequest *event);
        void localRequestTriggered(const TupProjectRequest *event);
        void autoSave();
        void modeHasChanged(TupProject::Mode mode);
        void expandColorPanel();
        void requestExportImageToServer(int frameIndex, int sceneIndex, const QString &title, const QString &topics, const QString &description);
        void openColorDialog(const QColor &);
        void updateColorFromFullScreen(const QColor &color);
        void updatePenFromFullScreen(const QPen &pen);
        void updateStoryboard(TupStoryboard *storyboard, int sceneIndex);
        void postStoryboard(int sceneIndex);
        void projectHasChanged();
        void closePolyLine();
        void projectSizeHasChanged(const QSize dimension);

    // protected:
    // void closeEvent(QCloseEvent *e);
};

#endif
