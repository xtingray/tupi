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

#include "tupabout.h"

/**
 * This class defines the About dialog of Tupi.
 * Here is where the dialog tabs are setted.
 * @author David Cuadrado
*/

TupAbout::TupAbout(QWidget *parent) : TabDialog(Cancel, parent)
{
    // SQA: Check if these instructions are doing something for real
    setWindowIcon(QIcon(THEME_DIR + "icons" + QDir::separator() + "about.png"));
    setWindowTitle(tr("About Tupi"));    
    setFixedSize(525, 458);

    QStringList path;
#ifdef Q_OS_WIN32
    QString resources = SHARE_DIR + "help" + QDir::separator();
#else
	QString resources = SHARE_DIR + "data" + QDir::separator() + "help" + QDir::separator();
#endif
    path << resources + "css";
    path << resources + "images";
	
    QString lang = "en";
    if (QString(QLocale::system().name()).length() > 1)
        lang = QString(QLocale::system().name()).left(2);

    Qt::WindowFlags flags = 0;
    flags = Qt::Dialog;
    flags |= Qt::CustomizeWindowHint;
    setWindowFlags(flags);

    // Credits Tab

    QDomDocument doc;
    QString creditsFile = DATA_DIR + "credits.xml";
    QFile file(creditsFile);
    QString creditsText;

    if (!file.open(QIODevice::ReadOnly)) {
        #ifdef K_DEBUG
            QString msg = "TupAbout::TupAbout() - Fatal Error: Can't open \"credits.xml\" file";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    if (!doc.setContent(&file)) {
        #ifdef K_DEBUG
            QString msg = "TupAbout::TupAbout() - Fatal Error: File \"credits.xml\" is corrupt!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "credits")
                   creditsText = e.text();
           }
           n = n.nextSibling();
    }

    m_credits = new TAnimWidget(QPixmap(THEME_DIR + QDir::separator() + "images" + QDir::separator() + "credits.png"), creditsText);
    addTab(m_credits, tr("Credits"));

    QPalette pal = m_credits->palette();
    pal.setColor(QPalette::Foreground, QColor(50, 50, 50, 255));
    m_credits->setPalette(pal);
    m_credits->setFont(QFont("verdana", 24));

    // Acknowledgment Tab 

    QString sponsorFile = QString() + "help" + QDir::separator() + lang + QDir::separator() + "thanks.html";
#ifdef Q_OS_WIN32
    QString sponsorPath = SHARE_DIR + sponsorFile;
#else
    QString sponsorPath = SHARE_DIR + "data" + QDir::separator() + sponsorFile;
#endif
    QTextBrowser *sponsorsText = new QTextBrowser;
    sponsorsText->setSearchPaths(path);
    sponsorsText->setOpenExternalLinks(true);
    sponsorsText->setSource(QUrl::fromLocalFile(sponsorPath));
    sponsorsText->moveCursor(QTextCursor::Start);

    addTab(sponsorsText, tr("Thanks"));

    // Tupi Description Tab 

    QString tupiFile = QString() + "help" + QDir::separator() + lang + QDir::separator() + "tupi_short.html";
#ifdef Q_OS_WIN32
    QString tupiPath = SHARE_DIR + tupiFile;
#else
    QString tupiPath = SHARE_DIR + "data" + QDir::separator() + tupiFile;
#endif
    QTextBrowser *tupiText = new QTextBrowser;
    tupiText->setSearchPaths(path);
    tupiText->setOpenExternalLinks(true);
    tupiText->setSource(QUrl::fromLocalFile(tupiPath));
    tupiText->moveCursor(QTextCursor::Start);

    addTab(tupiText, tr("About"));

    // 4: License Terms Tab

    QString licenseFile = QString() + "help" + QDir::separator() + lang + QDir::separator() + "philosophy.html"; 
#ifdef Q_OS_WIN32
    QString licensePath = SHARE_DIR + licenseFile;
#else
    QString licensePath = SHARE_DIR + "data" + QDir::separator() + licenseFile;
#endif
    QTextBrowser *licenseText = new QTextBrowser;
    licenseText->setSearchPaths(path);
    licenseText->setOpenExternalLinks(true);
    licenseText->setSource(QUrl::fromLocalFile(licensePath));
    licenseText->moveCursor(QTextCursor::Start);
    addTab(licenseText, tr("License Agreement"));
    setButtonText(Cancel, tr("Close"));
}

TupAbout::~TupAbout()
{
}
