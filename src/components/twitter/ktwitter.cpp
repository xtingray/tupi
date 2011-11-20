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
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include <QtGui>
#include <QtNetwork>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QEventLoop>

#include "ktwitter.h"
#include "tdebug.h"

QString KTwitter::TWITTER_HOST = QString("http://twitter.com");
QString KTwitter::IS_TWITTER_UP_URL = QString("/help/test.xml");
QString KTwitter::USER_TIMELINE_URL = QString("/statuses/user_timeline/maefloresta.xml");
QString KTwitter::TUPI_VERSION_URL = QString("http://www.maefloresta.com/updates/current_version.xml");
QString KTwitter::BROWSER_FINGERPRINT = QString("Tupi_Browser 1.0");

struct KTwitter::Private
{
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QString version;
    QString revision;
    QString codeName;
    QString word;
    QString reference;
    QString url;
    QString meaning;
    bool update;
};

KTwitter::KTwitter(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->update = false;
}

void KTwitter::start()
{
    loadTwitterMeaning();
    QString url = TWITTER_HOST + IS_TWITTER_UP_URL;

    k->manager = new QNetworkAccessManager(this);
    connect(k->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(closeRequest(QNetworkReply*)));

    k->request.setUrl(QUrl(url));
    k->request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toAscii());

    k->reply = k->manager->get(k->request);
    connect(k->reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError(QNetworkReply::NetworkError)));
}

KTwitter::~KTwitter()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    delete k;
}

void KTwitter::requestFile(QString target)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->request.setUrl(QUrl(target));
    k->request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toAscii());
    k->reply = k->manager->get(k->request);
}

void KTwitter::closeRequest(QNetworkReply *reply)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QByteArray array = reply->readAll();
    QString answer(array);

    if (answer.length() > 0) {
        if (answer.compare("<ok>true</ok>") == 0) {
            requestFile(TUPI_VERSION_URL);
        } else {
            if (answer.contains("branch", Qt::CaseSensitive)) {
                checkSoftwareUpdates(array);
                requestFile(TWITTER_HOST +  USER_TIMELINE_URL);
            } else {
                if (answer.contains("status", Qt::CaseSensitive) && (!answer.contains("Twitter is over capacity") 
                    && !answer.contains("whale_error.gif") && !answer.contains("503 Error"))) {
                    formatStatus(array);
                } else {
                    #ifdef K_DEBUG
                           tError() << "KTwitter::closeRequest() - Network Error: Invalid data!";
                    #endif
                }
            }
        }
    } else {
        #ifdef K_DEBUG
               tError() << "KTwitter::closeRequest() - Network Error: Gosh! No Internet? :S";
        #endif
    } 
}

void KTwitter::slotError(QNetworkReply::NetworkError error)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    switch (error) {
            case QNetworkReply::HostNotFoundError:
                 { 
                 #ifdef K_DEBUG
                        tError() << "KTwitter::slotError() - Network Error: Host not found";
                 #endif
                 }
            break;
            case QNetworkReply::TimeoutError:
                 {
                 #ifdef K_DEBUG
                        tError() << "KTwitter::slotError() - Network Error: Time out!";
                 #endif
                 }
            break;
            case QNetworkReply::ConnectionRefusedError:
                 {
                 #ifdef K_DEBUG
                        tError() << "KTwitter::slotError() - Network Error: Connection Refused!";
                 #endif
                 }
            break;
            case QNetworkReply::ContentNotFoundError:
                 {
                 #ifdef K_DEBUG
                        tError() << "KTwitter::slotError() - Network Error: Content not found!";
                 #endif
                 }
            break;
            case QNetworkReply::UnknownNetworkError:
            default:
                 {
                 #ifdef K_DEBUG
                        tError() << "KTwitter::slotError() - Network Error: Unknown Network error!";
                 #endif
                 }
            break;
    }
}

