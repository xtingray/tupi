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

#include "ktsoundlayer.h"
#include "taudioplayer.h"
#include "ktlibrary.h"
#include "ktproject.h"
#include "ktlibraryobject.h"

#include <QFileInfo>

struct KTSoundLayer::Private
{
    QString filePath, symbolName;
    int playerId;
};

KTSoundLayer::KTSoundLayer(KTScene *parent)
 : KTLayer(parent), k(new Private)
{
}

KTSoundLayer::~KTSoundLayer()
{
    delete k;
}

void KTSoundLayer::fromSymbol(const QString &symbolName)
{
    KTLibrary *library = project()->library();
    
    if (KTLibraryObject *object = library->findObject(symbolName)) {
        if (object->type() == KTLibraryObject::Sound) {
            k->symbolName = symbolName;
            k->filePath = object->dataPath();
            k->playerId = TAudioPlayer::instance()->load(k->filePath);
        }
    }
}

QString KTSoundLayer::filePath() const
{
    return k->filePath;
}

void KTSoundLayer::play()
{
    TAudioPlayer::instance()->setCurrentPlayer(k->playerId);
    TAudioPlayer::instance()->play();
}

void KTSoundLayer::stop()
{
    TAudioPlayer::instance()->setCurrentPlayer(k->playerId);
    TAudioPlayer::instance()->stop();
}

void KTSoundLayer::fromXml(const QString &xml)
{
    QDomDocument document;
    
    if (! document.setContent(xml))
        return;
    
    QDomElement root = document.documentElement();
    setLayerName(root.attribute("name", layerName()));
    
    fromSymbol(root.attribute("symbol"));
}

QDomElement KTSoundLayer::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("soundlayer");
    root.setAttribute("name", layerName());
    root.setAttribute("symbol", k->symbolName);
    
    return root;
}
