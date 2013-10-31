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

#include "tuppackagehandler.h"
#include "quazip.h"
#include "quazipfile.h"
#include "tdebug.h"
#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

/**
 * This class manages the file format for Tupi projects.
 * Here is where methods to handle Tupi files are defined.
 * @author David Cuadrado
*/

struct TupPackageHandler::Private
{
    QString importedProjectPath;
};

TupPackageHandler::TupPackageHandler() : k(new Private)
{
}

TupPackageHandler::~TupPackageHandler()
{
    delete k;
}

bool TupPackageHandler::makePackage(const QString &projectPath, const QString &packagePath)
{
    if (!QFile::exists(projectPath)) {
        #ifdef K_DEBUG
               tError() << "TupPackageHandler::makePackage() - Project path doesn't exist -> " << projectPath;
        #endif
        return false;
    }
    
    QFileInfo packageInfo(packagePath);
    QuaZip zip(packagePath);

    if (!zip.open(QuaZip::mdCreate)) {
        #ifdef K_DEBUG
               tError() << "TupPackageHandler::makePackage() - Error while create package: " << zip.getZipError();
        #endif
        return false;
    }

    if (! compress(&zip, projectPath)) {
        #ifdef K_DEBUG
               tError() << "TupPackageHandler::makePackage() - Error while compress project" << zip.getZipError();
        #endif
        return false;
    }
    
    zip.close();

    if (zip.getZipError() != 0) {
        #ifdef K_DEBUG
               tError() << "TupPackageHandler::makePackage() - Error: " << zip.getZipError();
        #endif
        return false;
    }
    
    return true;
}

bool TupPackageHandler::compress(QuaZip *zip, const QString &path)
{
    QFile inFile;
    QuaZipFile outFile(zip);
    char c;

    QFileInfoList files= QDir(path).entryInfoList();
    
    foreach (QFileInfo file, files) {
             QString filePath = path + "/" + file.fileName();

             if (file.fileName().startsWith("."))
                 continue;
        
             if (file.isDir()) {
                 compress(zip, file.path() + "/" + file.fileName());
                 continue;
             }
        
             if (!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(stripRepositoryFromPath(filePath), stripRepositoryFromPath(filePath)))) 
                 return false;

             inFile.setFileName(filePath);

             if (!inFile.open(QIODevice::ReadOnly)) {
                 #ifdef K_DEBUG
                        tError() << "Error opening file " << inFile.fileName() << " : " << inFile.errorString();
                 #endif
                 return false;
             }

             while (inFile.getChar(&c) && outFile.putChar(c)) {};
        
             if (outFile.getZipError()!=UNZ_OK)
                 return false;

             outFile.close();

             // if (outFile.getZipError()!=UNZ_OK)
             //     return false;

             inFile.close();
    }
    
    return true;
}

QString TupPackageHandler::stripRepositoryFromPath(QString path)
{
    path.remove(CACHE_DIR);

    if (path[0] == QDir::separator())
        path.remove(0, 1);

    return path;
}

bool TupPackageHandler::importPackage(const QString &packagePath)
{
    QuaZip zip(packagePath);
    
    if (!zip.open(QuaZip::mdUnzip)) {
        #ifdef K_DEBUG
               tDebug() << "TupPackageHandler::importPackage() - Error while open package: " << zip.getZipError();
        #endif
        return false;
    }

    zip.setFileNameCodec("IBM866"); // SQA: What is it? 
    
    QuaZipFile file(&zip);
    QFile out;
    QString name;
    char c;
    QuaZipFileInfo info;
    bool next = zip.goToFirstFile();

    while (next) {

           if (!zip.getCurrentFileInfo(&info)) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Can't get current file: " << zip.getZipError();
               #endif
               return false;
           }
        
           if (!file.open(QIODevice::ReadOnly)) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Can't open file "<< file.getZipError();
               #endif
               return false;
           }
        
           name = CACHE_DIR + file.getActualFileName();

           if (name.endsWith(QDir::separator()))
               name.remove(name.count()-1, 1);

           if (name.endsWith(".tpp"))
               k->importedProjectPath = QFileInfo(name).path();
        
           if (file.getZipError() != UNZ_OK) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Error while open package " << file.getZipError();
               #endif
               return false;
           }
        
           if (createPath(name)) {

               out.setFileName(name);
        
               if (! out.open(QIODevice::WriteOnly)) {
                   #ifdef K_DEBUG
                          tError() << "TupPackageHandler::importPackage() - Error while open file: " << out.fileName(); 
                          tError() << "TupPackageHandler::importPackage() - Error Description: " << out.errorString();
                          tError() << "TupPackageHandler::importPackage() - Error type: " << out.error(); 
                   #endif
                   return false;
               }
        
               while (file.getChar(&c)) 
                      out.putChar(c);

               out.close();
           } else {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Error creating path: " << name; 
               #endif
               return false;
           }

           if (file.getZipError()!=UNZ_OK) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Error while open package " << file.getZipError();
               #endif
               return false;
           }

           if (!file.atEnd()) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Not EOF Error";
               #endif
               return false;
           }

           file.close();

           if (file.getZipError()!=UNZ_OK) {
               #ifdef K_DEBUG
                      tError() << "TupPackageHandler::importPackage() - Error while open package " << file.getZipError();
               #endif
               return false;
           }
            
           next = zip.goToNextFile();
    }
    
    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        #ifdef K_DEBUG
               tError() << "TupPackageHandler::importPackage() - Error while open package " << file.getZipError();
        #endif
        return false;
    }
    
    return true;
}

bool TupPackageHandler::createPath(const QString &filePath)
{
    QFileInfo info(filePath);
    QDir path = info.dir();
    QString target = path.path();

    if (!path.exists()) 
        return path.mkpath(target);
    else 
        return true;
    
    return false;
}

QString TupPackageHandler::importedProjectPath() const
{
    return k->importedProjectPath;
}

QString TupPackageHandler::projectDirectory() const
{
    int index =  k->importedProjectPath.lastIndexOf(QDir::separator());
    QString directory = k->importedProjectPath.right(k->importedProjectPath.length() - (index + 1));

    return directory;
}
