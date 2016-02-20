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

#ifndef TUPTIMELINETABLE_H
#define TUPTIMELINETABLE_H

#include "tglobal.h"
#include "tuptimelineruler.h"
#include "tuptimelineheader.h"
#include "tupprojectactionbar.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QPainter>
#include <QHeaderView>

class TupTimeLineTable;
class TupTimeLineTableItemDelegate;

class TUPI_EXPORT TupTimeLineTableItem : public QTableWidgetItem
{
    public:
        enum Attributes
        {
            IsUsed = 0x0100,
            IsLocked,
            IsSound
        };
        
        TupTimeLineTableItem();
        virtual ~TupTimeLineTableItem();
        
        bool isUsed();
        bool isLocked();
        bool isSound();
};

class TupTimeLineRuler;

/**
 * @author David Cuadrado
*/
class TUPI_EXPORT TupTimeLineTable : public QTableWidget
{
    Q_OBJECT;
    
    friend class TupTimeLineTableItemDelegate;
    
    public:
        TupTimeLineTable(int sceneIndex = 0, QWidget *parent = 0);
        ~TupTimeLineTable();
        
        bool isSoundLayer(int row);
        void setLayerVisibility(int layerIndex, bool isVisible);
        void setLayerName(int layerIndex, const QString &name);
        void selectFrame(int layerIndex, int frameIndex);
        bool frameIsLocked(int layerIndex, int frameIndex);
        
    public slots:
        // Layers
        void insertLayer(int layerIndex, const QString &name);
        void insertSoundLayer(int layerIndex, const QString &name);
        void removeLayer(int layerIndex);
        void moveLayer(int oldLayerIndex, int newLayerIndex);
        int lastFrameByLayer(int layerIndex);
        void updateLayerHeader(int layerIndex);
        int currentLayer();
        int layersCount();

        // Frames
        void insertFrame(int layerIndex);
        void selectFrame(int frameIndex);
        void setAttribute(int layerIndex, int frameIndex, TupTimeLineTableItem::Attributes att, bool value);
        void removeFrame(int layerIndex, int position);
        void lockFrame(int layerIndex, int frameIndex, bool lock);
        void setItemSize(int w, int h);
        void exchangeFrame(int currentFrameIndex, int currentLayerIndex, int newFrameIndex, int newLayerIndex);
        
    private:
        void setup();
        
    protected:
        void fixSize();
        void mousePressEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void enterEvent(QEvent *event);
        void leaveEvent(QEvent *event);
        
    private slots:
        void frameSelectionFromRuler(int frameIndex);
        void frameSelectionFromLayerHeader(int layerIndex);
        void requestFrameSelection(int currentLayerIndex, int currentFrameIndex, int previousLayerIndex, int previousFrameIndex);
        void requestLayerMove(int logicalIndex, int oldLayerIndex, int newLayerIndex);
        
    signals:
        // void frameChanged(int sceneIndex, int layerIndex, int frameIndex);
        void frameSelected(int layerIndex, int frameIndex);
        void frameRemoved(int layerIndex, int frameIndex);
        void frameCopied(int layerIndex, int frameIndex);
        void visibilityChanged(int layerIndex, bool isVisible);
        void layerNameChanged(int layerIndex, const QString &name);
        void layerMoved(int oldLayerIndex, int newLayerIndex);
        
    private:
        struct Private;
        Private *const k;
};

#endif
