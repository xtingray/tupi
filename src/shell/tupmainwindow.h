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

#ifndef TUPMAINWINDOW_H
#define TUPMAINWINDOW_H

#include "tactionmanager.h"
#include "tupdocumentview.h"
#include "tupanimationspace.h"
#include "tuppreferencesdialog.h"
#include "tuphelpdialog.h"

// modules
#include "tupexposuresheet.h"
// #include "kinaswidget.h"
#include "tuppenwidget.h"
#include "tupcolorpalette.h"
#include "tupsceneswidget.h"
#include "tuplibrarywidget.h"
#include "tuptimeline.h"
#include "tupcamerawidget.h"
#include "tuphelpwidget.h"
#include "tuphelpbrowser.h"
#include "tuptwitterwidget.h"
#include "tupexportwidget.h"

#include "tabbedmainwindow.h"
#include "tupstatusbar.h"
#include "tosd.h"
#include "toolview.h"

// Including headers about projects management
#include "tupprojectmanager.h"
#include "tupnetprojectmanagerhandler.h"

#ifdef K_DEBUG
#ifdef Q_OS_WIN
#include <QDebug>
#else
#include "tdebug.h"
#include "tupdebugwidget.h"
#endif
#endif

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QUndoStack>
#include <QKeySequence>
#include <QTextBrowser>
#include <QToolBar>
#include <QDesktopWidget>

class TupProjectManagerParams;
class TupNetProjectManagerParams;
class TupProjectResponse;

/**
 * This class defines the main window application
 * @author David Cuadrado
*/

class TupMainWindow : public TabbedMainWindow
{
    Q_OBJECT

    public:

        enum Perspective {
             Animation = 0x01,
             Player = 0x02,
             News = 0x04,
             // Play = 0x08,
             All = Animation | Player | News
        };

        enum RequestType {
             None = 0,
             NewLocalProject,
             OpenLocalProject,
             NewNetProject,
             OpenNetProject,
             ImportProjectToNet
        };

        TupMainWindow();
        ~TupMainWindow();

    private:
        /**
         * Creates the file action 
         */
         void setupFileActions();
         void setPreferencesAction();
         // void setupWindowActions();
         // void setupInsertActions();

        /**
         * Sets up the actions in the toolbar
         */
         void setupToolBar();

        /**
         * Sets up he actions in the menu
         */
         void setupMenu();

         void setupHelpActions();
         // void setupActions();
         void setMenuItemsContext(bool flag);

         void connectWidgetToManager(QWidget *widget);
         void disconnectWidgetToManager(QWidget *widget);
         void connectWidgetToPaintArea(QWidget *widget);
         void connectWidgetToLocalManager(QWidget *widget);

         void setupNetworkProject();
         void setupNetworkProject(TupProjectManagerParams *params);
         void setupLocalProject(TupProjectManagerParams *params);
         void setUndoRedoActions();
         void resetUI();
         void updateRecentProjectList();
         void saveDefaultPath(const QString &dir);
         void resetExportWidget();

    protected:
         /**
          *  Event for main window closing control
          *
          * Reimplemented from QWidget.
          * @param close_event The input event
          */
          void closeEvent(QCloseEvent *event);

         /**
          *  Creates the application GUI according to the information from the data classes
          */
          virtual void createGUI();

         /**
          *  Updates the open recent menu item names according to the @a recent_names list of file names
          */
          void updateOpenRecentMenu(QMenu *menu, QStringList recents);

    public slots:
          void openProject(const QString &path);
          void updatePenColor(const QColor &color);
          void updatePenWidth(int width);

    private slots:
          void addTwitterPage();
          void setWorkSpace(const QStringList &users = QStringList());
          void createNewLocalProject();
          void newProject();
          bool closeProject();
          void unexpectedClose();
          void openProject();
          void openProjectFromServer();
          void importProjectToServer();
          void exportProject();
          void saveAs();
          void saveProject();

          void showAnimationMenu(const QPoint &p);

          void changePerspective(QAction *a);
          void changePerspective(int index);

          void addPage(QWidget *widget);
          void updateCurrentTab(int index);

          void requestProject();
          void createNewNetProject(const QString &title, const QStringList &users);
          void netProjectSaved();
          void updatePlayer();
          void updatePlayer(bool removeAction);

          void resizeProjectDimension(const QSize size);

    private slots:
          void preferences();
          void showHelp();
          void aboutTupi();
          void showTipDialog();
          void importPalettes();
          void openRecentProject();
          void createPaintCommand(const TupPaintAreaEvent *event);
          void callSave();
          void expandExposureView(TupProject::Mode contextMode);
          void resetMousePointer();
          void updateUsersOnLine(const QString &login, int state);
          void importPapagayoLipSync();
          void hideTopPanels();

    private:
          TupProjectManager *m_projectManager;
          // Tupi::RenderType m_renderType;
          QString m_fileName;
          bool lastSave;

    private:
          TupDocumentView *animationTab;
          TupAnimationspace *playerTab;
          // TupHelpBrowser *helpTab;
          TupTwitterWidget *newsTab;
          TupStatusBar *m_statusBar;
          TActionManager *m_actionManager;
          QMenu *m_fileMenu;
          QMenu *m_settingsMenu;
          QMenu *m_viewMenu;
          QMenu *m_insertMenu;
          QMenu *m_toolsMenu; 
          QMenu *m_windowMenu;
          QMenu *m_helpMenu;

          QStringList m_recentProjects;
          QMenu *m_recentProjectsMenu;

    // Network variables
    private:
          TupNetProjectManagerHandler *netProjectManager;
          bool isNetworked;
          ToolView *m_viewChat;

    // Components
    private:
          QToolBar *mainToolBar;
          QToolBar *alternativeToolBar;
          TupExposureSheet *m_exposureSheet;
          TupScenesWidget *m_scenes;
          TupTimeLine *m_timeLine;

#if defined(QT_GUI_LIB) && defined(K_DEBUG) && defined(Q_OS_UNIX)
          TupDebugWidget *m_debug;
#endif
          // TupHelpWidget *m_helper;
          TupLibraryWidget *m_libraryWidget;
          TupColorPalette *m_colorPalette;
          TupPenWidget *m_penWidget;
          ToolView *exposureView;
          ToolView *colorView;
          ToolView *penView;
          ToolView *libraryView;
          ToolView *scenesView;  
          ToolView *helpView;
          ToolView *timeView;
          ToolView *debugView;
          ToolView *exportView;
          TupExportWidget *exportWidget;

          TupCameraWidget *cameraWidget;
          bool isSaveDialogOpen; 
          bool internetOn;
          int lastTab;
          TupProject::Mode contextMode;
          TupMainWindow::RequestType requestType; 
          QString projectName;
          QString author;
          QString netUser;

    signals:
          void responsed(TupProjectResponse *);
          void updateAnimationModule(TupProject *, int, int, int);
};

#endif
