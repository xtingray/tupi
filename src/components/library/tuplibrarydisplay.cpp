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

#include "tuplibrarydisplay.h"

struct TupLibraryDisplay::Private
{
    TupItemPreview *previewPanel;
    TupSoundPlayer *soundPlayer;
};

TupLibraryDisplay::TupLibraryDisplay() : QWidget(), k(new Private)
{
    k->previewPanel = new TupItemPreview(this);
    k->soundPlayer = new TupSoundPlayer(this);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->addWidget(k->previewPanel);
    layout->addWidget(k->soundPlayer);
    layout->setContentsMargins(0, 0, 0, 0);
 
    showDisplay();
}

TupLibraryDisplay::~TupLibraryDisplay()
{
    delete k;
}

QSize TupLibraryDisplay::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(100, 100));
}

void TupLibraryDisplay::reset()
{
    k->previewPanel->reset();
}

void TupLibraryDisplay::render(QGraphicsItem *item)
{
    k->previewPanel->render(item);
}

void TupLibraryDisplay::showDisplay()
{
    if (!k->previewPanel->isVisible()) {
        k->previewPanel->show();
        k->soundPlayer->hide();
    }
}

void TupLibraryDisplay::setSoundObject(const QString &path)
{
    k->soundPlayer->setSoundObject(path);
}

void TupLibraryDisplay::showSoundPlayer()
{
    if (!k->soundPlayer->isVisible()) {
        k->previewPanel->hide();
        k->soundPlayer->show();
    }
}
