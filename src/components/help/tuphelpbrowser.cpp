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

// Help Browser

struct TupHelpBrowser::Private
{
    QTextBrowser *browser;
};

TupHelpBrowser::TupHelpBrowser(const QString &path, QWidget *parent) : QWidget(parent), k(new Private)
{
    k->browser = new QTextBrowser(this);
    k->browser->setOpenExternalLinks(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(k->browser);

    setSource(path);
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
    
#ifdef Q_OS_WIN32
    QString resources = SHARE_DIR + "help" + QDir::separator();
#else
    QString resources = SHARE_DIR + "data" + QDir::separator() + "help" + QDir::separator();
#endif    

    path << resources + "css";
    path << resources + "images";
    k->browser->setSearchPaths(path);

    k->browser->setSource(QUrl::fromLocalFile(filePath));
}

// SQA: These methods are just temporary for developing reasons

void TupHelpBrowser::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
            case (Qt::Key_R):
                  if (event->modifiers() == Qt::ControlModifier)
                      reload();
            break;
            case (Qt::Key_Escape):
                  emit closeDialog();
            break;
    }
}

void TupHelpBrowser::reload()
{
    k->browser->reload();
}
