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

#ifndef KTOOLVIEW_H
#define KTOOLVIEW_H

#include <QDockWidget>
#include <QIcon>
#include <QKeySequence>
#include "kideality.h"

class KViewButton;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_IDEAL_EXPORT KToolView : public QDockWidget
{
    Q_OBJECT;

    public:
        KToolView(const QString &title, const QIcon &icon = QIcon(), const QString &code = QString(), QWidget * parent = 0);
        virtual ~KToolView();

        void setDescription(const QString &description);
        KViewButton *button() const;
        void expandDock(bool flag);
        bool isExpanded();
        void setExpandingFlag();
        void setSizeHint();
        QSize sizeHint() const;
        void setPerspective(int wsp);
        int perspective() const;
        void setFixedSize(int s);
        int fixedSize() const;
        void enableButton(bool flag);
        QString getObjectID();
        bool isChecked();
        void setShortcut(QKeySequence shortcut);

    public slots:
        void saveSize(bool checked);

    private:
        void setup(const QString &label);

    protected:
        virtual void showEvent(QShowEvent *e);

/*		
#if QT_VERSION < 0x040200
    protected:
        virtual bool event(QEvent *e);

    private:
        Qt::DockWidgetArea m_area;
#endif
*/

    private:
        KViewButton *m_button;
        int m_size;
        int m_perspective;
        bool expanded;
};

#endif
