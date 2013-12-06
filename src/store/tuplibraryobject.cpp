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

#include "tuplibraryobject.h"
#include "tupitemfactory.h"
#include "tuppixmapitem.h"
// #include "taudioplayer.h"
#include "tdebug.h"

#include <QTemporaryFile>
#include <QDir>

struct TupLibraryObject::Private
{
    TupLibraryObject::Type type;
    QVariant data;
    QString dataPath;
    QString symbolName;
    QString smallId;
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

void TupLibraryObject::setType(TupLibraryObject::Type type)
{
    k->type = type;
}

TupLibraryObject::Type TupLibraryObject::type() const
{
    return k->type;
}

void TupLibraryObject::setSymbolName(const QString &name)
{
    k->symbolName = name;
    k->symbolName.replace(QDir::separator(), "-");
    k->smallId = k->symbolName.section('.', 0, 0);
    k->extension = k->symbolName.section('.', 1, 1).toUpper();
}

QString TupLibraryObject::symbolName() const
{
    return k->symbolName;
}

QString TupLibraryObject::smallId() const
{
    return k->smallId;
}

QString TupLibraryObject::extension() const
{
    return k->extension;
}

void TupLibraryObject::fromXml(const QString &xml)
{
    QDomDocument document;
    
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG  
               tError() << "TupLibraryObject::fromXml() - Fatal Error: Invalid XML structure!";
        #endif
        return;
    }
    
    QDomElement objectTag = document.documentElement();

    if (objectTag.tagName() == "object") {
        setSymbolName(objectTag.attribute("id"));
        if (k->symbolName.isEmpty()) {
            #ifdef K_DEBUG
                   tError() << "TupLibraryObject::fromXml - Fatal Error: Symbol name is empty!";
            #endif
            return;
        }
       
        bool isOk = false; 
        int index = objectTag.attribute("type").toInt(&isOk);
        if (isOk) {
            k->type = TupLibraryObject::Type(index);
        } else {
            #ifdef K_DEBUG
                   tError() << "TupLibraryObject::fromXml - Fatal Error: Invalid object type!";
            #endif
            return;
        }

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

                             QByteArray array = data.toLocal8Bit();
                             if (!array.isEmpty() && !array.isNull()) {
                                 loadRawData(array);
                             } else {
                                 #ifdef K_DEBUG
                                        tError() << "TupLibraryObject::fromXml() - Object data is empty! [ " << k->symbolName << " ]";
                                 #endif
                                 return;
                             }
                         } else {
                             #ifdef K_DEBUG
                                        tError() << "TupLibraryObject::fromXml() - Object data from xml is NULL! [ " << k->symbolName << " ]";
                             #endif
                             return;
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
                         return;
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
            default:
            {
            }
            break;
    }
    
    return object;
}

bool TupLibraryObject::loadRawData(const QByteArray &data)
{
    k->rawData = data;

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
                     return false;
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
                 return false;
            }
            break;
    }
    
    return true;
}

bool TupLibraryObject::loadDataFromPath(const QString &dataDir)
{
    switch (k->type) {
            case TupLibraryObject::Image:
            {
                 k->dataPath = dataDir + QDir::separator() + "images" + QDir::separator() + k->dataPath;
                 loadData(k->dataPath); 
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
                 loadData(k->dataPath);
            }
            break;
            default: 
                 return false; 
            break;
    }
    
    return true;
}

bool TupLibraryObject::loadData(const QString &path)
{
    switch (k->type) {
            case TupLibraryObject::Image:
            case TupLibraryObject::Svg:
            {
                 QFile file(path);
                 if (file.exists()) {
                     if (file.open(QIODevice::ReadOnly)) {
                         QByteArray array = file.readAll(); 
                         #ifdef K_DEBUG
                                tWarning() << "TupLibraryObject::loadData() - Object path: " << path;
                                tWarning() << "TupLibraryObject::loadData() - Object size: " << array.size();
                         #endif
                         if (!array.isEmpty() && !array.isNull()) {
                             loadRawData(array);
                         } else {
                             #ifdef K_DEBUG
                                    tWarning() << "TupLibraryObject::loadData() - Warning: Image file is empty -> " << path;
                             #endif
                             return false;
                         }
                     } else {
                         #ifdef K_DEBUG
                                tError() << "TupLibraryObject::loadData() - Fatal Error: Can't access image file -> " << path;
                         #endif
                         return false;
                     }
                 } else {
                     #ifdef K_DEBUG
                            tError() << "TupLibraryObject::loadData() - Fatal Error: Image file doesn't exist -> " << path;
                     #endif
                     return false;
                 }
            }
            break;
            case TupLibraryObject::Sound:
            {
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
