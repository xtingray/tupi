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

#ifndef KTMAINWINDOW_H
#define KTMAINWINDOW_H

#include "tactionmanager.h"
#include "ktviewdocument.h"

#include "ktanimationspace.h"
#include "ktpreferences.h"

// modules
#include "ktexposuresheet.h"
#include "kinaswidget.h"
#include "ktpenwidget.h"
#include "ktcamerawidget.h"
#include "ktcolorpalette.h"
#include "ktsceneswidget.h"
#include "ktlibrarywidget.h"
#include "kttimeline.h"
#include "ktdebugwidget.h"
#include "ktviewcamera.h"
#include "kthelpwidget.h"
#include "kthelpbrowser.h"
#include "ktwitterwidget.h"
#include "ktexportwidget.h"

#include "ktviewdocument.h"

#include "ktabbedmainwindow.h"
#include "ktstatusbar.h"

#include "tosd.h"
#include "ktoolview.h"

// Including headers about projects management
#include "ktprojectmanager.h"
#include "ktnetprojectmanagerhandler.h"
#include "ktsplash.h"

#include "ktviewdocument.h"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QUndoStack>

class KTProjectManagerParams;
class KTNetProjectManagerParams;
class KTProjectResponse;

/**
 * This class defines the main window application
 * @author David Cuadrado
*/

class KTMainWindow : public KTabbedMainWindow
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
             OpenNetProject
        };

        KTMainWindow(KTSplash *splash = 0, int parameters = 1);
        ~KTMainWindow();

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

         void ui4project(QWidget *widget);
         void ui4paintArea(QWidget *widget);
         void ui4localRequest(QWidget *widget);
         void flipTabs(int index);

         // void setupNetworkProject(const QString& projectName = QString(), const QString &server = QString(), int port = -1);
         void setupNetworkProject();
         void setupNetworkProject(KTProjectManagerParams *params);
         void setupLocalProject(KTProjectManagerParams *params);
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
          // void internetEnabled();

    private slots:
          void viewNewDocument();
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

          void requestNewProject();
          void createNewNetProject(const QString &title);

    private slots:
          void messageToStatus(const QString &);
          void preferences();
          void aboutTupi();
          void showTipDialog();
          void importPalettes();
          void connectToDisplays(const QWidget *widget);
          void saveProject();
          void openRecentProject();
          void createCommand(const KTPaintAreaEvent *event);
          void callSave();
          void expandExposureView(int index);
          void expandColorView();

    private:
          KTProjectManager *m_projectManager;
          Tupi::RenderType m_renderType;
          QString m_fileName;

    private:
          KTViewDocument *drawingTab;
          KTAnimationspace *animationTab;
          KTHelpBrowser *helpTab;
          KTwitterWidget *newsTab;
          KTStatusBar *m_statusBar;
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

    private: // Network variables
          KTNetProjectManagerHandler *netProjectManagerHandler;
          bool m_isNetworkProject;
          KToolView *m_viewChat;

    // Components
    private:
          KTExposureSheet *m_exposureSheet;
          KTScenesWidget *m_scenes;
          KTTimeLine *m_timeLine;

#if defined(QT_GUI_LIB) && defined(K_DEBUG)
          KTDebugWidget *m_debug;
#endif
          KTHelpWidget *m_helper;
          KTLibraryWidget *m_libraryWidget;
          KTColorPalette *m_colorPalette;
          KTPenWidget *m_penWidget;
          KToolView *exposureView;
          KToolView *colorView;
          KToolView *penView;
          KToolView *libraryView;
          KToolView *scenesView;  
          KToolView *helpView;
          KToolView *timeView;
          KToolView *debugView;
          KToolView *exportView;
          KTViewCamera *viewCamera;
          bool isSaveDialogOpen; 
          // bool projectSaved;
          bool internetOn;
          int lastTab;
          KTProject::Mode contextMode;
          KTMainWindow::RequestType requestType; 
          QString projectName;

    signals:
          void responsed(KTProjectResponse *);
          void updateAnimationModule(KTProject *, int, int, int);
};

#endif
