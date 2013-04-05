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

#include "tuplibraryobject.h"
#include "tupitemfactory.h"
#include "tuppixmapitem.h"
// #include "taudioplayer.h"
#include "tdebug.h"

#include <QTemporaryFile>
#include <QDir>

struct TupLibraryObject::Private
{
    int type;
    QVariant data;
    QString dataPath;
    QString symbolName;
    QString extension;
    QByteArray rawData; 
};

TupLibraryObject::TupLibraryObject(QObject *parent) : QObject(parent), k(new Private)
{
}

TupLibraryObject::~TupLibraryObject()
{
    delete k;
}

/**
 * Items, pics, text, and so...
 * @param data 
 */
void TupLibraryObject::setData(const QVariant &data)
{
    k->data = data;
}

QVariant TupLibraryObject::data() const
{
    return k->data;
}

void TupLibraryObject::setDataPath(const QString &path)
{
    k->dataPath = path;
}

QString TupLibraryObject::dataPath() const
{
    return k->dataPath;
}

void TupLibraryObject::setType(int type)
{
    k->type = type;
}

int TupLibraryObject::type() const
{
    return k->type;
}

void TupLibraryObject::setSymbolName(const QString &name)
{
    k->symbolName = name;
    k->symbolName.replace(QDir::separator(), "-");
    k->extension = k->symbolName.section('.', 1, 1).toUpper();
}

QString TupLibraryObject::symbolName() const
{
    return k->symbolName;
}

void TupLibraryObject::fromXml(const QString &xml)
{
    QDomDocument document;
    
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG  
               tError() << "TupLibraryObject::fromXml - [ Fatal Error ] - Invalid XML structure!";
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
                case TupLibraryObject::Item:
                case TupLibraryObject::Text:
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
                case TupLibraryObject::Image:
                case TupLibraryObject::Svg:
                case TupLibraryObject::Sound:
                     {
                         k->dataPath = objectTag.attribute("path");
                     }
                break;
                default:
                     {
                         #ifdef K_DEBUG
                                tError() << "TupLibraryObject::fromXml() - Unknown object type: " << k->type;
                         #endif
                     }
                break;
        }
    }
}

QDomElement TupLibraryObject::toXml(QDomDocument &doc) const
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
                     if (TupAbstractSerializable *serializable = dynamic_cast<TupAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }
            }
            break;
            case Svg:
            {
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (TupAbstractSerializable *serializable = dynamic_cast<TupAbstractSerializable *>(item))
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
                     if (TupAbstractSerializable *serializable = dynamic_cast<TupAbstractSerializable *>(item))
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

bool TupLibraryObject::loadRawData(const QByteArray &data)
{
    if (data.isEmpty()) {
        #ifdef K_DEBUG
               tError() << "TupLibraryObject::loadData() - [ Fatal Error ] - Data is empty!";
        #endif
        return false;
    }

    if (data.isNull()) {
        #ifdef K_DEBUG
               tError() << "TupLibraryObject::loadData() - [ Fatal Error ] - Data is null!";
        #endif
        return false;
    }

    k->rawData = data;
    bool ok = true;

    switch (k->type) {
            case TupLibraryObject::Item:
            {
                 TupItemFactory factory;
                 QGraphicsItem *item = factory.create(QString::fromLocal8Bit(data));
                 setData(QVariant::fromValue(item));
            }
            break;
            case TupLibraryObject::Text:
            {
                 setData(QString::fromLocal8Bit(data));
            }
            break;
            case TupLibraryObject::Image:
            {
                 QPixmap pixmap;
                 bool isOk = pixmap.loadFromData(data);

                 if (!isOk) {
                     #ifdef K_DEBUG
                            tError() << "TupLibraryObject::loadRawData() - [ Fatal Error ] - Can't load image -> " << k->symbolName;
                     #endif
                 }

                 TupPixmapItem *item = new TupPixmapItem;
                 item->setPixmap(pixmap);
                 setData(QVariant::fromValue(static_cast<QGraphicsItem *>(item)));
            }
            break;
            case TupLibraryObject::Svg:
            {
                 QString item(data);
                 setData(QVariant::fromValue(item));
            }
            break;
            case TupLibraryObject::Sound:
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

bool TupLibraryObject::loadDataFromPath(const QString &dataDir)
{
    switch (k->type) {
            case TupLibraryObject::Image:
            {
                 k->dataPath = dataDir + QDir::separator() + "images" + QDir::separator() + k->dataPath;
            
                 QFile file(k->dataPath);
            
                 if (file.open(QIODevice::ReadOnly)) {
                     loadRawData(file.readAll());
                 } else {
                     #ifdef K_DEBUG
                            tError() << "TupLibraryObject::loadDataFromPath() - [ Fatal Error ] - Can't access image file -> " << k->dataPath;
                     #endif
                 }
            }
            break;
            case TupLibraryObject::Sound:
            {
                 k->dataPath = dataDir + QDir::separator() + "audio" + QDir::separator() + k->dataPath;
            }
            break;
            case TupLibraryObject::Svg:
            {
                 k->dataPath = dataDir + QDir::separator() + "svg" + QDir::separator() + k->dataPath;

                 QFile file(k->dataPath);

                 if (file.open(QIODevice::ReadOnly)) {
                     loadRawData(file.readAll());
                 } else {
                     #ifdef K_DEBUG
                            tError() << "TupLibraryObject::loadDataFromPath() - [ Fatal Error ] - Can't access SVG file -> " << k->dataPath;
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

void TupLibraryObject::saveData(const QString &dataDir)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    switch (k->type) {
            case TupLibraryObject::Sound:
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
            case TupLibraryObject::Svg:
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
            case TupLibraryObject::Image:
            {
                 QString destination = dataDir + QDir::separator() + "images" + QDir::separator();
            
                 if (!QFile::exists(destination)) {
                     QDir dir;
                     dir.mkpath(destination);

                     #ifdef K_DEBUG
                            tWarning() << "TupLibraryObject::saveData() - Creating directory -> " << destination;
                     #endif
                 }

                 k->dataPath = destination + k->symbolName;

                 QFile file(k->dataPath);
                 if (!file.open(QIODevice::WriteOnly)) {
                     #ifdef K_DEBUG
                            tError() << "TupLibraryObject::saveData() - [ Fatal Error ] - Insufficient permissions to save file -> " << destination + k->symbolName;
                     #endif
                 } else {
                     qint64 isOk = file.write(k->rawData);
                     file.close();

                     if (isOk == -1) {
                         #ifdef K_DEBUG
                                tError() << "TupLibraryObject::saveData() - [ Fatal Error ] - Can't save file -> " << destination + k->symbolName;
                         #endif
                     } else {
                         #ifdef K_DEBUG
                                tWarning() << "TupLibraryObject::saveData() - Image file saved successfully -> " << destination + k->symbolName;
                         #endif
                     }
                 }
            }
            break;
            default: 
            break;
    }
}
