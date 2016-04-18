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

#include "tupmainwindow.h"
#include "tuptwitter.h"
#include "tupnewproject.h"
#include "tupabout.h"
#include "tuppackagehandler.h"
#include "tuppaletteimporter.h"
#include "tuppaintareaevent.h"
#include "tuppaintareacommand.h"

// Tupi Framework
#include "tipdialog.h"
#include "tosd.h"
// #include "taudioplayer.h"

#include "tupapplication.h"
#include "tuppluginmanager.h"
#include "tupprojectcommand.h"
#include "tuplocalprojectmanagerhandler.h"

// #ifdef USE_NET
#include "tupnetprojectmanagerparams.h"
#include "tupconnectdialog.h"
#include "tuplistpackage.h"
#include "tupimportprojectpackage.h"
#include "tuplistprojectspackage.h"
#include "tupsavepackage.h"
// #end

// Qt
#include <QImage>
#include <QPixmap>
#include <QResizeEvent>
#include <QMenu>
#include <QCloseEvent>
#include <QTextEdit>
#include <QFileDialog>
#include <QDomDocument>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QThread>
#include <QClipboard>

/**
 * This class defines the main window application.
 * Here is where all the Tupi GUI is initialized 
 * @author David Cuadrado
*/

/**
 * @if english
 * This is the constructor method for this class.
 * @endif
 * @if spanish
 * Este es el metodo constructor para esta clase.
 * @endif
 * @return 
*/

TupMainWindow::TupMainWindow() : TabbedMainWindow(), m_projectManager(0), animationTab(0), playerTab(0), 
               m_viewChat(0), m_exposureSheet(0), m_scenes(0), isSaveDialogOpen(false), internetOn(false)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow()]";
        #else
            TINIT;
        #endif
    #endif

    QFile file(THEME_DIR + "config/ui.qss");
    if (file.exists()) {
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        if (styleSheet.length() > 0)
            setStyleSheet(styleSheet);
        file.close();
    } else {
        #ifdef K_DEBUG
            QString msg = "TupMainWindow::TupMainWindow() - theme file doesn't exist -> " + QString(THEME_DIR + "config/ui.qss"); 
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
        
    // Loading audio player plugin
    // TAudioPlayer::instance()->loadEngine("gstreamer"); // FIXME: Move this to the settings 

    setObjectName("TupMainWindow_");

    // Naming the main frame...
    setWindowTitle(tr("Tupi: Open 2D Magic"));
    setWindowIcon(QIcon(THEME_DIR + "icons/about.png"));

    // Defining the render type for the drawings
    // m_renderType = Tupi::RenderType(TCONFIG->value("RenderType").toInt());

    // Calling out the project manager
    m_projectManager = new TupProjectManager(this);

    // Calling out the events/actions manager
    m_actionManager = new TActionManager(this);

    // Setting up all the GUI...
    createGUI(); // This method is called from the tupmainwindow_gui class
    setupMenu();
    setupToolBar();

    // Check if user wants to see a Tupi tip for every time he launches the program
    TCONFIG->beginGroup("General");
    bool showTips = TCONFIG->value("ShowTipOfDay", true).toBool();

    // If option is enabled, then, show a little dialog with a nice tip
    if (showTips)
        QTimer::singleShot(0, this, SLOT(showTipDialog()));

    // Time to load plugins... 
    TupPluginManager::instance()->loadPlugins();

    // Defining the Animation view, as the first interface to show up   
    setCurrentPerspective(Animation);

    if (TCONFIG->firstTime()) {
        TCONFIG->beginGroup("General");
        TCONFIG->setValue("OpenLastProject", false);
        TCONFIG->setValue("ShowTipOfDay", true);
        TCONFIG->setValue("ConfirmRemoveFrame", true); 
        TCONFIG->setValue("ConfirmRemoveLayer", true); 
        TCONFIG->setValue("ConfirmRemoveScene", true); 
        TCONFIG->setValue("ConfirmRemoveObject", true); 
        TCONFIG->beginGroup("PaintArea");
        TCONFIG->setValue("GridColor", "#0000b4");
        TCONFIG->setValue("GridSeparation", 10);
    }

    TupMainWindow::requestType = None;
    lastSave = false;
}

/**
 * @if english
 * This is the destructor method for this class.
 * @endif
 * @if spanish
 * Este es el metodo destructor para esta clase.
 * @endif
*/
TupMainWindow::~TupMainWindow()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupMainWindow()]";
        #else
            TEND;
        #endif
    #endif

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear(QClipboard::Clipboard);

    delete TupPluginManager::instance();
    delete TOsd::self();
 
    delete m_projectManager;
    delete penView;
}

/**
 * @if english
 * This method cleans and set the whole interface to start a new project.
 * @endif
 * @if spanish
 * Este metodo limpia y configura toda la interfaz para iniciar un nuevo proyecto.
 * @endif
*/

void TupMainWindow::createNewLocalProject()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::createNewLocalProject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupMainWindow::requestType = NewLocalProject;
    m_projectManager->setupNewProject();
    m_projectManager->setOpen(true);
 
    enableToolViews(true);
    setMenuItemsContext(true);

    setWorkSpace();
}

