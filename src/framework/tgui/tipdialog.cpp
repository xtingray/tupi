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

TipDialog::TipDialog(QStringList &labels, const QString &file, QWidget *parent) : QDialog(parent)
{
    setModal(true);
    tags = labels;
    m_database = new TipDatabase(file, parent);
    setupGUI();
}

TipDialog::TipDialog(QStringList &labels, TipDatabase *database, QWidget *parent) : QDialog(parent), m_database(database)
{
    tags = labels;
    setupGUI();
}

void TipDialog::setupGUI()
{
    setWindowTitle(tags.at(0));
    setWindowIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "today_tip.png"));

    int h;
    int s;
    int v;
    QColor baseColor = palette().base().color();
    baseColor.getHsv(&h,&s,&v);
    baseColor.setHsv(h, int(s*(71/76.0)), int(v*(67/93.0)));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    textBrowser = new QTextBrowser;
    textBrowser->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    textBrowser->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setOpenExternalLinks(true);

    QStringList path;
#ifdef Q_OS_WIN32
    QString resources = SHARE_DIR + "help" + QDir::separator();
#else
	QString resources = SHARE_DIR + "data" + QDir::separator() + "help" + QDir::separator();
#endif	

    path << resources + "css";
    path << resources + "images";
    textBrowser->setSearchPaths(path);
    
    layout->addWidget(textBrowser);
    layout->addWidget(new TSeparator);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    m_showOnStart = new QCheckBox(tags.at(1));
    buttonLayout->addWidget(m_showOnStart);
    connect(m_showOnStart, SIGNAL(clicked()), this, SLOT(setShowOnStart()));
    
    buttonLayout->addStretch(1);
    
    QPushButton *prevTip = new QPushButton(tags.at(2));
    buttonLayout->addWidget(prevTip);
    connect(prevTip, SIGNAL(clicked()), this, SLOT(showPrevTip()));
    
    QPushButton *nextTip = new QPushButton(tags.at(3));
    buttonLayout->addWidget(nextTip);
    connect(nextTip, SIGNAL(clicked()), this, SLOT(showNextTip()));
    
    QPushButton *close = new QPushButton(tags.at(4));
    buttonLayout->addWidget(close);
    connect(close, SIGNAL(clicked()), this, SLOT(accept()));
    
    layout->addLayout(buttonLayout);
    
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    TCONFIG->beginGroup("TipOfDay");
    m_showOnStart->setChecked(qvariant_cast<bool>(TCONFIG->value("ShowOnStart", true)));
    
    showNextTip();
}

TipDialog::~TipDialog()
{
}

void TipDialog::showPrevTip()
{
    m_database->prevTip();
    Tip tip = m_database->tip();
    textBrowser->setHtml(tip.text);
}

void TipDialog::showNextTip()
{
    m_database->nextTip();
    Tip tip = m_database->tip();
    textBrowser->setHtml(tip.text);
}

void TipDialog::setShowOnStart()
{
    TCONFIG->beginGroup("TipOfDay");
    TCONFIG->setValue("ShowOnStart", m_showOnStart->isChecked());
}

