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

#include "tbuttonbar.h"

TButtonBar::TButtonBar(Qt::ToolBarArea area, QWidget *parent) : QToolBar(parent), m_autoHide(false), m_blockHider(false), m_shouldBeVisible(true)
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

    setObjectName("TButtonBar-"+windowTitle());

    m_separator = addAction("");
    m_separator->setEnabled(false); // Separator
    m_separator->setVisible(false);

    connect(&m_hider, SIGNAL(timeout()), this, SLOT(hide()));
    connect(&m_buttons, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(hideOthers(QAbstractButton *)));
    connect(toggleViewAction(), SIGNAL(triggered(bool)), this, SLOT(onlySetShouldBeVisible(bool)));
}

TButtonBar::~TButtonBar()
{
}

QMenu *TButtonBar::createMenu()
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

void TButtonBar::addButton(TViewButton *viewButton)
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

void TButtonBar::removeButton(TViewButton *viewButton)
{
    if (! m_buttons.buttons().contains(viewButton)) 
        return;

    m_buttons.removeButton(viewButton);
    removeAction(m_actionForWidget[viewButton]);
    viewButton->setParent(0);

    if (isEmpty()) 
        hide();
}

bool TButtonBar::isEmpty() const
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

void TButtonBar::setExclusive(bool excl)
{
    m_buttons.setExclusive(excl);
}

void TButtonBar::setAutoHide(bool autohide)
{
    m_autoHide = autohide;
    if (autohide)
        hide();
}

bool TButtonBar::autohide() const
{
    return m_autoHide;
}

void TButtonBar::onlySetShouldBeVisible(bool shouldBeVisible)
{
    m_shouldBeVisible = shouldBeVisible;
}

void TButtonBar::setShouldBeVisible(bool shouldBeVisible)
{
    m_shouldBeVisible = shouldBeVisible;
    setVisible(shouldBeVisible);
}

bool TButtonBar::shouldBeVisible() const
{
    return m_shouldBeVisible;
}


void TButtonBar::setShowOnlyIcons()
{
    foreach (QAbstractButton *button, m_buttons.buttons()) {
             TViewButton *viewButton = static_cast<TViewButton *>(button);
             viewButton->setOnlyIcon();
    }
}

void TButtonBar::setShowOnlyTexts()
{
    foreach (QAbstractButton *button, m_buttons.buttons()) {
             TViewButton *viewButton = static_cast<TViewButton *>(button);
             viewButton->setOnlyText();
    }
}

void TButtonBar::disable(TViewButton *view)
{
    QAction *action = m_actionForWidget[view];

    if (action)
        action->setVisible(false);
}

void TButtonBar::enable(TViewButton *view)
{
    QAction *action = m_actionForWidget[view];

    if (action)
        action->setVisible(true);
}

bool TButtonBar::isExclusive() const
{
    return m_buttons.exclusive();
}

void TButtonBar::onlyShow(ToolView *tool, bool ensureVisible)
{
    TViewButton *button = tool->button();

    if (ensureVisible) {
        if (tool->isVisible())
            button->click();
    }

    hideOthers(button);
}

void TButtonBar::hideOthers(QAbstractButton *source)
{
    if (!m_buttons.exclusive()) {
        static_cast<TViewButton *>(source)->toggleView();
        return;
    }

    m_buttons.setExclusive(false);

    setUpdatesEnabled(false);

    foreach (QAbstractButton *b, m_buttons.buttons()) {
             TViewButton *button = static_cast<TViewButton *>(b);
             if (source != button) {
                 if (button->toolView()->isVisible()) {
                     button->blockSignals(true);
                     button->toggleView();
                     button->blockSignals(false);
                     break;
                 }
             }
    }

    static_cast<TViewButton *>(source)->toggleView();

    m_buttons.setExclusive(true);
    setUpdatesEnabled(true);
}

void TButtonBar::mousePressEvent(QMouseEvent *e)
{
    QToolBar::mousePressEvent(e);

    if (e->button() == Qt::RightButton) {
        m_blockHider = true;
        createMenu()->exec(e->globalPos());
        e->accept();

        m_blockHider = false;
    }
}

void TButtonBar::enterEvent(QEvent *e)
{
    QToolBar::enterEvent(e);
    doNotHide();
}

void TButtonBar::leaveEvent(QEvent *e)
{
    QToolBar::leaveEvent(e);

    if (m_autoHide && !m_hider.isActive() && !m_blockHider)
        m_hider.start(800);
}

void TButtonBar::doNotHide()
{
    if (m_hider.isActive())
        m_hider.stop();
}

void TButtonBar::showSeparator(bool e)
{
    m_separator->setVisible(e);
}

int TButtonBar::count() const
{
    return m_buttons.buttons().count();
}

void TButtonBar::setEnableButtonBlending(bool enable)
{
    foreach (QAbstractButton *button, m_buttons.buttons())
             static_cast<TViewButton *>(button)->setBlending(enable);
}
