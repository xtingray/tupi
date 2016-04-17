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

#ifndef TUPEXPOSUREHEADER_H
#define TUPEXPOSUREHEADER_H

#include "tglobal.h"

#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionButton>
#include <QMap>
#include <QList>
#include <QItemDelegate>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMenu>

struct ExposureLayerItem
{
    QString title;
    int lastFrame;
    bool isVisible;
    bool isLocked;
};

/**
 * @author Jorge Cuadrado
 */

class TUPI_EXPORT TupExposureHeader: public QHeaderView
{
    Q_OBJECT

    public:
        TupExposureHeader(QWidget * parent = 0);
        ~TupExposureHeader();
        void paintSection(QPainter *painter, const QRect & rect, int section) const;
        void insertSection(int section, const QString &text);
        void setSectionTitle(int section, const QString &text);
        void removeSection(int section);
        void moveHeaderSection(int index, int newIndex, bool isLocalRequest);
        void setLockFlag(int section, bool lock);
        bool sectionIsMoving();
        void setSectionVisibility(int section, bool visibility);
        int sectionsTotal();
        int currentSectionIndex();

        void setLastFrame(int section, int num);
        int lastFrame(int section);

    protected:
        virtual void mousePressEvent(QMouseEvent * event);

    public slots:
        void updateSelection(int section);

    private slots:
        void notifyVisibilityChange(int section);
        void showTitleEditor(int section);
        void hideTitleEditor();

    signals:
        void nameChanged(int section, const QString & name);
        void visibilityChanged(int section, bool visibility);
        void headerSelectionChanged(int section);

    private:
        QList<ExposureLayerItem> m_sections;
        QLineEdit *m_editor;
        int m_editedSection;
        bool m_sectionOnMotion;
        int m_currentSection;
        QString themeName;
};

#endif
