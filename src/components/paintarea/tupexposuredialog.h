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

#ifndef TUPEXPOSUREDIALOG_H
#define TUPEXPOSUREDIALOG_H

#include "tupproject.h"
#include <QDialog>

class TupExposureDialog : public QDialog
{
    Q_OBJECT

    public:
        TupExposureDialog(TupProject *project, int scene, int layer, int frame, bool isNetworked, const QStringList &onLineUsers, QWidget *parent);
        ~TupExposureDialog();
        void updateUsersList(const QStringList &onLineUsers);

    signals:
        void goToFrame(int frame, int layer, int scene);
        void goToScene(int scene);
        void callNewScene();
        void callNewLayer(int sceneIndex, int layerIndex);
        void callNewFrame(int sceneIndex, int layerIndex, int layersTotal, int frameIndex);
        void windowHasBeenClosed();

    private slots:
        void refreshUI(int frame, int layer);
        void goToScene(int column, int sceneIndex);
        void closeDialog();
        void createScene();
        void createLayer();
        void createFrame();

    private:
        void setButtonBar();
        void setSheet(int sceneIndex, int layerIndex, int frameIndex);
        struct Private;
        Private *const k;
};

#endif
