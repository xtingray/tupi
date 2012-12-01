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

#ifndef TUPSTORYBOARD_H
#define TUPSTORYBOARD_H

#include "tupabstractserializable.h"
#include "tupglobal_store.h"

#include <QObject>
#include <QDomDocument>
#include <QDomElement>

class STORE_EXPORT TupStoryboard : public QObject, public TupAbstractSerializable
{
    Q_OBJECT

    public:
        TupStoryboard(const QString &author);
        ~TupStoryboard();

        void init(int start, int size);
        void reset();
        void insertScene(int index);
        void appendScene();
        void moveScene(int oldIndex, int newIndex);
        void resetScene(int index);
        void removeScene(int index);

        void setStoryTitle(const QString &title);
        void setStoryAuthor(const QString &author);
        void setStorySummary(const QString &desc);

        QString storyTitle() const;
        QString storyAuthor() const;
        QString storySummary() const;

        void setSceneTitle(int index, const QString &title);
        void setSceneDuration(int index, const QString &duration);
        void setSceneDescription(int index, const QString &description);

        QString sceneTitle(int index) const;
        QString sceneDuration(int index) const;
        QString sceneDescription(int index) const;
        
        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

        int size();
        
    private:
        struct Private;
        Private *const k;
};

#endif
