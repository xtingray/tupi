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

#include "ktconnectpackage.h"
#include "talgorithm.h"
#include "tmd5hash.h"
#include "tdebug.h"

#include <QStringList>

/*
<user_connect version="0">
    <client type="0"/> // 0 for Tupi artist clients - 1 for Tupi admin clients
    <username>user_username</username>
    <password>md5_hashed_password</password>
</user_connect>
*/

KTConnectPackage::KTConnectPackage(const QString &server, const QString &username, const QString &passwd) : QDomDocument()
{
    QDomElement root = createElement("user_connect");
    root.setAttribute("version", "0");
    appendChild(root);

    QDomElement client = createElement("client");
    client.setAttribute("type", 0);
    root.appendChild(client);

    root.appendChild(createElement("username")).appendChild(createTextNode(username));

    if (server.compare("tupitube.com") != 0) {
        root.appendChild(createElement("password")).appendChild(createTextNode(TMD5Hash::hash(passwd)));
    } else {
        QString salt = TAlgorithm::randomString(15);

        QDomElement token = createElement("salt");
        token.setAttribute("method", "sha512");
        token.appendChild(createTextNode(salt));
        root.appendChild(token);

        QStringList passwdList = TMD5Hash::passwords(passwd);
        for (int i = 0; i < passwdList.size(); ++i) { 
             root.appendChild(createElement("password")).appendChild(createTextNode(passwdList.at(i)));
        }
    }
}

KTConnectPackage::~KTConnectPackage()
{
}
