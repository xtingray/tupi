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

#ifndef TGLOBAL_H
#define TGLOBAL_H

/**
 * This class defines enumerations used in the whole code of the project.
 *
 * @author David Cuadrado
*/

#if defined(QT_SHARED) || defined(QT_PLUGIN)
#define T_GUI_EXPORT Q_GUI_EXPORT
#define T_CORE_EXPORT Q_DECL_EXPORT
#define T_SOUND_EXPORT Q_DECL_EXPORT
#define TUPI_EXPORT Q_GUI_EXPORT
#define TUPI_PLUGIN Q_DECL_EXPORT
#else
#define T_GUI_EXPORT
#define T_CORE_EXPORT
#define T_SOUND_EXPORT
#define TUPI_EXPORT
#define TUPI_PLUGIN
#endif

#ifdef QT_GUI_LIB
#include <QImage>
#endif

#ifdef K_DEBUG

#ifdef Q_OS_WIN
#include <QDebug>
#else
#include "tdebug.h"
#endif

#endif

#define LIBRARY_DIR CONFIG_DIR+"/libraries"

namespace Tupi 
{
    enum RenderType
      {
       Image = 0,
       OpenGL,
       Native
      };

    enum MessageType
      {
       Information = 0,
       Warning,
       Error,
       Critical
      };
};

#endif

#define ZLAYER_LIMIT 10000
