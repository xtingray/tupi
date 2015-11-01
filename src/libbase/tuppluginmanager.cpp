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

#include "tuppluginmanager.h"
#include "tupfilterinterface.h"
#include "tuptoolinterface.h"
#include "tupexportinterface.h"

TupPluginManager *TupPluginManager::s_instance = 0;

struct TupPluginManager::Private
{
    QObjectList tools;
    QObjectList filters;
    QObjectList formats;
    QList<QPluginLoader *> loaders;
};

TupPluginManager::TupPluginManager(QObject *parent) : QObject(parent), k(new Private)
{
}

TupPluginManager::~TupPluginManager()
{
    unloadPlugins();
}

TupPluginManager *TupPluginManager::instance()
{
    if (!s_instance)
        s_instance = new TupPluginManager;
    
    return s_instance;
}

void TupPluginManager::loadPlugins()
{
    #ifdef K_DEBUG
        QString msg = "TupPluginManager::loadPlugins() - Loading plugins...";
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    k->filters.clear();
    k->tools.clear();
    k->formats.clear();
    
    QDir pluginDirectory = QDir(kAppProp->pluginDir());

    foreach (QString fileName, pluginDirectory.entryList(QStringList() << "*.so" << "*.dll" << "*.dylib", QDir::Files)) {
             QPluginLoader *loader = new QPluginLoader(pluginDirectory.absoluteFilePath(fileName));
             QObject *plugin = qobject_cast<QObject*>(loader->instance());

             #ifdef K_DEBUG
                 #ifdef Q_OS_WIN32
                     qWarning() << "TupPluginManager::loadPlugins() - Trying to load plugin from: " << fileName;
                 #else
                     tWarning("plugins") << "*** Trying to load plugin from: " << fileName;
                 #endif
             #endif
        
             if (plugin) {
                 AFilterInterface *filter = qobject_cast<AFilterInterface *>(plugin);
                 
                 if (filter) {
                     k->filters << plugin;
                 } else {
                     TupToolInterface *tool = qobject_cast<TupToolInterface *>(plugin);
                     if (tool) {
                         k->tools << plugin;
                     } else {
                         TupExportInterface *exporter = qobject_cast<TupExportInterface *>(plugin); 
                         if (exporter) {
                             k->formats << plugin;
                         }
                     } 
                 }

                 k->loaders << loader;
             } else {
                 #ifdef K_DEBUG
                     QString msg = "TupPluginManager::loadPlugins() - Cannot load plugin, error was: " + loader->errorString();
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
             }
    }
}

void TupPluginManager::unloadPlugins()
{
    #ifdef K_DEBUG
        QString msg = "TupPluginManager::unloadPlugins() - Unloading plugins...";
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    foreach (QPluginLoader *loader, k->loaders) {
             delete loader->instance();
             delete loader;
    }
}

QObjectList TupPluginManager::tools() const
{
    return k->tools;
}

QObjectList TupPluginManager::filters() const
{
    return k->filters;
}

QObjectList TupPluginManager::formats() const
{
    return k->formats;
}

