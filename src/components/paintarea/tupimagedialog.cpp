/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupimagedialog.h"
#include "tapplicationproperties.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tdebug.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

struct TupImageDialog::Private
{
    QLineEdit *titleEdit;
    QLineEdit *topicEdit;
    QTextEdit *descText;
};

TupImageDialog::TupImageDialog(QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);
    setWindowTitle(tr("Image Properties"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/animation_mode.png")));

    QLabel *titleLabel = new QLabel(tr("Title"));
    k->titleEdit = new QLineEdit(tr("My Picture"));
    connect(k->titleEdit, SIGNAL(textChanged(const QString &)), this, SLOT(resetTitleColor(const QString &)));
    titleLabel->setBuddy(k->titleEdit);

    QLabel *topicLabel = new QLabel(tr("Topics"));
    k->topicEdit = new QLineEdit(tr("#topic1 #topic2 #topic3"));
    connect(k->topicEdit, SIGNAL(textChanged(const QString &)), this, SLOT(resetTopicColor(const QString &)));
    topicLabel->setBuddy(k->topicEdit);

    QLabel *descLabel = new QLabel(tr("Description"));

    k->descText = new QTextEdit;
    k->descText->setAcceptRichText(false);
    k->descText->setFixedHeight(80);
    k->descText->setText(tr("Just a little taste of my style :)"));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(k->titleEdit);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(topicLabel);
    middleLayout->addWidget(k->topicEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);

    QPushButton *cancel = new QPushButton(tr("Cancel"));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    buttonLayout->addWidget(cancel);

    QPushButton *ok = new QPushButton(tr("Post Image"));
    connect(ok, SIGNAL(clicked()), this, SLOT(checkData()));
    buttonLayout->addWidget(ok);
    ok->setDefault(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addLayout(middleLayout);
    layout->addWidget(descLabel);
    layout->addWidget(k->descText);
    setLayout(layout);
}

TupImageDialog::~TupImageDialog()
{
}

void TupImageDialog::checkData()
{
    if (k->titleEdit->text().length() == 0) {
        k->titleEdit->setText(tr("Set a title for the picture here!"));
        k->titleEdit->selectAll();
        return;
    }

    if (k->topicEdit->text().length() == 0) {
        k->topicEdit->setText(tr("Set a title for the picture here!"));
        k->topicEdit->selectAll();
        return;
    }

    QDialog::accept();
}

void TupImageDialog::resetTitleColor(const QString &)
{
    QPalette palette = k->titleEdit->palette();
    if (k->titleEdit->text().length() > 0 && k->titleEdit->text().compare(tr("Set a title for the picture here!")) != 0) 
        palette.setBrush(QPalette::Base, Qt::white);
    else 
        palette.setBrush(QPalette::Base, QColor(255, 140, 138));

    k->titleEdit->setPalette(palette);
}

void TupImageDialog::resetTopicColor(const QString &)
{
    QPalette palette = k->topicEdit->palette();
    if (k->topicEdit->text().length() > 0 && k->topicEdit->text().compare(tr("Set some topic tags for the picture here!")) != 0)
        palette.setBrush(QPalette::Base, Qt::white);
    else
        palette.setBrush(QPalette::Base, QColor(255, 140, 138));

    k->topicEdit->setPalette(palette);
}

QString TupImageDialog::imageTitle() const
{
     return k->titleEdit->text();
}

QString TupImageDialog::imageTopics() const
{
     return k->topicEdit->text();
}

QString TupImageDialog::imageDescription() const
{
     return k->descText->toPlainText();
}

