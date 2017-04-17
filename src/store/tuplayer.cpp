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
    Frames undoFrames;
    Frames resettedFrames;
    Mouths lipsyncList;
    bool isVisible;
    QString name;
    int framesCounter;
    bool isLocked;
    int index;
    double opacity;

    QList<TupGraphicObject *> tweeningGraphicObjects;
    QList<TupSvgItem *> tweeningSvgObjects;
};

TupLayer::TupLayer(TupScene *scene, int index) : k(new Private)
{
    k->scene = scene;
    k->index = index;
    k->isVisible = true;
    k->name = tr("Layer");
    k->framesCounter = 0;
    k->isLocked = false;
    k->opacity = 1.0;
}

TupLayer::~TupLayer()
{
    k->frames.clear();
    k->lipsyncList.clear();
    k->tweeningGraphicObjects.clear();
    k->tweeningSvgObjects.clear();

    delete k;
}

Frames TupLayer::frames()
{
    return k->frames;
}

void TupLayer::setFrames(const Frames &frames)
{
    k->frames = frames;
    k->framesCounter = frames.count();
}

void TupLayer::setFrame(int index, TupFrame *frame)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLayer::setFrame()]";
        #else
            T_FUNCINFO;
            tWarning() << "At index -> " << index;
        #endif
    #endif

    k->frames.insert(index, frame);
    k->framesCounter++;
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
    k->framesCounter++;
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

bool TupLayer::restoreFrame(int index)
{
    if (k->undoFrames.count() > 0) {
        TupFrame *frame = k->undoFrames.takeLast();
        if (frame) {
            k->frames.insert(index, frame);
            k->framesCounter++;
            return true;
        }
        return false;
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLayer::restoreFrame() - Fatal Error: "
            "No available frames to restore index -> " + QString::number(index);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return false;
}

bool TupLayer::removeFrame(int position)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLayer::removeFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupFrame *toRemove = frameAt(position);

    if (toRemove) {
        k->undoFrames << k->frames.takeAt(position);
        k->framesCounter--;

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

bool TupLayer::resetFrame(int pos)
{
    TupFrame *toReset = frameAt(pos);

    if (toReset) {
        k->resettedFrames << k->frames.takeAt(pos); 
        TupFrame *frame = new TupFrame(this); 
        frame->setFrameName(tr("Frame"));
        k->frames.insert(pos, frame);

        return true;
    }

    return false;
}

bool TupLayer::restoreResettedFrame(int pos)
{
    if (k->resettedFrames.count() > 0) {
        TupFrame *frame = k->resettedFrames.takeLast();
        if (frame) {
            k->frames.removeAt(pos); 
            k->frames.insert(pos, frame);
            return true;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLayer::restoreResettedFrame() - Fatal Error: "
            "No available resetted frames to restore -> " + QString::number(k->resettedFrames.count());
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return false;
}

int TupLayer::resettedFramesCount()
{
    return k->resettedFrames.count();
}

void TupLayer::clear()
{
    for (int i=0; i<k->frames.count(); i++) {
         TupFrame *frame = k->frames.takeAt(i);
         frame->clear();
         delete frame;
         frame = NULL;
    }
    
    k->name = "";
    k->framesCounter = 0;
    k->lipsyncList.clear();
    k->tweeningGraphicObjects.clear();
    k->tweeningSvgObjects.clear();
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
    if (from < 0 || from >= k->frames.count() || to < 0 || to >= k->frames.count()) {
        #ifdef K_DEBUG
            QString msg = "TupLayer::exchangeFrame() - Fatal Error: frame indexes are invalid -> from: " + QString::number(from) + " / to: " + QString::number(to);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }

    k->frames.swap(from, to);

    return true;
}

bool TupLayer::expandFrame(int position, int size)
{
    if (position < 0 || position >= k->frames.count())
        return false;

    TupFrame *toExpand = frameAt(position);
    if (toExpand) {
        int limit = position + size;
        for (int i = position + 1; i <= limit; i++)
             k->frames.insert(i, toExpand);
        return true;
    }

    return false;
}


TupFrame *TupLayer::frameAt(int position) const
{
    if (position < 0 || position >= k->frames.count()) {        
        #ifdef K_DEBUG
            QString msg1 = "TupLayer::frameAt() - Fatal Error: frame index out of bound : " + QString::number(position);
            QString msg2 = "TupLayer::frameAt() - Fatal Error: index limit at layer(" + QString::number(k->index) + ") : " + QString::number(k->frames.count()-1);
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
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLayer::fromXml()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

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
    int framesCounter = k->frames.size();
    for (int i = 0; i < framesCounter; i++) {
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
    return k->framesCounter;
}

void TupLayer::addTweenObject(TupGraphicObject *object)
{
    k->tweeningGraphicObjects << object;
}

void TupLayer::addTweenObject(TupSvgItem *object)
{
    k->tweeningSvgObjects << object;
}

void TupLayer::updateTweenObject(int index, TupGraphicObject *object)
{
    k->tweeningGraphicObjects.replace(index, object);
}

void TupLayer::updateTweenObject(int index, TupSvgItem *object)
{
    k->tweeningSvgObjects.replace(index, object);
}

void TupLayer::removeTweenObject(TupGraphicObject *object)
{
    if (k->tweeningGraphicObjects.size() > 0)
        k->tweeningGraphicObjects.removeAll(object);
}

void TupLayer::removeTweenObject(TupSvgItem *object)
{
    if (k->tweeningSvgObjects.size() > 0)
        k->tweeningSvgObjects.removeAll(object);
}

QList<TupGraphicObject *> TupLayer::tweeningGraphicObjects() const
{
    return k->tweeningGraphicObjects;
}

QList<TupSvgItem *> TupLayer::tweeningSvgObjects() const
{
    return k->tweeningSvgObjects;
}

bool TupLayer::tweenExists(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    return false;
}

bool TupLayer::removeTween(const QString &name, TupItemTweener::Type type)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLayer::removeTween()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                     return true;
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                     return true;
                 }
             }
    }

    return false;
}

void TupLayer::removeAllTweens()
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             object->removeTween();
             removeTweenObject(object);
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             object->removeTween();
             removeTweenObject(object);
    }
}

void TupLayer::removeTweensFromFrame(int frameIndex)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->frame()->index() == frameIndex) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->frame()->index() == frameIndex) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }
}

/*
int TupLayer::tweensCount()
{
    return k->tweeningGraphicObjects.count() + k->tweeningSvgObjects.count();
}
*/
