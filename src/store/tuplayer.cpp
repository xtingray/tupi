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

#include "tuplayer.h"
#include "tupscene.h"
#include "tupprojectloader.h"

struct TupLayer::Private
{
    TupScene *scene;
    Frames frames;
    Mouths lipsyncList;
    bool isVisible;
    QString name;
    int framesCount;
    bool isLocked;
    int index;
    double opacity;
};

/*
TupLayer::TupLayer() : k(new Private)
{
}
*/

TupLayer::TupLayer(TupScene *scene, int index) : k(new Private)
// TupLayer::TupLayer(TupScene *parent, int index) : QObject(parent), k(new Private)
{
    k->scene = scene;
    k->index = index;
    k->isVisible = true;
    k->name = tr("Layer");
    k->framesCount = 0;
    k->isLocked = false;
    k->opacity = 1.0;
}

TupLayer::~TupLayer()
{
    k->frames.clear();
    k->lipsyncList.clear();

    delete k;
}

Frames TupLayer::frames()
{
    return k->frames;
}

void TupLayer::setFrames(const Frames &frames)
{
    k->frames = frames;
    k->framesCount = frames.count();
}

void TupLayer::setFrame(int index, TupFrame *frame)
{
    k->frames.insert(index, frame);
}

void TupLayer::setLayerName(const QString &name)
{
    k->name = name;
}

QString TupLayer::layerName() const
{
    return k->name;
}

void TupLayer::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

bool TupLayer::isLocked() const
{
    return k->isLocked;
}

void TupLayer::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool TupLayer::isVisible() const
{
    return k->isVisible;
}

void TupLayer::setOpacity(double opacity)
{
    k->opacity = opacity; 
}

double TupLayer::opacity()
{
    return k->opacity;
}

TupFrame *TupLayer::createFrame(QString name, int position, bool loaded)
{
    /*
    if (position < 0 || position > k->frames.count()) {
        tFatal() << "TupLayer::createFrame -> index is out of range: " << position << " - frames.count(): " << k->frames.count();
        return 0;
    }
    */

    if (position < 0)
        return 0;

    TupFrame *frame = new TupFrame(this);
    k->framesCount++;
    frame->setFrameName(name);
    k->frames.insert(position, frame);

    if (loaded)
        TupProjectLoader::createFrame(scene()->objectIndex(), objectIndex(), position, name, project());

    return frame;
}

TupLipSync *TupLayer::createLipSync(const QString &name, const QString &soundFile, int initFrame)
{
    TupLipSync *lipsync = new TupLipSync(name, soundFile, initFrame);
    k->lipsyncList << lipsync;

    return lipsync;
}

void TupLayer::addLipSync(TupLipSync *lipsync)
{
    if (lipsync)
        k->lipsyncList << lipsync;
}

int TupLayer::lipSyncCount()
{
     return k->lipsyncList.count();
}

Mouths TupLayer::lipSyncList()
{
     return k->lipsyncList;
}

bool TupLayer::removeFrame(int position)
{
    TupFrame *toRemove = frame(position);

    if (toRemove) {
        k->frames.removeAt(position);
        toRemove->setRepeat(toRemove->repeat()-1);
        k->framesCount--;

        return true;
    }

    return false;
}

bool TupLayer::removeLipSync(const QString &name)
{
    int size = k->lipsyncList.size();
    for (int i = 0; i < size; i++) {
         TupLipSync *lipsync = k->lipsyncList.at(i);
         if (lipsync->name().compare(name) == 0) {
             k->lipsyncList.removeAt(i);
             return true;
         }
    }

    return false;
}

bool TupLayer::resetFrame(int position)
{
    TupFrame *toReset = frame(position);

    if (toReset) {
        QString label = toReset->frameName();
        if (framesCount() == 1)
            label = tr("Frame") + " 1";
        TupFrame *frame = new TupFrame(this); 
        frame->setFrameName(label);
        k->frames.insert(position, frame);
        return true;
    }

    return false;
}

