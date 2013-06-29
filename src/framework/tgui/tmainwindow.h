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

#ifndef TMAINWINDOW_H
#define TMAINWINDOW_H

#include "tideality.h"

// Project begin: Aug 4 2006

#include <QMainWindow>
#include <QHash>
#include <QMap>
#include <QKeySequence>

class TButtonBar;
class ToolView;
class TMainWindowAbstractSettings;

/**
 * iDeality Main Window
 * @author David Cuadrado
*/

class T_IDEAL_EXPORT TMainWindow : public QMainWindow
{
    Q_OBJECT;
    public:
        enum
         {
           None = 0, DefaultPerspective
         };

        TMainWindow(QWidget *parent = 0);
        ~TMainWindow();

        ToolView *addToolView(QWidget *widget, Qt::DockWidgetArea area, int perspective = DefaultPerspective, const QString &code = QString(), QKeySequence shortcut = QKeySequence(""));

        void removeToolView(ToolView *view);

        // FIXME: remove tool view
        void moveToolView(ToolView *view, Qt::DockWidgetArea newPlace);

        void addToPerspective(QWidget *widget, int perspective = DefaultPerspective);
        void removeFromPerspective(QWidget *widget);

        void addToPerspective(QAction *action, int perspective);
        void addToPerspective(const QList<QAction *> &actions, int perspective);
        void removeFromPerspective(QAction *action);
        void setCurrentPerspective(int wsp);
        int currentPerspective() const;
        void enableToolViews(bool flag);

        void setAutoRestore(bool autoRestore);
        bool autoRestore() const;

        virtual QMenu *createPopupMenu();

        void setSettingsHandler(TMainWindowAbstractSettings *settings);
        void restoreGUI();
        void saveGUI();

        QHash<Qt::ToolBarArea, TButtonBar *> buttonBars() const;
        QHash<TButtonBar *, QList<ToolView*> > toolViews() const;

    private:
        Qt::DockWidgetArea toDockWidgetArea(Qt::ToolBarArea area);
        Qt::ToolBarArea toToolBarArea(Qt::DockWidgetArea area);

    public slots:
        void setEnableButtonBlending(bool enable);

    private slots:
        void relayoutViewButton(bool topLevel);
        void relayoutToolView();

    signals:
        void perspectiveChanged(int wps);

    protected:
        void addButtonBar(Qt::ToolBarArea area);

    protected:
        virtual void closeEvent(QCloseEvent *e);
        virtual void showEvent(QShowEvent *e);
        virtual bool event(QEvent *e);

    private:
        ToolView *m_forRelayout;

    private:
        QHash<Qt::ToolBarArea, TButtonBar *> m_buttonBars;
        QHash<TButtonBar *, QList<ToolView*> > m_toolViews;
        QHash<QWidget *, int> m_managedWidgets;
        QHash<QAction *, int> m_managedActions;

        int m_currentPerspective;

        TMainWindowAbstractSettings *m_settings;
        bool m_autoRestore;
};

#endif
