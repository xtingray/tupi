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

#include "tuphelpbrowser.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QDir>

// Help Browser

TupHelpBrowser::TupHelpBrowser(QWidget *parent) : QWidget(parent)
{
    setWindowTitle(tr("Help"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "help_mode.png")));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(15);
    m_separator = new QSplitter(this);
    layout->addWidget(m_separator);

    m_pageArea = new QTextBrowser(m_separator);
    m_pageArea->setOpenExternalLinks(true);
}

TupHelpBrowser::~TupHelpBrowser()
{
}

void TupHelpBrowser::setSource(const QString &filePath)
{
    QString locale = QString(QLocale::system().name()).left(2);
    if (locale.length() < 2)
        locale = "en";

    QStringList path;
    QString resources = SHARE_DIR + "data" + QDir::separator() + "help" + QDir::separator();
    path << resources + "css";
    path << resources + "images";
    path << resources + locale + QDir::separator() + "images";
    m_pageArea->setSearchPaths(path);

    m_pageArea->setSource(filePath);
}

// SQA: These methods are just temporary for developing reasons

void TupHelpBrowser::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
            case (Qt::Key_R):
                  if (event->modifiers() == Qt::ControlModifier)
                      reload();
            break;
    }
}

void TupHelpBrowser::reload()
{
    m_pageArea->reload();
}
