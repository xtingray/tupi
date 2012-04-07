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

#include "tupapplication.h"
#include "tupsplash.h"
#include "tupmainwindow.h"
#include "tuptwitter.h"
#include "tapplicationproperties.h"
#include "tdebug.h"
#include "tcollapsiblewidget.h"
#include "taudioplayer.h"

#ifdef Q_OS_UNIX
#include "tupcrashhandler.h"
#endif

#ifdef ENABLE_TUPISTYLE
#include "twaitstyle.h"
#endif

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDesktopWidget>
#include <QThread>
#include <QPlastiqueStyle>

#ifndef CONFIG_H
#define CONFIG_H
#endif

#ifndef HAVE_SOUND
#define HAVE_SOUND
#endif

#ifndef HAVE_FFMPEG
#define HAVE_FFMPEG
#endif

using namespace std;

class SleeperThread : public QThread
{
    public:
        static void msleep(unsigned long msecs)
        {
            QThread::msleep(msecs);
        }
};

/**
 * Main class of the application.
 * This is the code where Tupi application starts.
 * @author David Cuadrado
*/

/**
 * This method defines the console line options to run Tupi
 */
void usage();

/**
 * This method is the first one invoked when Tupi is launched
 */

int main(int argc, char ** argv)
{
    TupApplication application(argc, argv);

#ifdef K_DEBUG
       TDebug::setOutputChannel();
#endif

#ifdef Q_OS_UNIX
    // Initializing the crash handler, very useful to catch bugs
    TupCrashHandler::init();
#endif

    // Setting the current version for Tupi
    kAppProp->setVersion(VERSION);
    kAppProp->setCodeName(CODE_NAME);
    kAppProp->setRevision(REVISION);

    // Time to define global variables for Tupi
    TCONFIG->beginGroup("General");
    QDir appDirPath(QApplication::applicationDirPath());
    if (! TCONFIG->isOk()) {
#if defined(K_DEBUG)
        TCONFIG->setValue("Debug", "true");
#else
        TCONFIG->setValue("Debug", "false");
#endif
#if defined(__APPLE__)

        TCONFIG->setValue("Home", appDirPath.absolutePath());
#else
        TCONFIG->setValue("Home", QString::fromLocal8Bit(::getenv("TUPI_HOME")));
#endif
        TCONFIG->setValue("Cache", QDir::tempPath());
    }

#if defined(__APPLE__)
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(appDirPath.absolutePath());
    kAppProp->setPluginDir(appDirPath.absolutePath() + "/plugins");
    kAppProp->setShareDir(appDirPath.absolutePath()+ "/share");
#else
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(QString::fromLocal8Bit(::getenv("TUPI_BIN")));
    kAppProp->setPluginDir(QString::fromLocal8Bit(::getenv("TUPI_PLUGIN")));
    kAppProp->setShareDir(QString::fromLocal8Bit(::getenv("TUPI_SHARE")));
#endif
    QString locale = QString(QLocale::system().name()).left(2);

    if (locale.length() < 2)
        locale = "en";

    QDir dir(kAppProp->shareDir() + "data/" + locale + "/");
    if (! dir.exists())
        kAppProp->setDataDir(kAppProp->shareDir() + "data/en/");
    else
        kAppProp->setDataDir(kAppProp->shareDir() + "data/" + locale + "/");

    kAppProp->setThemeDir(kAppProp->shareDir() + "themes/default" + "/");

    /*
    tDebug() << "HOME_DIR: " << HOME_DIR;
    tDebug() << "kAppProp->shareDir(): " << kAppProp->shareDir();
    tDebug() << "DATA_DIR: " << DATA_DIR;
    tDebug() << "THEME_DIR: " << THEME_DIR;
    */

    // Setting the repository directory (where the projects are saved)
    application.createCache(TCONFIG->value("Cache").toString());

    // Downloading maefloresta Twitter status
    Tupwitter *ktwitter = new Tupwitter();
    ktwitter->start();

    // SQA: Tupi gui styles must be re-factored
    // Setting the gui style for the interface
#ifdef ENABLE_TUPISTYLE
    QApplication::setStyle(new KWaitStyle());
#elif defined(Q_OS_LINUX)
    QApplication::setStyle(new QPlastiqueStyle());
#endif

    // SQA: This code is deprecated
    /*
    if (kAppProp->homeDir().isEmpty() || application.isArg("r") || application.isArg("reconfigure")) {
        // Launching the basic configuration dialog
        if (! application.firstRun()) {
            // If dialog is canceled or Tupi can not be configured, kill the whole application
            #ifdef K_DEBUG
                  tFatal () << "********************* You need configure the application" << endl;
           #endif

           QMessageBox::critical(0, QObject::tr("Missing..."),
                                 QObject::tr("You need configure the application"));
           application.exit(-1);
           return -1;
        }

        // Setting the new global variables for Tupi
        kAppProp->setHomeDir(TCONFIG->value("Home").toString());
        application.createCache(TCONFIG->value("Cache").toString());
    }
    */

    // Time to apply the theme for the application GUI
    QString themefile = TCONFIG->value("ThemeFile").toString();

    if (! themefile.isEmpty())
        application.applyTheme(themefile);

    // Loading localization files... now you got Tupi in your native language

    QTranslator *translator = new QTranslator;
    translator->load(kAppProp->shareDir() + "data/translations/" + "tupi_" + locale + ".qm");
    application.installTranslator(translator);

    // Time to show the Tupi initial splash
    TupSplash *splash = new TupSplash;
    splash->show();
    QDesktopWidget desktop;
    splash->move((int) (desktop.screenGeometry().width() - splash->width())/2,
                 (int) (desktop.screenGeometry().height() - splash->height())/2);

    splash->setMessage(QObject::tr("Initializing..."));
    SleeperThread::msleep(500);

    splash->setMessage(QObject::tr("Loading modules..."));
    SleeperThread::msleep(500);

    TupMainWindow mainWindow(splash, argc);

    splash->setMessage(QObject::tr("Loaded!"));
    SleeperThread::msleep(500);

    mainWindow.showMaximized();

    delete splash;

    // Looking for plugins for Tupi
    #ifdef K_DEBUG
           tWarning() << "main.cpp - Loading plugins from: " << kAppProp->pluginDir();
    #endif
    QApplication::addLibraryPath(kAppProp->pluginDir());

    // Loading visual components required for the Crash Handler
    #ifdef Q_OS_UNIX
           CHANDLER->setConfig(DATA_DIR + "crashhandler.xml");
           CHANDLER->setImagePath(THEME_DIR + "icons/");
    #endif

    // If there is a second argument, it means to open a project from the command line
    if (argc == 2) {
        QString project = QString(argv[1]);
        if (project.endsWith(".tup") || project.endsWith(".TUP"))
            mainWindow.openProject(project);
    }

    // It's time to play with Tupi!
    return application.exec();
}