void TupMainWindow::createNewNetProject(const QString &title, const QStringList &users)
{
    isNetworked = true;
    projectName = title;
    setWindowTitle(tr("Tupi: Open 2D Magic") + " - " + projectName + " " + tr("[ by %1 | net mode ]").arg(netUser));

    if (m_viewChat) {
        removeToolView(m_viewChat);
        delete m_viewChat;
    }

    m_viewChat = addToolView(netProjectManager->communicationWidget(), Qt::BottomDockWidgetArea, All, "Chat");
    m_viewChat->setVisible(false);

    enableToolViews(true);
    setMenuItemsContext(true);
    m_exposureSheet->updateFramesState();
    m_projectManager->setOpen(true);

    setWorkSpace(users);
}

/**
 * @if english
 * This method supports all the low level tasks for the method createNewLocalProject().
 * @endif
 * @if spanish
 * Este metodo soporta todas las tareas de bajo nivel para el metodo createNewLocalProject().
 * @endif
*/

void TupMainWindow::setWorkSpace(const QStringList &users)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::setWorkSpace()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    // Downloading maefloresta Twitter status
    TupTwitter *twitter = new TupTwitter();
    twitter->start();
    connect(twitter, SIGNAL(pageReady()), this, SLOT(addTwitterPage()));

    if (m_projectManager->isOpen()) {
        if (TupMainWindow::requestType == NewLocalProject || TupMainWindow::requestType == NewNetProject)
            TOsd::self()->display(tr("Information"), tr("Opening a new document..."));

        contextMode = TupProject::FRAMES_EDITION;

        // Setting undo/redo actions
        setUndoRedoActions();

        animationTab = new TupDocumentView(m_projectManager->project(), this, isNetworked, users);

        TCONFIG->beginGroup("Network");
        QString server = TCONFIG->value("Server").toString();
        if (isNetworked && server.compare("tupitu.be") == 0) {
            connect(animationTab, SIGNAL(requestExportImageToServer(int, int, const QString &, const QString &, const QString &)),                         
                    netProjectManager, SLOT(sendExportImageRequest(int, int, const QString &, const QString &, const QString &)));
            connect(animationTab, SIGNAL(updateStoryboard(TupStoryboard *, int)), netProjectManager, SLOT(updateStoryboardRequest(TupStoryboard *, int)));
            connect(animationTab, SIGNAL(postStoryboard(int)), netProjectManager, SLOT(postStoryboardRequest(int))); 
        }

        animationTab->setWindowTitle(tr("Animation"));
        addWidget(animationTab);

        connectWidgetToManager(animationTab);
        connectWidgetToLocalManager(animationTab);
        connect(animationTab, SIGNAL(modeHasChanged(TupProject::Mode)), this, SLOT(expandExposureView(TupProject::Mode))); 
        connect(animationTab, SIGNAL(colorChangedFromFullScreen(const QColor &)), this, SLOT(updatePenColor(const QColor &)));
        // connect(animationTab, SIGNAL(updatePenFromFullScreen(const QPen &)), this, SLOT(updatePenWidth(const QPen &)));
        connect(animationTab, SIGNAL(projectSizeHasChanged(const QSize)), this, SLOT(resizeProjectDimension(const QSize))); 
        connect(animationTab, SIGNAL(newPerspective(int)), this, SLOT(changePerspective(int)));
        connect(animationTab, SIGNAL(contourColorChanged(const QColor &)), m_colorPalette, SLOT(updateContourColor(const QColor &))); 
        connect(animationTab, SIGNAL(fillColorChanged(const QColor &)), m_colorPalette, SLOT(updateFillColor(const QColor &)));
        connect(animationTab, SIGNAL(bgColorChanged(const QColor &)), m_colorPalette, SLOT(updateBgColor(const QColor &)));
        connect(animationTab, SIGNAL(penWidthChanged(int)), this, SLOT(updatePenWidth(int)));

        animationTab->setAntialiasing(true);

        int width = animationTab->workSpaceSize().width();
        int height = animationTab->workSpaceSize().height();
        animationTab->setWorkSpaceSize(width, height);

        TupProject *project = m_projectManager->project();
        int pWidth = project->dimension().width();
        int pHeight = project->dimension().height();

        double proportion = 1;
        if (pWidth > pHeight)
            proportion = (double) width / (double) pWidth;
        else
            proportion = (double) height / (double) pHeight;

        if (proportion <= 0.5) {
            animationTab->setZoomPercent("20");
        } else if (proportion > 0.5 && proportion <= 0.75) {
                   animationTab->setZoomPercent("25");
        } else if (proportion > 0.75 && proportion <= 1.5) {
                   animationTab->setZoomPercent("50");
        } else if (proportion > 1.5 && proportion < 2) {
                   animationTab->setZoomPercent("75");
        }

        // TupCamera Widget
        cameraWidget = new TupCameraWidget(m_projectManager->project(), isNetworked);
        connectWidgetToManager(cameraWidget);

        m_libraryWidget->setNetworking(isNetworked);

        if (isNetworked) {
            connect(cameraWidget, SIGNAL(requestForExportVideoToServer(const QString &, const QString &, const QString &, int, const QList<int>)), 
                    netProjectManager, SLOT(sendVideoRequest(const QString &, const QString &, const QString &, int, const QList<int>)));
        } else {
            connect(animationTab, SIGNAL(autoSave()), this, SLOT(callSave()));
        }

        playerTab = new TupAnimationspace(cameraWidget);
        playerTab->setWindowIcon(QIcon(THEME_DIR + "icons/play_small.png"));
        playerTab->setWindowTitle(tr("Player"));
        connect(playerTab, SIGNAL(newPerspective(int)), this, SLOT(changePerspective(int)));
        addWidget(playerTab);
        connect(animationTab, SIGNAL(updateFPS(int)), cameraWidget, SLOT(setStatusFPS(int)));

        exposureView->expandDock(true);

        // SQA: Code useful for future features
        // if (!isNetworked) 
        //     connect(animationTab, SIGNAL(autoSave()), this, SLOT(callSave()));

        m_projectManager->undoModified();
        m_colorPalette->init();
        m_colorPalette->setBgColor(project->bgColor());

        TCONFIG->beginGroup("PenParameters");
        int thickness = TCONFIG->value("Thickness", -1).toInt();
        m_penWidget->init(thickness);
        // m_penWidget->setThickness(thickness);

        if (TupMainWindow::requestType == OpenLocalProject || TupMainWindow::requestType == OpenNetProject)
            TOsd::self()->display(tr("Information"), tr("Project <b>%1</b> opened!").arg(m_projectManager->project()->projectName()));

        m_exposureSheet->setScene(0);
        connect(this, SIGNAL(tabHasChanged(int)), this, SLOT(updateCurrentTab(int)));

        m_projectManager->clearUndoStack();
    }
}

