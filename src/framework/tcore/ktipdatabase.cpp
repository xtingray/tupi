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

#include "ktipdatabase.h"

#include "talgorithm.h"

#include <QDomDocument>
#include <QFile>

#include "tdebug.h"

struct KTipDatabase::Private
{
    QList<KTip> tips;
    int currentTipIndex;
};

KTipDatabase::KTipDatabase(const QString &file, QWidget *parent) : QWidget(parent), k(new Private)
{
    loadTips(file);
    
    if (!k->tips.isEmpty())
        k->currentTipIndex = TAlgorithm::random() % k->tips.count();
}

KTipDatabase::~KTipDatabase()
{
    delete k;
}

KTip KTipDatabase::tip() const
{
    if (k->currentTipIndex >= 0 && k->currentTipIndex < k->tips.count())
        return k->tips[k->currentTipIndex];

    return KTip();
}

void KTipDatabase::nextTip()
{
    if (k->tips.isEmpty())
        return ;

    k->currentTipIndex += 1;

    if (k->currentTipIndex >= (int) k->tips.count())
        k->currentTipIndex = 0;
}

void KTipDatabase::prevTip()
{
    if (k->tips.isEmpty())
        return ;

    k->currentTipIndex -= 1;

    if (k->currentTipIndex < 0)
        k->currentTipIndex = k->tips.count() - 1;
}

void KTipDatabase::loadTips(const QString &filePath)
{
    QDomDocument doc;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly))
        return;
    
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();

    while(!n.isNull()) {

        QDomElement e = n.toElement();

        if(!e.isNull()) {
            if (e.tagName() == "tip") {
                int index = random() % 3;
                KTip tip;
                tip.text = "<html>\n";
                tip.text += "<head>\n";
                tip.text += "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=utf-8\">\n";
                tip.text += "<link rel=\"stylesheet\" type=\"text/css\" href=\"" + QString::fromLocal8Bit(::getenv("TUPI_SHARE")) + "/data/help/css/tupi.css\" />\n";

                tip.text += "</head>\n";
                tip.text += "<body class=\"tip_background0" + QString::number(index) + "\">\n";
                tip.text += e.text();
                tip.text += "\n</body>\n";
                tip.text += "</html>";
                k->tips << tip;
            }
        }
        n = n.nextSibling();

    }
}


