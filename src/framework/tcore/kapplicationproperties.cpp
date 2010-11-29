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

#include "kapplicationproperties.h"

#include <QDir>
#include <QLocale>

#include <QApplication>

KApplicationProperties *KApplicationProperties::s_instance = 0;

struct KApplicationProperties::Private
{
    QString homeDir;
    QString shareDir;
    QString dataDir;
    QString themeDir;
    QString pluginDir;
    QString version;
    QString codeName;
    QString revision;
    QString cacheDir;
};

KApplicationProperties::KApplicationProperties() : k(new Private)
{
}

KApplicationProperties::~KApplicationProperties()
{
    delete k;
}

void KApplicationProperties::setShareDir(const QString &path)
{
    k->shareDir = path;
}

void KApplicationProperties::setDataDir(const QString &path)
{
    k->dataDir = path;
}

void KApplicationProperties::setHomeDir(const QString &path)
{
    k->homeDir = path;
}

void KApplicationProperties::setThemeDir(const QString &path)
{
    k->themeDir = path;
}

void KApplicationProperties::setPluginDir(const QString &path)
{
    k->pluginDir = path;
}

void KApplicationProperties::setCacheDir(const QString &path)
{
    k->cacheDir = path;
}

void KApplicationProperties::setVersion(const QString &path)
{
    k->version = path;
}

void KApplicationProperties::setCodeName(const QString &path)
{
    k->codeName = path;
}

void KApplicationProperties::setRevision(const QString &path)
{
    k->revision = path;
}

QString KApplicationProperties::shareDir() const
{
    if (k->shareDir.isEmpty())
        return k->homeDir + "/share";

    return k->shareDir + "/";
}

QString KApplicationProperties::dataDir() const
{
    if (k->dataDir.isEmpty()) {
        QString locale = QString(QLocale::system().name()).left(2);
        if (locale.length() < 2)
            locale = "en";

        return k->shareDir + "/data/" + locale + "/";
    }

    return k->dataDir;
}

QString KApplicationProperties::homeDir() const
{
    return k->homeDir + "/";
}

QString KApplicationProperties::themeDir() const
{
    if (k->themeDir.isEmpty())
        return k->shareDir + "/themes/default" + "/";

    return k->themeDir;
}

QString KApplicationProperties::pluginDir() const
{
    return k->pluginDir + "/";
}

QString KApplicationProperties::configDir() const
{
    return QDir::homePath() + "/." + qApp->applicationName() + "/";
}

QString KApplicationProperties::cacheDir() const
{
    return k->cacheDir;
}

QString KApplicationProperties::version() const
{
    return k->version;
}

QString KApplicationProperties::codeName() const
{
    return k->codeName;
}

QString KApplicationProperties::revision() const
{
    return k->revision;
}

KApplicationProperties *KApplicationProperties::instance()
{
    if (s_instance == 0)
        s_instance = new KApplicationProperties;

    return s_instance;
}
