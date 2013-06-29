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

#include "tupstoryboard.h"
#include "tdebug.h"

struct TupStoryboard::Private
{
    QString title;
    QString author;
    QString topics;
    QString summary;

    QList<QString> scene;
    QList<QString> duration;
    QList<QString> description;
};

TupStoryboard::TupStoryboard(const QString &author) : k(new Private)
{
    k->title   = "";
    k->author  = author;
    k->topics  = "";
    k->summary = "";
}

TupStoryboard::~TupStoryboard()
{
}

void TupStoryboard::init(int start, int size)
{
    for (int i=start; i < size; i++) {
         k->scene << "";
         k->duration << "";
         k->description << "";
    }
}

void TupStoryboard::reset()
{
    k->title   = "";
    k->author  = "";
    k->topics  = "";
    k->summary = "";

    k->scene.clear();
    k->duration.clear();
    k->description.clear();
}

void TupStoryboard::insertScene(int index)
{
    if (index >= 0 && index < k->scene.size()) {
        k->scene.insert(index, "");
        k->duration.insert(index, "");
        k->description.insert(index, "");
    }
}

void TupStoryboard::appendScene()
{
    k->scene.append("");
    k->duration.append("");
    k->description.append("");
}

void TupStoryboard::moveScene(int oldIndex, int newIndex)
{
    if (oldIndex >= 0 && oldIndex < k->scene.size() && newIndex >= 0 && newIndex < k->scene.size()) {
        k->scene.swap(oldIndex, newIndex);
        k->duration.swap(oldIndex, newIndex);
        k->description.swap(oldIndex, newIndex);
    }
}

void TupStoryboard::resetScene(int index)
{
    if (index >= 0 && index < k->scene.size()) {
        k->scene.replace(index, "");
        k->duration.replace(index, "");
        k->description.replace(index, "");
    }
}

void TupStoryboard::removeScene(int index)
{
    if (index >= 0 && index < k->scene.size()) {
         k->scene.removeAt(index);
         k->duration.removeAt(index);
         k->description.removeAt(index);
    }
}

void TupStoryboard::setStoryTitle(const QString &title)
{
    k->title = title;
}

void TupStoryboard::setStoryTopics(const QString &topics)
{
    k->topics = topics;
}

void TupStoryboard::setStoryAuthor(const QString &author)
{
    k->author = author;
}

void TupStoryboard::setStorySummary(const QString &summary)
{
    k->summary = summary;
}

QString TupStoryboard::storyTitle() const
{
    return k->title;
}

QString TupStoryboard::storyAuthor() const
{
    return k->author;
}

QString TupStoryboard::storyTopics() const
{
    return k->topics;
}

QString TupStoryboard::storySummary() const
{
    return k->summary;
}

void TupStoryboard::setSceneTitle(int index, const QString &title)
{
    if (index >= 0 && index < k->scene.count()) {
        k->scene.replace(index, title);
    } else {
        #ifdef K_DEBUG
               tError() << "TupStoryboard::setSceneTitle() - Invalid index: " << index;
        #endif
    }
}

void TupStoryboard::setSceneDuration(int index, const QString &duration)
{
    if (index >= 0 && index < k->duration.count()) {
        k->duration.replace(index, duration);
    } else {
        #ifdef K_DEBUG
               tError() << "TupStoryboard::setSceneDuration() - Invalid index: " << index;
        #endif
    }
}

void TupStoryboard::setSceneDescription(int index, const QString &description)
{
    if (index >= 0 && index < k->description.count()) {
        k->description.replace(index, description);
    } else {
        #ifdef K_DEBUG
               tError() << "TupStoryboard::setSceneDuration() - Invalid index: " << index;
        #endif
    }
}

QString TupStoryboard::sceneTitle(int index) const
{
    if (index < k->scene.count())
        return k->scene.at(index);

    return "";
}

QString TupStoryboard::sceneDuration(int index) const
{
    if (index < k->duration.count())
        return k->duration.at(index);

    return "";
}

QString TupStoryboard::sceneDescription(int index) const
{
    if (index < k->description.count())
        return k->description.at(index);

    return "";
}

void TupStoryboard::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();
    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "title") {
                   k->title = cleanString(e.text());
               } else if (e.tagName() == "author") {
                          k->author = cleanString(e.text());
               } else if (e.tagName() == "topics") {
                          k->topics = cleanString(e.text());
               } else if (e.tagName() == "summary") {
                          k->summary = cleanString(e.text());
               } else if (e.tagName() == "scene") {
                          QDomNode n2 = e.firstChild();
                          while (!n2.isNull()) {
                                 QDomElement e2 = n2.toElement();
                                 if (e2.tagName() == "title") {
                                     k->scene << e2.text();
                                 } else if (e2.tagName() == "duration") {
                                            k->duration << e2.text();
                                 } else if (e2.tagName() == "description") {
                                            k->description << e2.text();
                                 }
                                 n2 = n2.nextSibling();
                          }
               }
           }
           n = n.nextSibling();
    }
}

QDomElement TupStoryboard::toXml(QDomDocument &doc) const
{
    QDomElement storyboard = doc.createElement("storyboard");

    QDomText titleDom   = doc.createTextNode(k->title);
    QDomText authorDom  = doc.createTextNode(k->author);
    QDomText topicsDom  = doc.createTextNode(k->topics);
    QDomText summaryDom = doc.createTextNode(k->summary);

    storyboard.appendChild(doc.createElement("title")).appendChild(titleDom);
    storyboard.appendChild(doc.createElement("author")).appendChild(authorDom);
    storyboard.appendChild(doc.createElement("topics")).appendChild(topicsDom);
    storyboard.appendChild(doc.createElement("summary")).appendChild(summaryDom);

    for (int i=0; i<k->scene.size(); i++) {
         QDomElement scene = doc.createElement("scene");
         QDomText titleDom = doc.createTextNode(k->scene.at(i));
         QDomText timeDom  = doc.createTextNode(k->duration.at(i));
         QDomText descDom  = doc.createTextNode(k->description.at(i));
         scene.appendChild(doc.createElement("title")).appendChild(titleDom);
         scene.appendChild(doc.createElement("duration")).appendChild(timeDom);
         scene.appendChild(doc.createElement("description")).appendChild(descDom);
         storyboard.appendChild(scene);
    }

    return storyboard;
}

int TupStoryboard::size()
{
    return k->scene.count();
}

QString TupStoryboard::cleanString(QString input) const
{
    input.replace(",", "\\,");
    input.replace("'", "\"");

    return input;
}

