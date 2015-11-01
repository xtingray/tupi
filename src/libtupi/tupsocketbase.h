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

#ifndef TUPSOCKETBASE_H
#define TUPSOCKETBASE_H

#include "tglobal.h"

#include <QTextStream>
#include <QQueue>
#include <QTcpSocket>
#include <QDomDocument>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupSocketBase : public QTcpSocket
{
    Q_OBJECT

    public:
        TupSocketBase(QObject *parent = 0);
        ~TupSocketBase();
        
        void send(const QString &str);
        void send(const QDomDocument &doc);
        
        virtual void readed(const QString &readed) = 0;
        
    protected slots:
        virtual void readFromServer();
        
    public slots:
        void sendQueue();
        void clearQueue();
        
    private:
        struct Private;
        Private *const k;
        
};

#endif
