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

#include "tuptwitter.h"
#include "tconfig.h"

#include <QDomDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QFile>

QString TupTwitter::NEWS_HOST = QString("http://www.maefloresta.com");
QString TupTwitter::IS_HOST_UP_URL = QString("/updates/test.xml");
QString TupTwitter::USER_TIMELINE_URL = QString("/updates/tweets.html");
QString TupTwitter::TUPI_VERSION_URL = QString("/updates/current_version.xml");
QString TupTwitter::TUPI_WEB_MSG = QString("/updates/web_msg.html");
QString TupTwitter::BROWSER_FINGERPRINT = QString("Tupi_Browser 1.0");

struct TupTwitter::Private
{
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *reply;

    QString version;
    QString revision;
    QString codeName;
    QString word;
    QString url;
    QString webMsg;
    bool update;
    QString themeName;
};

TupTwitter::TupTwitter(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->update = false;
    TCONFIG->beginGroup("General");
    k->themeName = TCONFIG->value("Theme", "Light").toString();
}

void TupTwitter::start()
{
    QString url = NEWS_HOST + IS_HOST_UP_URL;

    #ifdef K_DEBUG
        QString msg = "TupTwitter::start() - Getting news updates...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    k->manager = new QNetworkAccessManager(this);
    connect(k->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(closeRequest(QNetworkReply*)));

    k->request.setUrl(QUrl(url));
    k->request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toLatin1());

    k->reply = k->manager->get(k->request);
    connect(k->reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

TupTwitter::~TupTwitter()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupTwitter()]";
        #else
            TEND;
        #endif
    #endif

    delete k->manager;
    k->manager = NULL;
    delete k->reply;
    k->reply = NULL;
    delete k;
}

void TupTwitter::requestFile(const QString &target)
{
    #ifdef K_DEBUG
        QString msg = "TupTwitter::requestFile() - Requesting url -> " + target;
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    k->request.setUrl(QUrl(target));
    k->request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toLatin1());
    k->reply = k->manager->get(k->request);
}

void TupTwitter::closeRequest(QNetworkReply *reply)
{
    #ifdef K_DEBUG
        QString msg = "TupTwitter::closeRequest() - Closing request...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    QByteArray array = reply->readAll();
    QString answer(array);
    answer.chop(1);

    if (answer.length() > 0) {
        if (answer.compare("<ok>true</ok>") == 0) { // The webserver data is available! 
            requestFile(NEWS_HOST + TUPI_VERSION_URL);
        } else {
            if (answer.startsWith("<version>")) { // Processing Tupi versioning data
                checkSoftwareUpdates(array);
                requestFile(NEWS_HOST + USER_TIMELINE_URL);
            } else {
                if (answer.startsWith("<div")) { // Getting Twitter records 
                    formatStatus(array);
                    requestFile(NEWS_HOST + TUPI_WEB_MSG);
                } else {
                    if (answer.startsWith("<webmsg>")) { // Getting web msg
                        saveWebMsg(answer);
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupTwitter::closeRequest() - Network Error: Invalid data!";
                            #ifdef Q_OS_WIN
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                    }
                }
            }
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupTwitter::closeRequest() - Network Error: Gosh! No Internet? :S";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    } 
}

void TupTwitter::slotError(QNetworkReply::NetworkError error)
{
    switch (error) {
            case QNetworkReply::HostNotFoundError:
                 { 
                 #ifdef K_DEBUG
                     QString msg = "TupTwitter::slotError() - Network Error: Host not found";
                     #ifdef Q_OS_WIN
                            qDebug() << msg;
                     #else
                            tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::TimeoutError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupTwitter::slotError() - Network Error: Time out!";
                     #ifdef Q_OS_WIN
                            qDebug() << msg;
                     #else
                            tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::ConnectionRefusedError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupTwitter::slotError() - Network Error: Connection Refused!";
                     #ifdef Q_OS_WIN
                            qDebug() << msg;
                     #else
                            tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::ContentNotFoundError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupTwitter::slotError() - Network Error: Content not found!";
                     #ifdef Q_OS_WIN
                            qDebug() << msg;
                     #else
                            tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::UnknownNetworkError:
            default:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupTwitter::slotError() - Network Error: Unknown Network error!";
                     #ifdef Q_OS_WIN
                            qDebug() << msg;
                     #else
                            tError() << msg;
                     #endif
                 #endif
                 }
            break;
    }
}

void TupTwitter::checkSoftwareUpdates(QByteArray array)
{
    #ifdef K_DEBUG
        QString msg = "TupTwitter::checkSoftwareUpdates() - Processing updates file...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    QDomDocument doc;

    if (doc.setContent(array)) {
        QDomElement root = doc.documentElement();
        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "branch") {
                       k->version = e.text();
                       if (k->version.compare(kAppProp->version())!=0)
                           k->update = true;
                   } else if (e.tagName() == "rev") {
                              k->revision = e.text();
                              if (k->revision.compare(kAppProp->revision())!=0)
                                  k->update = true;
                   } else if (e.tagName() == "codeName") {
                              k->codeName = e.text();
                   }
               }
               n = n.nextSibling();
        }
    }
}

