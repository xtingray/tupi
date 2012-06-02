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

#include "tupmainwindow.h"
#include "tupapplication.h"

#include "tcommandhistory.h"
#include "toolview.h"
#include "tviewbutton.h"
#include "taudioplayer.h"
#include "tdebug.h"

#include <QKeySequence>
#include <QTextBrowser>
#include <QToolBar>
#include <QDesktopWidget>

/**
 * This class implements the Tupi GUI
 * Here is where all the Tupi GUI components are defined 
 * @author David Cuadrado 
*/

/**
 * @if english
 * This method includes the widgets around the border of the main window
 * @endif
 * @if spanish
 * Este metodo incluye los componentes alrededor del borde la ventana principal
 * @endif
*/

void TupMainWindow::createGUI()
{
    // Adding the color palette to the left side of the interface 

    m_colorPalette = new TupColorPalette;
    colorView = addToolView(m_colorPalette, Qt::LeftDockWidgetArea, Animation, "Color Palette", QKeySequence(tr("Shift+P")));
    //colorView->setShortcut(QKeySequence(tr("Shift+P")));

    m_actionManager->insert(colorView->toggleViewAction(), "show palette");
    addToPerspective(colorView->toggleViewAction(), Animation);

    connectToDisplays(m_colorPalette);
    connectWidgetToPaintArea(m_colorPalette);

    // Adding the pen parameters widget to the left side of the interface 

    m_penWidget = new TupPenWidget;
    penView = addToolView(m_penWidget, Qt::LeftDockWidgetArea, Animation, "Pen", QKeySequence(tr("Shift+B")));
    m_actionManager->insert(penView->toggleViewAction(), "show pen");
    addToPerspective(penView->toggleViewAction(), Animation);

    connectToDisplays(m_penWidget);
    connectWidgetToPaintArea(m_penWidget);

    // Adding the objects library widget to the left side of the interface

    m_libraryWidget = new TupLibraryWidget(this);
    m_libraryWidget->setLibrary(m_projectManager->project()->library());

    libraryView = addToolView(m_libraryWidget, Qt::LeftDockWidgetArea, Animation, "Library", QKeySequence(tr("Shift+L")));
    m_actionManager->insert(libraryView->toggleViewAction(), "show library");
    addToPerspective(libraryView->toggleViewAction(), Animation);
    connectToDisplays(m_libraryWidget);

    new TAction(QPixmap(THEME_DIR + "icons/bitmap.png"), tr("Bitmap"), QKeySequence(tr("Alt+B")), m_libraryWidget, SLOT(importBitmap()),
                m_actionManager, "importbitmap");

    new TAction(QPixmap(THEME_DIR + "icons/bitmap_array.png"), tr("Bitmap Array"), QKeySequence(tr("Alt+Shift+B")), 
                m_libraryWidget, SLOT(importBitmapArray()), m_actionManager, "importbitmaparray");

    new TAction(QPixmap(THEME_DIR + "icons/svg.png"), tr("SVG File"), QKeySequence(tr("Alt+S")), m_libraryWidget, SLOT(importSvg()),
                m_actionManager, "importsvg");

    new TAction(QPixmap(THEME_DIR + "icons/svg_array.png"), tr("SVG Array"), QKeySequence(tr("Alt+Shift+S")), m_libraryWidget, 
                SLOT(importSvgArray()), m_actionManager, "importsvgarray");

    //new TAction(QPixmap(), tr("Audio File..."), QKeySequence(), m_libraryWidget, SLOT(importSound()),
    //            m_actionManager, "importaudiofile");

    // SQA: Temporary code
    //m_actionManager->enable("importsvg", false);
    //m_actionManager->enable("importsvgarray", false);

    connectWidgetToManager(m_libraryWidget);
    connectWidgetToLocalManager(m_libraryWidget);

    // Adding the scenes widget to the right side of the interface

    m_scenes = new TupScenesWidget;
    scenesView = addToolView(m_scenes, Qt::RightDockWidgetArea, Animation, "Scenes Manager", QKeySequence(tr("Shift+C")));
    m_actionManager->insert(scenesView->toggleViewAction(), "show scenes");
    addToPerspective(scenesView->toggleViewAction(), Animation);

    connectWidgetToManager(m_scenes);
    connectWidgetToLocalManager(m_scenes);
    connectToDisplays(m_scenes);

    // Adding the exposure sheet to the right side of the interface
    m_exposureSheet = new TupExposureSheet;
    exposureView = addToolView(m_exposureSheet, Qt::RightDockWidgetArea, Animation, "Exposure Sheet", QKeySequence(tr("Shift+E")));
    m_actionManager->insert(exposureView->toggleViewAction(), "show exposure");
    addToPerspective(exposureView->toggleViewAction(), Animation);

    connectWidgetToManager(m_exposureSheet);
    connectWidgetToLocalManager(m_exposureSheet);
    connectToDisplays(m_exposureSheet);

    // Adding the help widget to the right side of the interface

    m_helper = new TupHelpWidget(SHARE_DIR + "data/help/");
    helpView = addToolView(m_helper, Qt::RightDockWidgetArea, All, "Help", QKeySequence(tr("Shift+H")));
    m_actionManager->insert(helpView->toggleViewAction(), "show help");
    addToPerspective(helpView->toggleViewAction(), All);

    TViewButton *helpButton = helpView->button();

    connect(helpButton, SIGNAL(helpIsOpen()), this,
            SLOT(setHelpPerspective()));

    connect(m_helper, SIGNAL(pageLoaded(const QString &)), this, 
            SLOT(showHelpPage(const QString &)));
    connectToDisplays(m_helper);

    // Adding the time line widget to the bottom side of the interface
    m_timeLine = new TupTimeLine;
    m_timeLine->setLibrary(m_projectManager->project()->library());
    timeView = addToolView(m_timeLine, Qt::BottomDockWidgetArea, Animation, "Time Line", QKeySequence(tr("Shift+T")));
    m_actionManager->insert(timeView->toggleViewAction(), "show timeline");
    addToPerspective(timeView->toggleViewAction(), Animation);

    connectWidgetToManager(m_timeLine);
    connectWidgetToLocalManager(m_timeLine);
    connectToDisplays(m_timeLine);

#if defined(QT_GUI_LIB) && defined(K_DEBUG)
    QDesktopWidget desktop;
    m_debug = new TupDebugWidget(this, desktop.screenGeometry().width());
    debugView = addToolView(m_debug, Qt::BottomDockWidgetArea, Animation, "Debug Term", QKeySequence(tr("Shift+D")));
    m_actionManager->insert(debugView->toggleViewAction(), "show debug");
    addToPerspective(debugView->toggleViewAction(), Animation);
#endif

    // Adding the script editor to the bottom side, if kinas was enabled
    /*
#ifdef ENABLE_KINAS
    KinasWidget *m_scriptEditor = new KinasWidget;
    addToolView(m_scriptEditor, Qt::BottomDockWidgetArea, Animation, "Tupi Script", QKeySequence(tr("Shift+K"));
#endif
    */

    enableToolViews(false);
}

