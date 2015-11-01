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

#ifndef TIPDATABASE_H
#define TIPDATABASE_H

#include "tglobal.h"
#include "talgorithm.h"

#include <QDomDocument>
#include <QFile>
#include <QWidget>
#include <QString>
#include <QList>

/**
 * @struct Tip
 * @author David Cuadrado
 */

struct Tip
{
    QString text;
};

/**
 * @class TipDatabase
 * 
 * This class handles the tips database about the application
 * 
 * El archivo de base de datos tiene la siguiente forma:
 * 
 * @verbatim
 * \<TipOfDay\>
 *     \<tip>Tip1.\</tip\>
 *     \<tip>Tipn\</tip\>
 * \</TipOfDay\>
 * @endverbatim
 * 
 * @author David Cuadrado
 */

class T_CORE_EXPORT TipDatabase : public QWidget
{
    public:
        TipDatabase(const QString &file, QWidget *parent=0);
        ~TipDatabase();
        Tip tip() const;
        void nextTip();
        void prevTip();
        
    private:
        void loadTips(const QString &file);
        
    private:
        struct Private;
        Private *const k;
};

#endif
