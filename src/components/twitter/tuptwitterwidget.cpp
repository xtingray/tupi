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

#include "tuptwitterwidget.h"

struct TupTwitterWidget::Private
{
    QSplitter *separator;
    QTextBrowser *pageArea;
    QTextDocument *document;
};

TupTwitterWidget::TupTwitterWidget(QWidget *parent) : QWidget(parent), k(new Private)
{
    setWindowTitle(tr("News!"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/news_mode.png")));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(15);
    k->separator = new QSplitter(this);
    layout->addWidget(k->separator);

    k->pageArea = new QTextBrowser(k->separator);
    k->document = new QTextDocument(k->pageArea);
    k->pageArea->setDocument(k->document);
}

TupTwitterWidget::~TupTwitterWidget()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupTwitterWidget()]";
        #else
            TEND;
        #endif
    #endif

    delete k;
}

void TupTwitterWidget::setSource(const QString &filePath)
{
    QStringList path;

#ifdef Q_OS_WIN
    QString resources = SHARE_DIR + "html/";
#else
    QString resources = SHARE_DIR + "data/html/";
#endif

    path << resources + "css";
    path << resources + "images";
    k->pageArea->setSearchPaths(path);
    k->pageArea->setOpenExternalLinks(true);
    k->pageArea->setSource(QUrl::fromLocalFile(filePath));
}

void TupTwitterWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
            case Qt::Key_1:
                  if (event->modifiers() == Qt::ControlModifier)
                      emit newPerspective(0);
            break;
            case Qt::Key_2:
                  if (event->modifiers() == Qt::ControlModifier)
                      emit newPerspective(1);
            break;
    }
}