bool TupLayer::moveFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;
 
    TupFrame *origin = k->frames.value(from);
    if (origin) {
        QString label = origin->frameName();
        TupFrame *frame = new TupFrame(this);
        frame->setFrameName(label);

        TupFrame *target = k->frames.value(to);
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

bool TupLayer::exchangeFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;

    k->frames.swap(from, to);

    return true;
}

bool TupLayer::expandFrame(int position, int size)
{
    if (position < 0 || position >= k->frames.count())
        return false;

    TupFrame *toExpand = frame(position);

    if (toExpand) {
        int limit = position + size;
        for (int i = position + 1; i <= limit; i++)
             k->frames.insert(i, toExpand);
        return true;
    }

    return false;
}


TupFrame *TupLayer::frame(int position) const
{
    if (position < 0 || position >= k->frames.count()) {        
        #ifdef K_DEBUG
            QString msg1 = "TupLayer::frame() - FATAL ERROR: frame index out of bound : " + QString::number(position);
            QString msg2 = "TupLayer::frame() - FATAL ERROR: index limit : " + QString::number(k->frames.count()-1);
            #ifdef Q_OS_WIN
                qDebug() << msg1;
                qDebug() << msg2;
            #else
                tError() << msg1;
                tError() << msg2;
            #endif
        #endif    

        return 0;
    }

    return k->frames.value(position);
}


void TupLayer::fromXml(const QString &xml)
{
    QDomDocument document;
    if (!document.setContent(xml))
        return;

    QDomElement root = document.documentElement();
    setLayerName(root.attribute("name", layerName()));
    setOpacity(root.attribute("opacity", "1.0").toDouble());
    setVisible(root.attribute("visible", "1").toInt());
    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "frame") {
                   TupFrame *frame = createFrame(e.attribute("name"), k->frames.count(), true);
                   if (frame) {
                       QString newDoc;
                       {
                         QTextStream ts(&newDoc);
                         ts << n;
                       }
                       frame->fromXml(newDoc);
                   }
               } else if (e.tagName() == "lipsync") {
                          TupLipSync *lipsync = createLipSync(e.attribute("name"), e.attribute("soundFile"), e.attribute("initFrame").toInt()); 
                          if (lipsync) {
                              QString newDoc;
                              {
                                QTextStream ts(&newDoc);
                                ts << n;
                              }
                              lipsync->fromXml(newDoc);
                          }
               }
           }
           n = n.nextSibling();
    }
}

QDomElement TupLayer::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("layer");
    root.setAttribute("name", k->name);
    root.setAttribute("opacity", QString::number(k->opacity));
    root.setAttribute("visible", QString::number(k->isVisible)); 

    doc.appendChild(root);
    int framesCount  = k->frames.size();
    for (int i = 0; i < framesCount; i++) {
         TupFrame *frame = k->frames.at(i);
         root.appendChild(frame->toXml(doc));
    }

    int lipsyncTotal = k->lipsyncList.size();
    for (int i = 0; i < lipsyncTotal; i++) {
         TupLipSync *lipSync = k->lipsyncList.at(i);
         root.appendChild(lipSync->toXml(doc));
    }

    return root;
}

TupScene *TupLayer::scene() const
{
    return k->scene;
    // return static_cast<TupScene *>(parent());
}

TupProject *TupLayer::project() const
{
    return scene()->project();
}

void TupLayer::updateLayerIndex(int index)
{
    k->index = index;
}

int TupLayer::layerIndex()
{
    return k->index;
}

int TupLayer::visualIndexOf(TupFrame *frame) const
{
    return k->frames.indexOf(frame);
}

int TupLayer::objectIndex() const
{
    return scene()->visualIndexOf(const_cast<TupLayer *>(this));
}

int TupLayer::framesCount() const
{
    return k->framesCount;
}
