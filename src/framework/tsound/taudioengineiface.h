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

#ifndef TAUDIOENGINEIFACE_H
#define TAUDIOENGINEIFACE_H

#include <QString>
#include <QPluginLoader>
#include <QUrl>

#include "qplugin.h"
#include "tglobal.h"

/**
 * @author David Cuadrado
*/

class T_CORE_EXPORT TAudioEngineIface
{
    public:
        virtual ~TAudioEngineIface() {}
        virtual QString key() const = 0;
        
        /**
         * If can't load this function returns -1, else returns the player id
         * @param url 
         * @param id 
         * @return 
         */
        virtual int load(const QUrl &url, int id = -1) = 0;
        
        virtual bool init() = 0;
        virtual bool play(int offset = 0) = 0;
        
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void seek(uint ms) = 0;
        
        virtual bool setCurrentPlayer(int id) = 0;
        
        virtual void setVolume(int percent) = 0;
};

Q_DECLARE_INTERFACE(TAudioEngineIface, "org.tupi.TAudioEngineIface/0.1");

#endif

