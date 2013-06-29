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

#include "tapplicationproperties.h"

#include <QDir>
#include <QLocale>
#include <QApplication>

TApplicationProperties *TApplicationProperties::s_instance = 0;

struct TApplicationProperties::Private
{
    QString homeDir;
    QString binDir;
    QString shareDir;
    QString dataDir;
    QString themeDir;
    QString repositoryDir;
    QString pluginDir;
    QString version;
    QString codeName;
    QString revision;
    QString cacheDir;
};

TApplicationProperties::TApplicationProperties() : k(new Private)
{
}

TApplicationProperties::~TApplicationProperties()
{
    delete k;
}

void TApplicationProperties::setHomeDir(const QString &path)
{
    k->homeDir = path;
}

void TApplicationProperties::setBinDir(const QString &path)
{
    k->binDir = path;
}

void TApplicationProperties::setShareDir(const QString &path)
{
    k->shareDir = path;
}

void TApplicationProperties::setDataDir(const QString &path)
{
    k->dataDir = path;
}

void TApplicationProperties::setThemeDir(const QString &path)
{
    k->themeDir = path;
}

void TApplicationProperties::setPluginDir(const QString &path)
{
    k->pluginDir = path;
}

void TApplicationProperties::setCacheDir(const QString &path)
{
    k->cacheDir = path;
}

void TApplicationProperties::setRepositoryDir(const QString &path)
{
    k->repositoryDir = path;
}

void TApplicationProperties::setVersion(const QString &path)
{
    k->version = path;
}

void TApplicationProperties::setCodeName(const QString &path)
{
    k->codeName = path;
}

void TApplicationProperties::setRevision(const QString &path)
{
    k->revision = path;
}

QString TApplicationProperties::homeDir() const
{
    return k->homeDir + QDir::separator();
}

QString TApplicationProperties::binDir() const
{
    return k->binDir + QDir::separator();
}

QString TApplicationProperties::shareDir() const
{
    if (k->shareDir.isEmpty())
        return k->homeDir + QDir::separator() + "share";

    return k->shareDir + QDir::separator();
}

QString TApplicationProperties::dataDir() const
{
    if (k->dataDir.isEmpty()) {
        QString locale = QString(QLocale::system().name()).left(2);
        if (locale.length() < 2)
            locale = "en";

        return k->shareDir + QDir::separator() + "data" + QDir::separator() + locale + QDir::separator();
    }

    return k->dataDir;
}

QString TApplicationProperties::themeDir() const
{
    if (k->themeDir.isEmpty())
        return k->shareDir + QDir::separator() + "themes" + QDir::separator() + "default" + QDir::separator();

    return k->themeDir;
}

QString TApplicationProperties::pluginDir() const
{
    return k->pluginDir + QDir::separator();
}

QString TApplicationProperties::configDir() const
{
    return QDir::homePath() + QDir::separator() + "." + qApp->applicationName() + QDir::separator();
}

QString TApplicationProperties::cacheDir() const
{
    return k->cacheDir + QDir::separator();
}

QString TApplicationProperties::repositoryDir() const
{
    return k->repositoryDir + QDir::separator();
}

QString TApplicationProperties::version() const
{
    return k->version;
}

QString TApplicationProperties::codeName() const
{
    return k->codeName;
}

QString TApplicationProperties::revision() const
{
    return k->revision;
}

TApplicationProperties *TApplicationProperties::instance()
{
    if (s_instance == 0)
        s_instance = new TApplicationProperties;
    return s_instance;
}