void KTwitter::checkSoftwareUpdates(QByteArray array)
{
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

void KTwitter::formatStatus(QByteArray array)
{
    QString output = "";
    QString name = "";
    QString description = "";
    QString website = "";
    QString followers = "";
    QString image = "";

    QDomDocument doc;

    if (doc.setContent(array)) {

        QDomElement root = doc.documentElement();
        QDomNode n = root.firstChild();
        int counter = 0;

        while (!n.isNull()) {
               QDomElement e = n.toElement();

               if (!e.isNull()) {
                   if (e.tagName() == "status") {
                       QDomNode n1 = e.firstChild();
                       QString date = "";
                       QString text = "";
                       while (!n1.isNull()) {
                              QDomElement e1 = n1.toElement();
                              if (!e1.isNull()) {
                                  if (e1.tagName() == "created_at") {
                                      QString data = e1.text(); 
                                      QStringList list1 = data.split(" ");
                                      for (int i=0; i<3; i++)
                                           date += list1.at(i) + " ";
                                           date += list1.at(3);
                                  } else {
                                      if (e1.tagName() == "text") {
                                          text = e1.text();
                                          int init = text.indexOf("http://");
                                          if (init >= 0) {
                                              int end = text.indexOf(" ", init);
                                              if (end < 0)
                                                  end = text.length();
                                              QString www = text.mid(init, end-init);
                                              QString link = "<a href=\"" + www + "\">";
                                              text.insert(end, "</a>");
                                              text.insert(init, link);
                                          }
                                      } else {
                                          if (counter == 0) {
                                              if (e1.tagName() == "user") {
                                                  QDomNode n2 = e1.firstChild();
                                                  while (!n2.isNull()) {
                                                         QDomElement e2 = n2.toElement();
                                                         if (!e2.isNull()) {
                                                             if (e2.tagName() == "name") {
                                                                 name = e2.text(); 
                                                             } else if (e2.tagName() == "description") {
                                                                        description = e2.text();
                                                                        description.replace("-", "<br/>-");
                                                             } else if (e2.tagName() == "profile_image_url") {
                                                                        image = e2.text();
                                                             } else if (e2.tagName() == "url") {
                                                                        website = "<a href=\"" + e2.text() + "\">";
                                                                        website += e2.text(); 
                                                                        website += "</a>";
                                                             } else if (e2.tagName() == "followers_count") {
                                                                        followers = e2.text();
                                                             }
                                                         }
                                                         n2 = n2.nextSibling();
                                                  }
                                                  counter = 1;
                                              }
                                          }
                                      }
                                  }
                              }

                              n1 = n1.nextSibling();
                       }

                       output += "       <p class=\"status\">\n";
                       output += "       <div class=\"date\">[ " + date + " ]</div><br/>\n";
                       output += "       <b>MaeFloresta:</b> ";
                       output += text + "\n";
                       output += "       <p/>\n";
                   }
               }

               n = n.nextSibling();
        }
     }

     QString html = "";

     html += "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
     html += "<html>\n";
     html += "<head>\n";
     html += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
     html += "<link rel=\"stylesheet\" type=\"text/css\" href=\"" + QString::fromLocal8Bit(::getenv("TUPI_SHARE")) \
             + "/data/help/css/tupi.css\">\n";
     html += "</head>\n";
     html += "<body class=\"tupi_background1\">\n";
     html += " <table class=\"twitter_base\">\n";
     html += "  <tr>\n";
     html += "  <td class=\"twitter_status\">\n";
     html += "    <table class=\"status_table\">\n";
     html += "     <tr>\n";
     html += "     <td>\n";
     html += "      &nbsp;&nbsp;\n";
     html += "      <img class=\"twitter_logo\" src=\"" + QString::fromLocal8Bit(::getenv("TUPI_SHARE")) \
             + "/data/help/images/twitter01.png\" alt=\"maefloresta\"/>\n";
     html += "      <font class=\"twitter_headline\" >&nbsp;&nbsp;maefloresta</font>\n";
     html += "      <br/>\n";
     html += "     </td>\n";
     html += "     </tr>\n";
     html += "     <tr>\n";

     QString css = "tupi_version";  
     if (k->update)
         css = "tupi_update"; 

     html += "     <td class=\"" + css + "\">\n";
     html += "       <center>" + tr("Latest version") + ": <b>" + k->version + "</b> &nbsp;&nbsp;&nbsp;" + tr("Revision") + ": <b>" + k->revision + "</b> &nbsp;&nbsp;&nbsp;" + tr("Code Name") + ": <b>" + k->codeName + "</b>";

     if (k->update)
         html += "<br/><b>[</b> <a href=\"http://www.maefloresta.com\">" + tr("Update here!") + "</a>  <b>]</b>"; 

     html += "</center>\n";

     html += "     </td>\n";
     html += "     </tr>\n";
     html += "     <tr>\n";
     html += "     <td>\n";
     html += "     <b>" + tr("This is what's happening") + ":</b>\n";
     html += "     </td>\n";
     html += "     </tr>\n";
     html += "     <tr>\n";
     html += "     <td>\n";

     html += output;

     html += "     </td>\n";
     html += "     </tr>\n";
     html += "    </table>\n";
     html += "  </td>\n";
     html += "  <td class=\"twitter_desc\">\n";
     html += "    <table class=\"desc_table\">\n";
     html += "     <tr><td>\n";
     html += "          <b>" + tr("Name") + ":</b> " + name + "<br/>\n";
     html += "          <b>" + tr("Description") + ":</b> " + description + "<br/>\n";
     html += "          <b>" + tr("Website") + ":</b> " +  website + "<br/>\n";
     html += "          <b>" + tr("Followers") + ":</b> " + followers + "<br/>\n";
     html += "     </td></tr>\n";
     html += "     </table>\n";
     html += "    <table class=\"twitter_slang\">\n";
     html += "     <tr><td>\n";
     html += "         <p class=\"twitter_slang_td\">\n";
     html += "         &nbsp;<br/>\n";
     html += "          <b><a href=\"" + k->url.simplified() + "\">" + k->reference + "</a></b><br/>\n";
     html += "          <b>" + k->word + ":</b> " + k->meaning + "<br/>\n";
     html += "         </p>";
     html += "     </td></tr>\n";
     html += "     </table>\n";
     html += "  </td>\n";
     html += "  </tr>\n";
     html += "</table>\n";
     html += "</body>\n";
     html += "</html>";

     QString twitterPath = QDir::homePath() + "/." + QCoreApplication::applicationName() + "/twitter.html";
     QFile file(twitterPath);
     file.open(QIODevice::WriteOnly);

     QByteArray data = html.toUtf8();
     file.write(data, qstrlen(data));
     file.close();

     // FIXME: This class doesn't close its network request correctly / refactoring required  
     // delete k->reply;
     // delete k->manager;

     emit pageReady();
}

void KTwitter::loadTwitterMeaning()
{
    QDomDocument doc;
    QString twitterFile = DATA_DIR + "twitter.xml";
    QFile file(twitterFile);

    if (!file.open(QIODevice::ReadOnly))
        return;

    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "word")
                   k->word = e.text();
               else if (e.tagName() == "ref")
                   k->reference = e.text();
               else if (e.tagName() == "url")
                   k->url = e.text();
               else if (e.tagName() == "meaning")
                   k->meaning = e.text();
           }
           n = n.nextSibling();
    }

}
