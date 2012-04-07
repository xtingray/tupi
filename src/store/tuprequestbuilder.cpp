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

#include "tuprequestbuilder.h"
#include "tdebug.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"

#include <QDomDocument>

TupRequestBuilder::TupRequestBuilder()
{
}

TupRequestBuilder::~TupRequestBuilder()
{
}

TupProjectRequest TupRequestBuilder::createItemRequest(int sceneIndex, int layerIndex, int frameIndex, int itemIndex, QPointF point, TupProject::Mode spaceMode, 
                                                     TupLibraryObject::Type type, int actionId, const QVariant &arg, const QByteArray &data)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("project_request");

    QDomElement scene = doc.createElement("scene");
    scene.setAttribute("index", sceneIndex);

    QDomElement layer = doc.createElement("layer");
    layer.setAttribute("index", layerIndex);

    QDomElement frame = doc.createElement("frame");
    frame.setAttribute("index", frameIndex);

    QDomElement item = doc.createElement("item");
    item.setAttribute("index", itemIndex);

    QDomElement objectType = doc.createElement("objectType");
    objectType.setAttribute("id", type);

    QDomElement position = doc.createElement("position");
    position.setAttribute("x", point.x());
    position.setAttribute("y", point.y());

    QDomElement space = doc.createElement("spaceMode");
    space.setAttribute("current", spaceMode);

    QDomElement action = doc.createElement("action");
    action.setAttribute("id", actionId);
    action.setAttribute("arg", arg.toString());
    action.setAttribute("part", TupProjectRequest::Item);

    TupRequestBuilder::appendData(doc, action, data);
    root.appendChild(action);
    item.appendChild(objectType);
    item.appendChild(position);
    item.appendChild(space);
    frame.appendChild(item);
    layer.appendChild(frame);
    scene.appendChild(layer);
    root.appendChild(scene);

    doc.appendChild(root);
    TupProjectRequest request(doc.toString(0));

    return request;
}

TupProjectRequest TupRequestBuilder::createFrameRequest(int sceneIndex, int layerIndex, int frameIndex, int actionId, const QVariant &arg, const QByteArray &data)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("project_request");

    QDomElement scene = doc.createElement("scene");
    scene.setAttribute("index", sceneIndex);

    QDomElement layer = doc.createElement("layer");
    layer.setAttribute("index", layerIndex);

    QDomElement frame = doc.createElement("frame");
    frame.setAttribute("index", frameIndex);

    QDomElement action = doc.createElement("action");
    action.setAttribute("id", actionId);
    action.setAttribute("arg", arg.toString());
    action.setAttribute("part", TupProjectRequest::Frame);

    TupRequestBuilder::appendData(doc, action, data);

    root.appendChild(action);
    layer.appendChild(frame);
    scene.appendChild(layer);
    root.appendChild(scene);
    doc.appendChild(root);

    return TupProjectRequest(doc.toString(0));
}


TupProjectRequest TupRequestBuilder::createLayerRequest(int sceneIndex, int layerIndex, int actionId, const QVariant &arg, const QByteArray &data)
{
    QDomDocument doc;

    QDomElement root = doc.createElement("project_request");

    QDomElement scene = doc.createElement("scene");
    scene.setAttribute("index", sceneIndex);

    QDomElement layer = doc.createElement("layer");
    layer.setAttribute("index", layerIndex);

    QDomElement action = doc.createElement("action");
    action.setAttribute("id", actionId);
    action.setAttribute("arg", arg.toString());
    action.setAttribute("part", TupProjectRequest::Layer);

    TupRequestBuilder::appendData(doc, action, data);

    root.appendChild(action);
    scene.appendChild(layer);
    root.appendChild(scene);

    doc.appendChild(root);

    return TupProjectRequest(doc.toString(0));
}

TupProjectRequest TupRequestBuilder::createSceneRequest(int sceneIndex, int actionId, const QVariant &arg, const QByteArray &data)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("project_request");

    QDomElement scene = doc.createElement("scene");
    scene.setAttribute("index", sceneIndex);

    QDomElement action = doc.createElement("action");
    action.setAttribute("id", actionId);
    action.setAttribute("arg", arg.toString());
    action.setAttribute("part", TupProjectRequest::Scene);

    TupRequestBuilder::appendData(doc, action, data);

    root.appendChild(action);
    root.appendChild(scene);

    doc.appendChild(root);

    return TupProjectRequest(doc.toString(0));
}

