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

#ifndef TCOMMANDHISTORY_H
#define TCOMMANDHISTORY_H

#include <QObject>
#include <QHash>

class QUndoStack;
class QMenu;
class QAction;

/**
 * @author David Cuadrado
*/

class TCommandHistory : public QObject
{
    Q_OBJECT
    
    public:
        TCommandHistory(QUndoStack *stack, QObject *parent = 0);
        ~TCommandHistory();
        
        QAction *redoAction() const;
        QAction *undoAction() const;
        
        QUndoStack *stack() const;
        
    public slots:
        void enableRedoMenu(bool e);
        void enableUndoMenu(bool e);
        
        void undo();
        void redo();
        
    private:
        void updateMenu();
        
    private slots:
        void updateFromIndex(int idx);
        void undoFromAction(QAction *a);
        void redoFromAction(QAction *a);
        
    private:
        QUndoStack *m_stack;
        QMenu *m_redoMenu;
        QMenu *m_undoMenu;
        
        int m_currentIndex;
        QHash<int, QAction *> m_actions;
        bool m_isLastRedo;
};

#endif