void TupMainWindow::addTwitterPage()
{
    QString twitterPath = QDir::homePath() + "/." + QCoreApplication::applicationName() + "/twitter.html";
    if (QFile::exists(twitterPath)) {
        #ifdef K_DEBUG
            QString msg = "TupMainWindow::addTwitterPage() - Loading page -> " + twitterPath;
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

        internetOn = true;
        newsTab = new TupTwitterWidget(this);
        newsTab->setSource(twitterPath);
        connect(newsTab, SIGNAL(newPerspective(int)), this, SLOT(changePerspective(int)));
        addWidget(newsTab);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupMainWindow::addTwitterPage() - Warning: Couldn't load page -> " + twitterPath;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif
    }
}

/*
void TupMainWindow::updateTabContext(int tab)
{
    if (tab == 0)
        animationTab->updatePerspective();
}
*/

/**
 * @if english
 * This method is the first level instruction called when a new project is requested.
 * @endif
 * @if spanish
 * Este metodo es la instruccion de primer nivel llamada cuando un nuevo proyecto es solicitado.
 * @endif
*/

void TupMainWindow::newProject()
{
    #ifdef K_DEBUG
        QString msg = "Creating new project...";
        #ifdef Q_OS_WIN
           qWarning() << msg;
        #else
           tWarning() << msg;
        #endif
    #endif

    TupNewProject *wizard = new TupNewProject(this);
    QDesktopWidget desktop;
    wizard->show();
    wizard->move((int) (desktop.screenGeometry().width() - wizard->width())/2 , 
                 (int) (desktop.screenGeometry().height() - wizard->height())/2);
    wizard->focusProjectLabel();

    if (wizard->exec() != QDialog::Rejected) {
        if (wizard->useNetwork()) {
            TupMainWindow::requestType = NewNetProject;
            setupNetworkProject(wizard->parameters());
            netUser = wizard->login();
        } else {
            setupLocalProject(wizard->parameters());
            createNewLocalProject();
        }
#if defined(QT_GUI_LIB) && defined(K_DEBUG) && !defined(Q_OS_WIN)
    m_debug->setProjectStatus(true); 
#endif
    }

    delete wizard;
}

/**
 * @if english
 * This method handles all the tasks required to close a project session.
 * @endif
 * @if spanish
 * Este metodo se encarga de todas las tareas requeridas para cerrar un proyecto en sesion.
 * @endif
 * @return true if the project is closed successfully
*/

bool TupMainWindow::closeProject()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::closeProject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!mainToolBar->isVisible())
        hideTopPanels();

    if (!m_projectManager->isOpen())
        return true;

    if (m_projectManager->isModified()) {
        QDesktopWidget desktop;

        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Question"));
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("The document has been modified."));
        msgBox.setInformativeText(tr("Do you want to save the project?"));

        msgBox.addButton(QString(tr("Save")), QMessageBox::AcceptRole);
        msgBox.addButton(QString(tr("Discard")), QMessageBox::NoRole);
        msgBox.addButton(QString(tr("Cancel")), QMessageBox::DestructiveRole);

        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2 , 
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int ret = msgBox.exec();

        switch (ret) {
            case QMessageBox::AcceptRole:
                 lastSave = true;
                 saveProject();
                 break;
            case QMessageBox::DestructiveRole:
                 return false;
                 break;
            case QMessageBox::NoRole:
                 break;
        }

    }
    resetUI();

    return true;
}

void TupMainWindow::resetUI()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::resetUI()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    setCurrentTab(0);

    colorView->expandDock(false);
    penView->expandDock(false);
    libraryView->expandDock(false);
    scenesView->expandDock(false);
    timeView->expandDock(false);

#if defined(QT_GUI_LIB) && defined(K_DEBUG) && !defined(Q_OS_WIN)
    debugView->expandDock(false);
