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

#include "tupchat.h"
#include "tdebug.h"

#include <QTextBrowser>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTime>
#include <QList>
#include <QKeyEvent>

class TupChat::Private
{
    public:
        Private()
        {
        }
        
        ~Private()
        {
            delete lineEdit;
            delete browser;
        }
        
        QLineEdit *lineEdit;
        QTextBrowser *browser;
        QPushButton *send;
        QList<QString> *lines;
        int cursorDown; 
        int cursorUp;
};

TupChat::TupChat(QWidget *parent) : QWidget(parent), k(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *layout = new QGridLayout(this);
    
    setWindowTitle("chat");
    
    k->browser = new QTextBrowser;
    k->browser->setOpenExternalLinks(true);
    layout->addWidget(k->browser, 0, 0);
    
    QHBoxLayout *box = new QHBoxLayout;
    
    k->lineEdit = new QLineEdit;
    box->addWidget(k->lineEdit);
    
    k->send = new QPushButton(tr("Send"));
    box->addWidget(k->send);
    
    layout->addLayout(box, 1, 0);

    k->lines = new QList<QString>();
    k->lines->append("");
    k->cursorDown = 0;
    k->cursorUp = 0;
    
    connect(k->lineEdit, SIGNAL(returnPressed()), k->send, SLOT(animateClick()));
    connect(k->send, SIGNAL(clicked()), this, SLOT(sendMessage()));
}

TupChat::~TupChat()
{
    delete k;
}

void TupChat::addMessage(const QString &from, const QString &message)
{
    QTime time = QTime::currentTime();
    int hours = time.hour();
    QString h = QString::number(hours);
    if (hours < 10)
        h = "0" + h;

    int minutes = time.minute();
    QString min = QString::number(minutes);
    if (minutes < 10)
        min = "0" + min;

    QString record = "[" + h + ":" + min + "]";

    QString msg = formatMessage(message);

    QString css = "font-size: 12px;"; 
    k->browser->append("<div style=\"" + css + "\">" + record + " " + QString("<b>%1:</b>").arg(from) + " " + msg + "</div>");
}

QString TupChat::formatMessage(const QString &msg)
{
    QString base = msg;
    QString result = base;
    int index = base.indexOf("http://");

    if (index != -1) {
        bool flag = true;
        int count = 1;
        while (flag) {
               count++;
               int space = base.indexOf(" ", index);
               QString url;
               if (space != -1) {
                   url = base.mid(index, space);
                   result.insert(space, "</a>"); 
               } else {
                   space = base.length();
                   url = base.mid(index, space);
                   result.insert(space, "</a>");
               }

               QString last = url.right(1);
               while (!last[0].isLetter()) {
                      url.chop(1);
                      last = url.right(1);
               }

               result.insert(index, "<a href=\"" + url + "\">");
               space = result.lastIndexOf("</a>") + 4;
               index = result.indexOf("http://", space);
               if (index == -1)
                   flag = false; 
               else
                   base = result;
        }
    } 

    return result;
}

void TupChat::sendMessage()
{
    QString text = k->lineEdit->text();
    k->lineEdit->clear();

    if (!text.isEmpty()) {
        if (text.toLower().indexOf("<") != -1 && text.toLower().indexOf(">") != -1) {
            QString css = "font-size: 10px;";
            k->browser->append("<div style=\"" + css + "\">" + "<b>" + tr("Error:") + "</b> " 
                               + tr("Invalid Message. It won't be sent. Please, don't use HTML tags") 
                               + "</div>");
        } else {
            k->lines->append(text);
            k->cursorUp = k->lines->size()-1;
            k->cursorDown = k->cursorUp;
            emit requestSendMessage(text);
        }
    }
}

void TupChat::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
            case (Qt::Key_Up):
                  if ((k->cursorDown >= 0) && (k->cursorDown <= k->lines->size()-1)) {
                       QString text = k->lines->at(k->cursorDown);
                       if (!text.isNull()) {
                           k->lineEdit->setText(text);
                           if (k->cursorDown == 0) {
                               k->cursorUp = k->cursorDown + 1;
                               k->cursorDown = k->lines->size() - 1;
                           } else { 
                               if (k->cursorDown == k->lines->size() - 1)
                                   k->cursorUp = 0;
                               else
                                   k->cursorUp = k->cursorDown + 1;
                               k->cursorDown--;
                           }
                      }
                  }
            break;
            case (Qt::Key_Down):
                  if ((k->cursorUp >= 0) && (k->cursorUp <= k->lines->size()-1)) {
                       QString text = k->lines->at(k->cursorUp);
                       if (!text.isNull()) {
                           k->lineEdit->setText(text);
                           if (k->cursorUp == k->lines->size() - 1) {
                               k->cursorUp = 0;
                               k->cursorDown = k->lines->size() - 2;
                           } else {
                               if (k->cursorUp == 0)
                                   k->cursorDown = k->lines->size() - 1;
                               else
                                   k->cursorDown = k->cursorUp - 1;
                               k->cursorUp++;
                           }
                       }
                  }
            break;
    }
}
