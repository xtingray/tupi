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

#ifndef TUPTIMELINEHEADER_H
#define TUPTIMELINEHEADER_H

#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QLineEdit>

struct TimeLineLayerItem
{
    QString title;
    int lastFrame;
    bool isVisible;
    bool isLocked;
    bool isSound;
};

class TUPI_EXPORT TupTimeLineHeader : public QHeaderView
{
    Q_OBJECT

    public:
        TupTimeLineHeader(QWidget * parent = 0);
        ~TupTimeLineHeader();
        void insertSection(int index, const QString &name);
        void setSectionVisibility(int index, bool visibility);
        void setSectionTitle(int index, const QString &name);
        void removeSection(int index);
        int  lastFrame(int index);
        void updateLastFrame(int index, bool addition);
        void resetLastFrame(int index);
        bool isSound(int index);
        void setSoundFlag(int index, bool flag);
        int currentSectionIndex();
        void moveHeaderSection(int position, int newPosition, bool isLocalRequest);
        bool sectionIsMoving();

    protected:
        void paintSection(QPainter *painter, const QRect & rect, int index) const;
        virtual void mousePressEvent(QMouseEvent *event);

    public slots:
        void updateSelection(int index);

    private slots:
        void showTitleEditor(int section);
        void hideTitleEditor();

    signals:
        void headerSelectionChanged(int section);
        void visibilityChanged(int section, bool isVisible);
        void nameChanged(int section, const QString &name);

    private:
        struct Private;
        Private *const k;
};

#endif
