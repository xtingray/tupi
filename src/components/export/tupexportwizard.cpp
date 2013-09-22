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

#include "tupexportwizard.h"
#include "tvhbox.h"
#include "tseparator.h"
#include "tdebug.h"

// Qt
#include <QLabel>
#include <QBitmap>
#include <QDebug>

struct TupExportWizard::Private
{
    QStackedWidget *history;
    QPushButton *cancelButton;
    QPushButton *backButton;
    QPushButton *nextButton;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
    QString format;
    int formatCode;
};

TupExportWizard::TupExportWizard(QWidget *parent) : QDialog(parent), k(new Private)
{
    setModal(true);

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

TupExportWizard::~TupExportWizard()
{
}

TupExportWizardPage *TupExportWizard::addPage(TupExportWizardPage *newPage)
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

    // k->nextButton->setEnabled(newPage->isComplete());

    connect(newPage, SIGNAL(completed()), this, SLOT(pageCompleted()));
    connect(newPage, SIGNAL(emptyField()), this, SLOT(disableButton()));

    if (tag.compare("PLUGIN") == 0) {
        // connect(newPage, SIGNAL(formatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));
        connect(newPage, SIGNAL(animatedImageFormatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));
        connect(newPage, SIGNAL(imagesArrayFormatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));
        connect(newPage, SIGNAL(animationFormatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));
    }

    if (tag.compare("ANIMATION") == 0 || tag.compare("IMAGES_ARRAY") == 0 || tag.compare("ANIMATED_IMAGE") == 0 || tag.compare("PROPERTIES") == 0) 
        connect(newPage, SIGNAL(isDone()), this, SLOT(closeDialog()));

    return newPage;
}

void TupExportWizard::showPage(TupExportWizardPage *page)
{
    k->history->setCurrentWidget(page);
}

void TupExportWizard::showPage(int index)
{
    k->history->setCurrentIndex(index);
}

void TupExportWizard::cancel()
{       
    close();
}

void TupExportWizard::back()
{
    TupExportWizardPage *current = qobject_cast<TupExportWizardPage *>(k->history->currentWidget());
    QString tag = current->getTag();

    if (current)
        current->aboutToBackPage();

    if (tag.compare("ANIMATED_IMAGE") == 0) {
        k->history->setCurrentIndex(k->history->currentIndex()-3);
    } else if (tag.compare("IMAGES_ARRAY") == 0) {
               k->history->setCurrentIndex(k->history->currentIndex()-2);
    } else if (tag.compare("ANIMATION") == 0 || tag.compare("SCENE") == 0) {
               k->history->setCurrentIndex(k->history->currentIndex()-1);
    }

    if (tag.compare("SCENE") == 0 || tag.compare("PROPERTIES") == 0)
        k->backButton->setEnabled(false);

    k->nextButton->setEnabled(true);

    if (tag.compare("ANIMATION") == 0 || tag.compare("IMAGES_ARRAY") == 0 || tag.compare("ANIMATED_IMAGE") == 0 || tag.compare("PROPERTIES") == 0) 
        k->nextButton->setText(tr("Next"));
}

void TupExportWizard::next()
{
    TupExportWizardPage *current = qobject_cast<TupExportWizardPage *>(k->history->currentWidget());

    if (current)
        current->aboutToNextPage();

    QString tag = current->getTag();

    if (tag.compare("PLUGIN") == 0) {
        k->backButton->setEnabled(true);
        k->history->setCurrentIndex(k->history->currentIndex()+1);
    }

    if (tag.compare("ANIMATION") == 0)
        emit exportAnimation();

    if (tag.compare("ANIMATED_IMAGE") == 0)
        emit exportAnimatedImage();

    if (tag.compare("IMAGES_ARRAY") == 0)
        emit exportImagesArray();

    if (tag.compare("PROPERTIES") == 0)
        emit saveVideoToServer();

    if (tag.compare("SCENE") == 0)  {
        k->nextButton->setText(tr("Export")); 
        k->backButton->setEnabled(true);

        tError() << "TupExportWizard::next() - formatCode: " << k->formatCode;

        if (k->formatCode == 4096) { // ANIMATED PNG
            tError() << "TupExportWizard::next() - Calling signal for Animated PNG! - setAnimatedImageFileName()";
            emit setAnimatedImageFileName();
            k->history->setCurrentIndex(k->history->currentIndex()+3);
        } else if (k->format.compare(".jpg") == 0 || k->format.compare(".png") == 0) { // IMAGES ARRAY
                   tError() << "TupExportWizard::next() - Calling signal for Images Array! - setImagesArrayFileName()";
                   emit setImagesArrayFileName();
                   k->history->setCurrentIndex(k->history->currentIndex()+2);
        } else {
            tError() << "TupExportWizard::next() - Calling signal for Animation! - setAnimationFileName()";
            emit setAnimationFileName();
            k->history->setCurrentIndex(k->history->currentIndex()+1); // ANIMATION 
        }
    } 

    pageCompleted();
}

void TupExportWizard::pageCompleted()
{
    TupExportWizardPage *current = qobject_cast<TupExportWizardPage *>(k->history->currentWidget());
    QString tag = current->getTag();

    if (tag.compare("SCENE") == 0 || tag.compare("PLUGIN") == 0) {
        k->nextButton->setEnabled(current->isComplete());
    } else {
        if (tag.compare("IMAGES_ARRAY") == 0 || tag.compare("ANIMATION") == 0 || tag.compare("ANIMATED_IMAGE") == 0)
            k->nextButton->setText(tr("Export"));
        if (tag.compare("PROPERTIES") == 0)
            k->nextButton->setText(tr("Post"));
        k->nextButton->setEnabled(true);
    }

    if (k->history->currentIndex() == 1)
        emit updateScenes();
}

void TupExportWizard::disableButton() 
{
    if (k->nextButton->isEnabled())
        k->nextButton->setEnabled(false);
}

void TupExportWizard::closeDialog()
{
    close();
}

void TupExportWizard::setFormat(int code, const QString &extension)
{
    tError() << "TupExportWizard::setFormat() - Setting code: " << code;
    tError() << "TupExportWizard::setFormat() - Setting extension: " << extension;

    k->formatCode = code;
    k->format = extension;
}

struct TupExportWizardPage::Private
{
    QFrame *container;
    QGridLayout *layout;
    QLabel *image;
    QString tag;
};

TupExportWizardPage::TupExportWizardPage(const QString &title, QWidget *parent) : TVHBox(parent), k(new Private)
{
    TVHBox *boxTitle = new TVHBox(this, Qt::Vertical);
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

void TupExportWizardPage::setPixmap(const QPixmap &px)
{
    k->image->setPixmap(px);
    k->image->show();
}

void TupExportWizardPage::setWidget(QWidget *w)
{
    k->layout->addWidget(w, 0, 1);
}

void TupExportWizardPage::setTag(const QString &label)
{
    k->tag = label;
}

const QString TupExportWizardPage::getTag()
{
    return k->tag;
}

TupExportWizardPage::~TupExportWizardPage() {};