/**
 * @if english
 * This method links widgets with status displayer
 * @endif
 * @if spanish
 * Este metodo enlaza widgets con el visualizador de estados
 * @endif
 */

void TupMainWindow::connectToDisplays(const QWidget *widget)
{
    connect(widget, SIGNAL(sendToStatus(const QString &)), this, SLOT(messageToStatus(const QString &)));
}

/**
 * @if english
 * This method defines the main menu for the application
 * @endif
 * @if spanish
 * Este metodo define el menu principal para la aplicacion
 * @endif
*/

void TupMainWindow::setupMenu()
{
    // Setting up the file menu
    setupFileActions();

    // Menu File	
    m_fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(m_fileMenu);

    // Adding Option New	
    /*
    QMenu *newMenu = new QMenu(tr("&New"), this);
    newMenu->setIcon(QPixmap(THEME_DIR + "icons/file_new.png"));
    m_fileMenu->addMenu(newMenu);
    newMenu->addAction(m_actionManager->find("newproject"));
    newMenu->addSeparator();
    */

    m_fileMenu->addAction(m_actionManager->find("newproject"));
    m_fileMenu->addAction(m_actionManager->find("openproject"));
    m_fileMenu->addAction(m_actionManager->find("opennetproject"));
    m_fileMenu->addAction(m_actionManager->find("exportprojectserver"));

    // Adding Option Open Recent	
    m_recentProjectsMenu = new QMenu(tr("Recents"), this);
    // m_recentProjectsMenu->setIcon(QPixmap(THEME_DIR + "icons/recent_files.png"));

    TCONFIG->beginGroup("General");
    QStringList recents = TCONFIG->value("Recents").toString().split(';');
    updateOpenRecentMenu(m_recentProjectsMenu, recents);	
    m_fileMenu->addMenu(m_recentProjectsMenu);

    // Adding Options save, save as, close, export, import palettes and exit	
    m_fileMenu->addAction(m_actionManager->find("saveproject"));

    m_fileMenu->addAction(m_actionManager->find("saveprojectas"));
    m_fileMenu->addAction(m_actionManager->find("closeproject"));

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actionManager->find("export"));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actionManager->find("ImportPalettes"));
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actionManager->find("Exit"));
    m_fileMenu->addSeparator();

    // Setting up the Settings menu
    setupSettingsActions();
    m_settingsMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(m_settingsMenu);

    // Adding Options wizard and preferences
    //m_settingsMenu->addAction(m_actionManager->find("wizard"));
    m_settingsMenu->addAction(m_actionManager->find("preferences"));
    // Temporary out while SQA is done
    // m_actionManager->enable("preferences", false);

