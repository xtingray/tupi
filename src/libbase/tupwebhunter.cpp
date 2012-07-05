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

#include "tupwebhunter.h"
#include "tdebug.h"

QString TupWebHunter::CURRENCY_HOST = QString("http://www.webservicex.net//currencyconvertor.asmx/ConversionRate?FromCurrency=USD&ToCurrency=COP");
QString TupWebHunter::BROWSER_FINGERPRINT = QString("Tupi_Browser 1.0");

TupWebHunter::TupWebHunter()
{
}

void TupWebHunter::start()
{
    QString url = CURRENCY_HOST;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(closeRequest(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toAscii());

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

TupWebHunter::~TupWebHunter()
{
}

void TupWebHunter::closeRequest(QNetworkReply *reply)
{
    QByteArray array = reply->readAll();
    QString answer(array);

    answer = answer.mid(answer.indexOf("\n"), answer.length()).trimmed();

    QDomDocument doc;
    if (doc.setContent(answer)) {
        QDomElement root = doc.documentElement();
        if (!root.text().isNull())
            emit dataReady(root.text());
    }
}

void TupWebHunter::slotError(QNetworkReply::NetworkError error)
{
    emit dataReady(tr("Information Temporarily Unavailable"));

    switch (error) {
            case QNetworkReply::HostNotFoundError:
                 { 
                     tError() << "TupWebHunter::slotError() - Network Error: Host not found";
                 }
            break;
            case QNetworkReply::TimeoutError:
                 {
                     tError() << "TupWebHunter::slotError() - Network Error: Time out!";
                 }
            break;
            case QNetworkReply::ConnectionRefusedError:
                 {
                     tError() << "TupWebHunter::slotError() - Network Error: Connection Refused!";
                 }
            break;
            case QNetworkReply::ContentNotFoundError:
                 {
                     tError() << "TupWebHunter::slotError() - Network Error: Content not found!";
                 }
            break;
            case QNetworkReply::UnknownNetworkError:
            default:
                 {
                     tError() << "TupWebHunter::slotError() - Network Error: Unknown Network error!";
                 }
            break;
    }
}
