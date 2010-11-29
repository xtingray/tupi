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

#include "ktnetprojectmanagerparams.h"

KTNetProjectManagerParams::KTNetProjectManagerParams()
           : KTProjectManagerParams(), m_server("localhost"), m_port(6502)
{
}

KTNetProjectManagerParams::~KTNetProjectManagerParams()
{
}

void KTNetProjectManagerParams::setLogin(const QString &login)
{
    m_login = login;
}

QString KTNetProjectManagerParams::login() const
{
    return m_login;
}

void KTNetProjectManagerParams::setPassword(const QString &passwd)
{
    m_password = passwd;
}

QString KTNetProjectManagerParams::password() const
{
    return m_password;
}

void KTNetProjectManagerParams::setServer(const QString &server)
{
    m_server = server;
}

QString KTNetProjectManagerParams::server() const
{
    return m_server;
}

void KTNetProjectManagerParams::setPort(int port)
{
    m_port = port;
}

int KTNetProjectManagerParams::port() const
{
    return m_port;
}
