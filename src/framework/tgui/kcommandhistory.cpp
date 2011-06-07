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

#include "kcommandhistory.h"
#include <tdebug.h>

#include <QUndoStack>
#include <QMenu>

KCommandHistory::KCommandHistory(QUndoStack *stack, QObject *parent) 
    : QObject(parent), m_stack(stack), m_currentIndex(0), m_isLastRedo(false)
{
    m_undoMenu = new QMenu(tr("Undo"));
    m_redoMenu = new QMenu(tr("Redo"));
    
    m_undoMenu->menuAction()->setEnabled(false);
    m_redoMenu->menuAction()->setEnabled(false);
    
    connect(m_undoMenu->menuAction(), SIGNAL(triggered()), this, SLOT(undo()));
    connect(m_redoMenu->menuAction(), SIGNAL(triggered()), this, SLOT(redo()));
    
    connect(m_undoMenu, SIGNAL(triggered(QAction *)), this, SLOT(undoFromAction(QAction *)));
    connect(m_redoMenu, SIGNAL(triggered(QAction *)), this, SLOT(redoFromAction(QAction *)));
    
    connect( m_stack, SIGNAL(indexChanged(int)), this, SLOT(updateFromIndex(int)));
    connect(m_stack, SIGNAL(canRedoChanged(bool)), this, SLOT(enableRedoMenu(bool)));
    connect(m_stack, SIGNAL(canUndoChanged(bool)), this, SLOT(enableUndoMenu(bool)));
    
    updateMenu();
}

KCommandHistory::~KCommandHistory()
{
}

QUndoStack *KCommandHistory::stack() const
{
    return m_stack;
}

QAction *KCommandHistory::redoAction() const
{
    return m_redoMenu->menuAction();
}

QAction *KCommandHistory::undoAction() const
{
    return m_undoMenu->menuAction();
}

void KCommandHistory::updateMenu()
{
    for (int i = 0; i < m_stack->index(); i++)
         m_redoMenu->addAction(m_stack->text(i))->setData(i);
    
    for (int i = m_stack->index(); i < m_stack->count(); i++)
         m_undoMenu->addAction(m_stack->text(i))->setData(i);
    
    m_currentIndex = m_stack->index();
}

void KCommandHistory::updateFromIndex(int idx)
{
    qDebug("*** KCommandHistory::updateFromIndex");
    
    tfDebug << idx << " == " << m_stack->count() << " == " << m_currentIndex;
    tDebug() << m_stack->text(idx-1);
    
    if (idx > m_stack->count()) {
        qDebug("*** Exiting...");
        m_stack->setIndex(m_stack->count());
        return;
    }
    
    if (idx == m_stack->count()  && !m_isLastRedo) { // Added

        QAction *a = m_undoMenu->addAction(m_stack->text(idx-1));
        a->setData(idx);
        a->setText(QString::number(idx)+": "+m_stack->text(idx-1));
        
        m_actions.insert(idx, a);
        
        m_undoMenu->menuAction()->setEnabled(true);
        m_undoMenu->setDefaultAction(a);
        m_undoMenu->setActiveAction(a);
        
        m_redoMenu->clear();

    } else if (idx > m_currentIndex) {
               // redo clicked
               qDebug("REDO");
               tDebug() << idx << " " << m_currentIndex;
        
               if (m_actions.contains(idx)) {
                   m_redoMenu->removeAction(m_actions[idx]);
                   m_undoMenu->addAction(m_actions[idx]);
                   m_undoMenu->menuAction()->setEnabled(true);
               } else {
                   tError() << "Error!";
               }
    } else if (idx < m_currentIndex) {

               // Undo clicked
               qDebug("UNDO");
               tDebug() << idx << " " << m_currentIndex;
        
               if (m_actions.contains(idx-1)) {
                   m_undoMenu->removeAction(m_actions[idx-1]);
                   m_redoMenu->addAction(m_actions[idx-1]);
                   m_redoMenu->menuAction()->setEnabled(true);
               } else {
                   tError() << "Error!";
               }

    }
    
    m_currentIndex = m_stack->index();
}

void KCommandHistory::undoFromAction(QAction *a)
{
    int idx = a->data().toInt();
    m_stack->blockSignals(true);

    for (int i = qMin(idx, m_currentIndex); i < qMax(idx, m_currentIndex); i++) {
        if (!m_stack->canUndo()) {
            tError() << "Cannot undo!!!";
            break;
        }
        
        m_stack->undo();
        
        if (m_actions.contains(i)) {
            m_undoMenu->removeAction(m_actions[i]);
            m_redoMenu->addAction(m_actions[i]);
        } else {
            qDebug("History item not found!");
        }
    }
    
    if (m_undoMenu->isEmpty()) 
        m_undoMenu->menuAction()->setEnabled(false);
    else 
        m_undoMenu->menuAction()->setEnabled(true);
    
    if (!m_redoMenu->isEmpty()) 
        m_redoMenu->menuAction()->setEnabled(true);
    
    m_undoMenu->setDefaultAction(m_actions[m_stack->index()]);
    m_stack->blockSignals(false);
}

void KCommandHistory::redoFromAction(QAction *a)
{
    int idx = a->data().toInt();
    
    m_stack->blockSignals(true);
    for (int i = qMax(idx, m_currentIndex)-1; i >= qMin(idx, m_currentIndex)-1; i--) {
         SHOW_VAR(i);
         if (!m_stack->canRedo()) {
             tError() << "Cannot redo!!!";
             break;
         }
        
         m_stack->redo();
        
         if (m_actions.contains(i)) {
             m_redoMenu->removeAction(m_actions[i]);
             m_undoMenu->addAction(m_actions[i]);
         } else {
             qDebug("ERROR REDO");
        }
    }
    
    if (m_redoMenu->isEmpty())
        m_redoMenu->menuAction()->setEnabled(false);
    else
        m_redoMenu->menuAction()->setEnabled(true);
    
    if (!m_undoMenu->isEmpty()) 
        m_undoMenu->menuAction()->setEnabled(true);
    
    if (m_actions.contains(m_stack->index()+1))
        m_redoMenu->setDefaultAction(m_actions[m_stack->index()+1]);

    m_stack->blockSignals(false);
}

void KCommandHistory::enableRedoMenu(bool e)
{
    T_FUNCINFO;
    m_redoMenu->menuAction()->setEnabled(e);
}

void KCommandHistory::enableUndoMenu(bool e)
{
    T_FUNCINFO;
    m_undoMenu->menuAction()->setEnabled(e);
}

void KCommandHistory::undo()
{
    m_isLastRedo = false;
    m_stack->undo();
}

void KCommandHistory::redo()
{
    m_isLastRedo = true;
    m_stack->redo();
}
