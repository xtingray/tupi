/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include <QPainter>
#include <QDialog>
#include <QPushButton>
#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QtDebug>
#include <QProcess>
#include <signal.h>

#include "tupcrashwidget.h"
#include "tupcrashhandler.h"

#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"

#include <unistd.h>

class TextArea : public QTextBrowser
{
    Q_OBJECT
    public:
        TextArea();
        ~TextArea();

    public slots:
        void setSource(const QUrl & name);
};

TextArea::TextArea()
{
    setOpenExternalLinks(true);
}

TextArea::~ TextArea()
{
}

void TextArea::setSource(const QUrl &name)
{
    if (name.scheme() == "http") {
        TCONFIG->beginGroup("General");
        QString browser = TCONFIG->value("Browser").toString();
        if (!browser.isEmpty())
            QProcess::startDetached(browser, QStringList() << name.toString());
    } else {
            QTextBrowser::setSource(name);
    }
}

#include "tupcrashwidget.moc"

TupCrashWidget::TupCrashWidget(int sig) : QDialog(0), m_sig(sig)
{
    setModal(true);

    setWindowTitle(CHANDLER->title());
    setWindowIcon(QPixmap(THEME_DIR + "icons/skull.png"));

    m_layout = new QVBoxLayout(this);
    m_tabber = new QTabWidget(this);
    m_layout->addWidget(m_tabber);

    QWidget *page1 = new QWidget;
    QVBoxLayout *page1layout = new QVBoxLayout(page1);
    QLabel *message = new QLabel("<font color="+CHANDLER->messageColor().name()+">"+ CHANDLER->message()+"</color>");

    page1layout->addWidget(message);

    QHBoxLayout *hbox = new QHBoxLayout;

    QString text = CHANDLER->defaultText();
    QImage img(CHANDLER->defaultImage());

    if (CHANDLER->containsSignalEntry(sig)) {
        text = CHANDLER->signalText(sig);
        img = QImage(CHANDLER->signalImage(sig));
    }

    QLabel *sigImg = new QLabel;
    sigImg->setPixmap(QPixmap::fromImage(img));

    hbox->addWidget(sigImg);

    TextArea *sigText = new TextArea();
    sigText->setHtml(text);
    hbox->addWidget(sigText);

    page1layout->addLayout(hbox);

    m_tabber->addTab(page1, tr("What's happening?"));

    QPushButton *launch = new QPushButton(CHANDLER->launchButtonLabel(),this);
    connect(launch, SIGNAL(clicked()), SLOT(restart()));
    m_layout->addWidget(launch);

    QPushButton *end = new QPushButton(CHANDLER->closeButtonLabel(),this);
    connect(end, SIGNAL(clicked()), SLOT(exit()));
    m_layout->addWidget(end);

    setLayout(m_layout);
}

TupCrashWidget::~TupCrashWidget()
{
}

void TupCrashWidget::setPid(int pid)
{
    m_pid = pid;
}

void TupCrashWidget::addBacktracePage(const QString &execInfo, const QString &backtrace)
{
    #ifdef K_DEBUG
        T_FUNCINFO << execInfo << " " << backtrace;
    #endif

    QWidget *btPage = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(btPage);
    layout->addWidget(new QLabel(tr("Executable information")));

    TextArea *fileInfo = new TextArea;
    fileInfo->setHtml(execInfo);

    layout->addWidget(fileInfo);
    layout->addWidget(new QLabel(tr("Backtrace")));

    TextArea *btInfo = new TextArea;
    btInfo->setHtml(backtrace);

    layout->addWidget(btInfo);

    m_tabber->addTab(btPage, tr("Backtrace"));
}

void TupCrashWidget::restart()
{
   QString path = QString::fromLocal8Bit(::getenv("TUPI_BIN")) + "/tupi &";
   // QByteArray ba = path.toAscii();
   QByteArray ba = path.toLatin1();
   system(ba.data());
   kill(m_pid, 9);
}

void TupCrashWidget::exit()
{
   kill(m_pid, 9);
}

