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

#include "ktmainwindow.h"
#include "ktnewproject.h"
#include "ktabout.h"
#include "ktpackagehandler.h"
#include "ktpaletteimporter.h"
#include "ktpaintareaevent.h"
#include "ktpaintareacommand.h"

// Tupi Framework
#include "ktipdialog.h"
#include "tdebug.h"
#include "tosd.h"
#include "kimageeffect.h"
#include "kaudioplayer.h"

#include "ktapplication.h"
#include "ktpluginmanager.h"
#include "ktprojectcommand.h"
#include "ktlocalprojectmanagerhandler.h"

// #ifdef USE_NET
#include "ktnetprojectmanagerparams.h"
#include "ktconnectdialog.h"
#include "ktlistpackage.h"
#include "ktimportprojectpackage.h"
#include "ktlistprojectspackage.h"
#include "ktsavepackage.h"
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

class SleeperThread : public QThread
{
    public:
        static void msleep(unsigned long msecs)
        {
            QThread::msleep(msecs);
        }
};

/**
 * @if english
 * This is the constructor method for this class.
 * @endif
 * @if spanish
 * Este es el metodo constructor para esta clase.
 * @endif
 * @return 
*/

KTMainWindow::KTMainWindow(KTSplash *splash, int parameters) : 
              KTabbedMainWindow(), m_projectManager(0), drawingTab(0), animationTab(0), 
              m_viewChat(0), m_exposureSheet(0), m_scenes(0), isSaveDialogOpen(false), internetOn(false)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    // Loading audio player plugin
    KAudioPlayer::instance()->loadEngine("gstreamer"); // FIXME: Move this to the settings 
    setObjectName("KTMainWindow_");

    // Defining the status bar
    m_statusBar = new KTStatusBar(this);
    setStatusBar(m_statusBar);

    // Naming the main frame...
    setWindowTitle(tr("Tupi: 2D Magic"));
    setWindowIcon(QIcon(THEME_DIR + "icons/about.png"));

    // Defining the render type for the drawings
    m_renderType = Tupi::RenderType(TCONFIG->value("RenderType").toInt());

    // Calling out the project manager
    m_projectManager = new KTProjectManager(this);

    splash->setMessage(tr("Setting up the project manager"));
    SleeperThread::msleep(500);

    // Calling out the events/actions manager
    splash->setMessage(tr("Loading action manager..."));
    m_actionManager = new TActionManager(this);

    // Defining the menu bar
    splash->setMessage(tr("Creating menu bar..."));
    SleeperThread::msleep(500);

    setupActions();
	
    splash->setMessage(tr("Creating GUI..."));
    SleeperThread::msleep(500);
	
    // Setting up all the GUI...
    createGUI(); // This method is called from the ktmainwindow_gui class
    setupMenu();
    setupToolBar();

    // Check if user wants to see a Tupi tip for every time he launches the program
    TCONFIG->beginGroup("TipOfDay");
    //bool showTips = qvariant_cast<bool>(TCONFIG->value("ShowOnStart", true));
    bool showTips = TCONFIG->value("ShowOnStart", true).toBool();

    // If option is enabled, then, show a little dialog with a nice tip
    if (showTips)
        QTimer::singleShot(0, this, SLOT(showTipDialog()));

    // Time to load plugins... 
    KTPluginManager::instance()->loadPlugins();

    // Defining the Animation view, as the first interface to show up	
    setCurrentPerspective(Animation);

    TCONFIG->beginGroup("General");
    // check if into the config file, user always wants to start opening his last project 
    // created
    bool openLast = TCONFIG->value("OpenLastProject").toBool();

    if (openLast && parameters == 1)
        openProject(TCONFIG->value("LastProject").toString());

    if (TCONFIG->firstTime()) {
        TCONFIG->setValue("OpenLastProject", openLast);
        TCONFIG->setValue("AutoSave", 2);
    }

    KTMainWindow::requestType = None;
}

/**
 * @if english
 * This is the destructor method for this class.
 * @endif
 * @if spanish
 * Este es el metodo destructor para esta clase.
 * @endif
*/
KTMainWindow::~KTMainWindow()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear(QClipboard::Clipboard);

    delete KTPluginManager::instance();
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

void KTMainWindow::createNewLocalProject()
{
    KTMainWindow::requestType = NewLocalProject;

    m_projectManager->setupNewProject();
    m_projectManager->setOpen(true);
 
    enableToolViews(true);
    setMenuItemsContext(true);

    setWorkSpace();
}

