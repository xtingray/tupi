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
#include "kdebug.h"
#include "kosd.h"
#include "kimageeffect.h"
#include "kaudioplayer.h"

#include "ktapplication.h"
#include "ktpluginmanager.h"
#include "ktprojectcommand.h"
#include "ktlocalprojectmanagerhandler.h"

// #ifdef USE_NET
#include "ktnetprojectmanagerhandler.h"
#include "ktnetprojectmanagerparams.h"
#include "ktconnectdialog.h"
#include "ktsavenetproject.h"
#include "ktlistpackage.h"
#include "ktimportprojectpackage.h"
#include "ktlistprojectspackage.h"
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
       KINIT;
    #endif

    // Loading audio player plugin
    KAudioPlayer::instance()->loadEngine("gstreamer"); // FIXME: Move this to the settings 
    setObjectName("KTMainWindow_");

    // Defining the status bar
    m_statusBar = new KTStatusBar(this);
    setStatusBar(m_statusBar);

    // Naming the main frame...
    setWindowTitle(tr("Tupi: Magic 2D"));
    setWindowIcon(QIcon(THEME_DIR + "icons/about.png"));

    // Defining the render type for the drawings
    m_renderType = Tupi::RenderType(KCONFIG->value("RenderType").toInt());

    // Calling out the project manager
    m_projectManager = new KTProjectManager(this);
    m_projectManager->setHandler(new KTLocalProjectManagerHandler);

    splash->setMessage(tr("Setting up the project manager"));
    SleeperThread::msleep(500);

    // Calling out the events/actions manager
    splash->setMessage(tr("Loading action manager..."));
    m_actionManager = new KActionManager(this);

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
    KCONFIG->beginGroup("TipOfDay");
    //bool showTips = qvariant_cast<bool>(KCONFIG->value("ShowOnStart", true));
    bool showTips = KCONFIG->value("ShowOnStart", true).toBool();

    // If option is enabled, then, show a little dialog with a nice tip
    if (showTips)
        QTimer::singleShot(0, this, SLOT(showTipDialog()));

    // Time to load plugins... 
    KTPluginManager::instance()->loadPlugins();

    // Defining the Drawing view, as the first interface to show up	
    setCurrentPerspective(Drawing);

    KCONFIG->beginGroup("General");
    // check if into the config file, user always wants to start opening his last project 
    // created
    bool openLast = KCONFIG->value("OpenLastProject").toBool();

    if (openLast && parameters == 1)
        openProject(KCONFIG->value("LastProject").toString());

    if (KCONFIG->firstTime()) {
        KCONFIG->setValue("OpenLastProject", openLast);
        KCONFIG->setValue("AutoSave", 2);
    }

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
    KEND;
#endif

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear(QClipboard::Clipboard);

    delete KTPluginManager::instance();
    delete KOsd::self();
}

/**
 * @if english
 * This method cleans and set the whole interface to start a new project.
 * @endif
 * @if spanish
 * Este metodo limpia y configura toda la interfaz para iniciar un nuevo proyecto.
 * @endif
*/

void KTMainWindow::createNewProject()
{
    // Modal
    //if (!closeProject())
    //    return;

    m_projectManager->setupNewProject();
    enableToolViews(true);
    setMenuItemsContext(true);

    if (!m_isNetworkProject)
        viewNewDocument();
}

/**
 * @if english
 * This method supports all the low level tasks for the method createNewProject().
 * @endif
 * @if spanish
 * Este metodo soporta todas las tareas de bajo nivel para el metodo createNewProject().
 * @endif
*/

//void KTMainWindow::viewNewDocument(const QString &title)

