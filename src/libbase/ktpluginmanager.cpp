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

#include "ktpluginmanager.h"
#include "ktfilterinterface.h"
#include "kttoolinterface.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QPluginLoader>
#include <QDir>

KTPluginManager *KTPluginManager::s_instance = 0;

struct KTPluginManager::Private
{
    QObjectList tools;
    QObjectList filters;
    QList<QPluginLoader *> loaders;
};

KTPluginManager::KTPluginManager(QObject *parent) : QObject(parent), k(new Private)
{
}

KTPluginManager::~KTPluginManager()
{
    unloadPlugins();
}

KTPluginManager *KTPluginManager::instance()
{
    if (!s_instance)
        s_instance = new KTPluginManager;
    
    return s_instance;
}

void KTPluginManager::loadPlugins()
{
    #ifdef K_DEBUG
           tDebug("plugins") << "KTPluginManager::loadPlugins() - Loading plugins...";
    #endif

    k->filters.clear();
    k->tools.clear();
    
    QDir pluginDirectory = QDir(PLUGINS_DIR);

    foreach (QString fileName, pluginDirectory.entryList(QStringList() << "*.so" << "*.dll" << "*.dylib", QDir::Files)) {
             QPluginLoader *loader = new QPluginLoader(pluginDirectory.absoluteFilePath(fileName));
             QObject *plugin = qobject_cast<QObject*>(loader->instance());

             #ifdef K_DEBUG
                    tDebug("plugins") << "*** Trying to load plugin from: " << fileName;
             #endif
        
             if (plugin) {
                 AFilterInterface *filter = qobject_cast<AFilterInterface *>(plugin);
                 KTToolInterface *tool = qobject_cast<KTToolInterface *>(plugin);
            
                 if (filter) {
                     k->filters << plugin;
                 } else if (tool) {
                            k->tools << plugin;
                 }
            
                 k->loaders << loader;
             } else {
                 #ifdef K_DEBUG
                        tError("plugins") << "KTPluginManager::loadPlugins() - Cannot load plugin, error was: " << loader->errorString();
                 #endif
             }
    }
}

void KTPluginManager::unloadPlugins()
{
    #ifdef K_DEBUG
           tDebug("plugins") << "KTPluginManager::unloadPlugins() - Unloading plugins...";
    #endif

    foreach (QPluginLoader *loader, k->loaders) {
             delete loader->instance();
             delete loader;
    }
}

QObjectList KTPluginManager::tools() const
{
    return k->tools;
}

QObjectList KTPluginManager::filters() const
{
    return k->filters;
}