void KTMainWindow::createNewNetProject(const QString &title)
{
    isNetworked = true;
    projectName = title;
    setWindowTitle(tr("Tupi: 2D Magic") + " - " + projectName + " " + tr("[ by %1 | net mode ]").arg(netUser));

    if (m_viewChat) {
        removeToolView(m_viewChat);
        delete m_viewChat;
    }

    m_viewChat = addToolView(netProjectManagerHandler->communicationWidget(), Qt::BottomDockWidgetArea, All, "Chat");
    m_viewChat->setVisible(false);

    enableToolViews(true);
    setMenuItemsContext(true);
    m_exposureSheet->updateFramesState(m_projectManager->project());
    m_projectManager->setOpen(true);

    setWorkSpace();
}

/**
 * @if english
 * This method supports all the low level tasks for the method createNewLocalProject().
 * @endif
 * @if spanish
 * Este metodo soporta todas las tareas de bajo nivel para el metodo createNewLocalProject().
 * @endif
*/

void KTMainWindow::setWorkSpace()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (m_projectManager->isOpen()) {

        if (KTMainWindow::requestType == NewLocalProject || KTMainWindow::requestType == NewNetProject)
            TOsd::self()->display(tr("Information"), tr("Opening a new document..."));

        contextMode = KTProject::FRAMES_EDITION;

        // Setting undo/redo actions
        setUndoRedoActions();

        drawingTab = new KTViewDocument(m_projectManager->project(), this, isNetworked ? false : true);

        TCONFIG->beginGroup("Network");
        QString server = TCONFIG->value("Server").toString();
        if (isNetworked && server.compare("tupitube.com") == 0) {
            connect(drawingTab, SIGNAL(requestExportImageToServer(int, int, const QString &, const QString &)),                         
                    netProjectManagerHandler, SLOT(sendExportImageRequestToServer(int, int, const QString &, const QString &)));
        }

        connectToDisplays(drawingTab);

        drawingTab->setWindowTitle(tr("Animation"));
        addWidget(drawingTab);

        connectToDisplays(drawingTab);
        connectWidgetToManager(drawingTab);
        connectWidgetToLocalManager(drawingTab);
        connect(drawingTab, SIGNAL(modeHasChanged(int)), this, SLOT(expandExposureView(int))); 
        connect(drawingTab, SIGNAL(expandColorPanel()), this, SLOT(expandColorView()));
      
        drawingTab->setAntialiasing(true);

        // KTViewCamera *
        viewCamera = new KTViewCamera(m_projectManager->project(), isNetworked);
        connectWidgetToManager(viewCamera);

        m_libraryWidget->setNetworking(isNetworked);

        if (isNetworked)
            connect(viewCamera, SIGNAL(requestForExportVideoToServer(const QString &, const QString &, int, const QList<int>)), 
                    this, SLOT(postVideo(const QString &, const QString &, int, const QList<int>)));
        else
            connect(drawingTab, SIGNAL(autoSave()), this, SLOT(callSave()));

        animationTab = new KTAnimationspace(viewCamera);
        animationTab->setWindowIcon(QIcon(THEME_DIR + "icons/play.png"));
        animationTab->setWindowTitle(tr("Player"));
        addWidget(animationTab);

        helpTab = new KTHelpBrowser(this);
        helpTab->setDataDirs(QStringList() << m_helper->helpPath());

        QString lang = (QLocale::system().name()).left(2);
        if (lang.length() < 2)  
            lang = "en";

        QString helpPath = SHARE_DIR + "data/help/" + QString(lang + "/cover.html");

        QFile file(helpPath);
        if (!file.exists())
            helpPath = SHARE_DIR + "data/help/" + QString("en/cover.html");

        helpTab->setSource(helpPath);

        addWidget(helpTab);

        QString twitterPath = QDir::homePath() + "/." + QCoreApplication::applicationName() 
                              + "/twitter.html";

        if (QFile::exists(twitterPath)) {
            internetOn = true;
            newsTab = new KTwitterWidget(this); 
            newsTab->setSource(twitterPath);
            addWidget(newsTab);
        } 

        connect(this, SIGNAL(tabHasChanged(int)), this, SLOT(updateCurrentTab(int)));

        exposureView->expandDock(true);

        // if (!isNetworked)
        //     connect(drawingTab, SIGNAL(autoSave()), this, SLOT(callSave()));

        m_projectManager->undoModified();

        // SQA: Check if this instruction is really required
        m_colorPalette->init();

        TCONFIG->beginGroup("PenParameters");
        int thicknessValue = TCONFIG->value("Thickness", -1).toInt();
        m_penWidget->init();
        m_penWidget->setThickness(thicknessValue);

        if (KTMainWindow::requestType == OpenLocalProject || KTMainWindow::requestType == OpenNetProject)
            TOsd::self()->display(tr("Information"), tr("Project <b>%1</b> opened!").arg(m_projectManager->project()->projectName()));

        connect(m_projectManager, SIGNAL(modified(bool)), this, SLOT(updatePlayer(bool)));

        m_exposureSheet->setScene(0);
    }
}

