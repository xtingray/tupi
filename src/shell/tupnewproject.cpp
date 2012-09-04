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

#include "tupnewproject.h"
#include "tupnetprojectmanagerparams.h"

#include "tformfactory.h"
#include "tconfig.h"
#include "tapplication.h"
#include "tosd.h"
#include "tdebug.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QColorDialog>
#include <QStyleOptionButton>
#include <QComboBox>
#include <QLineEdit>

//SQA: Add a field to define the project description 

struct TupNewProject::Private
{
    QLineEdit *projectName;
    QLineEdit *authorName;
    QLineEdit *description;

    QColor color;
    QPushButton *colorButton;
    QSpinBox *fps;

    TXYSpinBox *size;
    bool useNetwork;

    QGroupBox *netOptions;
    QBoxLayout *netLayout;

    QLineEdit *server;
    QSpinBox *port;
    QLineEdit *login;
    QLineEdit *password;

    QCheckBox *storePassword;
};

TupNewProject::TupNewProject(QWidget *parent) : TabDialog(parent), k(new Private)
{
    setWindowIcon(QPixmap(THEME_DIR + "icons/new.png"));

    setWindowTitle(tr("Create a new project"));
    setModal(true);

    QFrame *infoContainer = new QFrame();
    QGridLayout *layout = new QGridLayout(infoContainer);

    QLabel *labelProjectName = new QLabel(tr("Project Name"), infoContainer);
    layout->addWidget(labelProjectName, 0, 0);

    k->projectName = new QLineEdit(infoContainer);
    k->projectName->setText(tr("my_project"));
    layout->addWidget(k->projectName, 0, 1);

    QLabel *labelAuthorName = new QLabel(tr("Author"), infoContainer);
    layout->addWidget(labelAuthorName, 1, 0);

    k->authorName = new QLineEdit(infoContainer);
    k->authorName->setText(tr("Your name"));
    layout->addWidget(k->authorName, 1, 1);

    QLabel *labelDescription = new QLabel(tr("Description"), infoContainer);
    layout->addWidget(labelDescription, 2, 0);

    k->description = new QLineEdit(infoContainer);
    k->description->setText(tr("Just for fun!"));
    layout->addWidget(k->description, 2, 1);

    QBoxLayout *presetsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    QLabel *presetsLabel = new QLabel(tr("Presets") + " ");

    QComboBox *presets = new QComboBox();
    presets->addItem(tr("Free format"));
    presets->addItem(tr("480p (PAL DV/DVD) - 25"));
    presets->addItem(tr("576p (PAL DV/DVD) - 25"));
    presets->addItem(tr("720p (HD) - 25"));
    presets->addItem(tr("1280p (Full HD) - 25"));
    connect(presets, SIGNAL(currentIndexChanged(int)), this, SLOT(setPresets(int)));

    presetsLayout->addWidget(presetsLabel);
    presetsLayout->addWidget(presets);
    layout->addLayout(presetsLayout, 3, 0, 1, 2, Qt::AlignCenter);

    QGroupBox *renderAndFps= new QGroupBox(tr("Options"));
	
    QBoxLayout *subLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    renderAndFps->setLayout(subLayout);

    k->color = QColor("#fff");
    k->colorButton = new QPushButton();
    k->colorButton->setText(tr("Background"));
    k->colorButton->setToolTip(tr("Click here to change background color"));
    k->colorButton->setPalette(QPalette(k->color));
    k->colorButton->setAutoFillBackground(true);

    connect(k->colorButton, SIGNAL(clicked()), this, SLOT(setBgColor()));
	
    QBoxLayout *fpsLayout = new QBoxLayout(QBoxLayout::LeftToRight);

    QLabel *label = new QLabel(tr("FPS"));
    k->fps = new QSpinBox();
    k->fps->setValue(24);

    fpsLayout->addWidget(label);
    fpsLayout->addWidget(k->fps);
    subLayout->addWidget(k->colorButton);
    subLayout->addLayout(fpsLayout);
    // subLayout->addSpacing(30);

    k->size = new TXYSpinBox(tr("Dimension"), infoContainer);
    k->size->setMinimum(50);
    k->size->setMaximum(15000);
    k->size->setX(520);
    k->size->setY(380);

    QWidget *panel = new QWidget;
    QVBoxLayout *sizeLayout = new QVBoxLayout(panel);
    sizeLayout->addWidget(k->size);
    // sizeLayout->addWidget(test);

    layout->addWidget(panel, 4, 0);
    layout->addWidget(renderAndFps, 4, 1);

    QCheckBox *activeNetOptions = new QCheckBox(tr("Tupitube project"));
    connect(activeNetOptions, SIGNAL(toggled(bool)), this, SLOT(enableNetOptions(bool)));

    layout->addWidget(activeNetOptions, 5, 0, 1, 2, Qt::AlignLeft);

    addTab(infoContainer, tr("Project info"));

    QFrame *netContainer = new QFrame();
    k->netLayout = new QBoxLayout(QBoxLayout::TopToBottom, netContainer);
    k->netLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    setupNetOptions();

    addTab(netContainer, tr("Network"));
    enableNetOptions(false);
}

TupNewProject::~TupNewProject()
{
    if (k->useNetwork) {
        TConfig *config = kApp->config("Network");
        config->setValue("Server", k->server->text());
        config->setValue("Port", k->port->value());
        config->setValue("Login", k->login->text());
        if (k->storePassword->isChecked()) {
            config->setValue("Password", k->password->text());
            config->setValue("StorePassword", "1");
        } else {
            config->setValue("Password", "");
            config->setValue("StorePassword", "0");
        }
    }

    delete k;
}

