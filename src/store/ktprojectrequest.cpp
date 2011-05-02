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

#include "ktprojectrequest.h"
#include "ktprojectresponse.h"
#include "ktrequestbuilder.h"

#include "kdebug.h"

KTProjectRequestArgument::KTProjectRequestArgument()
{
}

KTProjectRequestArgument::KTProjectRequestArgument(const QString &v) : m_value(v)
{
}

KTProjectRequestArgument::~KTProjectRequestArgument()
{
}

void KTProjectRequestArgument::operator = (const QString &value)
{
    setValue(value);
}

void KTProjectRequestArgument::setValue(const QString &value)
{
    m_value = value;
}

bool KTProjectRequestArgument::toBool()
{
    if (m_value == "false" || m_value == "0")
        return false;

    return true;
}

int KTProjectRequestArgument::toInt()
{
    return m_value.toInt();
}

double KTProjectRequestArgument::toReal()
{
    return m_value.toDouble();
}

QString KTProjectRequestArgument::toString()
{
    return m_value;
}

struct KTProjectRequest::Private
{
    Private(const QString &xml) : xml(xml), id(Project), isExternal(false) 
     {
     }

    QString xml;
    int id;
    bool isExternal;
};

KTProjectRequest::KTProjectRequest(const QString &xml) : k(new Private(xml))
{
}

KTProjectRequest::~KTProjectRequest()
{
    delete k;
}

void KTProjectRequest::setId(int id)
{
    k->id = id;
}

int KTProjectRequest::id() const
{
    return k->id;
}

bool KTProjectRequest::isValid() const
{
     return !k->xml.isEmpty(); // TODO: Verficar que sea XML
}

QString KTProjectRequest::xml() const
{
     return k->xml;
}

void KTProjectRequest::setExternal(bool b)
{
     k->isExternal = b;
}

bool KTProjectRequest::isExternal() const
{
     return k->isExternal;
}

KTProjectRequest &KTProjectRequest::operator=(const KTProjectRequest &other)
{
    k->xml = other.k->xml;
    k->id = other.k->id;
    k->isExternal = other.k->isExternal;

    return *this;
}
