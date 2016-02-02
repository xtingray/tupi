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

#ifndef TUPPACKAGEHANDLER_H
#define TUPPACKAGEHANDLER_H

#include "tglobal.h"
#include "tapplicationproperties.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

// SQA: Pending to enable in the future
// #include "quazip/JlCompress.h"

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>

// class QuaZip;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupPackageHandler
{
    public:

        TupPackageHandler();
        ~TupPackageHandler();
        
        bool makePackage(const QString &projectPath, const QString &packagePath);
        bool importPackage(const QString &packagePath);
        
        QString importedProjectPath() const;
        QString projectDirectory() const;
        
    private:
        bool compress(QuaZip *zip, const QString &path);
        QString stripRepositoryFromPath(QString path);
        bool createPath(const QString &filePath);
        
    private:
        struct Private;
        Private *const k;
};

#endif