void TupNewProject::setupNetOptions()
{
    k->server = new QLineEdit;
    k->port = new QSpinBox;
    k->port->setMinimum(1024);
    k->port->setMaximum(65000);

    k->login = new QLineEdit;
    k->password = new QLineEdit;

    TConfig *config = kApp->config("Network");

    k->server->setText(config->value("Server", "tupitube.com").toString());
    k->port->setValue(config->value("Port", 5000).toInt());

    k->login->setText(config->value("Login", "").toString());
    k->password->setText(config->value("Password", "").toString());

    k->password->setEchoMode(QLineEdit::Password);

    QPlainTextEdit *text = new QPlainTextEdit;
    text->setMaximumHeight(70);
    text->appendPlainText(QString("This feature allows you to work with another artists") 
                           + QString(" around the Internet on the same project in real time."));
    text->setEnabled(false);
    k->netLayout->addWidget(text);

    k->netOptions = new QGroupBox(tr("Settings"));
    QVBoxLayout *layout = new QVBoxLayout(k->netOptions);
    layout->addLayout(TFormFactory::makeGrid(QStringList() << tr("Username") << tr("Password") << tr("Server") << tr("Port"), 
                         QWidgetList() << k->login << k->password << k->server << k->port));

    k->netLayout->addWidget(k->netOptions);

    k->storePassword = new QCheckBox(tr("Store password"));
    k->storePassword->setChecked(TCONFIG->value("StorePassword").toInt());
    k->netLayout->addWidget(k->storePassword);
}

TupProjectManagerParams *TupNewProject::parameters()
{
    if (k->useNetwork) {
        TupNetProjectManagerParams *params = new TupNetProjectManagerParams;
        params->setProjectName(k->projectName->text());
        params->setAuthor(k->authorName->text());
        params->setDescription(k->description->text());
        params->setBgColor(k->color);
        const QSize size(k->size->x(), k->size->y());
        params->setDimension(size);
        params->setFPS(k->fps->value());

        // Network settings
        params->setServer(k->server->text());
        params->setPort(k->port->value());
        params->setLogin(k->login->text());
        params->setPassword(k->password->text());

        return params;
    }

    TupProjectManagerParams *params = new TupProjectManagerParams;
    params->setProjectName(k->projectName->text());
    params->setAuthor(k->authorName->text());
    params->setDescription(k->description->text());
    params->setBgColor(k->color);
    const QSize size(k->size->x(), k->size->y());
    params->setDimension(size);
    params->setFPS(k->fps->value());

    return params;
}

bool TupNewProject::useNetwork() const
{
    return k->useNetwork;
}

void TupNewProject::ok()
{
    if (k->projectName->text().isEmpty()) {
        TOsd::self()->display(tr("Error"), tr("Please, set a name for the project"), TOsd::Error);
        return;
    }

    if (k->useNetwork) {
        if (k->login->text().isEmpty()) {
            TOsd::self()->display(tr("Error"), tr("Please, fill in your username"), TOsd::Error);
            return;
        }

        if (k->password->text().isEmpty()) {
            TOsd::self()->display(tr("Error"), tr("Please, fill in your password"), TOsd::Error);
            return;
        }

        if (k->server->text().isEmpty()) {
            TOsd::self()->display(tr("Error"), tr("Please, fill in the server name or IP"), TOsd::Error);
            return;
        }
    }

    TabDialog::ok();
}

void TupNewProject::enableNetOptions(bool isEnabled)
{
    k->useNetwork = isEnabled;
    enableTab(1, isEnabled);
}

void TupNewProject::focusProjectLabel() 
{
    k->projectName->setFocus();
    k->projectName->selectAll();
}

void TupNewProject::setBgColor()
{
     k->color = QColorDialog::getColor(Qt::white, this);

     if (k->color.isValid()) {
         k->colorButton->setText(k->color.name());
         k->colorButton->setPalette(QPalette(k->color));
         k->colorButton->setAutoFillBackground(true);
     } else {
         k->color = QColor("#fff");
         k->colorButton->setText(tr("White"));
     }

     k->colorButton->setPalette(QPalette(k->color));
     k->colorButton->setAutoFillBackground(true);
}

void TupNewProject::setPresets(int index)
{
    switch(index) {
           case FREE: 
           {
               k->size->setX(520);
               k->size->setY(380);
               k->fps->setValue(24);
               k->size->setEnabled(true);
               k->fps->setEnabled(true);
           }
           break;
           case FORMAT_480P:
           {
               k->size->setX(720);
               k->size->setY(480);
               k->fps->setValue(25);
               k->size->setEnabled(false);
               k->fps->setEnabled(false);
           }
           break;
           case FORMAT_576P:
           {
               k->size->setX(720);
               k->size->setY(576);
               k->fps->setValue(25);
               k->size->setEnabled(false);
               k->fps->setEnabled(false);
           }
           break;
           case FORMAT_720P:
           {
               k->size->setX(1280);
               k->size->setY(720);
               k->fps->setValue(25);
               k->size->setEnabled(false);
               k->fps->setEnabled(false);
           }
           break;
           case FORMAT_1280P:
           {
               k->size->setX(1920);
               k->size->setY(1280);
               k->fps->setValue(25);
               k->size->setEnabled(false);
               k->fps->setEnabled(false);
           }
           break;
    }
}

QString TupNewProject::login() const
{
    return k->login->text();
}
