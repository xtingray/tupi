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

#include "tupprojectactionbar.h"

struct TupProjectActionBar::Private
{
    Private(Qt::Orientation orientation) : orientation(orientation), isAnimated(true) {}
    QString container;
    Qt::Orientation orientation;
    int fixedSize;
    QButtonGroup actions;
    QBoxLayout *buttonLayout;
    bool isAnimated;
};

TupProjectActionBar::TupProjectActionBar(const QString &container, Actions actions, Qt::Orientation orientation, QWidget *parent) : QWidget(parent), k(new Private(orientation))
{
    k->container = container;
    connect(&k->actions, SIGNAL(buttonClicked(int)), this, SLOT(emitActionSelected(int)));
    setup(actions);
    setFixedSize(22);
}

TupProjectActionBar::~TupProjectActionBar()
{
}

void TupProjectActionBar::setFixedSize(int s)
{
    k->fixedSize = s;
}

void TupProjectActionBar::setup(Actions actions)
{
    QBoxLayout *mainLayout = 0;
    
    switch (k->orientation) {
            case Qt::Vertical:
            {
                 mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
                 k->buttonLayout = new QBoxLayout(QBoxLayout::TopToBottom);
            }
            break;
            case Qt::Horizontal:
            {
                 mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
                 k->buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
            }
            break;
    }
    
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    
    k->buttonLayout->setSpacing(1);
    k->buttonLayout->setMargin(1);
    k->buttonLayout->addStretch();
    int size = 16;

    if (actions & InsertFrame) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/add_frame.png"), size);
        button->setToolTip(tr("Insert frame"));
        // SQA: This short-cut has been moved to Zoom In feature
        button->setShortcut(QKeySequence(Qt::Key_9));
        
        k->actions.addButton(button, InsertFrame);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & ExtendFrame) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/extend_frame.png"), size);
        button->setToolTip(tr("Extend frame"));
        // button->setShortcut(QKeySequence());

        k->actions.addButton(button, ExtendFrame);

        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & RemoveFrame) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/remove_frame.png"), size);
        button->setToolTip(tr("Remove frame"));
        // SQA: This short-cut has been moved to Zoom Out feature
        button->setShortcut(QKeySequence(Qt::Key_0));
        
        k->actions.addButton(button, RemoveFrame);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & MoveFrameBackward) {
        TImageButton *button = 0;
        if (k->container.compare("Exposure") == 0) {
            button = new TImageButton(QIcon(THEME_DIR + "icons/move_frame_up.png"), size);
        } else {
            if (k->container.compare("TimeLine") == 0)
                button = new TImageButton(QIcon(THEME_DIR + "icons/move_frame_backward.png"), size);
        }
        
        button->setToolTip(tr("Move frame backward"));
        button->setShortcut(QKeySequence(tr("F8")));

        k->actions.addButton(button, MoveFrameBackward);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }
    
    if (actions & MoveFrameForward) {
        TImageButton *button = 0;

        if (k->container.compare("Exposure") == 0) {
            button = new TImageButton(QIcon(THEME_DIR + "icons/move_frame_down.png"), size);
        } else {
            if (k->container.compare("TimeLine") == 0)
                button = new TImageButton(QIcon(THEME_DIR + "icons/move_frame_forward.png"), size);
        }
 
        button->setToolTip(tr("Move frame forward"));
        button->setShortcut(QKeySequence(tr("F9")));
        
        k->actions.addButton(button, MoveFrameForward);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }
    
    if (actions & LockFrame) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/padlock.png"), size);
        button->setToolTip(tr("Lock frame"));
        button->setShortcut(QKeySequence(tr("F10")));
        
        k->actions.addButton(button, LockFrame);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & LockLayer) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/move_layer_down.png" ), 22);
        button->setToolTip(tr("Lock layer"));
        
        k->actions.addButton(button, LockLayer);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & Separator) {
        k->buttonLayout->addSpacing(5);
        k->buttonLayout->addWidget(new TSeparator(Qt::Vertical));
    }

    if (actions & InsertLayer) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/add_layer.png"), size);
        button->setToolTip(tr("Insert layer"));
        button->setShortcut(QKeySequence(tr("F5")));

        k->actions.addButton(button, InsertLayer);

        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & RemoveLayer) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/remove_layer.png"), size);
        button->setToolTip(tr("Remove layer"));
        button->setShortcut(QKeySequence(tr("F6")));

        k->actions.addButton(button, RemoveLayer);

        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & MoveLayerUp) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/move_layer_up.png"), size);
        button->setToolTip(tr("Move layer up"));
        button->setShortcut(QKeySequence(tr("F9")));

        k->actions.addButton(button, MoveLayerUp);

        k->buttonLayout->addWidget(button);
        button->setAnimated(true);
    }

    if (actions & MoveLayerDown) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/move_layer_down.png" ), size);
        button->setToolTip(tr("Move layer down"));
        button->setShortcut(QKeySequence(tr("F10")));

        k->actions.addButton(button, MoveLayerDown);

        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & Separator) {
        k->buttonLayout->addSpacing(5);
        k->buttonLayout->addWidget(new TSeparator(Qt::Vertical));
    }

    if (actions & InsertScene) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/add_scene.png"), size); 
        button->setToolTip(tr("Insert scene"));
        
        k->actions.addButton(button, InsertScene);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }
     
    if (actions & RemoveScene) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/remove_scene.png"), size);
        button->setToolTip(tr("Remove scene"));

        k->actions.addButton(button, RemoveScene);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    if (actions & MoveSceneUp) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/move_scene_up.png"), size);
        button->setToolTip(tr("Move scene up"));
        
        k->actions.addButton(button, MoveSceneUp);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }
     
    if (actions & MoveSceneDown) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/move_scene_down.png"), size);
        button->setToolTip( tr("Move scene down"));
        
        k->actions.addButton(button, MoveSceneDown);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }
    
    if (actions & LockScene) {
        TImageButton *button = new TImageButton(QIcon(THEME_DIR + "icons/lock_scene.png"), size);
        button->setToolTip(tr("Lock scene"));
        
        k->actions.addButton(button, LockScene);
        
        k->buttonLayout->addWidget(button);
        button->setAnimated(k->isAnimated);
    }

    k->buttonLayout->addStretch();
    
    mainLayout->addWidget(new TSeparator(Qt::Horizontal));
    mainLayout->addLayout(k->buttonLayout);
    mainLayout->addWidget(new TSeparator(Qt::Horizontal));
}