void TupTwitter::formatStatus(QByteArray array)
{
    #ifdef K_DEBUG
        QString msg = "TupTwitter::formatStatus() - Formatting news file...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    QString tweets = QString(array);
    QString html = "";

    html += "<html>\n";
    html += "<head>\n";
    html += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
    html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"file:tupi.css\">\n";
    html += "</head>\n";

    if (k->themeName.compare("Dark") == 0) {
        html += "<body class=\"twitter_gray\">\n";
        html += "<div class=\"tupi_background5\">";
    } else {
        html += "<body class=\"twitter_white\">\n";
        html += "<div class=\"tupi_background1\">";
    }

    html += "<center><img src=\"file:maefloresta.png\" alt=\"maefloresta\"/></center>\n";
    html += "<div class=\"twitter_headline\"><center>&nbsp;&nbsp;@maefloresta</center></div></div>\n";
    QString css = "twitter_tupi_version";  
    if (k->update)
        css = "twitter_tupi_update"; 

    html += "<div class=\"" + css + "\"><center>\n";
    html += tr("Latest version") + ": <b>" + k->version + "</b> &nbsp;&nbsp;&nbsp;"; 
    html += tr("Revision") + ": <b>" + k->revision + "</b> &nbsp;&nbsp;&nbsp;";
    html += tr("Code Name") + ": <b>" + k->codeName + "</b>";

    if (k->update)
        html += "&nbsp;&nbsp;&nbsp;<b>[</b> <a href=\"http://www.maefloresta.com\">" + tr("It's time to upgrade! Click here!") + "</a>  <b>]</b>"; 

    html += "</center></div>\n";
    html += "<div class=\"twitter_tupi_donation\"><center>\n";
    html += "<a href=\"http://www.maefloresta.com/portal/donations\">" + tr("Want to help us to make a better project? Click here!") + "</a>";
    html += "</center></div>\n";
    html += tweets;
    html += "</body>\n";
    html += "</html>";

    QString twitterPath = QDir::homePath() + "/." + QCoreApplication::applicationName() + "/twitter.html";
    QFile file(twitterPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << html;
        file.close();
    }

    /*
    if (file.open(QIODevice::WriteOnly)) {
        QByteArray data = html.toUtf8();
        file.write(data, qstrlen(data));
        file.close();
    }
    */

    #ifdef K_DEBUG
        msg = "TupTwitter::formatStatus() - Saving file -> " + twitterPath;
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    emit pageReady();
}

void TupTwitter::saveWebMsg(const QString &answer)
{
    QFile file(QDir::homePath() + "/." + QCoreApplication::applicationName() + "/webmsg.html");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << answer;
        file.close();
    }

    k->reply->deleteLater();
    k->manager->deleteLater();
}
