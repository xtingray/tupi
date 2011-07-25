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

#ifndef KTIPDATABASE_H
#define KTIPDATABASE_H

#include "tglobal.h"

#include <QWidget>
#include <QString>
#include <QList>

/**
 * @struct KTip
 * @author David Cuadrado
 */

struct KTip
{
    QString text;
};

/**
 * @class KTipDatabase
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

class K_CORE_EXPORT KTipDatabase : public QWidget
{
    public:
        KTipDatabase(const QString &file, QWidget *parent=0);
        ~KTipDatabase();
        KTip tip() const;
        void nextTip();
        void prevTip();
        
    private:
        void loadTips(const QString &file);
        
    private:
        struct Private;
        Private *const k;
};

#endif
