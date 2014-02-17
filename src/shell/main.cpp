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

#include "tupapplication.h"
#include "tupmainwindow.h"
#include "tuptwitter.h"
#include "tapplicationproperties.h"
#include "tdebug.h"
#include "tcollapsiblewidget.h"

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
#include <QStyleFactory>

/**
 * Main class of the application.
 * This is the code where Tupi application starts.
 * @author David Cuadrado
*/

/**
 * This method is the first one invoked when Tupi is launched
 */

int main(int argc, char ** argv)
{
    TupApplication application(argc, argv);
    QString slash = QDir::separator();

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

        #if defined(Q_OS_MAC)
            TCONFIG->setValue("Home", appDirPath.absolutePath());
        #else
            TCONFIG->setValue("Home", QString::fromLocal8Bit(::getenv("TUPI_HOME")));
        #endif
        TCONFIG->setValue("Cache", QDir::tempPath());
    }

#if defined(Q_OS_MAC)
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(appDirPath.absolutePath());
    kAppProp->setPluginDir(appDirPath.absolutePath() + slash + "plugins");
    kAppProp->setShareDir(appDirPath.absolutePath() + slash + "share");
#else
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(QString::fromLocal8Bit(::getenv("TUPI_BIN")));
    kAppProp->setPluginDir(QString::fromLocal8Bit(::getenv("TUPI_PLUGIN")));
    kAppProp->setShareDir(QString::fromLocal8Bit(::getenv("TUPI_SHARE")));
#endif

    QString locale = QString(QLocale::system().name()).left(2);

    if (locale.length() < 2)
        locale = "en";

    QDir dir(kAppProp->shareDir() + "data" + slash + locale + slash);
    if (! dir.exists())
        kAppProp->setDataDir(kAppProp->shareDir() + "data" + slash + "en" + slash);
    else
        kAppProp->setDataDir(kAppProp->shareDir() + "data" + slash + locale + slash);

    kAppProp->setThemeDir(kAppProp->shareDir() + "themes" + slash + "default" + slash);

    // Setting the repository directory (where the projects are saved)
    application.createCache(TCONFIG->value("Cache").toString());

    // SQA: Temporarily disabled until starting the port to Qt5
    // Downloading maefloresta Twitter status
    // Tupwitter *twitter = new Tupwitter();
    // twitter->start();

    QStyle *style = QStyleFactory::create("fusion");
    QApplication::setStyle(style);

    // Loading localization files... now you got Tupi in your native language

    QTranslator *translator = new QTranslator;
    translator->load(kAppProp->shareDir() + "data" + slash + "translations" + slash + "tupi_" + locale + ".qm");
    application.installTranslator(translator);

    TupMainWindow mainWindow(argc);
    mainWindow.showMaximized();

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
