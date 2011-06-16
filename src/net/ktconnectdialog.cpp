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

#include "ktconnectdialog.h"
#include "ktnetprojectmanagerparams.h"
#include "tconfig.h"
#include "kformfactory.h"
#include "tosd.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDialogButtonBox>

struct KTConnectDialog::Private
{
    public:
        Private() {}
        ~Private()
        {
            delete login;
            delete password;
            delete server;
            delete port;
        }
        
        QLineEdit *login;
        QLineEdit *password;
        QLineEdit *server;
        QSpinBox *port;
        
        QCheckBox *storePassword;
};

KTConnectDialog::KTConnectDialog(QWidget *parent) : QDialog(parent), k(new Private())
{
    setWindowTitle(tr("Connection Dialog"));
    k->login = new QLineEdit;
    k->password = new QLineEdit;
    k->password->setEchoMode(QLineEdit::Password);
    
    k->server = new QLineEdit;
    k->port = new QSpinBox;
    k->port->setMinimum(1);
    k->port->setMaximum(65000);
    
    QGridLayout *layout = KFormFactory::makeGrid(QStringList() << tr("Login") << tr("Password") << tr("Server") << tr("Port"), QWidgetList() << k->login << k->password << k->server << k->port);
    
    k->storePassword = new QCheckBox(tr("Store password"));
    layout->addWidget(k->storePassword, 5, 1);
    
    QDialogButtonBox *box = new QDialogButtonBox;
    
    QPushButton *ok = box->addButton(QDialogButtonBox::Ok);
    connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    QPushButton *cancel = box->addButton(QDialogButtonBox::Cancel);
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    mainLayout->addLayout(layout);
    mainLayout->addWidget(box);
    
    setLayout(mainLayout);
    
    loadSettings();
}

KTConnectDialog::~KTConnectDialog()
{
    saveSettings();
}

void KTConnectDialog::setServer(const QString &server)
{
    k->server->setText(server);
}

void KTConnectDialog::setPort(int port)
{
    k->port->setValue(port);
}

QString KTConnectDialog::login() const
{
    return k->login->text();
}

QString KTConnectDialog::password() const
{
    return k->password->text();
}

QString KTConnectDialog::server() const
{
    return k->server->text();
}

int KTConnectDialog::port() const
{
    return k->port->value();
}

void KTConnectDialog::loadSettings()
{
    TCONFIG->beginGroup("Network");
    k->server->setText(TCONFIG->value("server", "localhost").toString());
    k->port->setValue(TCONFIG->value("port", 6502).toInt());
    k->login->setText(TCONFIG->value("login", "").toString());
    k->password->setText(TCONFIG->value("password", "").toString());
    
    k->storePassword->setChecked(TCONFIG->value("storePassword").toInt());
}

void KTConnectDialog::saveSettings()
{
    TCONFIG->beginGroup("Network");
    
    TCONFIG->setValue("server", k->server->text());
    TCONFIG->setValue("port", k->port->value());
    TCONFIG->setValue("login", k->login->text());
    
    if (k->storePassword->isChecked())
        TCONFIG->setValue("password", k->password->text());
    else 
        TCONFIG->setValue("password", "");
    
    TCONFIG->setValue("storePassword", k->storePassword->isChecked() ? 1 : 0);
    TCONFIG->sync();
}

void KTConnectDialog::accept()
{
    if (k->password->text().isEmpty()) {
        TOsd::self()->display(tr("Error"), tr("Please, fill in your password"), TOsd::Error);
        return;
    }

    QDialog::accept();    
}
