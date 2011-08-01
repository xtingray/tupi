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

#include "ktnotice.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTime>

class KTNotice::Private
{
    public:
        Private()
        {
        }
        
        ~Private()
        {
            delete browser;
        }
        
        QTextEdit *browser;
};

KTNotice::KTNotice(QWidget *parent) : QWidget(parent), k(new Private())
{
    setAttribute(Qt::WA_DeleteOnClose);
    QGridLayout *layout = new QGridLayout(this);
    
    setWindowTitle(tr("Notices"));
    
    k->browser = new QTextEdit;
    k->browser->setReadOnly(true);
    layout->addWidget(k->browser, 0, 0);

    /*    
    QHBoxLayout *box = new QHBoxLayout;
    
    k->lineEdit = new QLineEdit;
    box->addWidget(k->lineEdit);
    
    k->send = new QPushButton(tr("Send"));
    box->addWidget(k->send);
    
    layout->addLayout(box, 1, 0);

    connect(k->lineEdit, SIGNAL(returnPressed()), k->send, SLOT(animateClick()));
    connect(k->send, SIGNAL(clicked()), this, SLOT(sendMessage()));
    */
}

KTNotice::~KTNotice()
{
    delete k;
}

void KTNotice::addMessage(const QString &message)
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

    QString css = "font-size: 12px;";
    k->browser->append("<div style=\"" + css + "\">" + record + " * " + message + "</div>");
}

/*
void KTNotice::sendMessage()
{
    QString text = k->lineEdit->text();
    k->lineEdit->clear();

    if (!text.isEmpty())
        emit requestSendMessage(text);
}
*/