void KTMainWindow::viewNewDocument()
{
    #ifdef K_DEBUG
       K_FUNCINFO;
    #endif

    if (m_projectManager->isOpen()) {

        contextMode = KTProject::FRAMES_EDITION;

        // Setting undo/redo actions
        setUndoRedoActions();

        // messageToStatus(tr("Opening a new paint area..."));
        KOsd::self()->display(tr("Information"), tr("Opening a new document..."));

        drawingTab = new KTViewDocument(m_projectManager->project());
        connectToDisplays(drawingTab);

        drawingTab->setWindowTitle(tr("Illustration"));

        addWidget(drawingTab);

        connectToDisplays(drawingTab);
        ui4project(drawingTab);
        ui4localRequest(drawingTab);
        connect(drawingTab, SIGNAL(modeHasChanged(int)), this, SLOT(expandExposureView(int))); 
        connect(drawingTab, SIGNAL(expandColorPanel()), this, SLOT(expandColorView()));
      
        drawingTab->setAntialiasing(true);

        // KTViewCamera *
        viewCamera = new KTViewCamera(m_projectManager->project());
        ui4project(viewCamera);

        animationTab = new KTAnimationspace(viewCamera);
        animationTab->setWindowIcon(QIcon(THEME_DIR + "icons/animation_mode.png"));
        animationTab->setWindowTitle(tr("Animation"));
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

        QString twitterPath = QDir::homePath() + "/." + QCoreApplication::applicationName() + "/twitter.html";

        if (QFile::exists(twitterPath)) {
            internetOn = true;
            newsTab = new KTwitterWidget(this); 
            newsTab->setSource(twitterPath);
            addWidget(newsTab);
        } 

        connect(this, SIGNAL(tabHasChanged(int)), this, SLOT(updateCurrentTab(int)));

        exposureView->expandDock(true);
        connect(drawingTab, SIGNAL(autoSave()), this, SLOT(callSave()));

        m_projectManager->undoModified();

        KCONFIG->beginGroup("PenParameters");
        int thicknessValue = KCONFIG->value("Thickness", -1).toInt();
        m_penWidget->init();
        m_penWidget->setThickness(thicknessValue);
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
       kWarning() << "Creating new project...";
    #endif

    projectSaved = false;

    KTNewProject *wizard = new KTNewProject(this);
    QDesktopWidget desktop;
    wizard->show();
    wizard->move((int) (desktop.screenGeometry().width() - wizard->width())/2 , 
                 (int) (desktop.screenGeometry().height() - wizard->height())/2);
    wizard->focusProjectLabel();

    //connectToDisplays(wizard);

    if (wizard->exec() != QDialog::Rejected) {

       if (wizard->useNetwork())
           setupNetworkProject(wizard->parameters());
       else
           setupLocalProject(wizard->parameters());

       createNewProject();
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

    setCurrentTab(0);

    // if (colorView->isExpanded())
        colorView->expandDock(false);

    // if (penView->isExpanded())
        penView->expandDock(false);

    // if (libraryView->isExpanded())
        libraryView->expandDock(false);

    //if (helpView->isExpanded())
        helpView->expandDock(false);
    
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

    m_projectManager->closeProject();

    // Cleaning widgets
    m_exposureSheet->blockSignals(true);
    m_exposureSheet->closeAllScenes();
    m_exposureSheet->blockSignals(false);

    m_timeLine->closeAllScenes();
    m_scenes->closeAllScenes();
    m_libraryWidget->resetGUI();

    m_fileName = QString();

    enableToolViews(false);

    if (scenesView->isExpanded())
        scenesView->expandDock(false);

    if (exposureView->isExpanded())
        exposureView->expandDock(false);

    m_statusBar->setStatus(tr(""));
    projectSaved = false;

    setUpdatesEnabled(true);

    setWindowTitle(tr("Tupi: Magic 2D"));

    return true;
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

bool KTMainWindow::setupNetworkProject(const QString& projectName ,const QString &server , int port)
{
    KTConnectDialog *cndialog = new KTConnectDialog(this);

    if (!server.isEmpty())
        cndialog->setServer(server);

    if (port != -1)
        cndialog->setPort(port);

    KTNetProjectManagerParams *params = new KTNetProjectManagerParams();

    if (cndialog->exec() == QDialog::Accepted) {
        params->setServer(cndialog->server());
        params->setPort(cndialog->port());
        params->setLogin(cndialog->login());
        params->setPassword(cndialog->password());
        params->setProjectName(projectName);

        return setupNetworkProject(params);
    }

    return false;
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

bool KTMainWindow::setupNetworkProject(KTProjectManagerParams *params)
{
    if (closeProject()) {
        KTNetProjectManagerHandler *netProjectManagerHandler =  new KTNetProjectManagerHandler;
        connect(netProjectManagerHandler, SIGNAL(openNewArea(const QString&)), this, SLOT(viewNewDocument(const QString&)));
        m_projectManager->setHandler(netProjectManagerHandler);
        m_projectManager->setParams(params);
        m_isNetworkProject = true;

        if (m_viewChat) {
            removeToolView(m_viewChat);
            delete m_viewChat;
        }

        m_viewChat = addToolView(netProjectManagerHandler->comunicationWidget(), Qt::RightDockWidgetArea, All, "Chat");
        m_viewChat->setVisible(false);

        return true;
    }

    return false;
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

bool KTMainWindow::setupLocalProject(KTProjectManagerParams *params)
{
    if (closeProject()) {
        m_projectManager->setHandler(new KTLocalProjectManagerHandler);
        m_projectManager->setParams(params);
        m_isNetworkProject = false;
        setWindowTitle(params->projectName() + " - " + tr("Tupi: Magic 2D"));

        return true;
    }

    return false;
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

    QString package = QFileDialog::getOpenFileName(this, tr("Import project package"), home, 
                      tr("Tupi Project Package (*.tup);;Tupi Net Project (*.ntup)"));

    if (package.isEmpty()) 
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

void KTMainWindow::openProject(const QString &path)
{
    #ifdef K_DEBUG
       kWarning() << "Opening project: " << path;
    #endif

    if (path.isEmpty())
        return;

    if (path.endsWith(".ntup")) {
        KTSaveNetProject loader;
        KTNetProjectManagerParams *params = loader.params(path);

        setupNetworkProject(params->projectName(), params->server(), params->port());
        delete params;
    } else if (path.endsWith(".tup")) {
               m_projectManager->setHandler(new KTLocalProjectManagerHandler);
               m_isNetworkProject = false;
    }

    if (closeProject()) {
        setUpdatesEnabled(false);
        tabWidget()->setCurrentWidget(drawingTab);

        if (m_projectManager->loadProject(path)) {
            if (QDir::isRelativePath(path))
                m_fileName = QDir::currentPath() + "/" + path;
            else
                m_fileName = path;

            setWindowTitle(m_projectManager->project()->projectName() + " - " + tr("Tupi: Magia 2D"));

            viewNewDocument();
			
            // SQA: Apparently this code is not required
            /*
            KTFrameResponse response(KTProjectRequest::Frame, KTProjectRequest::Select);
            response.setSceneIndex(0);
            response.setLayerIndex(0);
            response.setFrameIndex(0);
            drawingTab->handleProjectResponse(&response);
            m_exposureSheet->handleProjectResponse(&response);
            m_timeLine->handleProjectResponse(&response);
            */

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

            // Showing a info message in a bubble
            KOsd::self()->display(tr("Information"), tr("Project %1 opened!").arg(m_projectManager->project()->projectName()));
        } else {
                 setUpdatesEnabled(true);
                 KOsd::self()->display(tr("Error"), tr("Cannot open project!"), KOsd::Error);
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
    if (setupNetworkProject()) {
        KTNetProjectManagerHandler *handler = static_cast<KTNetProjectManagerHandler *>
                                              (m_projectManager->handler());
        if (handler->isValid()) {
            KTListProjectsPackage package;
            handler->sendPackage(package);
        }
    }
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
     if (setupNetworkProject()) {
         KTNetProjectManagerHandler *handler = static_cast<KTNetProjectManagerHandler *>
                                               (m_projectManager->handler());

         if (handler->isValid()) {
             const char *home = getenv("HOME");
             QString file = QFileDialog::getOpenFileName(this, tr("Import project package"), 
                                                     home, tr("Tupi Project Package (*.tup)"));
             KTImportProjectPackage package(file);		
             handler->sendPackage(package);
         }
     }
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
       kDebug("project") << "KTMainWindow::save() - Saving...";
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

void KTMainWindow::ui4project(QWidget *widget)
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

void KTMainWindow::ui4paintArea(QWidget *widget)
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

void KTMainWindow::ui4localRequest(QWidget *widget)
{
    connect(widget, SIGNAL(localRequestTriggered(const KTProjectRequest *)), m_projectManager, 
            SLOT(handleLocalRequest(const KTProjectRequest *)));
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
    home.append("/" + m_projectManager->project()->projectName());

    isSaveDialogOpen = true;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Build project package"), home, 
                       tr("Tupi Project Package (*.tup);;Tupi Net Project (*.ntup)"));

    if (fileName.isEmpty()) {
        isSaveDialogOpen = false;
        return;
    }

    int indexPath = fileName.lastIndexOf("/");
    int indexFile = fileName.length() - indexPath;
    QString name = fileName.right(indexFile - 1);
    QString path = fileName.left(indexPath + 1);

    QDir directory(path);
    if (!directory.exists()) {
        KOsd::self()->display(tr("Error"), tr("Directory \"" + path.toLocal8Bit() + "\" does not exist! Please, choose another path."), KOsd::Error);
        return;
    } else {
        QFile file(directory.filePath(name));
        if (!file.open(QIODevice::ReadWrite)) {
            file.remove();
            KOsd::self()->display(tr("Error"), tr("You have no permission to create this file. Please, choose another path."), KOsd::Error);
            return;
        }
        file.remove();
    }

    m_fileName = fileName;
    isSaveDialogOpen = false;

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
    if (isSaveDialogOpen)
        return;

    if (m_fileName.isEmpty()) {
        saveAs();
        return;
    }

    if (m_projectManager->saveProject(m_fileName)) {  
        KOsd::self()->display(tr("Information"), tr("Project %1 saved").arg(m_projectManager->project()->projectName()));
        projectSaved = true;
        int indexPath = m_fileName.lastIndexOf("/");
        int indexFile = m_fileName.length() - indexPath;
        QString name = m_fileName.right(indexFile - 1);
        int indexDot = name.lastIndexOf(".");
        name = name.left(indexDot);

        setWindowTitle(name + " - " + tr("Tupi: Magic 2D"));
    } else {
        KOsd::self()->display(tr("Error"), tr("Cannot save the project!"), KOsd::Error);
    }

    if (isSaveDialogOpen)
        isSaveDialogOpen = false;
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

    KCONFIG->beginGroup("General");
    KCONFIG->setValue("LastProject", lastProject);
    KCONFIG->setValue("Recents", m_recentProjects);

    KMainWindow::closeEvent(event);
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
    if (!drawingTab) 
        return;

    KTPaintAreaCommand *command = drawingTab->createCommand(event);

    if (command) { 
        m_projectManager->undoHistory()->push(command);

        if (event->action() == 2)
            m_penWidget->setPenColor(qvariant_cast<QColor>(event->data()));
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

    if (index == 1) {  // Animation mode 

        if (lastTab == 2)
            helpView->expandDock(false);
        viewCamera->updatePhotograms(m_projectManager->project());
        lastTab = 1;
        viewCamera->setFocus();

    } else {

        if (index == 0) { // Illustration mode
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
    if (projectSaved && m_projectManager->isModified())
        saveProject();
}


void KTMainWindow::expandExposureView(int index) {
    contextMode = static_cast<KTProject::Mode>(index);

    if (contextMode == KTProject::FRAMES_EDITION) {
        exposureView->expandDock(true);
        exposureView->enableButton(true);
    } else {
        exposureView->expandDock(false);
        exposureView->enableButton(false);
    }
}

void KTMainWindow::expandColorView() {
    if (colorView->isExpanded())
        colorView->expandDock(false); 
    else
        colorView->expandDock(true);
}
