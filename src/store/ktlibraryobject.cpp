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

#include <QTemporaryFile>
#include <QDir>

#include "kaudioplayer.h"
#include "kdebug.h"

struct KTLibraryObject::Private
{
    int type;
    QVariant data;
    QString dataPath;
    QString symbolName;
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
}

QString KTLibraryObject::symbolName() const
{
    return k->symbolName;
}

void KTLibraryObject::fromXml(const QString &xml)
{
    QDomDocument document;
    
    if (! document.setContent(xml))
        return;
    
    QDomElement objectTag = document.documentElement();
    
    if (objectTag.tagName() == "object") {
        kFatal() << "KTLibraryObject::fromXml() -> Setting Object id as: " << objectTag.attribute("id");
        setSymbolName(objectTag.attribute("id"));
        
        if (k->symbolName.isEmpty()) 
            return;
        
        k->type = objectTag.attribute("type").toInt();
        
        k->dataPath = objectTag.attribute("path");
        
        QDomElement objectData = objectTag.firstChild().toElement();
        if (!objectTag.isNull()) {
            QString data;
            {
                QTextStream ts(&data);
                ts << objectData;
            }
            loadData(data.toLocal8Bit());
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
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (KTAbstractSerializable *serializable = dynamic_cast<KTAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }
            
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

bool KTLibraryObject::loadData(const QByteArray &data)
{
    if (data.isEmpty())
        return false;
    
    bool ok = true;

    switch (k->type) {
            case KTLibraryObject::Item:
            {
                 KTItemFactory factory;
                 QGraphicsItem *item = factory.create(QString::fromLocal8Bit(data));
                 setData(QVariant::fromValue(item));
            }
            break;
            case KTLibraryObject::Image:
            {
                 QPixmap pixmap;
                 pixmap.loadFromData(data);
            
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
            case KTLibraryObject::Text:
            {
                 setData(QString::fromLocal8Bit(data));
            }
            break;
            case KTLibraryObject::Sound:
            {
                 QTemporaryFile soundFile(QDir::tempPath() + "/tupi_sound_file_XXXXXX");
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
                 k->dataPath = dataDir + "/images/" + k->dataPath;
            
                 QFile f(k->dataPath);
            
                 if (f.open(QIODevice::ReadOnly))
                     loadData(f.readAll());
            }
            break;
            case KTLibraryObject::Sound:
            {
                 k->dataPath = dataDir + "/audio/" + k->dataPath;
            }
            break;
            case KTLibraryObject::Svg:
            {
                 k->dataPath = dataDir + "/svg/" + k->dataPath;

                 QFile f(k->dataPath);

                 if (f.open(QIODevice::ReadOnly))
                     loadData(f.readAll());
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
    switch (k->type) {
            case KTLibraryObject::Sound:
            {
                 QString saved = dataDir + "/audio/";
            
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
                 QString saved = dataDir + "/svg/";

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
                 QString dest = dataDir + "/images/";
            
                 if (! QFile::exists(dest)) {
                     QDir dir;
                     dir.mkpath(dest);
                 }
            
                 (qgraphicsitem_cast<KTPixmapItem *> (qvariant_cast<QGraphicsItem *>(k->data)))->pixmap().save(dest + k->symbolName, "PNG");
          
                 k->dataPath = dest + k->symbolName;
            }
            break;
            default: 
            break;
    }
}