#endif

    setUpdatesEnabled(false);
    setMenuItemsContext(false);
    updateOpenRecentMenu(m_recentProjectsMenu, m_recentProjects);

    if (animationTab)
        animationTab->closeArea();

    if (lastTab == 0) {
        if (internetOn)
            removeWidget(newsTab, true);

        removeWidget(playerTab, true);
        removeWidget(animationTab, true);
    } else {
        if (lastTab == 1) {
            if (internetOn)
                removeWidget(newsTab, true);

            removeWidget(animationTab, true);
            removeWidget(playerTab, true);
        } else if (lastTab == 2) {
                   removeWidget(animationTab, true);
                   removeWidget(playerTab, true);   

                   if (internetOn)
                       removeWidget(newsTab, true);
        } else if (lastTab == 3) {
                   removeWidget(animationTab, true);
                   removeWidget(playerTab, true);

                   if (internetOn)
                       removeWidget(newsTab, true);
        }
    }

    if (internetOn) { 
        delete newsTab;
        newsTab = 0;
    }

    delete playerTab;
    playerTab = 0;

    delete animationTab;
    animationTab = 0;

    m_exposureSheet->closeAllScenes();
    m_timeLine->closeAllScenes();
    m_scenes->closeAllScenes();
    m_libraryWidget->resetGUI();

    m_fileName = QString();

    enableToolViews(false);

    if (exposureView->isExpanded())
        exposureView->expandDock(false);

    setUpdatesEnabled(true);

    setWindowTitle(tr("Tupi: Open 2D Magic"));

    if (isNetworked) { 
        m_viewChat->expandDock(false);
        // netProjectManager->closeProject();
    }

    m_projectManager->closeProject();
    m_projectManager->removeProjectPath(CACHE_DIR + projectName);

    resetMousePointer();

#if defined(QT_GUI_LIB) && defined(K_DEBUG) && !defined(Q_OS_WIN)
    m_debug->setProjectStatus(false);
#endif
}

/**
 * @if english
 * This method sets up a Tupi network project.
 * @endif
 * @if spanish
 * Este metodo configura un proyecto para trabajo en red de Tupi.
 * @endif
 * @return true if the network project can be configured
*/

void TupMainWindow::setupNetworkProject()
{
    TupConnectDialog *netDialog = new TupConnectDialog(this);
    QDesktopWidget desktop;
    netDialog->show();
    netDialog->move((int) (desktop.screenGeometry().width() - netDialog->width())/2,
                    (int) (desktop.screenGeometry().height() - netDialog->height())/2);

    TupNetProjectManagerParams *params = new TupNetProjectManagerParams();

    if (netDialog->exec() == QDialog::Accepted) {
        params->setServer(netDialog->server());
        params->setPort(netDialog->port());
        netUser = netDialog->login();
        params->setLogin(netUser);
        params->setPassword(netDialog->password());
        // params->setProjectName(projectName);

        setupNetworkProject(params);
    }
}

/**
 * @if english
 * This method sets up a Tupi network project.
 * @endif
 * @if spanish
 * Este metodo configura un proyecto para trabajo en red de Tupi.
 * @endif
 * @return 
*/

void TupMainWindow::setupNetworkProject(TupProjectManagerParams *params)
{
    if (closeProject()) {
        netProjectManager =  new TupNetProjectManagerHandler;
        connect(netProjectManager, SIGNAL(authenticationSuccessful()), this, SLOT(requestProject()));
        connect(netProjectManager, SIGNAL(openNewArea(const QString &, const QStringList &)), 
                this, SLOT(createNewNetProject(const QString &, const QStringList &)));
        connect(netProjectManager, SIGNAL(updateUsersList(const QString &, int)), this, SLOT(updateUsersOnLine(const QString &, int)));
        connect(netProjectManager, SIGNAL(connectionHasBeenLost()), this, SLOT(unexpectedClose()));
        connect(netProjectManager, SIGNAL(savingSuccessful()), this, SLOT(netProjectSaved()));
        connect(netProjectManager, SIGNAL(postOperationDone()), this, SLOT(resetMousePointer()));

        m_projectManager->setHandler(netProjectManager, true);
        m_projectManager->setParams(params);
        author = params->author();
    }
}

/**
 * @if english
 * This method sets up a Tupi local/single project.
 * @endif
 * @if spanish
 * Este metodo configura un proyecto local/individual de Tupi.
 * @endif
 * @return true if the local project can be configured
*/

void TupMainWindow::setupLocalProject(TupProjectManagerParams *params)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::setupLocalProject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (closeProject()) {
        isNetworked = false;
        m_projectManager->setHandler(new TupLocalProjectManagerHandler, false);
        m_projectManager->setParams(params);
        projectName = params->projectName();
        author = params->author();
        setWindowTitle(tr("Tupi: Open 2D Magic") +  " - " + projectName + " [ " + tr("by") + " " + author + " ]");
    }
}

/**
 * @if english
 * This method opens a Tupi project. 
 * @endif
 * @if spanish
 * Este metodo abre un proyecto de Tupi.
 * @endif
*/

void TupMainWindow::openProject()
{
    const char *home = getenv("HOME");

    QString package = QFileDialog::getOpenFileName(this, tr("Open Tupi project"), home,
                      tr("Tupi Project Package (*.tup)"));

    if (package.isEmpty() || !package.endsWith(".tup")) 
        return;

    openProject(package);
}

