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

KTPluginManager::KTPluginManager(QObject *parent) : QObject(parent)
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
    m_filters.clear();
    m_tools.clear();
    
    QDir m_pluginDirectory = QDir(PLUGINS_DIR);

    foreach (QString fileName, m_pluginDirectory.entryList(QStringList() << "*.so" << "*.dll" << "*.dy", QDir::Files)) {
        QPluginLoader *loader = new QPluginLoader(m_pluginDirectory.absoluteFilePath(fileName));
        QObject *plugin = qobject_cast<QObject*>(loader->instance());
        
        tDebug("plugins") << "****** Trying to load plugin from: " << fileName;
        
        if (plugin) {
            AFilterInterface *aFilter = qobject_cast<AFilterInterface *>(plugin);
            KTToolInterface *aTool = qobject_cast<KTToolInterface *>(plugin);
            
            if (aFilter) {
                m_filters << plugin;
            } else if (aTool) {
                m_tools << plugin;
            }
            
            m_loaders << loader;
        } else {
            tFatal("plugins") << "Cannot load plugin, error was: " << loader->errorString();
        }
    }
}

void KTPluginManager::unloadPlugins()
{
    tDebug("plugins") << "Unloading plugins...";
    foreach (QPluginLoader *loader, m_loaders) {
             delete loader->instance();
             delete loader;
    }
}

QObjectList KTPluginManager::tools() const
{
    return m_tools;
}

QObjectList KTPluginManager::filters() const
{
    return m_filters;
}