// SQA: Check why this function do nothing :S

void TupProjectActionBar::insertSeparator(int position)
{
    /*
    Qt::Orientation sepOrientation = Qt::Vertical;
    switch (k->orientation) {
            case Qt::Vertical:
            {
                 sepOrientation = Qt::Horizontal;
            }
            break;
            case Qt::Horizontal:
            {
                 sepOrientation = Qt::Vertical;
            }
            break;
    }
    */

    k->buttonLayout->insertWidget(position + 1, new TSeparator(Qt::Vertical), 1, Qt::AlignCenter);
}

void TupProjectActionBar::insertBlankSpace(int position)
{
    /*
    Qt::Orientation sepOrientation = Qt::Vertical;
    switch (k->orientation) {
            case Qt::Vertical:
            {
                 sepOrientation = Qt::Horizontal;
            }
            break;
            case Qt::Horizontal:
            {
                 sepOrientation = Qt::Vertical;
            }
            break;
    }
    */

    QWidget *widget = new QWidget();
    widget->setFixedSize(5,5);
   
    k->buttonLayout->insertWidget(position + 1, widget, 1, Qt::AlignCenter);
}

TImageButton *TupProjectActionBar::button(Action action)
{
    return qobject_cast<TImageButton *>(k->actions.button(action));
}

void TupProjectActionBar::emitActionSelected(int action)
{
    switch(action)
    {
        case RemoveFrame:
        {
            TCONFIG->beginGroup("General");
            bool ask = TCONFIG->value("ConfirmRemoveFrame", true).toBool();

            if (ask) {
                TOptionalDialog dialog(tr("Do you want to remove this frame?"), tr("Confirmation"), this);
                dialog.setModal(true);
                QDesktopWidget desktop;
                dialog.move((int) (desktop.screenGeometry().width() - dialog.sizeHint().width())/2,
                            (int) (desktop.screenGeometry().height() - dialog.sizeHint().height())/2);

                if (dialog.exec() == QDialog::Rejected)
                    return;

                TCONFIG->beginGroup("General");
                TCONFIG->setValue("ConfirmRemoveFrame", dialog.shownAgain());
                TCONFIG->sync();
            }
        }
        break;
        case RemoveLayer:
        {
            TCONFIG->beginGroup("General");
            bool ask = TCONFIG->value("ConfirmRemoveLayer", true).toBool();

            if (ask) {
                TOptionalDialog dialog(tr("Do you want to remove this layer?"), tr("Confirmation"), this);
                QDesktopWidget desktop;
                dialog.move((int) (desktop.screenGeometry().width() - dialog.sizeHint().width())/2,
                            (int) (desktop.screenGeometry().height() - dialog.sizeHint().height())/2);

                if (dialog.exec() == QDialog::Rejected)
                    return;

                TCONFIG->beginGroup("General");
                TCONFIG->setValue("ConfirmRemoveLayer", dialog.shownAgain());
                TCONFIG->sync();
            }
        }
        break;
        case RemoveScene:
        {
            TCONFIG->beginGroup("General");
            bool ask = TCONFIG->value("ConfirmRemoveScene", true).toBool();

            if (ask) {
                TOptionalDialog dialog(tr("Do you want to remove this scene?"), tr("Confirmation"), this);
                QDesktopWidget desktop;
                dialog.move((int) (desktop.screenGeometry().width() - dialog.sizeHint().width())/2,
                            (int) (desktop.screenGeometry().height() - dialog.sizeHint().height())/2);

                if (dialog.exec() == QDialog::Rejected)
                    return;

                TCONFIG->beginGroup("General");
                TCONFIG->setValue("ConfirmRemoveScene", dialog.shownAgain());
                TCONFIG->sync();
            }
        }
        break;
    }
    
    emit actionSelected(action);
}

