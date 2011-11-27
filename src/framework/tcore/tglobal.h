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

#ifndef TGLOBAL_H
#define TGLOBAL_H

#if defined(QT_SHARED) || defined(QT_PLUGIN)
# define K_GUI_EXPORT Q_GUI_EXPORT
# define K_CORE_EXPORT Q_DECL_EXPORT
# define K_SOUND_EXPORT Q_DECL_EXPORT
#else
# define K_GUI_EXPORT
# define K_CORE_EXPORT
# define K_SOUND_EXPORT
#endif

// #include "tapplicationproperties.h"

#define SHARE_DIR kAppProp->shareDir()
#define DATA_DIR kAppProp->dataDir()
#define THEME_DIR kAppProp->themeDir()
#define HOME_DIR kAppProp->homeDir()
#define CONFIG_DIR kAppProp->configDir()
#define PLUGINS_DIR kAppProp->pluginDir()
#define CACHE_DIR kAppProp->cacheDir()
#define REPOSITORY_DIR kAppProp->repositoryDir()

#endif