TupProjectRequest TupRequestBuilder::createLibraryRequest(int actionId, const QVariant &arg, TupLibraryObject::Type type, TupProject::Mode spaceMode,
                                                        const QByteArray &data, const QString &folder, int sceneIndex, int layerIndex, int frameIndex)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("project_request");

    QDomElement scene = doc.createElement("scene");
    scene.setAttribute("index", sceneIndex);

    QDomElement layer = doc.createElement("layer");
    layer.setAttribute("index", layerIndex);

    QDomElement frame = doc.createElement("frame");
    frame.setAttribute("index", frameIndex);

    QDomElement library = doc.createElement("library");

    QDomElement symbol = doc.createElement("symbol");
    symbol.setAttribute("folder", folder);
    symbol.setAttribute("type", type);
    symbol.setAttribute("spaceMode", spaceMode);

    QDomElement action = doc.createElement("action");
    action.setAttribute("id", actionId);
    action.setAttribute("arg", arg.toString());
    action.setAttribute("part", TupProjectRequest::Library);

    TupRequestBuilder::appendData(doc, action, data);

    root.appendChild(action);

    library.appendChild(symbol);

    root.appendChild(library);

    root.appendChild(scene);
    scene.appendChild(layer);
    layer.appendChild(frame);

    doc.appendChild(root);

    return TupProjectRequest(doc.toString(0));
}

void TupRequestBuilder::appendData(QDomDocument &doc, QDomElement &element, const QByteArray &data)
{
    if (!data.isNull() && !data.isEmpty()) {
        QDomElement edata = doc.createElement("data");

        QDomCDATASection cdata = doc.createCDATASection(QString(data.toBase64()));

        edata.appendChild(cdata);
        element.appendChild(edata);
    }
}

TupProjectRequest TupRequestBuilder::fromResponse(TupProjectResponse *response)
{
    TupProjectRequest request;

    switch (response->part()) {
            case TupProjectRequest::Item:
                 {
                    request = TupRequestBuilder::createItemRequest(static_cast<TupItemResponse*> (response)->sceneIndex(), static_cast<TupItemResponse*> (response)->layerIndex(), 
                                                                  static_cast<TupItemResponse*> (response)->frameIndex(), static_cast<TupItemResponse*> (response)->itemIndex(), 
                                                                  static_cast<TupItemResponse*> (response)->position(), TupProject::Mode(static_cast<TupItemResponse*> (response)->spaceMode()), 
                                                                  TupLibraryObject::Type(static_cast<TupItemResponse*> (response)->itemType()), response->action(), response->arg().toString(), 
                                                                  response->data());
                 }
            break;
            case TupProjectRequest::Frame:
                 {
                    request = TupRequestBuilder::createFrameRequest(static_cast<TupFrameResponse*> (response)->sceneIndex(), static_cast<TupFrameResponse*> (response)->layerIndex(), static_cast<TupFrameResponse*> (response)->frameIndex(), response->action(), response->arg().toString(), response->data());
                 }
            break;
            case TupProjectRequest::Layer:
                 {
                    request = TupRequestBuilder::createLayerRequest(static_cast<TupLayerResponse*> (response)->sceneIndex(), static_cast<TupLayerResponse*> (response)->layerIndex(), response->action(), response->arg().toString(), response->data());
                 }
            break;
            case TupProjectRequest::Scene:
                 {
                    request = TupRequestBuilder::createSceneRequest(static_cast<TupSceneResponse*> (response)->sceneIndex(), response->action(), response->arg().toString(), response->data());
                 }
            break;
            case TupProjectRequest::Library:
                 {
                    request = TupRequestBuilder::createLibraryRequest(response->action(), response->arg().toString(), TupLibraryObject::Type(static_cast<TupLibraryResponse*>(response)->symbolType()), 
                                                                     TupProject::Mode(static_cast<TupLibraryResponse*>(response)->spaceMode()), response->data(), static_cast<TupLibraryResponse*>(response)->parent(), 
                                                                     static_cast<TupLibraryResponse*>(response)->sceneIndex(), static_cast<TupLibraryResponse*>(response)->layerIndex(),  
                                                                     static_cast<TupLibraryResponse*>(response)->frameIndex());
                 }
            break;
            default:
                 {
                    #ifdef K_DEBUG
                           tWarning() << "wOw! Unknown response! O_o";
                    #endif
                 }
    }

    return request;
}
