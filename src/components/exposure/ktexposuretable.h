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

#ifndef KTEXPOSURETABLE_H
#define KTEXPOSURETABLE_H

#include <QTableWidget>

class KTExposureVerticalHeader;
class QMenu;

/**
 * @author Jorge Cuadrado
*/

class KTExposureTable : public QTableWidget
{
    Q_OBJECT

    friend class KTExposureVerticalHeader;
    friend class KTExposureItemDelegate;

    public:
        enum Attribute
        {
          IsEmpty = 1000,
          IsLocked
        };

        enum FrameType 
        {
          Unset = 0,
          Empty,
          Used
        };

        KTExposureTable(QWidget * parent = 0);
        ~KTExposureTable();

        int currentLayer() const;
        int currentFrame() const;
        void insertLayer(int index, const QString & name);
        void insertFrame(int layerIndex, int frameIndex, const QString & name, bool external);
        void removeLayer(int layerIndex);
        void removeFrame(int layerIndex, int frameIndex, bool fromMenu);
        void exchangeFrame(int oldPosLayer, int oldPosFrame, int newPosLayer, int newPosFrame, bool external);
        void moveLayer(int oldPosLayer, int newPosLayer);

        void setLockFrame(int layerIndex, int frameIndex, bool locked);
        void setLockLayer(int layerIndex,  bool locked);
        void setVisibilityChanged(int visualIndex, bool visibility);
        int usedFrames(int column) const;

        QString frameName(int layerIndex, int frameIndex);
        void setFrameName(int layerIndex, int frameIndex,const QString & name);
        void setLayerName(int layerIndex, const QString & name);

        bool frameIsLocked(int layerIndex, int frameIndex);
        void selectFrame(int layerIndex, int frameIndex);
        int layersTotal();
        int framesTotal();
        int framesTotalAtCurrentLayer();

        void setMenu(QMenu *menu);
        void emitCellClicked(int frame, int layer);
        void reset();

        KTExposureTable::FrameType frameState(int layerIndex, int frameIndex);
        void updateFrameState(int layerIndex, int frameIndex, KTExposureTable::FrameType value);
		
    private:
        struct Private;
        Private *const k;

    private slots:
        void emitRequestSetUsedFrame(int frameIndex,  int layerIndex);
        void emitRequestRenameFrame(QTableWidgetItem * item);
        void emitRequestSelectFrame(int currentRow, int currentColumn, int previousRow, int previousColumn);
        void emitRequestMoveLayer(int logicalIndex, int oldVisualIndex, int newVisualIndex);
        void updateLayerSelection(int layerIndex);

    protected:
        bool edit(const QModelIndex & index, EditTrigger trigger, QEvent * event);
        void mousePressEvent(QMouseEvent * event);
        void keyPressEvent(QKeyEvent * event);
        void enterEvent(QEvent *event);
        void leaveEvent(QEvent *event);

    protected slots:
        void commitData(QWidget * editor);

    signals:
        void requestSetUsedFrame(int layerIndex, int frameIndex);
        void requestRenameFrame(int layerIndex, int frameIndex,const QString & name);
        void requestSelectFrame(int layerIndex, int frameIndex);

        void requestRenameLayer(int layerIndex, const QString & name);
        void requestMoveLayer(int oldIndex, int newIndex);
        void requestChangeVisibilityLayer(int visualIndexLayer, bool visibility);
};

#endif
