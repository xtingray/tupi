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

#ifndef TUPFRAMESTABLE_H
#define TUPFRAMESTABLE_H

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHash>

class TupFramesTable;
class TupFramesTableItemDelegate;

class TupFramesTableItem : public QTableWidgetItem
{
    public:
        enum Attributes
        {
            IsUsed = 0x01,
            IsLocked,
            IsSound
        };
        
        TupFramesTableItem();
        virtual ~TupFramesTableItem();
        
        bool isUsed();
        bool isLocked();
        bool isSound();
};

class TupTLRuler;

/**
 * @author David Cuadrado
*/
class TupFramesTable : public QTableWidget
{
    Q_OBJECT;
    
    friend class TupFramesTableItemDelegate;
    
    public:
        TupFramesTable(int sceneIndex = 0, QWidget *parent = 0);
        ~TupFramesTable();
        
        bool isSoundLayer(int row);
        
    public slots:
        // Layers
        void insertLayer(int layerPos, const QString &name);
        void insertSoundLayer(int layerPos, const QString &name);
        
        void removeCurrentLayer();
        void removeLayer(int pos);
        void moveLayer(int pos, int newPos);
        
        int lastFrameByLayer(int layerPos);
        
        // Frames
        void insertFrame(int layerPos, const QString &name);
        
        void setCurrentFrame(TupFramesTableItem *);
        void setCurrentLayer(int layerPos);
        void selectFrame(int index);
        
        void setAttribute(int row, int col, TupFramesTableItem::Attributes att, bool value);
        
        void removeFrame(int layerPos, int position);
        
        void lockFrame(int layerPosition, int position, bool lock);
        
        void setItemSize(int w, int h);
        
    private:
        void setup();
        
    protected:
        void fixSize();
        
    private slots:
        void emitFrameSelected(int col);
        // void emitFrameSelectionChanged();
        void emitFrameSelected(QTableWidgetItem *curr, QTableWidgetItem *prev);
        void emitRequestSelectFrame(int currentRow, int currentColumn, int previousRow, int previousColumn);
        
    signals:
        void frameRequest(int action, int frame, int layer, int scene, const QVariant &argument = QVariant());
        void emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex);
        void emitSelection(int currentRow, int currentColumn);
        
    private:
        struct Private;
        Private *const k;
};

#endif
