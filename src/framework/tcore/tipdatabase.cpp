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

#include "tipdatabase.h"

struct TipDatabase::Private
{
    QList<QString> tips;
    int currentTipIndex;

    QList<QString> videos;
    int currentVideoIndex;
};

TipDatabase::TipDatabase(const QString &videoPath, const QString &tipPath, QWidget *parent) : QWidget(parent), k(new Private)
{
    loadVideos(videoPath);
    if (!k->videos.isEmpty())
        k->currentVideoIndex = TAlgorithm::random() % k->videos.count();

    loadTips(tipPath);
    if (!k->tips.isEmpty())
        k->currentTipIndex = TAlgorithm::random() % k->tips.count();
}

TipDatabase::~TipDatabase()
{
    delete k;
}

QString TipDatabase::video() const
{
    if (k->currentVideoIndex >= 0 && k->currentVideoIndex < k->videos.count())
        return k->videos[k->currentVideoIndex];

    return "";
}

QString TipDatabase::tip() const
{
    if (k->currentTipIndex >= 0 && k->currentTipIndex < k->tips.count())
        return k->tips[k->currentTipIndex];

    return "";
}

void TipDatabase::nextVideo()
{
    if (k->videos.isEmpty())
        return;

    k->currentVideoIndex += 1;

    if (k->currentVideoIndex >= (int) k->videos.count())
        k->currentVideoIndex = 0;
}

void TipDatabase::nextTip()
{
    if (k->tips.isEmpty())
        return;

    k->currentTipIndex += 1;

    if (k->currentTipIndex >= (int) k->tips.count())
        k->currentTipIndex = 0;
}

void TipDatabase::previousVideo()
{
    if (k->videos.isEmpty())
        return;

    k->currentVideoIndex -= 1;

    if (k->currentVideoIndex < 0)
        k->currentVideoIndex = k->videos.count() - 1;
}

void TipDatabase::prevTip()
{
    if (k->tips.isEmpty())
        return;

    k->currentTipIndex -= 1;

    if (k->currentTipIndex < 0)
        k->currentTipIndex = k->tips.count() - 1;
}

void TipDatabase::loadVideos(const QString &videoPath)
{
    QDomDocument doc;
    QFile file(videoPath);
   
    if (!file.open(QIODevice::ReadOnly))
        return;
   
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
   
    QDomElement element = doc.documentElement();
    QDomNode node = element.firstChild();

    while(!node.isNull()) {
        QDomElement e1 = node.toElement();
        if (!e1.isNull()) {
            if (e1.tagName() == "video") {
                QDomNode n1 = e1.firstChild();
                QString url = "";
                QString title = "";
                while(!n1.isNull()) {
                    QDomElement e2 = n1.toElement();
                    if (!e2.isNull()) {
                        if (e2.tagName() == "url") {
                            url = e2.text();
                        }
                        if (e2.tagName() == "title") {
                            title = e2.text();
                            if (title.length() > 35)
                                title = title.left(35) + "...";
                        }
                    }
                    n1 = n1.nextSibling();
                }

                QString record;
                record = "<html>\n";
                record += "<head>\n";
                record += "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=utf-8\">\n";
                record += "<link rel=\"stylesheet\" type=\"text/css\" href=\"file:tupi.css\" />\n";
                record += "</head>\n";
                record += "<body class=\"tupi_background5\">\n";
                record += "<p><center><b>";
                record += tr("Did you already watch this video?");
                record += "</b></center></p>\n";
                record += "<p><center>";
                record += "<a href=\"https://youtube.com/watch?v=" + url + "\">";
                record += "<img src=\"file:youtube_player.png\"/>";
                record += "</a>";
                record += "</center></p>\n";
                record += "<p><center>";
                record += "<a href=\"https://youtube.com/watch?v=" + url + "\">";
                record += title;
                record += "</a>";
                record += "</center></p>\n";
                record += "\n</body>\n";
                record += "</html>";

                k->videos << record;
            }
        }
        node = node.nextSibling();
    }
}

void TipDatabase::loadTips(const QString &tipPath)
{
    QDomDocument doc;
    QFile file(tipPath);
    
    if (!file.open(QIODevice::ReadOnly))
        return;
    
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    
    QDomElement element = doc.documentElement();
    QDomNode n = element.firstChild();

    while(!n.isNull()) {
        QDomElement e = n.toElement();

        if(!e.isNull()) {
            if (e.tagName() == "tip") {
                int index = TAlgorithm::random() % 3;
                QString record;
                record = "<html>\n";
                record += "<head>\n";
                record += "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=utf-8\">\n";
                record += "<link rel=\"stylesheet\" type=\"text/css\" href=\"file:tupi.css\" />\n";
                record += "</head>\n";
                record += "<body class=\"tip_background0" + QString::number(index) + "\">\n";
                record += e.text();
                record += "\n</body>\n";
                record += "</html>";
                k->tips << record;
            }
        }
        n = n.nextSibling();

    }
}

int TipDatabase::videosCount()
{
    return k->videos.count();
}