/**
 * @if english
 * This method does all the tasks required to open a project.
 * @endif
 * @if spanish
 * Este metodo realiza todas las tareas requeridas para abrir un proyecto.
 * @endif
*/

void TupMainWindow::openProject(const QString &path)
{
    #ifdef K_DEBUG
        QString msg = "TupMainWindow::openProject() - Opening project: " + path;
        #ifdef Q_OS_WIN
           qWarning() << msg;
        #else
           tWarning() << msg;
        #endif
    #endif

    if (path.isEmpty() || !path.endsWith(".tup"))
        return;

    m_projectManager->setHandler(new TupLocalProjectManagerHandler, false);
    isNetworked = false;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_actionManager->enable("open_project", false);
    if (closeProject()) {
        setUpdatesEnabled(false);
        tabWidget()->setCurrentWidget(animationTab);

        if (m_projectManager->loadProject(path)) {
            if (QDir::isRelativePath(path))
                m_fileName = QDir::currentPath() + "/" + path;
            else
                m_fileName = path;

            TupMainWindow::requestType = OpenLocalProject;
            projectName = m_projectManager->project()->projectName();
            updateRecentProjectList();
            updateOpenRecentMenu(m_recentProjectsMenu, m_recentProjects);

            enableToolViews(true);
            setMenuItemsContext(true);
            setUpdatesEnabled(true);

            m_exposureSheet->updateFramesState();

            author = m_projectManager->project()->author();
            if (author.length() <= 0)
                author = "Anonymous";

            setWindowTitle(tr("Tupi: Open 2D Magic") + " - " + projectName + " [ " + tr("by") + " " + author + " ]");
            setWorkSpace();

            m_exposureSheet->updateLayerOpacity(0, 0);
            m_exposureSheet->initLayerVisibility();
            m_timeLine->initLayerVisibility();
        } else {
            setUpdatesEnabled(true);
            TOsd::self()->display(tr("Error"), tr("Cannot open project!"), TOsd::Error);
        }
    }
    m_actionManager->enable("open_project", true);
    QApplication::restoreOverrideCursor();
}

void TupMainWindow::updateRecentProjectList()
{
    int pos = m_recentProjects.indexOf(m_fileName);
    if (pos == -1) {
        m_recentProjects.push_front(m_fileName);
        if (m_recentProjects.count() > 5)
            m_recentProjects.removeLast();
    } else {
        m_recentProjects.push_front(m_recentProjects.takeAt(pos));
    }
}

/**
 * @if english
 * This method opens an animation project from a Tupi Server.
 * @endif
 * @if spanish
 * Este metodo abre un proyecto de animacion desde un servidor de Tupi.
 * @endif
*/

void TupMainWindow::openProjectFromServer()
{
    TupMainWindow::requestType = OpenNetProject;
    setupNetworkProject();
}

/**
 * @if english
 * This method sends a local Tupi project into the animations server.
 * @endif
 * @if spanish
 * Este metodo envia un proyecto local de Tupi en el servidor de animaciones.
 * @endif
*/

void TupMainWindow::importProjectToServer()
{
    TupMainWindow::requestType = ImportProjectToNet;
    setupNetworkProject();
}

/**
 * @if english
 * This method opens the Tupi preferences dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo de preferencias de Tupi.
 * @endif
*/

void TupMainWindow::preferences()
{
    TupPreferencesDialog *dialog = new TupPreferencesDialog(this);
    dialog->show();

    QDesktopWidget desktop;
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 , 
                      (int) (desktop.screenGeometry().height() - dialog->height())/2);

    if (dialog->exec() == QDialog::Accepted) {
        if (animationTab)
            animationTab->updateWorkspace();
    }
}

/**
 * @if english
 * This method opens the help dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo de ayuda.
 * @endif
*/

void TupMainWindow::showHelp()
{
    QString lang = (QLocale::system().name()).left(2);
    if (lang.length() < 2)
        lang = "en";

    QString cover = QString() + "help/" + lang + "/cover.html";

#ifdef Q_OS_WIN
    QString helpPath = SHARE_DIR + cover;
#else
    QString helpPath = SHARE_DIR + "data/" + cover;
#endif

    QFile file(helpPath);
    if (!file.exists()) {
        #ifdef Q_OS_WIN
            helpPath = SHARE_DIR + "help/en/cover.html";
        #else
            helpPath = SHARE_DIR + "data/help/en/cover.html";
        #endif
    }

    TupHelpDialog *dialog = new TupHelpDialog(helpPath);
    dialog->showMaximized();
}

/**
 * @if english
 * This method opens the "About Tupi" dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo "Acerca de Tupi".
 * @endif
*/

void TupMainWindow::aboutTupi()
{
    TupAbout *about = new TupAbout(this);
    about->show();
}

/**
 * @if english
 * This method opens the tips dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo de consejos utiles.
 * @endif
*/
void TupMainWindow::showTipDialog()
{
    QStringList labels;
    labels << tr("Tip of the day") << tr("Show on start") << tr("Previous tip") << tr("Next tip") << tr("Close");

    TipDialog *tipDialog = new TipDialog(labels, DATA_DIR + "tips.xml", this);
    tipDialog->show();

    QDesktopWidget desktop;
    tipDialog->move((int) (desktop.screenGeometry().width() - tipDialog->width())/2 , 
                    (int) (desktop.screenGeometry().height() - tipDialog->height())/2);
}

