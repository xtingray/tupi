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

#include "tipdialog.h"

TipDialog::TipDialog(QStringList &labels, const QString &videos, const QString &tips, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    tags = labels;
    recordsDatabase = new TipDatabase(videos, tips, parent);
    setupGUI();
}

void TipDialog::setupGUI()
{
    tabIndex = 0;
    setWindowTitle(tags.at(0));
    setWindowIcon(QPixmap(THEME_DIR + "icons/bubble.png"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    textBrowser = new QTextBrowser;
    textBrowser->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textBrowser->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setOpenExternalLinks(true);

    QStringList path;
#ifdef Q_OS_WIN
    QString resources = SHARE_DIR + "html/";
#else
    QString resources = SHARE_DIR + "data/html/";
#endif	

    path << resources + "css";
    path << resources + "images";
    textBrowser->setSearchPaths(path);

    tabWidget = new QTabWidget;

    if (recordsDatabase->videosCount() > 0) {
        videoBrowser = new QTextBrowser;
        videoBrowser->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        videoBrowser->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
        videoBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        videoBrowser->setOpenExternalLinks(true);
        videoBrowser->setSearchPaths(path);

        tabWidget->addTab(videoBrowser, tr("Animations"));
        connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabIndex(int)));
        nextVideo();
    }

    tabWidget->addTab(textBrowser, tr("Tips"));
    
    layout->addWidget(tabWidget);
    layout->addWidget(new TSeparator);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    buttonLayout->addStretch(1);
    
    QPushButton *prevButton = new QPushButton(tags.at(1));
    buttonLayout->addWidget(prevButton);
    connect(prevButton, SIGNAL(clicked()), this, SLOT(showPreviousItem()));

    QPushButton *nextButton = new QPushButton(tags.at(2));
    buttonLayout->addWidget(nextButton);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(showNextItem()));

    buttonLayout->addSpacing(100);
    
    QPushButton *close = new QPushButton(tags.at(3));
    buttonLayout->addWidget(close);
    connect(close, SIGNAL(clicked()), this, SLOT(accept()));
    
    layout->addLayout(buttonLayout);
    
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    nextTip();
}

TipDialog::~TipDialog()
{
}

void TipDialog::showPreviousItem()
{
    if (tabWidget->count() == 2) {
        if (tabIndex == Animation)
            previousVideo();
        else
            previousTip();
    } else {
        previousTip();
    }
}

void TipDialog::previousVideo()
{
    recordsDatabase->previousVideo();
    QString video = recordsDatabase->video();
    videoBrowser->setHtml(video);
}

void TipDialog::previousTip()
{
    recordsDatabase->prevTip();
    QString tip = recordsDatabase->tip();
    textBrowser->setHtml(tip);
}

void TipDialog::showNextItem()
{
    if (tabWidget->count() == 2) {
        if (tabIndex == Animation)
            nextVideo();
        else
            nextTip();
    } else {
        nextTip();
    }
}

void TipDialog::nextVideo()
{
    recordsDatabase->nextVideo();
    QString video = recordsDatabase->video();
    videoBrowser->setHtml(video);
}

void TipDialog::nextTip()
{
    recordsDatabase->nextTip();
    QString tip = recordsDatabase->tip();
    textBrowser->setHtml(tip);
}

void TipDialog::updateTabIndex(int index)
{
    tabIndex = index;
}