// Temporary out while SQA is done
    // Setting up the insert menu
    // setupInsertActions();
    // Menu Insert
    m_insertMenu = new QMenu(tr("&Import"), this);
    menuBar()->addMenu(m_insertMenu);

    // Adding Options insert scene, insert layer and insert frame
    /*
    m_insertMenu->addAction(m_actionManager->find("InsertScene"));
    m_insertMenu->addAction(m_actionManager->find("InsertLayer"));
    m_insertMenu->addAction(m_actionManager->find("InsertFrame"));
    m_insertMenu->addSeparator();
    */

    // Adding Options import bitmap and import audio file
    m_insertMenu->addAction(m_actionManager->find("importbitmap"));
    m_insertMenu->addAction(m_actionManager->find("importbitmaparray"));
    m_insertMenu->addAction(m_actionManager->find("importsvg"));
    m_insertMenu->addAction(m_actionManager->find("importsvgarray"));

    //m_insertMenu->addAction(m_actionManager->find("importaudiofile"));

    // Setting up the window menu
    // setupWindowActions();
    m_windowMenu = new QMenu(tr("&Window"),this);
    menuBar()->addMenu(m_windowMenu);

    // Adding Options show debug, palette, pen, library, timeline, scenes, exposure, help
    m_windowMenu->addAction(m_actionManager->find("show palette"));
    m_windowMenu->addAction(m_actionManager->find("show pen"));
    m_windowMenu->addAction(m_actionManager->find("show library"));
    m_windowMenu->addAction(m_actionManager->find("show timeline"));
    m_actionManager->enable("show timeline", false);
    m_windowMenu->addAction(m_actionManager->find("show scenes"));
    m_windowMenu->addAction(m_actionManager->find("show exposure"));
    m_windowMenu->addAction(m_actionManager->find("show help"));

#if defined(QT_GUI_LIB) && defined(K_DEBUG)
    m_windowMenu->addAction(m_actionManager->find("show debug"));
