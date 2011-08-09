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

#include "ktlayer.h"
#include "ktscene.h"
#include "tdebug.h"
#include "ktprojectloader.h"

struct KTLayer::Private
{
    Frames frames;
    bool isVisible;
    QString name;
    int framesCount;
    bool isLocked;
    int index;
    //int zLevelBase;
};

KTLayer::KTLayer(KTScene *parent, int index) : QObject(parent), k(new Private)
{
    k->index = index;
    k->isVisible = true;
    k->name = tr("Layer");
    k->framesCount = 0;
    k->isLocked = false;
}

KTLayer::~KTLayer()
{
    k->frames.clear(true);
    delete k;
}

Frames KTLayer::frames()
{
   /*
    tFatal() << "";
    tFatal() << "KTLayer::frames - LAYER NAME: " << k->name;
    tFatal() << "";
    */

    return k->frames;
}

void KTLayer::setFrames(const Frames &frames)
{
    k->frames = frames;
    k->framesCount = frames.count();
}

void KTLayer::setLayerName(const QString &name)
{
    k->name = name;
}

void KTLayer::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

bool KTLayer::isLocked() const
{
    return k->isLocked;
}

void KTLayer::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
    // emit visibilityChanged(isVisible);
}

QString KTLayer::layerName() const
{
    return k->name;
}

bool KTLayer::isVisible() const
{
    return k->isVisible;
}

KTFrame *KTLayer::createFrame(QString name, int position, bool loaded)
{
    /*
    if (position < 0 || position > k->frames.count()) {
        tFatal() << "KTLayer::createFrame -> index is out of range: " << position << " - frames.count(): " << k->frames.count();
        return 0;
    }
    */

    if (position < 0)
        return 0;

    KTFrame *frame = new KTFrame(this);
    k->framesCount++;
    frame->setFrameName(name);
    k->frames.insert(position, frame);

    if (loaded)
        KTProjectLoader::createFrame(scene()->objectIndex(), objectIndex(), position, frame->frameName(), project());

    return frame;
}

bool KTLayer::removeFrame(int position)
{
    KTFrame *toRemove = frame(position);

    if (toRemove) {
        k->frames.removeObject(position);
        toRemove->setRepeat(toRemove->repeat()-1);

        /*
        if (toRemove->repeat() < 1) {
            tFatal() << "KTLayer::removeFrame -> Deleting pointer!";
            delete toRemove;
        }
        */

        k->framesCount--;

        return true;
    }

    return false;
}

bool KTLayer::resetFrame(int position)
{
    KTFrame *toReset = frame(position);

    if (toReset) {
        QString label = toReset->frameName();
        KTFrame *frame = new KTFrame(this); 
        frame->setFrameName(label);
        k->frames.insert(position, frame);
        return true;
    }

    return false;
}

bool KTLayer::moveFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;
 
    KTFrame *origin = k->frames.value(from);
    if (origin) {
        QString label = origin->frameName();
        KTFrame *frame = new KTFrame(this);
        frame->setFrameName(label);

        KTFrame *target = k->frames.value(to);
        if (target) {
            QString targetLabel = target->frameName();
            origin->setFrameName(targetLabel);
            k->frames.insert(to, origin); 
            k->frames.insert(from, frame); 
            return true;
        }
    }
    
    return false;
}

bool KTLayer::exchangeFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;

    k->frames.exchangeObject(from, to);

    return true;
}

bool KTLayer::expandFrame(int position, int size)
{
    if (position < 0 || position >= k->frames.count())
        return false;

    KTFrame *toExpand = frame(position);

    if (toExpand) {
        k->frames.expandValue(position, size);
        return true;
    }

    return false;
}


KTFrame *KTLayer::frame(int position) const
{
    if (position < 0 || position >= k->frames.count()) {
        #ifdef K_DEBUG
               T_FUNCINFO << " FATAL ERROR: index out of bound (KTLayer) : " << position;
               T_FUNCINFO << " FATAL ERROR: index limit : " << k->frames.count()-1;
        #endif
        return 0;
    }

    return k->frames.value(position);
}


void KTLayer::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();

    setLayerName(root.attribute("name", layerName()));

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "frame") {
                   KTFrame *frame = createFrame(e.attribute("name"), k->frames.count(), true);

                   if (frame) {
                       QString newDoc;

                       {
                         QTextStream ts(&newDoc);
                         ts << n;
                       }

                       frame->fromXml(newDoc);
                   }
               }
           }

           n = n.nextSibling();
    }
}

QDomElement KTLayer::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("layer");
    root.setAttribute("name", k->name);
    doc.appendChild(root);

    foreach (KTFrame *frame, k->frames.values())
             root.appendChild(frame->toXml(doc));

    return root;
}

KTScene *KTLayer::scene() const
{
    return static_cast<KTScene *>(parent());
}

KTProject *KTLayer::project() const
{
    return scene()->project();
}

int KTLayer::layerIndex()
{
    return k->index;
}

/*
int KTLayer::logicalIndexOf(KTFrame *frame) const
{
    return k->frames.logicalIndex(frame);
}
*/

int KTLayer::visualIndexOf(KTFrame *frame) const
{
    return k->frames.objectIndex(frame);
}

/*
int KTLayer::logicalIndex() const
{
    return scene()->logicalIndexOf(const_cast<KTLayer *>(this));
}
*/

int KTLayer::objectIndex() const
{
    return scene()->visualIndexOf(const_cast<KTLayer *>(this));
}

int KTLayer::framesTotal() const
{
    return k->framesCount;
}