/**
 * @if english
 * This method imports Gimp color palettes for Tupi.
 * @endif
 * @if spanish
 * Este metodo importa paletas de colores de Gimp para Tupi.
 * @endif
*/

void TupMainWindow::importPalettes()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Import Gimp palettes"), QDir::homePath(), tr("Gimp Palette (*.gpl *.txt *.css)"));

    if (files.count() > 0) { 
        QStringList::ConstIterator file = files.begin();
        bool isOk = true;
        while (file != files.end()) {
               TupPaletteImporter importer;
               bool ok = importer.import(*file, TupPaletteImporter::Gimp);
               if (ok) {
                   QString home = getenv("HOME");
                   QString path = home + "/.tupi/palettes";
                   ok = importer.saveFile(path);
                   if (ok) {
                       m_colorPalette->parsePaletteFile(importer.filePath());
                   } else {
                       #ifdef K_DEBUG
                           QString msg = "TupMainWindow::importPalettes() - Fatal Error: Couldn't import file -> " + QString(*file);
                           #ifdef Q_OS_WIN
                               qDebug() << msg;
                           #else
                               tError() << msg;
                           #endif
                       #endif
                       isOk = false;
                   }
               } else {
                   #ifdef K_DEBUG
                       QString msg = "TupMainWindow::importPalettes() - Fatal Error: Couldn't import palette -> " + QString(*file);
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   #endif
                   isOk = false;
               }
               file++;
        }

        if (isOk)
            TOsd::self()->display(tr("Information"), tr("Gimp palette import was successful"), TOsd::Info);
        else
            TOsd::self()->display(tr("Error"), tr("Gimp palette import was unsuccessful"), TOsd::Error);
    }
}

/**
 * @if english
 * This method defines the events handlers for the project opened.
 * @endif
 * @if spanish
 * Este metodo define los manejadores de eventos para el proyecto abierto.
 * @endif
*/

void TupMainWindow::connectWidgetToManager(QWidget *widget)
{
    connect(widget, SIGNAL(requestTriggered(const TupProjectRequest *)), m_projectManager, 
            SLOT(handleProjectRequest(const TupProjectRequest *)));

    connect(m_projectManager, SIGNAL(responsed(TupProjectResponse*)), widget, 
            SLOT(handleProjectResponse(TupProjectResponse *)));

    // SQA: Pending for revision
    //connect(widget, SIGNAL(postPage(QWidget *)), this, SLOT(addPage(QWidget *)));
}

/**
 * @if english
 * This method defines the events handlers for the local requests.
 * @endif
 * @if spanish
 * Este metodo define los manejadores de eventos para las solicitudes locales.
 * @endif
*/

void TupMainWindow::connectWidgetToLocalManager(QWidget *widget)
{
    connect(widget, SIGNAL(localRequestTriggered(const TupProjectRequest *)),
            m_projectManager, SLOT(handleLocalRequest(const TupProjectRequest *)));
}

void TupMainWindow::disconnectWidgetToManager(QWidget *widget)
{
    disconnect(widget, SIGNAL(requestTriggered(const TupProjectRequest *)), m_projectManager,
            SLOT(handleProjectRequest(const TupProjectRequest *)));

    disconnect(m_projectManager, SIGNAL(responsed(TupProjectResponse*)), widget,
            SLOT(handleProjectResponse(TupProjectResponse *)));
}

/**
 * @if english
 * This method defines the events handlers for the paint area.
 * @endif
 * @if spanish
 * Este metodo define los manejadores de eventos para el area de dibujo.
 * @endif
*/

void TupMainWindow::connectWidgetToPaintArea(QWidget *widget)
{
    connect(widget, SIGNAL(paintAreaEventTriggered(const TupPaintAreaEvent *)), 
            this, SLOT(createPaintCommand(const TupPaintAreaEvent *)));
}

/**
 * @if english 
 * This method is in charge of the function "Save as" for Tupi projects.
 * @endif
 * @if spanish
 * Este metodo se encarga de la funcion "Salvar como" para proyectos de Tupi.
 * @endif
*/

void TupMainWindow::saveAs()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::saveAs()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QString home = getenv("HOME");
    home.append("/" + projectName);
    isSaveDialogOpen = true;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project As"), home,
                       tr("Tupi Project Package (*.tup)"));
    if (fileName.isEmpty()) {
        isSaveDialogOpen = false;
        return;
    }

    isSaveDialogOpen = false;
    int indexPath = fileName.lastIndexOf("/");
    int indexFile = fileName.length() - indexPath;
    QString name = fileName.right(indexFile - 1);
    QString path = fileName.left(indexPath + 1);

    QDir directory(path);
    if (!directory.exists()) {
        TOsd::self()->display(tr("Error"), tr("Directory does not exist! Please, choose another path."), TOsd::Error);
        #ifdef K_DEBUG
            QString file = path.toLocal8Bit();
            QString msg = "TupMainWindow::saveAs() - Fatal Error: Directory doesn't exist! -> " + file;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    } else {
        QFile file(directory.filePath(name));
        if (!file.open(QIODevice::ReadWrite)) {
            file.remove();
            TOsd::self()->display(tr("Error"), tr("Insufficient permissions. Please, pick another path."), TOsd::Error);
            return;
        }
        file.remove();
    }

    int dotIndex = name.lastIndexOf(".tup");
    projectName = name.left(dotIndex);

    m_fileName = fileName;

    if (isNetworked) {
        isNetworked = false;
        m_projectManager->setHandler(new TupLocalProjectManagerHandler, false);
        setWindowTitle(tr("Tupi: Open 2D Magic") + " - " + projectName + " [ " + tr("by") + " " + author + " ]");
    }

    saveProject();
}

