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

#include "ktitemtweener.h"
#include "ktsvg2qt.h"
#include "kdebug.h"

#include <QGraphicsItem>
#include <QHash>

#define VERIFY_STEP(index) if (index > k->frames || k->frames == 0) { \
                               kWarning("items") << "Invalid step " << index << " for tweening, maximun step are " << k->frames << "; In " << __FUNCTION__; \
                               return; }

#define STEP(index) index / (double)k->frames

struct KTItemTweener::Private
{
    Private() : frames(0) {}
    
    QString name;
    int initFrame;
    int frames;
    QString path;

    QHash<int, KTTweenerStep *> steps; // TODO: remove when Qt 4.3

    inline KTTweenerStep *step(int step)
    {
        KTTweenerStep *currentStep = steps[step];
        if (!currentStep) {
            currentStep = new KTTweenerStep(step);
            steps.insert(step, currentStep);
        }
        
        return currentStep;
    }
};

KTItemTweener::KTItemTweener() : k(new Private)
{
    k->frames = 0;
}

KTItemTweener::~KTItemTweener()
{
    qDeleteAll(k->steps);
    delete k;
}

QString KTItemTweener::name()
{
    return k->name;
}

void KTItemTweener::addStep(const KTTweenerStep &step)
{
    int counter = step.index();
    
    VERIFY_STEP(counter);
    
    if (step.has(KTTweenerStep::Position))
        setPosAt(counter, step.position());
    
    if (step.has(KTTweenerStep::Scale))
        setScaleAt(counter, step.horizontalScale(), step.verticalScale());
    
    if (step.has(KTTweenerStep::Translation))
        setTranslationAt(counter, step.xTranslation(), step.yTranslation());
    
    if (step.has(KTTweenerStep::Shear))
        setScaleAt(counter, step.horizontalShear(), step.verticalShear());
    
    if (step.has(KTTweenerStep::Rotation))
        setRotationAt(counter, step.rotation());
}

KTTweenerStep * KTItemTweener::stepAt(int index)
{
    return k->step(index);
}

void KTItemTweener::setPosAt(int index, const QPointF &pos)
{
    VERIFY_STEP(index);
    k->step(index)->setPosition(pos);
}

void KTItemTweener::setScaleAt(int index, double sx, double sy)
{
    VERIFY_STEP(index);
    k->step(index)->setScale(sx, sy);
}

void KTItemTweener::setTranslationAt(int index, double x, double y)
{
    VERIFY_STEP(index);
    k->step(index)->setTranslation(x, y);
}

void KTItemTweener::setRotationAt(int index, double angle)
{
    VERIFY_STEP(index);
    k->step(index)->setRotation(angle);
}

void KTItemTweener::setFrames(int frames)
{
    k->frames = frames;
}

int KTItemTweener::frames() const
{
    return k->frames;
}

int KTItemTweener::startFrame()
{
    return k->initFrame;
}

void KTItemTweener::fromXml(const QString &xml)
{
    QDomDocument doc;

    if (doc.setContent(xml)) {
        QDomElement root = doc.documentElement();

        k->name = root.attribute("name");
        k->initFrame = root.attribute("init").toInt();
        k->frames = root.attribute("frames").toInt();
        k->path = root.attribute("coords");

        QDomNode node = root.firstChild();
        
        while (!node.isNull()) {
               QDomElement e = node.toElement();
            
               if (!e.isNull()) {
                   if (e.tagName() == "step") {
                       QString stepDoc;
                       {
                           QTextStream ts(&stepDoc);
                           ts << node;
                       }
                    
                       KTTweenerStep *step = new KTTweenerStep(0);
                       step->fromXml(stepDoc);
                    
                       addStep(*step);
                    
                       delete step;
                   }
               }
            
               node = node.nextSibling();
        }
    }
}

QDomElement KTItemTweener::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("tweening");
    root.setAttribute("name", k->name);
    root.setAttribute("init", k->initFrame);
    root.setAttribute("frames", k->frames);
    root.setAttribute("coords", k->path);
    
    foreach (KTTweenerStep *step, k->steps.values())
             root.appendChild(step->toXml(doc));
    
    return root;
}

QGraphicsPathItem *KTItemTweener::graphicsPath() const
{
    QGraphicsPathItem *item = new QGraphicsPathItem();
    QPainterPath path;
    KTSvg2Qt::svgpath2qtpath(k->path, path);

    item->setPath(path);

    return item;
}
