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

#ifndef TUPEXPOSUREHEADER_H
#define TUPEXPOSUREHEADER_H

#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionButton>
//#include <QMap>
#include <QVector>
//#include <QItemDelegate>
#include <QLineEdit>
#include <QMouseEvent>
//#include <QMenu>

//#include "tdebug.h"
//#include <tupglobal.h>

struct LayerItem
{
    QString title;
    int lastFrame;
    bool isVisible;
    bool isLocked;
};

/**
 * @author Jorge Cuadrado
 */
class TupExposureHeader: public QHeaderView
{
    Q_OBJECT

    public:
        TupExposureHeader(QWidget * parent = 0);
        ~TupExposureHeader();
        void paintSection(QPainter *painter, const QRect & rect, int layerIndex) const;
        void insertLayer(int layerIndex, const QString &text);
        void setLayerName(int layerIndex, const QString &text);
        void setLastFrame(int layerIndex, int num);
        int lastFrame(int layerIndex);
        void removeLayer(int layerIndex);
        void moveLayer(int index, int newIndex);
        void setLockLayer(int logicalndex, bool lock);
        bool signalMovedBlocked();
        void setVisibilityChanged(int logicalndex, bool visibility);
        int layersTotal();

    public slots:
        void updateSelection(int col);

    private slots:
        void emitVisibilityChanged(int section);
        void showEditorName(int section);
        void hideEditorName();

    protected:
        virtual void mousePressEvent(QMouseEvent * event);

    private:
        QVector<LayerItem> m_layers;
        QLineEdit *m_editor;
        int m_sectionEdited;
        int m_blockSectionMoved;
        int currentCol;

    signals:
        void changedName(int indexLayer, const QString & name);
        void visibilityChanged(int indexLayer, bool visibility);
        void selectionChanged(int indexLayer);
};

#endif
