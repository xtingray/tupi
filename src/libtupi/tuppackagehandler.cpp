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
            QString msg = "TupPackageHandler::makePackage() - Project path doesn't exist -> " + projectPath;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return false;
    }

    // SQA: This code will be enabled in the future
    // return JlCompress::compressDir(packagePath, projectPath, true);
    
    QFileInfo packageInfo(packagePath);
    QuaZip zip(packagePath);

    if (!zip.open(QuaZip::mdCreate)) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::makePackage() - Error while create package: " + QString::number(zip.getZipError());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        
        return false;
    }

    if (! compress(&zip, projectPath)) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::makePackage() - Error while compress project: " + QString::number(zip.getZipError());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return false;
    }
    
    zip.close();

    if (zip.getZipError() != 0) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::makePackage() - Error: " + QString::number(zip.getZipError());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
                     QString msg = "Error opening file " + inFile.fileName() + " : " + inFile.errorString();
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
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
    /* SQA: Handy code to include in the future
    QFileInfo file(packagePath);
    k->importedProjectPath = CACHE_DIR + file.baseName();
    QStringList list = JlCompress::extractDir(packagePath, k->importedProjectPath);
    if (list.size() == 0) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::importPackage() - Project file is empty! -> " + packagePath;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return false;
    }
    */
        
    QuaZip zip(packagePath);
    
    if (!zip.open(QuaZip::mdUnzip)) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::importPackage() - Error while open package: " + QString::number(zip.getZipError());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
                   QString msg = "TupPackageHandler::importPackage() - Can't get current file: " + QString::number(zip.getZipError());
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif

               return false;
           }
        
           if (!file.open(QIODevice::ReadOnly)) {
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Can't open file " + QString::number(file.getZipError());
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif

               return false;
           }

           #ifdef Q_OS_WIN32
               name = file.getActualFileName();
            #else
               name = CACHE_DIR + file.getActualFileName();    
           #endif

           // name = CACHE_DIR + file.getActualFileName();

           if (name.endsWith(QDir::separator()))
               name.remove(name.count()-1, 1);

           if (name.endsWith(".tpp"))
               k->importedProjectPath = QFileInfo(name).path();
        
           if (file.getZipError() != UNZ_OK) {           
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Error while open package " + QString::number(file.getZipError());
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               
               return false;
           }
        
           if (createPath(name)) {
               out.setFileName(name);
        
               if (! out.open(QIODevice::WriteOnly)) {               
                   #ifdef K_DEBUG
                       QString msg1 = "TupPackageHandler::importPackage() - Error while open file: " + out.fileName();
                       QString msg2 = "TupPackageHandler::importPackage() - Error Description: " + out.errorString();
                       QString msg3 = "TupPackageHandler::importPackage() - Error type: " + QString::number(out.error());
                       #ifdef Q_OS_WIN32
                           qDebug() << msg1;
                           qDebug() << msg2;
                           qDebug() << msg3;
                       #else
                           tError() << msg1;
                           tError() << msg2;
                           tError() << msg3;
                       #endif
                   #endif                   

                   return false;
               }
        
               while (file.getChar(&c)) 
                      out.putChar(c);

               out.close();
           } else {           
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Error creating path: " + name;
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               
               return false;
           }

           if (file.getZipError()!=UNZ_OK) {
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Error while open package " + QString::number(file.getZipError());
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               return false;
           }

           if (!file.atEnd()) {
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Not EOF Error";
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               return false;
           }

           file.close();

           if (file.getZipError()!=UNZ_OK) {
               #ifdef K_DEBUG
                   QString msg = "TupPackageHandler::importPackage() - Error while open package " + QString::number(file.getZipError());
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
               return false;
           }
            
           next = zip.goToNextFile();
    }
    
    zip.close();

    if (zip.getZipError() != UNZ_OK) {
        #ifdef K_DEBUG
            QString msg = "TupPackageHandler::importPackage() - Error while open package " + QString::number(file.getZipError());
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
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