/**
 * This method defines the console line options to run Tupi
 */

void usage()
{
    // SQA: This code is deprecated
#if defined(Q_OS_UNIX)
    // Characters \033[1;33m and \033[1;34m are useful for colored messages
    puts(QString("\033[1;33m"+QApplication::applicationName() + kAppProp->version()).toLocal8Bit());
    puts(QString(QObject::tr("2D Animation Toolkit")+"\033[0;0m" ).toLocal8Bit());
    puts(QString("\033[1;34m"+QObject::tr("Usage: %1 [option]").arg(kApp->argv()[0])+"\033[0;0m").toLocal8Bit());
    puts(QString("\033[1;31m"+QObject::tr("Options: ")).toLocal8Bit());
    puts("-r, --reconfigure");
    puts(QObject::tr("\t\tReconfigure %1").arg(QApplication::applicationName()).toLocal8Bit());
    puts("\033[0;0m");
#else
    puts(QString(QApplication::applicationName() + kApp->version()).toLocal8Bit());
    puts(QObject::tr("Usage: %1 [option]").arg(kApp->argv()[0]).toLocal8Bit());
    puts(QObject::tr("Options: ").toLocal8Bit());
    puts("-r, --reconfigure");
    puts(QObject::tr("\t\tReconfigure %1").arg(QApplication::applicationName()).toLocal8Bit());
#endif
}

