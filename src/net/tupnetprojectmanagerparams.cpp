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

#include "tupnetprojectmanagerparams.h"

TupNetProjectManagerParams::TupNetProjectManagerParams()
           : TupProjectManagerParams(), m_server("localhost"), m_port(6502)
{
}

TupNetProjectManagerParams::~TupNetProjectManagerParams()
{
}

void TupNetProjectManagerParams::setLogin(const QString &login)
{
    m_login = login;
}

QString TupNetProjectManagerParams::login() const
{
    return m_login;
}

void TupNetProjectManagerParams::setPassword(const QString &passwd)
{
    m_password = passwd;
}

QString TupNetProjectManagerParams::password() const
{
    return m_password;
}

void TupNetProjectManagerParams::setServer(const QString &server)
{
    m_server = server;
}

QString TupNetProjectManagerParams::server() const
{
    return m_server;
}

void TupNetProjectManagerParams::setPort(int port)
{
    m_port = port;
}

int TupNetProjectManagerParams::port() const
{
    return m_port;
}
