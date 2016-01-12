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

#ifndef TUPLAYER_H
#define TUPLAYER_H

#include "tglobal.h"
#include "tupabstractserializable.h"
#include "tupframe.h"
#include "tuplipsync.h"

#include <QDomDocument>
#include <QDomElement>
#include <QList>
#include <QTextStream>

typedef QList<TupFrame *> Frames;
typedef QList<TupLipSync *> Mouths;

class TupScene;
class TupProject;

/**
 * @brief This class represents a layer. Layers are defined by the TupDocument class and they contain TupFrames
 * @author David Cuadrado 
*/

class TUPI_EXPORT TupLayer : public QObject, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        TupLayer();
        TupLayer(TupScene *parent, int index = 0);
        ~TupLayer();
        
        Frames frames();
        void setFrames(const Frames &frames);
        void setFrame(int index, TupFrame *frame);
        
        void setLayerName(const QString &name);
        QString layerName() const;
        
        void setLocked(bool isLocked);
        bool isLocked() const;

        void setVisible(bool isVisible);
        bool isVisible() const;

        void setOpacity(double opacity);
        double opacity();
        
        TupFrame *createFrame(QString name, int position, bool loaded = false);
        bool removeFrame(int position);
        bool resetFrame(int position);
        bool moveFrame(int from, int to);
        bool exchangeFrame(int from, int to);
        bool expandFrame(int position, int size);
        
        TupFrame *frame(int position) const;

        TupLipSync *createLipSync(const QString &name, const QString &soundFile, int initFrame);
        void addLipSync(TupLipSync *lipsync);
        int lipSyncCount();
        Mouths lipSyncList();
        bool removeLipSync(const QString &name);
        
        TupScene *scene() const;
        TupProject *project() const;

        void updateLayerIndex(int index);
        int layerIndex();
        
        int visualIndexOf(TupFrame *frame) const;
        
        int objectIndex() const;

        int framesCount() const;

    public:
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;
        
    private:
        struct Private;
        Private *const k;
};

#endif
