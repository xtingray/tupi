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

#include "ktlibraryobject.h"
#include "ktitemfactory.h"
#include "ktpixmapitem.h"
#include "taudioplayer.h"
#include "tdebug.h"

#include <QTemporaryFile>
#include <QDir>

struct KTLibraryObject::Private
{
    int type;
    QVariant data;
    QString dataPath;
    QString symbolName;
    QString extension;
    QByteArray rawData; 
};

KTLibraryObject::KTLibraryObject(QObject *parent) : QObject(parent), k(new Private)
{
}

KTLibraryObject::~KTLibraryObject()
{
    delete k;
}

/**
 * Items, pics, text, and so...
 * @param data 
 */
void KTLibraryObject::setData(const QVariant &data)
{
    k->data = data;
}

QVariant KTLibraryObject::data() const
{
    return k->data;
}

void KTLibraryObject::setDataPath(const QString &path)
{
    k->dataPath = path;
}

QString KTLibraryObject::dataPath() const
{
    return k->dataPath;
}

void KTLibraryObject::setType(int type)
{
    k->type = type;
}

int KTLibraryObject::type() const
{
    return k->type;
}

void KTLibraryObject::setSymbolName(const QString &name)
{
    k->symbolName = name;
    k->symbolName.replace(QDir::separator(), "-");
    k->extension = k->symbolName.section('.', 1, 1).toUpper();
}

QString KTLibraryObject::symbolName() const
{
    return k->symbolName;
}

void KTLibraryObject::fromXml(const QString &xml)
{
    QDomDocument document;
    
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG  
               tError() << "KTLibraryObject::fromXml - [ Fatal Error ] - Invalid XML structure!";
        #endif
        return;
    }
    
    QDomElement objectTag = document.documentElement();

    if (objectTag.tagName() == "object") {
        setSymbolName(objectTag.attribute("id"));
        
        if (k->symbolName.isEmpty()) 
            return;
        
        k->type = objectTag.attribute("type").toInt();

        switch (k->type) {
                case KTLibraryObject::Item:
                case KTLibraryObject::Text:
                     {
                         QDomElement objectData = objectTag.firstChild().toElement();
                         if (!objectTag.isNull()) {
                             QString data;
                             {
                                 QTextStream ts(&data);
                                 ts << objectData;
                             }
                             loadRawData(data.toLocal8Bit());
                         } 
                     }
                break;
                case KTLibraryObject::Image:
                case KTLibraryObject::Svg:
                case KTLibraryObject::Sound:
                     {
                         k->dataPath = objectTag.attribute("path");
                     }
                break;
                default:
                     {
                         #ifdef K_DEBUG
                                tError() << "KTLibraryObject::fromXml() - Unknown object type: " << k->type;
                         #endif
                     }
                break;
        }
    }
}

QDomElement KTLibraryObject::toXml(QDomDocument &doc) const
{
    QDomElement object = doc.createElement("object");
    object.setAttribute("id", k->symbolName);
    object.setAttribute("type", k->type);
    
    QFileInfo finfo(k->dataPath);
    
    switch (k->type) {
            case Text:
            case Item:
            {
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (KTAbstractSerializable *serializable = dynamic_cast<KTAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }
            }
            break;
            case Svg:
            {
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (KTAbstractSerializable *serializable = dynamic_cast<KTAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }

                 object.setAttribute("path", finfo.fileName());
            }
            break;
            case Image:
            {
                 /*
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (KTAbstractSerializable *serializable = dynamic_cast<KTAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }
                 */
            
                 object.setAttribute("path", finfo.fileName());
            }
            break;
            case Sound:
            {
                 object.setAttribute("path", finfo.fileName());
            }
            break;
    }
    
    return object;
}

bool KTLibraryObject::loadRawData(const QByteArray &data)
{
    if (data.isEmpty()) {
        #ifdef K_DEBUG
               tError() << "KTLibraryObject::loadData() - [ Fatal Error ] - Data is empty!";
        #endif
        return false;
    }

    if (data.isNull()) {
        #ifdef K_DEBUG
               tError() << "KTLibraryObject::loadData() - [ Fatal Error ] - Data is null!";
        #endif
        return false;
    }

    k->rawData = data;
    bool ok = true;

    switch (k->type) {
            case KTLibraryObject::Item:
            {
                 KTItemFactory factory;
                 QGraphicsItem *item = factory.create(QString::fromLocal8Bit(data));
                 setData(QVariant::fromValue(item));
            }
            break;
            case KTLibraryObject::Text:
            {
                 setData(QString::fromLocal8Bit(data));
            }
            break;
            case KTLibraryObject::Image:
            {
                 QPixmap pixmap;
                 bool isOk = pixmap.loadFromData(data);

                 if (!isOk) {
                     #ifdef K_DEBUG
                            tError() << "KTLibraryObject::loadRawData() - [ Fatal Error ] - Can't load image -> " << k->symbolName;
                     #endif
                 }

                 KTPixmapItem *item = new KTPixmapItem;
                 item->setPixmap(pixmap);
                 setData(QVariant::fromValue(static_cast<QGraphicsItem *>(item)));
            }
            break;
            case KTLibraryObject::Svg:
            {
                 QString item(data);
                 setData(QVariant::fromValue(item));
            }
            break;
            case KTLibraryObject::Sound:
            {
                 QTemporaryFile soundFile(QDir::tempPath() + QDir::separator() + "tupi_sound_file_XXXXXX");
                 soundFile.setAutoRemove(false);

                 if (soundFile.open()) {
                     soundFile.write(data);
                     setData(soundFile.fileName());
                     soundFile.close();
                 }
            }
            break;
            default:
            {
                 ok = false;
            }
            break;
    }
    
    return ok;
}

