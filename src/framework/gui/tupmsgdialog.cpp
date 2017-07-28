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

#include "tupmsgdialog.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "talgorithm.h"

#include <QVBoxLayout>
#include <QPushButton>

TupMsgDialog::TupMsgDialog(const QString &message, QSize dialogSize, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    msg = message;
    size = dialogSize;
    if (size == QSize(0, 0))
        size = QSize(200, 100);

    setupGUI();
}

TupMsgDialog::~TupMsgDialog()
{
}

void TupMsgDialog::setupGUI()
{
    setWindowTitle(tr("Breaking News!"));
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

    int index = TAlgorithm::random() % 3;
    QString html = "<html>\n";
    html += "<head>\n";
    html += "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=utf-8\">\n";
    html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"file:tupi.css\" />\n";
    html += "</head>\n";
    html += "<body class=\"tip_background0" + QString::number(index) + "\">\n";
    html += msg;
    html += "\n</body>\n";
    html += "</html>";

    textBrowser->setHtml(html);

    QPushButton *closeButton = new QPushButton(tr("Close"));
    layout->addWidget(closeButton);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton, 1, Qt::AlignHCenter);

    layout->addWidget(textBrowser);
    layout->addWidget(new TSeparator);
    layout->addLayout(buttonLayout);

    setAttribute(Qt::WA_DeleteOnClose, true);

    setFixedSize(size);
}
