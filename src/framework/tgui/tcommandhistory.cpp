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

#include "tcommandhistory.h"

TCommandHistory::TCommandHistory(QUndoStack *stack, QObject *parent) 
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

TCommandHistory::~TCommandHistory()
{
}

QUndoStack *TCommandHistory::stack() const
{
    return m_stack;
}

QAction *TCommandHistory::redoAction() const
{
    return m_redoMenu->menuAction();
}

QAction *TCommandHistory::undoAction() const
{
    return m_undoMenu->menuAction();
}

void TCommandHistory::updateMenu()
{
    for (int i = 0; i < m_stack->index(); i++)
         m_redoMenu->addAction(m_stack->text(i))->setData(i);
    
    for (int i = m_stack->index(); i < m_stack->count(); i++)
         m_undoMenu->addAction(m_stack->text(i))->setData(i);
    
    m_currentIndex = m_stack->index();
}

void TCommandHistory::updateFromIndex(int idx)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << idx << " == " << m_stack->count() << " == " << m_currentIndex;
            qDebug() << m_stack->text(idx-1);
        #else
            tDebug() << idx << " == " << m_stack->count() << " == " << m_currentIndex;
            tDebug() << m_stack->text(idx-1);
        #endif
    #endif
    
    if (idx > m_stack->count()) {
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
               // qDebug("REDO");
               // tDebug() << idx << " " << m_currentIndex;
        
               if (m_actions.contains(idx)) {
                   m_redoMenu->removeAction(m_actions[idx]);
                   m_undoMenu->addAction(m_actions[idx]);
                   m_undoMenu->menuAction()->setEnabled(true);
               } else {
                   #ifdef K_DEBUG
                       QString msg = "TCommandHistory::updateFromIndex() - Error: invalid id " + QString::number(idx);
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   #endif
               }
    } else if (idx < m_currentIndex) {
               // Undo clicked
               // qDebug("UNDO");
               // tDebug() << idx << " " << m_currentIndex;
        
               if (m_actions.contains(idx-1)) {
                   m_undoMenu->removeAction(m_actions[idx-1]);
                   m_redoMenu->addAction(m_actions[idx-1]);
                   m_redoMenu->menuAction()->setEnabled(true);
               } else {
                   #ifdef K_DEBUG
                       QString msg = "TCommandHistory::updateFromIndex() - Error: invalid id " + QString::number(idx);
                       #ifdef Q_OS_WIN
                           qDebug() << msg;
                       #else
                           tError() << msg;
                       #endif
                   #endif
               }
    }
    
    m_currentIndex = m_stack->index();
}

void TCommandHistory::undoFromAction(QAction *a)
{
    int idx = a->data().toInt();
    m_stack->blockSignals(true);

    for (int i = qMin(idx, m_currentIndex); i < qMax(idx, m_currentIndex); i++) {
        if (!m_stack->canUndo()) {
            #ifdef K_DEBUG
                QString msg = "TCommandHistory::undoFromAction() - Error: Cannot undo!!!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            break;
        }
        
        m_stack->undo();
        
        if (m_actions.contains(i)) {
            m_undoMenu->removeAction(m_actions[i]);
            m_redoMenu->addAction(m_actions[i]);
        } else {
            #ifdef K_DEBUG
                QString msg = "TCommandHistory::undoFromAction() - Error: History item not found!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
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

void TCommandHistory::redoFromAction(QAction *a)
{
    int idx = a->data().toInt();
    
    m_stack->blockSignals(true);
    for (int i = qMax(idx, m_currentIndex)-1; i >= qMin(idx, m_currentIndex)-1; i--) {
         #ifdef K_DEBUG
             QString msg = "TupCommandExecutor::createItem() - Error: Invalid scene index!";
             #ifdef Q_OS_WIN
                 qDebug() << "SHOW_VAR: " << i;
             #else
                 SHOW_VAR(i);
             #endif
          #endif

         if (!m_stack->canRedo()) {
             #ifdef K_DEBUG
                 QString msg = "TCommandHistory::redoFromAction() - Error: Cannot redo!!!";
                 #ifdef Q_OS_WIN
                     qDebug() << msg;
                 #else
                     tError() << msg;
                 #endif
             #endif

             break;
         }
        
         m_stack->redo();
        
         if (m_actions.contains(i)) {
             m_redoMenu->removeAction(m_actions[i]);
             m_undoMenu->addAction(m_actions[i]);
         } else {
             #ifdef K_DEBUG
                 QString msg = "TCommandHistory::redoFromAction() - Error while doing REDO";
                 #ifdef Q_OS_WIN
                     qDebug() << msg;
                 #else
                     tError() << msg;
                 #endif
             #endif
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

void TCommandHistory::enableRedoMenu(bool e)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TCommandHistory::enableRedoMenu()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    m_redoMenu->menuAction()->setEnabled(e);
}

void TCommandHistory::enableUndoMenu(bool e)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TCommandHistory::enableUndoMenu()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    m_undoMenu->menuAction()->setEnabled(e);
}

void TCommandHistory::undo()
{
    m_isLastRedo = false;
    m_stack->undo();
}

void TCommandHistory::redo()
{
    m_isLastRedo = true;
    m_stack->redo();
}
