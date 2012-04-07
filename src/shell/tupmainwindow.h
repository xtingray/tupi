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

#ifndef TUPMAINWINDOW_H
#define TUPMAINWINDOW_H

#include "tactionmanager.h"
#include "tupviewdocument.h"
#include "tupanimationspace.h"
#include "tuppreferences.h"

// modules
#include "tupexposuresheet.h"
#include "kinaswidget.h"
#include "tuppenwidget.h"
#include "tupcamerawidget.h"
#include "tupcolorpalette.h"
#include "tupsceneswidget.h"
#include "tuplibrarywidget.h"
#include "tuptimeline.h"
#include "tupdebugwidget.h"
#include "tupviewcamera.h"
#include "tuphelpwidget.h"
#include "tuphelpbrowser.h"
#include "tuptwitterwidget.h"
#include "tupexportwidget.h"

#include "tupviewdocument.h"
#include "tabbedmainwindow.h"
#include "tupstatusbar.h"
#include "tosd.h"
#include "toolview.h"

// Including headers about projects management
#include "tupprojectmanager.h"
#include "tupnetprojectmanagerhandler.h"
#include "tupsplash.h"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QUndoStack>

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
             Help = 0x04,
             News = 0x08,
             All = Animation | Player | Help | News
        };

        enum RequestType {
             None = 0,
             NewLocalProject,
             OpenLocalProject,
             NewNetProject,
             OpenNetProject,
             ImportProjectToNet
        };

        TupMainWindow(TupSplash *splash = 0, int parameters = 1);
        ~TupMainWindow();

    private:
        /**
         * Creates the file action 
         */
         void setupFileActions();
         void setupSettingsActions();
         // void setupWindowActions();
         void setupInsertActions();

        /**
         * Sets up the actions in the toolbar
         */
         void setupToolBar();

        /**
         * Sets up he actions in the menu
         */
         void setupMenu();

         void setupHelpActions();
         void setupActions();
         void setMenuItemsContext(bool flag);

         void connectWidgetToManager(QWidget *widget);
         void connectWidgetToPaintArea(QWidget *widget);
         void connectWidgetToLocalManager(QWidget *widget);

         void setupNetworkProject();
         void setupNetworkProject(TupProjectManagerParams *params);
         void setupLocalProject(TupProjectManagerParams *params);
         void setUndoRedoActions();
         void resetUI();

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

    private slots:
          void setWorkSpace();
          void createNewLocalProject();
          void newProject();
          bool closeProject();
          void unexpectedClose();
          void openProject();
          void openProjectFromServer();
          void importProjectToServer();
          void exportProject();

          void save();
          void saveAs();

          void showHelpPage(const QString &document);
          void showWidgetPage();

          void showAnimationMenu(const QPoint &p);

          void changePerspective(QAction *a);
          void setHelpPerspective();

          void addPage(QWidget *widget);
          void updateCurrentTab(int index);

          void requestProject();
          void createNewNetProject(const QString &title);
          void netProjectSaved();
          void updatePlayer(bool remove);

    private slots:
          void messageToStatus(const QString &);
          void preferences();
          void aboutTupi();
          void showTipDialog();
          void importPalettes();
          void connectToDisplays(const QWidget *widget);
          void saveProject();
          void openRecentProject();
          void createCommand(const TupPaintAreaEvent *event);
          void callSave();
          void expandExposureView(int index);
          void expandColorView();
          void postVideo(const QString &title, const QString &topics, const QString &description, int fps, const QList<int> sceneIndexes);
          void postStoryboard(const QString &title, const QString &topics, const QString &description, const QList<int> sceneIndexes);
          void resetMousePointer();

    private:
          TupProjectManager *m_projectManager;
          Tupi::RenderType m_renderType;
          QString m_fileName;
          bool lastSave;

    private:
          TupViewDocument *drawingTab;
          TupAnimationspace *animationTab;
          TupHelpBrowser *helpTab;
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
          TupNetProjectManagerHandler *netProjectManagerHandler;
          bool isNetworked;
          ToolView *m_viewChat;

    // Components
    private:
          TupExposureSheet *m_exposureSheet;
          TupScenesWidget *m_scenes;
          TupTimeLine *m_timeLine;

#if defined(QT_GUI_LIB) && defined(K_DEBUG)
          TupDebugWidget *m_debug;
#endif
          TupHelpWidget *m_helper;
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
          TupViewCamera *viewCamera;
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
