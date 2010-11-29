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

#include "ktrequestparser.h"
#include "kdebug.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include "ktprojectresponse.h"
#include "ktlibraryobject.h"

struct KTRequestParser::Private
{
    QString sign;
    KTProjectResponse *response;
};

KTRequestParser::KTRequestParser() : KTXmlParserBase(), k(new Private())
{
    k->response = 0;
}

KTRequestParser::~KTRequestParser()
{
    delete k;
}

void KTRequestParser::initialize()
{
    k->response = 0;
}

bool KTRequestParser::startTag(const QString& qname, const QXmlAttributes& atts)
{
    if (qname == "request") {
        k->sign = atts.value("sign");
    } else if (qname == "item") {
               static_cast<KTItemResponse *>(k->response)->setItemIndex(atts.value("index").toInt());
    } else if (qname == "objectType") {
               static_cast<KTItemResponse *>(k->response)->setItemType(KTLibraryObject::Type(atts.value("id").toInt())); 
    } else if (qname == "posx") {
               static_cast<KTItemResponse *>(k->response)->setPosX(atts.value("x").toInt());
    } else if (qname == "posy") {
               static_cast<KTItemResponse *>(k->response)->setPosY(atts.value("y").toInt());
    } else if (qname == "frame") {
               static_cast<KTFrameResponse *>(k->response)->setFrameIndex(atts.value("index").toInt());
    } else if (qname == "data") {
               setReadText(true);
    } else if (qname == "layer") {
               static_cast<KTLayerResponse *>(k->response)->setLayerIndex(atts.value("index").toInt());
    } else if (qname == "scene") {
               static_cast<KTSceneResponse *>(k->response)->setSceneIndex(atts.value("index").toInt());
    } else if (qname == "symbol") {
               static_cast<KTLibraryResponse*>(k->response)->setSymbolType(KTLibraryObject::Type(atts.value("type").toInt()));
               static_cast<KTLibraryResponse*>(k->response)->setParent(atts.value("folder"));
    } else if (qname == "action") {
               k->response = KTProjectResponseFactory::create(atts.value("part").toInt(), atts.value("id").toInt());
               k->response->setArg(atts.value("arg"));
    }

    return true;
}

bool KTRequestParser::endTag(const QString& qname)
{
    return true;
}

void KTRequestParser::text( const QString &ch )
{
    if (currentTag() == "data")
        k->response->setData(QByteArray::fromBase64(QByteArray(ch.toLocal8Bit())));
}

KTProjectResponse *KTRequestParser::response() const
{
    return k->response;
}

QString KTRequestParser::sign() const
{
    return k->sign;
}