#endif

    // m_actionManager->enable("show help", false);
    m_windowMenu->addSeparator();

    // Setup perspective menu
    m_viewMenu = new QMenu(tr("Modules"),this);
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);

    // Adding Option Animation
    QAction *drawingPerspective = new QAction(tr("Animation"), this);
    drawingPerspective->setIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")); 
    drawingPerspective->setIconVisibleInMenu(true);
    drawingPerspective->setShortcut(QKeySequence("Ctrl+1"));
    drawingPerspective->setData(Animation);
    group->addAction(drawingPerspective);

    // Adding Option Player 
    QAction *animationPerspective = new QAction(tr("Player"), this);
    animationPerspective->setIcon(QPixmap(THEME_DIR + "icons/play_small.png"));
    animationPerspective->setIconVisibleInMenu(true);
    animationPerspective->setShortcut(QKeySequence("Ctrl+2"));
    animationPerspective->setData(Player);
    group->addAction(animationPerspective);

   // Adding Option Help 
    QAction *helpPerspective = new QAction(tr("Help"), this);
    helpPerspective->setIcon(QPixmap(THEME_DIR + "icons/help_mode.png"));
    helpPerspective->setIconVisibleInMenu(true);
    helpPerspective->setShortcut(QKeySequence("Ctrl+3"));
    helpPerspective->setData(Help);
    group->addAction(helpPerspective);

   // Adding Option News 
    QAction *newsPerspective = new QAction(tr("News"), this);
    newsPerspective->setIcon(QPixmap(THEME_DIR + "icons/news_mode.png"));
    newsPerspective->setIconVisibleInMenu(true);
    newsPerspective->setShortcut(QKeySequence("Ctrl+4"));
    newsPerspective->setData(News);
    group->addAction(newsPerspective);

    m_viewMenu->addActions(group->actions());
    connect(group, SIGNAL(triggered(QAction *)), this, SLOT(changePerspective(QAction *)));
    menuBar()->addMenu(m_viewMenu);
	
    // Setting up the help menu
    setupHelpActions();
    m_helpMenu = new QMenu(tr("&Help"),this);
    menuBar()->addMenu(m_helpMenu);
    m_helpMenu->addAction(m_actionManager->find("tipofday"));
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_actionManager->find("about tupi"));

    setMenuItemsContext(false);
}

void TupMainWindow::setMenuItemsContext(bool flag)
{
    m_actionManager->enable("saveproject", flag);
    m_actionManager->enable("saveprojectas", flag);
    m_actionManager->enable("closeproject", flag);
    m_actionManager->enable("export", flag);
    m_actionManager->enable("importbitmap", flag);

    m_insertMenu->setEnabled(flag);
    m_windowMenu->setEnabled(flag);
    m_viewMenu->setEnabled(flag);
}

void TupMainWindow::setupActions()
{
/*
    TAction *next = new TAction(QPixmap(), tr( "Back Frame" ), QKeySequence(Qt::Key_PageUp), this, 
                    SLOT(selectBackFrame()), m_actionManager, "BackFrame");
    next->setShortcutContext ( Qt::ApplicationShortcut );
    TAction *back = new TAction( QPixmap(), tr( "Next Frame" ), QKeySequence(Qt::Key_PageDown), this, 
                    SLOT(selectNextFrame()), m_actionManager, "Next Frame");
    back->setShortcutContext ( Qt::ApplicationShortcut );
    addAction(back);
    addAction(next);
*/
}

/**
 * @if english
 * This method defines the actions for the options in the menu File
 * @endif
 * @if spanish
 * Este metodo define las acciones para las opciones del menu Archivo
 * @endif
*/