/**
 * @if english 
 * This method does all the tasks required to save a Tupi Project.
 * @endif
 * @if spanish
 * Este metodo se encarga de todas las tareas necesarias para guardar un proyecto.
 * @endif
*/

void TupMainWindow::saveProject()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::saveProject()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!isNetworked) {
        if (isSaveDialogOpen)
            return;

        if (m_fileName.isEmpty()) {
            saveAs();
            return;
        }

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        m_actionManager->enable("save_project", false);
        m_actionManager->enable("save_project_as", false);

        if (m_projectManager->saveProject(m_fileName)) {  
            updateRecentProjectList();
            
            TOsd::self()->display(tr("Information"), tr("Project <b>%1</b> saved").arg(projectName));
            // projectSaved = true;
            int indexPath = m_fileName.lastIndexOf("/");
            int indexFile = m_fileName.length() - indexPath;
            QString name = m_fileName.right(indexFile - 1);
            int indexDot = name.lastIndexOf(".");
            name = name.left(indexDot);

            setWindowTitle(tr("Tupi: Open 2D Magic") +  " - " + name + " [ " + tr("by") +  " " +  author + " ]");
        } else {
            TOsd::self()->display(tr("Error"), tr("Cannot save the project!"), TOsd::Error);
        }
        m_actionManager->enable("save_project", true);
        m_actionManager->enable("save_project_as", true);
        QApplication::restoreOverrideCursor();

        if (isSaveDialogOpen)
            isSaveDialogOpen = false;
    } else {
        TupSavePackage package(lastSave);
        netProjectManager->sendPackage(package);

        if (!lastSave)
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        else
            lastSave = false;
    }
}

/**
 * @if english 
 * This method opens a recent project.
 * @endif
 * @if spanish
 * Este metodo abre un proyecto reciente.
 * @endif
*/

void TupMainWindow::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openProject(action->text());
}

/**
 * @if english 
 * This method shows the Animation module menu.
 * @endif
 * @if spanish
 * Este metodo muestra el menu del modulo de Animacion.
 * @endif
*/
// SQA: Check if this method is still used for something
void TupMainWindow::showAnimationMenu(const QPoint &p)
{
    QMenu *menu = new QMenu(tr("Animation"), playerTab);
    menu->addAction(tr("New camera"), this, SLOT(newViewCamera()));
    menu->exec(p);
    delete menu;
}

/**
 * @if english 
 * This method redefines the closeEvent method for the main window.
 * @endif
 * @if spanish
 * Este metodo redefine el metodo closeEvent para el marco principal de la interfaz.
 * @endif
*/

void TupMainWindow::closeEvent(QCloseEvent *event)
{
    QString lastProject = m_fileName;

    if (!closeProject()) {
        event->ignore();
        return;
    }

    QString newsPath = QDir::homePath() + "/." + QCoreApplication::applicationName() + "/twitter.html";
    if (QFile::exists(newsPath)) {
        QFile file(newsPath);
        file.remove();
    }

    TCONFIG->beginGroup("General");
    // TCONFIG->setValue("LastProject", lastProject);
    TCONFIG->setValue("Recents", m_recentProjects);

    TMainWindow::closeEvent(event);
}

/**
 * @if english 
 * This method creates a command for the paint area and include it into the undo/redo history.
 * @endif
 * @if spanish
 * Este metodo crea un comando para el area de dibujo y lo incluye en el historial de hacer/deshacer.
 * @endif
*/

void TupMainWindow::createPaintCommand(const TupPaintAreaEvent *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::createPaintCommand()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!animationTab) {
        #ifdef K_DEBUG
            QString msg = "TupMainWindow::createPaintCommand() - No animation tab... aborting!"; 
            #ifdef Q_OS_WIN
               qDebug() << msg;
            #else
               tFatal() << msg;
            #endif
        #endif
        return;
    }

    TupPaintAreaCommand *command = animationTab->createPaintCommand(event);

    if (command) { 
        // SQA: Implement Undo procedure for "Color" actions 
        m_projectManager->createCommand((TupProjectCommand *)command);

        // Updating color on the Pen module interface
        if (event->action() == TupPaintAreaEvent::ChangePenColor)
            m_penWidget->setPenColor(qvariant_cast<QColor>(event->data()));

        if (event->action() == TupPaintAreaEvent::ChangePenWidth)
            m_penWidget->setPenWidth(qvariant_cast<int>(event->data()));
    } 
}

void TupMainWindow::updatePenColor(const QColor &color)
{
    TupPaintAreaEvent *event = new TupPaintAreaEvent(TupPaintAreaEvent::ChangePenColor, color);
    createPaintCommand(event);
}