/**
 * @if english
 * This method is the first level instruction called when a new project is requested.
 * @endif
 * @if spanish
 * Este metodo es la instruccion de primer nivel llamada cuando un nuevo proyecto es solicitado.
 * @endif
*/

void KTMainWindow::newProject()
{
    #ifdef K_DEBUG
           tWarning() << "Creating new project...";
    #endif

    KTNewProject *wizard = new KTNewProject(this);
    QDesktopWidget desktop;
    wizard->show();
    wizard->move((int) (desktop.screenGeometry().width() - wizard->width())/2 , 
                 (int) (desktop.screenGeometry().height() - wizard->height())/2);
    wizard->focusProjectLabel();

    if (wizard->exec() != QDialog::Rejected) {
        if (wizard->useNetwork()) {
            KTMainWindow::requestType = NewNetProject;
            setupNetworkProject(wizard->parameters());
            netUser = wizard->login();
        } else {
            setupLocalProject(wizard->parameters());
            createNewLocalProject();
        }
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

bool KTMainWindow::closeProject()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

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

void KTMainWindow::resetUI()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    setCurrentTab(0);

    // if (colorView->isExpanded())
        colorView->expandDock(false);

    // if (penView->isExpanded())
        penView->expandDock(false);

    // if (libraryView->isExpanded())
        libraryView->expandDock(false);

    //if (helpView->isExpanded())
        helpView->expandDock(false);

    //if (scenesView->isExpanded())
        scenesView->expandDock(false);
    
    //if (timeView->isExpanded())
        timeView->expandDock(false);

#if defined(QT_GUI_LIB) && defined(K_DEBUG)
    //if (debugView->isExpanded())
        debugView->expandDock(false);
#endif

    setUpdatesEnabled(false);
    setMenuItemsContext(false);

    if (drawingTab)
        drawingTab->closeArea();

    if (lastTab == 0) {

        if (internetOn)
            removeWidget(newsTab, true);

        removeWidget(helpTab, true);
        removeWidget(animationTab, true);
        removeWidget(drawingTab, true);

    } else {
      if (lastTab == 1) {

          if (internetOn)
              removeWidget(newsTab, true);

          removeWidget(helpTab, true);
          removeWidget(drawingTab, true);
          removeWidget(animationTab, true);

      } else if (lastTab == 2) {

                 removeWidget(drawingTab, true);
                 removeWidget(animationTab, true);   

                 if (internetOn)
                     removeWidget(newsTab, true);

                 removeWidget(helpTab, true);

      } else if (lastTab == 3) {

                 removeWidget(drawingTab, true);
                 removeWidget(animationTab, true);
                 removeWidget(helpTab, true);

                 if (internetOn)
                     removeWidget(newsTab, true);

      }
    }

    if (internetOn) { 
        delete newsTab;
        newsTab = 0;
    }

    delete helpTab;
    helpTab = 0;

    delete animationTab;
    animationTab = 0;

    delete drawingTab;
    drawingTab = 0;

    // Cleaning widgets
    m_exposureSheet->blockSignals(true);
    m_exposureSheet->closeAllScenes();
    m_exposureSheet->blockSignals(false);

    m_timeLine->closeAllScenes();
    m_scenes->closeAllScenes();
    m_libraryWidget->resetGUI();

    m_fileName = QString();

    enableToolViews(false);

    if (exposureView->isExpanded())
        exposureView->expandDock(false);

    m_statusBar->setStatus(tr(""));
    // projectSaved = false;

    setUpdatesEnabled(true);

    setWindowTitle(tr("Tupi: 2D Magic"));

    if (isNetworked) { 
        m_viewChat->expandDock(false);
        // netProjectManagerHandler->closeProject();
    }

    disconnect(m_projectManager, SIGNAL(modified(bool)), this, SLOT(updatePlayer(bool)));
    m_projectManager->closeProject();
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

void KTMainWindow::setupNetworkProject()
{
    KTConnectDialog *netDialog = new KTConnectDialog(this);
    QDesktopWidget desktop;
    netDialog->show();
    netDialog->move((int) (desktop.screenGeometry().width() - netDialog->width())/2,
                    (int) (desktop.screenGeometry().height() - netDialog->height())/2);

    KTNetProjectManagerParams *params = new KTNetProjectManagerParams();

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

void KTMainWindow::setupNetworkProject(KTProjectManagerParams *params)
{
    if (closeProject()) {
        netProjectManagerHandler =  new KTNetProjectManagerHandler;
        connect(netProjectManagerHandler, SIGNAL(authenticationSuccessful()), this, SLOT(requestProject()));
        connect(netProjectManagerHandler, SIGNAL(openNewArea(const QString&)), this, SLOT(createNewNetProject(const QString&)));
        connect(netProjectManagerHandler, SIGNAL(connectionHasBeenLost()), this, SLOT(unexpectedClose()));
        connect(netProjectManagerHandler, SIGNAL(savingSuccessful()), this, SLOT(netProjectSaved()));

        m_projectManager->setHandler(netProjectManagerHandler, true);
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

void KTMainWindow::setupLocalProject(KTProjectManagerParams *params)
{
    if (closeProject()) {
        isNetworked = false;
        m_projectManager->setHandler(new KTLocalProjectManagerHandler, false);
        m_projectManager->setParams(params);
        projectName = params->projectName();
        author = params->author();
        setWindowTitle(tr("Tupi: 2D Magic") +  " - " + projectName + " [ " + tr("by") + " " + author + " ]");
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

void KTMainWindow::openProject()
{
    const char *home = getenv("HOME");

    QString package = QFileDialog::getOpenFileName(this, tr("Open Tupi project"), home,
                      tr("Tupi Project Package (*.tup)"));

    if (package.isEmpty() || !package.endsWith(".tup")) 
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); 
    openProject(package);
    QApplication::restoreOverrideCursor();
}

/**
 * @if english
 * This method does all the tasks required to open a project.
 * @endif
 * @if spanish
 * Este metodo realiza todas las tareas requeridas para abrir un proyecto.
 * @endif
*/

void KTMainWindow::openProject(const QString &path)
{
    #ifdef K_DEBUG
           tWarning() << "KTMainWindow::openProject() - Opening project: " << path;
    #endif

    if (path.isEmpty() || !path.endsWith(".tup"))
        return;

    m_projectManager->setHandler(new KTLocalProjectManagerHandler, false);
    isNetworked = false;

    if (closeProject()) {
        setUpdatesEnabled(false);
        tabWidget()->setCurrentWidget(drawingTab);

        if (m_projectManager->loadProject(path)) {

            if (QDir::isRelativePath(path))
                m_fileName = QDir::currentPath() + QDir::separator() + path;
            else
                m_fileName = path;

            KTMainWindow::requestType = OpenLocalProject;
            projectName = m_projectManager->project()->projectName();

            int pos = m_recentProjects.indexOf(m_fileName);

            if (pos == -1) {
                if (m_recentProjects.count() <= 6)
                    m_recentProjects << m_fileName;
                else
                    m_recentProjects.push_front(m_fileName);
            } else {
                m_recentProjects.push_front(m_recentProjects.takeAt(pos));
            }

            updateOpenRecentMenu(m_recentProjectsMenu, m_recentProjects);

            enableToolViews(true);
            setMenuItemsContext(true);
            setUpdatesEnabled(true);

            m_exposureSheet->updateFramesState(m_projectManager->project());

            author = m_projectManager->project()->author();
            if (author.length() <= 0)
                author = "Anonymous";

            setWindowTitle(tr("Tupi: Magia 2D") + " - " + projectName + " [ " + tr("by") + " " + author + " ]");
            setWorkSpace();
        } else {
                 setUpdatesEnabled(true);
                 TOsd::self()->display(tr("Error"), tr("Cannot open project!"), TOsd::Error);
        }
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

void KTMainWindow::openProjectFromServer()
{
    KTMainWindow::requestType = OpenNetProject;
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

void KTMainWindow::importProjectToServer()
{
    KTMainWindow::requestType = ImportProjectToNet;
    setupNetworkProject();
}

/**
 * @if english
 * This method calls the methods required to save the current project.
 * @endif
 * @if spanish
 * Este metodo llama a los metodos requeridos para salvar el proyecto actual.
 * @endif
*/

void KTMainWindow::save()
{
    #ifdef K_DEBUG
           tWarning("project") << "KTMainWindow::save() - Saving...";
    #endif
    QTimer::singleShot(0, this, SLOT(saveProject()));
}

/**
 * @if english
 * This method opens the Tupi preferences dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo de preferencias de Tupi.
 * @endif
*/

void KTMainWindow::preferences()
{
    m_statusBar->setStatus(tr("Preferences Dialog Opened"));

    KTPreferences *preferences = new KTPreferences(this);
    connect(preferences, SIGNAL(timerChanged()), drawingTab, SLOT(updateTimer()));
    preferences->show();

    QDesktopWidget desktop;
    preferences->move((int) (desktop.screenGeometry().width() - preferences->width())/2 , 
                      (int) (desktop.screenGeometry().height() - preferences->height())/2);

    preferences->exec();
    delete preferences;
}

/**
 * @if english
 * This method opens the "About Tupi" dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo "Acerca de Tupi".
 * @endif
*/

void KTMainWindow::aboutTupi()
{
    KTAbout *about = new KTAbout(this);
    about->exec();

    delete about;
}

/**
 * @if english
 * This method opens the tips dialog.
 * @endif
 * @if spanish
 * Este metodo abre el dialogo de consejos utiles.
 * @endif
*/
void KTMainWindow::showTipDialog()
{
    QStringList labels;
    labels << tr("Tip of the day") << tr("Show on start") << tr("Previous tip") << tr("Next tip") << tr("Close");

    KTipDialog *tipDialog = new KTipDialog(labels, DATA_DIR + "tips.xml", this);
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

void KTMainWindow::importPalettes()
{
    const char *home = getenv("HOME");
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Import gimp palettes"), home, 
                                                       tr("Gimp Palette (*.gpl)"));

    m_statusBar->setStatus(tr("Importing palettes"));
    QStringList::ConstIterator it = files.begin();
	
    //int progress = 1;
    while (it != files.end()) {
           KTPaletteImporter importer;
           importer.import(*it, KTPaletteImporter::Gimp);
           ++it;

           importer.saveFile(SHARE_DIR + "data/palettes");
           m_colorPalette->parsePaletteFile( importer.filePath());
           //m_statusBar->advance(progress++, files.count());
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

void KTMainWindow::connectWidgetToManager(QWidget *widget)
{
    connect(widget, SIGNAL(requestTriggered(const KTProjectRequest *)), m_projectManager, 
            SLOT(handleProjectRequest(const KTProjectRequest *)));

    connect(m_projectManager, SIGNAL(responsed(KTProjectResponse*)), widget, 
            SLOT(handleProjectResponse(KTProjectResponse *)));

    // PENDING TO CHECK
    //connect(widget, SIGNAL(postPage(QWidget *)), this, SLOT(addPage(QWidget *)));
}

/**
 * @if english
 * This method defines the events handlers for the paint area.
 * @endif
 * @if spanish
 * Este metodo define los manejadores de eventos para el area de dibujo.
 * @endif
*/

void KTMainWindow::connectWidgetToPaintArea(QWidget *widget)
{
    connect(widget, SIGNAL(paintAreaEventTriggered(const KTPaintAreaEvent *)), this, 
            SLOT(createCommand(const KTPaintAreaEvent *)));
}

/**
 * @if english
 * This method defines the events handlers for the local requests.
 * @endif
 * @if spanish
 * Este metodo define los manejadores de eventos para las solicitudes locales.
 * @endif
*/

void KTMainWindow::connectWidgetToLocalManager(QWidget *widget)
{
    connect(widget, SIGNAL(localRequestTriggered(const KTProjectRequest *)), 
            m_projectManager, SLOT(handleLocalRequest(const KTProjectRequest *)));
}

/**
 * @if english
 * This method sets a message into the status bar.
 * @endif
 * @if spanish
 * Este metodo asigna un mensaje a la barra de estados.
 * @endif
 */

void KTMainWindow::messageToStatus(const QString &msg)
{
    m_statusBar->setStatus(msg);
}

/**
 * @if english
 * This method display a help page.
 * @endif
 * @if spanish
 * Este metodo despliega una pagina de ayuda.
 * @endif
*/

//void KTMainWindow::showHelpPage(const QString &title, const QString &filePath)

void KTMainWindow::showHelpPage(const QString &filePath)
{
    helpTab->setSource(filePath);
}

/**
 * @if english 
 * This method is in charge of the function "Save as" for Tupi projects.
 * @endif
 * @if spanish
 * Este metodo se encarga de la funcion "Salvar como" para proyectos de Tupi.
 * @endif
*/

void KTMainWindow::saveAs()
{
    QString home = getenv("HOME");
    home.append(QDir::separator() + projectName);

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
        TOsd::self()->display(tr("Error"), tr("Directory \"" + path.toLocal8Bit() + "\" does not exist! Please, choose another path."), TOsd::Error);
        return;
    } else {
        QFile file(directory.filePath(name));
        if (!file.open(QIODevice::ReadWrite)) {
            file.remove();
            TOsd::self()->display(tr("Error"), tr("You have no permission to create this file. Please, choose another path."), TOsd::Error);
            return;
        }
        file.remove();
    }

    int dotIndex = name.lastIndexOf(".tup");
    projectName = name.left(dotIndex);

    m_fileName = fileName;

    if (isNetworked) {
        isNetworked = false;
        m_projectManager->setHandler(new KTLocalProjectManagerHandler, false);
        setWindowTitle(tr("Tupi: 2D Magic") + " - " + projectName + " [ " + tr("by") + " " + author + " ]");
    }

    save();
}

/**
 * @if english 
 * This method does all the tasks required to save a Tupi Project.
 * @endif
 * @if spanish
 * Este metodo se encarga de todas las tareas necesarias para guardar un proyecto.
 * @endif
*/

void KTMainWindow::saveProject()
{
    if (!isNetworked) {

        if (isSaveDialogOpen)
            return;

        if (m_fileName.isEmpty()) {
            saveAs();
            return;
        }

        if (m_projectManager->saveProject(m_fileName)) {  
            TOsd::self()->display(tr("Information"), tr("Project <b>%1</b> saved").arg(projectName));
            // projectSaved = true;
            int indexPath = m_fileName.lastIndexOf("/");
            int indexFile = m_fileName.length() - indexPath;
            QString name = m_fileName.right(indexFile - 1);
            int indexDot = name.lastIndexOf(".");
            name = name.left(indexDot);

            setWindowTitle(tr("Tupi: 2D Magic") +  " - " + name + " [ " + tr("by") +  " " +  author + " ]");
        } else {
            TOsd::self()->display(tr("Error"), tr("Cannot save the project!"), TOsd::Error);
        }

        if (isSaveDialogOpen)
            isSaveDialogOpen = false;
    } else {
        KTSavePackage package;
        netProjectManagerHandler->sendPackage(package);
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

void KTMainWindow::openRecentProject()
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

void KTMainWindow::showAnimationMenu(const QPoint &p)
{
    QMenu *menu = new QMenu(tr("Animation"), animationTab);
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

void KTMainWindow::closeEvent(QCloseEvent *event)
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
    TCONFIG->setValue("LastProject", lastProject);
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

void KTMainWindow::createCommand(const KTPaintAreaEvent *event)
{
    if (!drawingTab) {
        #ifdef K_DEBUG
               tFatal() << "KTMainWindow::createCommand() - No drawingTab... Aborting!";
        #endif
        return;
    }

    KTPaintAreaCommand *command = drawingTab->createCommand(event);

    if (command) { 
        // tFatal() << "KTMainWindow::createCommand() - Paint command is valid!";
        m_projectManager->undoHistory()->push(command);

        // if (event->action() == 2) {
        if (event->action() == KTPaintAreaEvent::ChangeColorPen) {
            // tFatal() << "KTMainWindow::createCommand() - event action == ChangeColorPen";
            m_penWidget->setPenColor(qvariant_cast<QColor>(event->data()));
        } else if (event->action() == KTPaintAreaEvent::ChangeBrush) {
                   // tFatal() << "KTMainWindow::createCommand() - event action == ChangeBrush";
                   // tFatal() << "KTMainWindow::createCommand() - action: " << event->action();
                   // m_penWidget->setBrush(qvariant_cast<QBrush>(event->data()));
        }
    } else {
        // tFatal() << "KTMainWindow::createCommand() - Paint command is NULL!";
    }
}

/**
 * @if english 
 * This method adds a page/tab to the main window.
 * @endif
 * @if spanish
 * Este metodo adiciona una pagina/tab a la ventana principal.
 * @endif
*/

void KTMainWindow::addPage(QWidget *widget)
{
    addWidget(widget);
}

void KTMainWindow::updateCurrentTab(int index)
{
    // SQA: Check/Test the content of this method

    if (index == 1) {  // Player mode 
        if (lastTab == 2)
            helpView->expandDock(false);
        lastTab = 1;
        viewCamera->updateFirstFrame();
        viewCamera->setFocus();
    } else {
        if (index == 0) { // Animation mode
            if (lastTab == 1)
                viewCamera->doStop();

            if (scenesView->isExpanded()) {
                helpView->expandDock(false);
                scenesView->expandDock(true);
            }     

            if (contextMode == KTProject::FRAMES_EDITION) {
                if (exposureView->isExpanded()) {
                    helpView->expandDock(false);
                    exposureView->expandDock(true);
                } 
            } else {
                exposureView->expandDock(false);
                exposureView->enableButton(false);
            }

            if (lastTab == 2)
                helpView->expandDock(false);

            drawingTab->updatePaintArea();

            lastTab = 0;
        } else {
            if (index == 2) { // Help mode
                helpView->expandDock(true);
                lastTab = 2;
            } else if (index == 3) { // News mode
                helpView->expandDock(false);
                lastTab = 3;   
            }
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

void KTMainWindow::exportProject()
{
    QDesktopWidget desktop;

    KTExportWidget exportWidget(m_projectManager->project(), this);
    exportWidget.show();
    exportWidget.move((int) (desktop.screenGeometry().width() - exportWidget.width())/2, 
                      (int) (desktop.screenGeometry().height() - exportWidget.height())/2);

    exportWidget.exec();
}

void KTMainWindow::callSave()
{
    if (m_projectManager->isModified())
        saveProject();
}

void KTMainWindow::expandExposureView(int index) 
{
    contextMode = KTProject::Mode(index);

    if (contextMode == KTProject::FRAMES_EDITION) {
        exposureView->expandDock(true);
        exposureView->enableButton(true);
    } else {
        exposureView->expandDock(false);
        exposureView->enableButton(false);
    }
}

void KTMainWindow::expandColorView() 
{
    if (colorView->isExpanded())
        colorView->expandDock(false); 
    else
        colorView->expandDock(true);
}

void KTMainWindow::requestProject()
{
    if (KTMainWindow::requestType == NewNetProject) {
        m_projectManager->setupNewProject();
    } else if (KTMainWindow::requestType == OpenNetProject) {
               KTListProjectsPackage package;
               netProjectManagerHandler->sendPackage(package);
    } else if (KTMainWindow::requestType == ImportProjectToNet) {
               const char *home = getenv("HOME");
               QString file = QFileDialog::getOpenFileName(this, tr("Import project package"),
                                                           home, tr("Tupi Project Package (*.tup)"));
               if (file.length() > 0) {
                   QFile project(file);
                   if (project.exists()) {
                       if (project.size() > 0) {
                           KTImportProjectPackage package(file);
                           netProjectManagerHandler->sendPackage(package);
                        } else {
                           TOsd::self()->display(tr("Error"), tr("Can't import project. File is empty!"), TOsd::Error);
                           netProjectManagerHandler->closeProject();
                        }
                   } else {
                        TOsd::self()->display(tr("Error"), tr("Can't save the project. File doesn't exist!"), TOsd::Error);
                        netProjectManagerHandler->closeProject();
                   }
               } else {
                   netProjectManagerHandler->closeProject();
               }
    }
}

void KTMainWindow::unexpectedClose()
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

void KTMainWindow::netProjectSaved()
{
    m_projectManager->undoModified();
}

void KTMainWindow::postVideo(const QString &title, const QString &description, int fps, const QList<int> sceneIndexes)
{
    netProjectManagerHandler->sendVideoRequest(title, description, fps, sceneIndexes);
}

void KTMainWindow::updatePlayer(bool remove)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!remove) {
        int sceneIndex = drawingTab->currentSceneIndex();
        viewCamera->updateScenes(sceneIndex);
    } 
}
