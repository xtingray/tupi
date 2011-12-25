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

#include "kexportwizard.h"
#include "kvhbox.h"
#include "tseparator.h"
#include "tdebug.h"
#include "kimageeffect.h"

// Qt
#include <QLabel>
#include <QBitmap>
#include <QDebug>

struct KExportWizard::Private
{
    QStackedWidget *history;
    QPushButton *cancelButton;
    QPushButton *backButton;
    QPushButton *nextButton;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
    QString format;
};

KExportWizard::KExportWizard(QWidget *parent) : QDialog(parent), k(new Private)
{
    k->cancelButton = new QPushButton(tr("Cancel"));
    k->backButton = new QPushButton(tr("Back"));
    k->nextButton = new QPushButton(tr("Next"));

    connect(k->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(k->backButton, SIGNAL(clicked()), this, SLOT(back()));
    connect(k->nextButton, SIGNAL(clicked()), this, SLOT(next()));

    k->buttonLayout = new QHBoxLayout;
    k->buttonLayout->addStretch(1);
    k->buttonLayout->addWidget(k->cancelButton);
    k->buttonLayout->addWidget(k->backButton);
    k->buttonLayout->addWidget(k->nextButton);

    k->history = new QStackedWidget;

    k->mainLayout = new QVBoxLayout;
    k->mainLayout->addWidget(k->history);
    k->mainLayout->addLayout(k->buttonLayout);
    setLayout(k->mainLayout);
}

KExportWizard::~KExportWizard()
{
}

KExportWizardPage *KExportWizard::addPage(KExportWizardPage *newPage)
{
    QString tag = newPage->getTag();
    newPage->setParent(k->history);
    newPage->show();

    k->history->addWidget(newPage);

    if (k->history->count() == 1) { // First Page
        newPage->setFocus();
        k->backButton->setEnabled(false);
        k->nextButton->setDefault(true);
    } 

    k->nextButton->setEnabled(newPage->isComplete());
    connect(newPage, SIGNAL(completed()), this, SLOT(pageCompleted()));
    connect(newPage, SIGNAL(emptyField()), this, SLOT(disableButton()));

    if (tag.compare("PLUGIN") == 0)
        connect(newPage, SIGNAL(formatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));

    if (tag.compare("EXPORT") == 0 || tag.compare("IMAGES") == 0 || tag.compare("PROPERTIES") == 0) 
        connect(newPage, SIGNAL(isDone()), this, SLOT(closeDialog()));

    return newPage;
}

void KExportWizard::showPage(KExportWizardPage *page)
{
    k->history->setCurrentWidget(page);
}

void KExportWizard::showPage(int index)
{
    k->history->setCurrentIndex(index);
}

void KExportWizard::cancel()
{       
    close();
}

void KExportWizard::back()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(k->history->currentWidget());
    QString tag = current->getTag();

    if (current)
        current->aboutToBackPage();

    if (tag.compare("IMAGES") == 0)
        k->history->setCurrentIndex(k->history->currentIndex()-2);
    else
        k->history->setCurrentIndex(k->history->currentIndex()-1);
    
    if (tag.compare("SCENE") == 0 || tag.compare("PROPERTIES") == 0)
        k->backButton->setEnabled(false);

    k->nextButton->setEnabled(true);

    if (tag.compare("EXPORT") == 0 || tag.compare("IMAGES") == 0 || tag.compare("PROPERTIES") == 0) 
        k->nextButton->setText(tr("Next"));
}

void KExportWizard::next()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(k->history->currentWidget());
    QString tag = current->getTag();

    if (current)
        current->aboutToNextPage();

    if (tag.compare("PLUGIN") == 0) {
        k->backButton->setEnabled(true);
        k->history->setCurrentIndex(k->history->currentIndex()+1);
    }

    if (tag.compare("EXPORT") == 0)
        emit saveFile();

    if (tag.compare("IMAGES") == 0)
        emit exportArray();

    if (tag.compare("PROPERTIES") == 0)
        emit saveVideoToServer();

    if (tag.compare("SCENE") == 0)  {
        k->nextButton->setText(tr("Export")); 
        k->backButton->setEnabled(true);
        emit setFileName();

        if (k->format.compare(".jpg") == 0 || k->format.compare(".png") == 0)
            k->history->setCurrentIndex(k->history->currentIndex()+2);
        else
            k->history->setCurrentIndex(k->history->currentIndex()+1);
    } 

    pageCompleted();
}

void KExportWizard::pageCompleted()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(k->history->currentWidget());
    QString tag = current->getTag();

    if (tag.compare("SCENE") == 0 || tag.compare("PLUGIN")== 0) {
        k->nextButton->setEnabled(current->isComplete());
    } else {
        if (tag.compare("IMAGES") == 0 || tag.compare("EXPORT"))
            k->nextButton->setText(tr("Export"));
        if (tag.compare("PROPERTIES") == 0)
            k->nextButton->setText(tr("Post"));
        k->nextButton->setEnabled(true);
    }

    if (k->history->currentIndex() == 1)
        emit updateScenes();
}

void KExportWizard::disableButton() 
{
    if (k->nextButton->isEnabled())
        k->nextButton->setEnabled(false);
}

void KExportWizard::closeDialog()
{
    close();
}

void KExportWizard::setFormat(int code, const QString &extension)
{
    k->format = extension;
}

struct KExportWizardPage::Private
{
    QFrame *container;
    QGridLayout *layout;
    QLabel *image;
    QString tag;
};

KExportWizardPage::KExportWizardPage(const QString &title, QWidget *parent) : KVHBox(parent), k(new Private)
{
    KVHBox *boxTitle = new KVHBox(this, Qt::Vertical);
    new QLabel(title, boxTitle);
    new TSeparator(boxTitle);
    boxLayout()->setAlignment(boxTitle, Qt::AlignTop);

    k->container = new QFrame(this);
    k->layout = new QGridLayout(k->container);

    k->image = new QLabel;
    k->layout->addWidget(k->image, 0, 0, Qt::AlignLeft);
    k->image->hide();

    new TSeparator(this);

    hide();
}

void KExportWizardPage::setPixmap(const QPixmap &px)
{
    k->image->setPixmap(px);
    k->image->show();
}

void KExportWizardPage::setWidget(QWidget *w)
{
    k->layout->addWidget(w, 0, 1);
}

void KExportWizardPage::setTag(const QString &label)
{
    k->tag = label;
}

const QString KExportWizardPage::getTag()
{
    return k->tag;
}

KExportWizardPage::~KExportWizardPage() {};