void TupMainWindow::updatePenWidth(int width)
{
    TupPaintAreaEvent *event = new TupPaintAreaEvent(TupPaintAreaEvent::ChangePenWidth, width);
    createPaintCommand(event);
}

/**
 * @if english 
 * This method adds a page/tab to the main window.
 * @endif
 * @if spanish
 * Este metodo adiciona una pagina/tab a la ventana principal.
 * @endif
*/

void TupMainWindow::addPage(QWidget *widget)
{
    addWidget(widget);
}

void TupMainWindow::updateCurrentTab(int index)
{
    // SQA: Check/Test the content of this method

    if (index == 1) {  // Player mode 
        lastTab = 1;
        updatePlayer();
        cameraWidget->updateFirstFrame();
        cameraWidget->setFocus();
    } else {
        if (index == 0) { // Animation mode
            animationTab->updatePerspective(); // Just for Papagayo UI

            if (lastTab == 1)
                cameraWidget->doStop();

            if (scenesView->isExpanded())
                scenesView->expandDock(true);

            if (contextMode == TupProject::FRAMES_EDITION) {
                if (!exposureView->isExpanded())
                    exposureView->expandDock(true);
            } else {
                exposureView->expandDock(false);
                exposureView->enableButton(false);
            }

            animationTab->updatePaintArea();
            lastTab = 0;
        } else {
            if (index == 3)
                lastTab = 3;
        }
    }
}

/**
 * @if english 
 * This method exports the animation project to a video/image format.
 * @endif
 * @if spanish
 * Este metodo exporta un proyecto de animacion a un formato de video/imagen.
 * @endif
*/

void TupMainWindow::exportProject()
{
    QDesktopWidget desktop;

    TupExportWidget exportWidget(m_projectManager->project(), this);
    exportWidget.show();
    exportWidget.move((int) (desktop.screenGeometry().width() - exportWidget.width())/2, 
                      (int) (desktop.screenGeometry().height() - exportWidget.height())/2);

    exportWidget.exec();
}

void TupMainWindow::callSave()
{
    if (m_projectManager->isModified())
        saveProject();
}

void TupMainWindow::expandExposureView(TupProject::Mode mode) 
{
    contextMode = mode;
    if (contextMode == TupProject::FRAMES_EDITION) {
        exposureView->expandDock(true);
        exposureView->enableButton(true);
    } else {
        exposureView->expandDock(false);
        exposureView->enableButton(false);
    }
}

void TupMainWindow::requestProject()
{
    if (TupMainWindow::requestType == NewNetProject) {
        m_projectManager->setupNewProject();
    } else if (TupMainWindow::requestType == OpenNetProject) {
               TupListProjectsPackage package;
               netProjectManager->sendPackage(package);
    } else if (TupMainWindow::requestType == ImportProjectToNet) {
               const char *home = getenv("HOME");
               QString file = QFileDialog::getOpenFileName(this, tr("Import project package"),
                                                           home, tr("Tupi Project Package (*.tup)"));
               if (file.length() > 0) {
                   QFile project(file);
                   if (project.exists()) {
                       if (project.size() > 0) {
                           TupImportProjectPackage package(file);
                           netProjectManager->sendPackage(package);
                        } else {
                           TOsd::self()->display(tr("Error"), tr("Can't import project. File is empty!"), TOsd::Error);
                           netProjectManager->closeProject();
                        }
                   } else {
                        TOsd::self()->display(tr("Error"), tr("Can't save the project. File doesn't exist!"), TOsd::Error);
                        netProjectManager->closeProject();
                   }
               } else {
                   netProjectManager->closeProject();
               }
    }
}

void TupMainWindow::unexpectedClose()
{
    if (m_projectManager->isOpen())
        resetUI();

    QDesktopWidget desktop;
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Fatal Error"));
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("The connection to the server has been lost."));
    msgBox.setInformativeText(tr("Please, try to connect again in a while"));

    msgBox.addButton(QString(tr("Close")), QMessageBox::DestructiveRole);

    msgBox.show();
    msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2,
                (int) (desktop.screenGeometry().height() - msgBox.height())/2);

    msgBox.exec();
}

void TupMainWindow::netProjectSaved()
{
    m_projectManager->undoModified();
    QApplication::restoreOverrideCursor();
}

void TupMainWindow::updatePlayer(bool removeAction)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupMainWindow::updatePlayer()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!removeAction)
        updatePlayer();
}

void TupMainWindow::updatePlayer()
{
    if (animationTab) {
        int sceneIndex = animationTab->currentSceneIndex();
        cameraWidget->updateScenes(sceneIndex);
    }
}

void TupMainWindow::resetMousePointer()
{
    QApplication::restoreOverrideCursor();
}

void TupMainWindow::updateUsersOnLine(const QString &login, int state)
{
    animationTab->updateUsersOnLine(login, state);
}

void TupMainWindow::resizeProjectDimension(const QSize dimension)
{
    m_projectManager->updateProjectDimension(dimension);
    disconnectWidgetToManager(cameraWidget);
    delete cameraWidget; 
    cameraWidget = new TupCameraWidget(m_projectManager->project(), isNetworked);
    connectWidgetToManager(cameraWidget);

    playerTab->setCameraWidget(cameraWidget);
}