void TupMainWindow::setupFileActions()
{
    TAction *newProject = new TAction(QPixmap(THEME_DIR + "icons/new.png"), tr("New project"), QKeySequence(tr("Ctrl+N")),
                                      this, SLOT(newProject()), m_actionManager);
    newProject->setStatusTip(tr("Open new project"));
    m_actionManager->insert(newProject, "newproject", "file");

    TAction *openFile = new TAction(QPixmap(THEME_DIR + "icons/open.png"), tr("Open project"), QKeySequence(tr("Ctrl+O")), 
                                    this, SLOT(openProject()), m_actionManager);
    m_actionManager->insert( openFile, "openproject", "file" );
    openFile->setStatusTip(tr("Load existent project"));

    TAction *openNetFile = new TAction(QPixmap(THEME_DIR + "icons/net_document.png"), tr("Open project from server..."), 
                                       tr(""), this, SLOT(openProjectFromServer()), m_actionManager);
    m_actionManager->insert(openNetFile, "opennetproject", "file");

    TAction *importNetFile = new TAction(QPixmap(THEME_DIR + "icons/import_project.png"), tr("Export project to server..."), tr(""), this, 
                                         SLOT(importProjectToServer()), m_actionManager);
    m_actionManager->insert(importNetFile, "exportprojectserver", "file");

    TAction *save = new TAction(QPixmap(THEME_DIR + "icons/save.png"), tr( "Save project" ),
                                QKeySequence(tr("Ctrl+S")), this, SLOT(saveProject()), m_actionManager);
    m_actionManager->insert(save, "saveproject", "file");
    save->setStatusTip(tr("Save current project in current location"));

    TAction *saveAs = new TAction(QPixmap(THEME_DIR + "icons/save_as.png"), tr("Save project &As..."), 
                                  QKeySequence(tr("Ctrl+Shift+S")), m_actionManager);

    connect(saveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    saveAs->setStatusTip(tr("Open dialog box to save current project in any location"));
    m_actionManager->insert(saveAs, "saveprojectas", "file");

    TAction *close = new TAction(QPixmap(THEME_DIR + "icons/close.png"), tr("Cl&ose project"), 
                                 QKeySequence(tr("Ctrl+W")), m_actionManager);
    connect(close, SIGNAL(triggered()), this, SLOT(closeProject()));
    close->setStatusTip(tr("Close active project"));
    m_actionManager->insert(close, "closeproject", "file");

    // Import Palette action

    TAction *importPalette = new TAction(QPixmap(THEME_DIR + "icons/import.png"), tr("&Import GIMP palettes"),
                                         QKeySequence(tr("Ctrl+G")), this, SLOT(importPalettes()), m_actionManager);
    importPalette->setStatusTip(tr("Import palettes"));
    m_actionManager->insert(importPalette, "importpalettes", "file");

    // Export Project action
    TAction *exportProject = new TAction(QPixmap(THEME_DIR + "icons/export.png"), tr("&Export Project"), QKeySequence(tr("Ctrl+R")),
                                         this, SLOT(exportProject()), m_actionManager);
    exportProject->setStatusTip(tr("Export project to several video formats"));
    m_actionManager->insert(exportProject, "export", "file");

    // Exit action
    TAction *exit = new TAction(QPixmap(THEME_DIR + "icons/exit.png"), tr("E&xit"), QKeySequence(tr("Ctrl+Q")),
                                qApp, SLOT(closeAllWindows()), m_actionManager);
    exit->setStatusTip(tr("Close application"));
    m_actionManager->insert(exit, "exit", "file");

    // when the last window is closed, the application should quit
    connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
}

/**
 * @if english
 * This method defines the actions for the options in the menu Settings
 * @endif
 * @if spanish
 * Este metodo define las acciones para las opciones del menu Configuracion
 * @endif
*/

void TupMainWindow::setupSettingsActions()
{
    TAction *wizard = new TAction(tr("Launch configuration wizard..."), QKeySequence(), 
                                  qobject_cast<TupApplication*>(qApp), SLOT(firstRun()), m_actionManager, "wizard");
    wizard->setStatusTip(tr("Launch first configuration wizard"));

    TAction * preferences = new TAction(QPixmap(THEME_DIR + "icons/properties.png"), tr("Pr&eferences..."), 
                                        QKeySequence(tr("Ctrl+P")), this, SLOT( preferences()),
                                        m_actionManager, "preferences");
    preferences->setStatusTip(tr("Opens the preferences dialog box"));
}

/**
 * @if english
 * This method defines the actions for the options in the menu Help
 * @endif
 * @if spanish
 * Este metodo define las acciones para las opciones del menu Ayuda
 * @endif
*/

void TupMainWindow::setupHelpActions()
{
    new TAction(QPixmap(THEME_DIR + "icons/about.png"), tr("About Tupi"), QKeySequence(tr("Ctrl+K")), this, SLOT(aboutTupi()), m_actionManager, "about tupi");
    new TAction(QPixmap(THEME_DIR + "icons/today_tip.png"), tr("Tip of the day"), QKeySequence(tr("Ctrl+T")), this, SLOT(showTipDialog()), m_actionManager, "tipofday");
}

/**
 * @if english
 * This method defines the actions for the options in the menu Window
 * @endif
 * @if spanish
 * Este metodo define las acciones para las opciones del menu Ventana
 * @endif

void TupMainWindow::setupWindowActions()
{
    // Temporary commented code - SQA required 
    #if defined(QT_GUI_LIB) && defined(K_DEBUG)
        new TAction(QPixmap(), tr("Show Debug Dialog"), QKeySequence(), TDebug::browser(), SLOT(show()), m_actionManager,
                    "show debug");
    #endif
}
*/

/**
 * @if english
 * This method defines the actions for the options in the menu Insert
 * @endif
 * @if spanish
 * Este metodo define las acciones para las opciones del menu Insertar
 * @endif
*/

void TupMainWindow::setupInsertActions()
{
/*
    new TAction(QPixmap(THEME_DIR + "icons/scene.png"), tr("Insert scene"), QKeySequence(), m_scenes, 
                SLOT(emitRequestInsertScene()), m_actionManager, "InsertScene");

    new TAction(QPixmap(THEME_DIR + "icons/layer.png"), tr("Insert layer"), QKeySequence(), m_exposureSheet, 
                SLOT(createLayer()), m_actionManager, "InsertLayer");

    new TAction(QPixmap(THEME_DIR + "icons/frame.png"), tr("Insert frame"), QKeySequence(), m_projectManager, 
                SLOT(createFrame()), m_actionManager, "InsertFrame");
*/
}

/**
 * @if english
 * This method defines the Toolbar and its actions
 * @endif
 * @if spanish
 * Este metodo define la barra de herramientas y sus acciones
 * @endif
*/

void TupMainWindow::setupToolBar()
{
    QToolBar * toolbar = new QToolBar(tr("Bar Actions"), this);
    toolbar->setIconSize(QSize(22,22));
    addToolBar(Qt::TopToolBarArea, toolbar);

    toolbar->addAction(m_actionManager->find("newproject"));
    toolbar->addAction(m_actionManager->find("openproject"));
    toolbar->addAction(m_actionManager->find("opennetproject"));
    toolbar->addAction(m_actionManager->find("saveproject"));
    toolbar->addAction(m_actionManager->find("saveprojectas"));
    toolbar->addAction(m_actionManager->find("closeproject"));
}

/**
 * @if english
 * This method updates the entries in the recent menu option
 * @endif
 * @if spanish
 * Este metodo actualiza las entradas en la opcion "Recientes" del menu
 * @endif
*/

void TupMainWindow::updateOpenRecentMenu(QMenu *menu, QStringList recents)
{	
    int i = 0;
    QAction *action[recents.length()];

    menu->clear();
    m_recentProjects.clear();

    foreach (QString recent, recents) {

             // if (!recent.isEmpty() && m_recentProjects.indexOf(recent) == -1) {
             if (!recent.isEmpty()) {

                 m_recentProjects << recent;
                 action[i] = new QAction(QPixmap(THEME_DIR + "icons/recent_files.png"), recent, this); 
                 action[i]->setIconVisibleInMenu(true);
                 menu->addAction(action[i]);
                 connect(action[i], SIGNAL(triggered()), this, SLOT(openRecentProject()));
                 i++;

             } else {

                 m_recentProjectsMenu->setEnabled(false);
                 return; 

             }
    }

    if (i>0 && !m_recentProjectsMenu->isEnabled())
        m_recentProjectsMenu->setEnabled(true);
}

void TupMainWindow::showWidgetPage()
{
/*
    TAction *action = qobject_cast<TAction *>(sender());

    if (action) {
        QWidget *widget = 0;
        DiDockWidget::Position position;
        QString actionText = "";
 		
        if ( action == m_actionManager->find("show timeline") ) {
            widget = m_timeLine;
            position = DiDockWidget::Bottom;
            actionText = "time line widget";
        } else if ( action == m_actionManager->find("show exposure") ) {
            widget = m_exposureSheet;
            position = DiDockWidget::Right;
            actionText = "exposure widget";
        } else if ( action == m_actionManager->find("show library") ) {
            widget = m_libraryWidget;
            position = DiDockWidget::Left;
            actionText = "library widget";
	}
	else if ( action == m_actionManager->find("show scenes") ) {
            widget = m_scenes;
            position = DiDockWidget::Right;
            actionText = "scenes widget";
        } else if ( action == m_actionManager->find("show help") ) {
            widget = m_helper;
            position = DiDockWidget::Right;
            actionText = "help widget";
 	} else if ( action == m_actionManager->find("show palette") ) {
            widget = m_colorPalette;
            position = DiDockWidget::Left;
            actionText = "color palette widget";
        }

        if ( widget ) {
            if ( widget->isVisible() ) {
                toolWindow( position)->centralWidget()->setExpanded(false);
                action->setText("Show "+actionText);
            } else {
                toolWindow( position)->centralWidget()->raiseWidget(widget);
                action->setText("Hide "+actionText);
            }
        }
    }
*/
}

/**
 * @if english
 * This method changes the perspective view according the events
 * @endif
 * @if spanish
 * Este metodo cambia la vista de perspectiva de acuerdo a los eventos
 * @endif
*/

void TupMainWindow::changePerspective(QAction *a)
{
    int perspective = a->data().toInt();

    // Animation or Player perspective
    if (perspective == Animation || perspective == Player) {
        setCurrentTab(perspective - 1);
    } else { 
        if (perspective == Help) // Help perspective 
            setCurrentTab(2);
        else if (perspective == News) // News perspective
                 setCurrentTab(3);
    }

    a->setChecked(true);
}

void TupMainWindow::setHelpPerspective()
{
    setCurrentTab(2);
}

void TupMainWindow::setUndoRedoActions()
{
    // Setting undo/redo actions
    QAction *undo = m_projectManager->undoHistory()->createUndoAction(this, tr("Undo"));
    undo->setIcon(QPixmap(THEME_DIR + "icons/undo.png"));
    undo->setIconVisibleInMenu(true);
    undo->setShortcut(QKeySequence(tr("Ctrl+Z")));
    //undo->setShortcut(QKeySequence(QKeySequence::Undo));

    QAction *redo =  m_projectManager->undoHistory()->createRedoAction(this, tr("Redo"));
    redo->setIcon(QPixmap(THEME_DIR + "icons/redo.png"));
    redo->setIconVisibleInMenu(true);
    redo->setShortcut(QKeySequence(tr("Ctrl+Y")));
    //redo->setShortcut(QKeySequence(QKeySequence::Redo));

    kApp->insertGlobalAction(undo, "undo");
    kApp->insertGlobalAction(redo, "redo");
}