bool KTLibraryObject::loadDataFromPath(const QString &dataDir)
{
    switch (k->type) {
            case KTLibraryObject::Image:
            {
                 k->dataPath = dataDir + QDir::separator() + "images" + QDir::separator() + k->dataPath;
            
                 QFile file(k->dataPath);
            
                 if (file.open(QIODevice::ReadOnly)) {
                     loadRawData(file.readAll());
                 } else {
                     #ifdef K_DEBUG
                            tError() << "KTLibraryObject::loadDataFromPath() - [ Fatal Error ] - Can't access image file -> " << k->dataPath;
                     #endif
                 }
            }
            break;
            case KTLibraryObject::Sound:
            {
                 k->dataPath = dataDir + QDir::separator() + "audio" + QDir::separator() + k->dataPath;
            }
            break;
            case KTLibraryObject::Svg:
            {
                 k->dataPath = dataDir + QDir::separator() + "svg" + QDir::separator() + k->dataPath;

                 QFile file(k->dataPath);

                 if (file.open(QIODevice::ReadOnly)) {
                     loadRawData(file.readAll());
                 } else {
                     #ifdef K_DEBUG
                            tError() << "KTLibraryObject::loadDataFromPath() - [ Fatal Error ] - Can't access SVG file -> " << k->dataPath;
                     #endif
                 }
            }
            break;
            default: 
                 return false; 
            break;
    }
    
    return true;
}

void KTLibraryObject::saveData(const QString &dataDir)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    switch (k->type) {
            case KTLibraryObject::Sound:
            {
                 QString saved = dataDir + QDir::separator() + "audio" + QDir::separator();
            
                 if (! QFile::exists(saved)) {
                     QDir dir;
                     dir.mkpath(saved);
                 }
           
                 // SQA: Sound support 
                 // QFile::copy(QString(k->data.toString()), saved + k->symbolName);
                 // QFile::remove(QString(k->data.toString()));
            
                 k->dataPath = saved + k->symbolName;
                 k->data = "";
            }
            break;
            case KTLibraryObject::Svg:
            {
                 QString saved = dataDir + QDir::separator() + "svg" + QDir::separator();

                 if (! QFile::exists(saved)) {
                     QDir dir;
                     dir.mkpath(saved);
                 }

                 QFile file(saved + k->symbolName);
                 if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                     return;

                 QTextStream out(&file);
                 out << qvariant_cast<QString>(k->data);

                 k->dataPath = saved + k->symbolName;
            }
            break;
            case KTLibraryObject::Image:
            {
                 QString destination = dataDir + QDir::separator() + "images" + QDir::separator();
            
                 if (!QFile::exists(destination)) {
                     QDir dir;
                     dir.mkpath(destination);

                     #ifdef K_DEBUG
                            tWarning() << "KTLibraryObject::saveData() - Creating directory -> " << destination;
                     #endif
                 }

                 k->dataPath = destination + k->symbolName;

                 QFile file(k->dataPath);
                 if (!file.open(QIODevice::WriteOnly)) {
                     #ifdef K_DEBUG
                            tError() << "KTLibraryObject::saveData() - [ Fatal Error ] - Insufficient permissions to save file -> " << destination + k->symbolName;
                     #endif
                 } else {
                     qint64 isOk = file.write(k->rawData);
                     file.close();

                     if (isOk == -1) {
                         #ifdef K_DEBUG
                                tError() << "KTLibraryObject::saveData() - [ Fatal Error ] - Can't save file -> " << destination + k->symbolName;
                         #endif
                     } else {
                         #ifdef K_DEBUG
                                tWarning() << "KTLibraryObject::saveData() - Image file saved successfully -> " << destination + k->symbolName;
                         #endif
                     }
                 }
            }
            break;
            default: 
            break;
    }
}
