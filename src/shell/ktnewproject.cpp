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

#include "ktnewproject.h"
#include "ktnetprojectmanagerparams.h"

#include "kformfactory.h"
#include "tconfig.h"
#include "tapplication.h"
#include "tosd.h"
#include "tdebug.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QColorDialog>
#include <QStyleOptionButton>

//SQA: Add a field to define the project description 

struct KTNewProject::Private
{
    QLineEdit *projectName;
    QLineEdit *authorName;
    QColor color;
    QPushButton *colorButton;
    QSpinBox *fps;

    KXYSpinBox *size;
    bool useNetwork;

    QGroupBox *netOptions;

    QLineEdit *server;
    QSpinBox *port;
    QLineEdit *login;
    QLineEdit *password;
};

KTNewProject::KTNewProject(QWidget *parent) : KTabDialog(parent), k(new Private)
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
    k->authorName->setText(QString::fromLocal8Bit(::getenv("USER")));
    layout->addWidget(k->authorName, 1, 1);

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

    k->size = new KXYSpinBox(tr("Dimension"), infoContainer);
    k->size->setMaximum(5000);
    k->size->setX(520);
    k->size->setY(380);

    QWidget *panel = new QWidget;
    QVBoxLayout *sizeLayout = new QVBoxLayout(panel);
    sizeLayout->addWidget(k->size);

    layout->addWidget(panel, 3, 0);
    layout->addWidget(renderAndFps, 3, 1);

    addTab(infoContainer, tr("Project info"));

    QFrame *netContainer = new QFrame();
    QBoxLayout *netLayout = new QBoxLayout(QBoxLayout::TopToBottom, netContainer);
    netLayout->setAlignment(Qt::AlignHCenter);

    QCheckBox *activeNetOptions = new QCheckBox(tr("Create a network project"));
    connect(activeNetOptions, SIGNAL(toggled(bool)), this, SLOT(enableNetOptions(bool)));

    QBoxLayout *labelLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    labelLayout->setAlignment(Qt::AlignHCenter);
    labelLayout->addWidget(activeNetOptions);

    netLayout->addLayout(labelLayout);

    setupNetOptions();

    netLayout->addWidget(k->netOptions);

    addTab(netContainer, tr("Network"));

    // SQA: Only for devel issues
    // enableTab(1, false);

    enableNetOptions(false);
}

KTNewProject::~KTNewProject()
{
    if (k->useNetwork) {
        TConfig *config = kApp->config("Network");
        config->setValue("server", k->server->text());
        config->setValue("port", k->port->value());
        config->setValue("login", k->login->text());
        config->setValue("password", k->password->text());
    }

    delete k;
}

void KTNewProject::setupNetOptions()
{
    k->netOptions = new QGroupBox(tr("Network"));
    QVBoxLayout *layout = new QVBoxLayout(k->netOptions);

    k->server = new QLineEdit;
    k->port = new QSpinBox;
    k->port->setMinimum(1024);
    k->port->setMaximum(65000);

    k->login = new QLineEdit;
    k->password = new QLineEdit;

    TConfig *config = kApp->config("Network");

    k->server->setText(config->value("server", "localhost").toString());
    k->port->setValue(config->value("port", 5000).toInt());

    k->login->setText(config->value("login", "").toString());
    k->password->setText(config->value("password", "").toString());

    k->password->setEchoMode(QLineEdit::Password);

    layout->addLayout(KFormFactory::makeGrid(QStringList() << tr("Login") << tr("Password") << tr("Server") << tr("Port"), QWidgetList() << k->login << k->password << k->server << k->port));
}

KTProjectManagerParams *KTNewProject::parameters()
{
    if (k->useNetwork) {
        KTNetProjectManagerParams *params = new KTNetProjectManagerParams;
        params->setProjectName(k->projectName->text());
        params->setAuthor(k->authorName->text());
        const QSize size(k->size->x(),k->size->y());
        params->setDimension(size);
        params->setFPS(k->fps->value());
        // Network settings
        params->setServer(k->server->text());
        params->setPort(k->port->value());
        params->setLogin(k->login->text());
        params->setPassword(k->password->text());

        return params;
    }

    KTProjectManagerParams *params = new KTProjectManagerParams;
    params->setProjectName(k->projectName->text());
    params->setAuthor(k->authorName->text());
    params->setBgColor(k->color);
    const QSize size(k->size->x(),k->size->y());
    params->setDimension(size);
    params->setFPS(k->fps->value());

    return params;
}

bool KTNewProject::useNetwork() const
{
    return k->useNetwork;
}

void KTNewProject::ok()
{
    if (k->projectName->text().isEmpty()) {
        TOsd::self()->display(tr("Error"), tr("Please fill the project name field"), TOsd::Error);
        return;
    }

    KTabDialog::ok();
}

void KTNewProject::enableNetOptions(bool isEnabled)
{
    k->useNetwork = isEnabled;

    k->server->setEnabled(isEnabled);
    k->port->setEnabled(isEnabled);
    k->login->setEnabled(isEnabled);
    k->password->setEnabled(isEnabled);
}

void KTNewProject::focusProjectLabel() 
{
    k->projectName->setFocus();
    k->projectName->selectAll();
}

void KTNewProject::setBgColor()
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
