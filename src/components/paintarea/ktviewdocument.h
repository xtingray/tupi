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

#ifndef KTVIEWAREA_H
#define KTVIEWAREA_H

#include "ktdocumentruler.h"
#include "tactionmanager.h"
#include "tosd.h"
#include "ktfilterinterface.h"
#include "kttoolinterface.h"
#include "ktconfigurationarea.h"
#include "ktglobal.h"

#include <QMainWindow>

class KTProjectRequest;
class KTProject;
class KTBrushManager;
class KTPaintAreaCommand;
class KTPaintAreaEvent;
class KTProjectResponse;

/**
 * @author Jorge Cuadrado
*/

class KTViewDocument : public QMainWindow
{
    Q_OBJECT

    public:
        KTViewDocument(KTProject *project, QWidget *parent = 0, bool isLocal = true);
        ~KTViewDocument();
        void closeArea();
        QSize sizeHint() const;

        void setAntialiasing(bool useIt );
        void setOpenGL(bool useIt);
        void setDrawGrid(bool draw);

        QPainter::RenderHints renderHints() const;
        void setRotationAngle(int angle);
        void setZoom(qreal factor);

        KTBrushManager *brushManager() const;
        KTPaintAreaCommand *createCommand(const KTPaintAreaEvent *event);
        void updatePaintArea();
        KTProject::Mode spaceContext();
        KTProject *project();

    private slots:
        void setNextOnionSkin(int n);
        void setPreviousOnionSkin(int n);

        void toggleShowGrid();

        // void setZoomFactor(int porcent);
        void scaleRuler(double factor);
        void changeRulerOrigin(const QPointF &zero);
        void saveTimer();
        void showFullScreen();
        void closeFullScreen();

    private:
        struct Private;
        Private *const k;

    private:
        void setupDrawActions();
        void createToolBar();
        void createMenu();
        void createTools();

    private slots: 
        // Plugins
        void loadPlugins();
        void setSpaceContext();
        void updateBgColor(const QColor color);
        void enableOnionFeature();
        void setDefaultOnionFactor();
        void setOnionFactor(double value);

    private slots:
        void showPos(const QPointF &p);	
        void setCursor(const QCursor &c);

        void updateZoomFactor( double f);
        // void configure();
        void selectToolFromMenu(QAction *act);
        //void firstCommand();
        void callAutoSave();

    public slots:
        void undo();
        void redo();
        void selectTool();
        void applyFilter();

        bool handleProjectResponse(KTProjectResponse *event);

    signals:
        void sendToStatus(const QString& msg);
        void requestTriggered(const KTProjectRequest *event);
        void localRequestTriggered(const KTProjectRequest *event);
        void autoSave();
        void modeHasChanged(int mode);
        void expandColorPanel();

    protected:
        // void closeEvent(QCloseEvent *e);
};

#endif
