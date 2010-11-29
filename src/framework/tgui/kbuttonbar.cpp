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

#include "kbuttonbar.h"
#include "kviewbutton.h"
#include "ktoolview.h"
#include "kdebug.h"

#include <QToolButton>
#include <QBoxLayout>
#include <QAction>
#include <QCheckBox>
#include <QMenu>
#include <QMouseEvent>

#include <QtDebug>

KButtonBar::KButtonBar(Qt::ToolBarArea area, QWidget *parent) : QToolBar(parent), m_autoHide(false), m_blockHider(false)
{
    setMovable(false);
    setIconSize(QSize(16,16));
    m_buttons.setExclusive(true);

    switch (area) {
            case Qt::LeftToolBarArea:
                 {
                    setWindowTitle(tr("Left button bar"));
                 }
                 break;
            case Qt::RightToolBarArea:
                 {
                    setWindowTitle(tr("Right button bar"));
                 }
                 break;
            case Qt::BottomToolBarArea:
                 {
                    setWindowTitle(tr("Bottom button bar"));
                 }
                 break;
            case Qt::TopToolBarArea:
                 {
                    setWindowTitle(tr("Top button bar"));
                 }
                 break;
            default: break;
    }

    setObjectName("KButtonBar-"+windowTitle());

    m_separator = addAction("");
    m_separator->setEnabled(false); // Separator
    m_separator->setVisible(false);

    connect(&m_hider, SIGNAL(timeout()), this, SLOT(hide()));
    connect(&m_buttons, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(hideOthers(QAbstractButton *)));
}

KButtonBar::~KButtonBar()
{
}

QMenu *KButtonBar::createMenu()
{
    QMenu *menu = new QMenu(windowTitle(), this);
    QAction *a = 0;

    a = menu->addAction(tr("Only icons"));
    connect(a, SIGNAL(triggered()), this, SLOT(setShowOnlyIcons()));

    a = menu->addAction(tr("Only texts"));
    connect(a, SIGNAL(triggered()), this, SLOT(setShowOnlyTexts()));

    menu->addSeparator();

    a = menu->addAction(tr("Exclusive space"));
    a->setCheckable(true);
    a->setChecked(isExclusive());
    //a->setChecked(false);

    connect(a, SIGNAL(triggered(bool)), this, SLOT(setExclusive(bool)));

    a = menu->addAction(tr("Auto hide"));
    a->setCheckable(true);
    a->setChecked(autohide());
    connect(a, SIGNAL(triggered(bool)), this, SLOT(setAutoHide(bool)));

    return menu;
}

void KButtonBar::addButton(KViewButton *viewButton)
{
    QAction *act = addWidget(viewButton);
    m_buttons.addButton(viewButton);

    if (viewButton->toolView()->isVisible()) {
        hideOthers(viewButton);
        viewButton->toggleView();
    }

    m_actionForWidget[viewButton] = act;
    act->setVisible(true);

    if (!isVisible()) 
        show();
}

void KButtonBar::removeButton(KViewButton *viewButton)
{
    if (! m_buttons.buttons().contains(viewButton)) 
        return;

    m_buttons.removeButton(viewButton);
    removeAction(m_actionForWidget[viewButton]);
    viewButton->setParent(0);

    if (isEmpty()) 
        hide();
}

bool KButtonBar::isEmpty() const
{
    // O(n) -> very slow...

    bool isEmpty = true;

    foreach (QAbstractButton *button, m_buttons.buttons()) {
             isEmpty = isEmpty && button->isHidden();
             if (! isEmpty) 
                 break;
    }

    return isEmpty;
}

void KButtonBar::setExclusive(bool excl)
{
    m_buttons.setExclusive(excl);
}

void KButtonBar::setAutoHide(bool autohide)
{
    m_autoHide = autohide;
    if (autohide)
        hide();
}

bool KButtonBar::autohide() const
{
    return m_autoHide;
}

void KButtonBar::setShowOnlyIcons()
{
    foreach (QAbstractButton *b, m_buttons.buttons()) {
             KViewButton *viewButton = static_cast<KViewButton *>(b);
             viewButton->setOnlyIcon();
    }
}

void KButtonBar::setShowOnlyTexts()
{
    foreach (QAbstractButton *b, m_buttons.buttons()) {
             KViewButton *viewButton = static_cast<KViewButton *>(b);
             viewButton->setOnlyText();
    }
}

void KButtonBar::disable(KViewButton *v)
{
    QAction *a = m_actionForWidget[v];

    if (a)
        a->setVisible(false);
}

void KButtonBar::enable(KViewButton *v)
{
    QAction *a = m_actionForWidget[v];

    if (a)
        a->setVisible(true);
}

bool KButtonBar::isExclusive() const
{
    return m_buttons.exclusive();
}

void KButtonBar::onlyShow(KToolView *tool, bool ensureVisible)
{
    KViewButton *button = tool->button();

    if (ensureVisible) {
        if (tool->isVisible())
            button->click();
    }

    hideOthers(button);
}

void KButtonBar::hideOthers(QAbstractButton *source)
{
    if (!m_buttons.exclusive()) {
        static_cast<KViewButton *>(source)->toggleView();
        return;
    }

    m_buttons.setExclusive(false);

    setUpdatesEnabled(false);

    foreach (QAbstractButton *b, m_buttons.buttons()) {
             KViewButton *button = static_cast<KViewButton *>(b);
             if (source != button) {
                 if (button->toolView()->isVisible()) {
                     button->blockSignals(true);
                     button->toggleView();
                     button->blockSignals(false);
                     break;
                 }
             }
    }

    static_cast<KViewButton *>(source)->toggleView();

    m_buttons.setExclusive(true);
    setUpdatesEnabled(true);
}

void KButtonBar::mousePressEvent(QMouseEvent *e)
{
    QToolBar::mousePressEvent(e);

    if (e->button() == Qt::RightButton) {
        m_blockHider = true;
        createMenu()->exec(e->globalPos());
        e->accept();

        m_blockHider = false;
    }
}

void KButtonBar::enterEvent(QEvent *e)
{
    QToolBar::enterEvent(e);
    doNotHide();
}

void KButtonBar::leaveEvent(QEvent *e)
{
    QToolBar::leaveEvent(e);

    if (m_autoHide && !m_hider.isActive() && !m_blockHider)
        m_hider.start(800);
}

void KButtonBar::doNotHide()
{
    if (m_hider.isActive())
        m_hider.stop();
}

void KButtonBar::showSeparator(bool e)
{
    m_separator->setVisible(e);
}

int KButtonBar::count() const
{
    return m_buttons.buttons().count();
}

void KButtonBar::setEnableButtonBlending(bool enable)
{
    foreach (QAbstractButton *button, m_buttons.buttons())
             static_cast<KViewButton *>(button)->setBlending(enable);
}
