/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#ifndef TUPEXPOSURESCENE_H
#define TUPEXPOSURESCENE_H 

#include "tglobal.h"
#include "tupscene.h" 

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

class TupExposureScene : public QGroupBox 
{
    Q_OBJECT

    public:
        TupExposureScene(const QString &title, TupScene *scene, int currentLayer, int currentFrame);
        ~TupExposureScene();
        void addNewLayer();
        void addNewFrame();

        int currentFrame();
        int currentLayer();
        int framesCount(); 
        int layersCount();

    signals:
        void updateUI(int frame, int layer);

    private slots:
        void goToFrame(int frame, int layer);

    private:
        struct Private;
        Private *const k;
};

#endif
