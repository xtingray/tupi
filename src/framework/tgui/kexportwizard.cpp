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
#include "kseparator.h"
#include "kdebug.h"
#include "kimageeffect.h"

// Qt
#include <QLabel>
#include <QBitmap>
#include <QDebug>

KExportWizard::KExportWizard(QWidget *parent) : QDialog(parent)
{
    m_cancelButton = new QPushButton(tr("Cancel"));
    m_backButton = new QPushButton(tr("Back"));
    m_nextButton = new QPushButton(tr("Next"));

    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(m_backButton, SIGNAL(clicked()), this, SLOT(back()));
    connect(m_nextButton, SIGNAL(clicked()), this, SLOT(next()));

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addStretch(1);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_backButton);
    m_buttonLayout->addWidget(m_nextButton);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(&m_history);
    m_mainLayout->addLayout(m_buttonLayout);
    setLayout(m_mainLayout);
}

KExportWizard::~KExportWizard()
{
}

KExportWizardPage *KExportWizard::addPage(KExportWizardPage *newPage)
{
    QString tag = newPage->getTag();
    newPage->setParent(&m_history);
    newPage->show();

    m_history.addWidget(newPage);

    if (m_history.count() == 1) { // First Page
        newPage->setFocus();
        m_backButton->setEnabled(false);
        m_nextButton->setDefault(true);
    } 

    m_nextButton->setEnabled(newPage->isComplete());
    connect(newPage, SIGNAL(completed()), this, SLOT(pageCompleted()));
    connect(newPage, SIGNAL(emptyField()), this, SLOT(disableButton()));

    if (tag.compare("PLUGIN")==0)
        connect(newPage, SIGNAL(formatSelected(int, const QString &)), this, SLOT(setFormat(int, const QString &)));

    if (tag.compare("EXPORT")==0 || tag.compare("IMAGES")==0) 
        connect(newPage, SIGNAL(isDone()), this, SLOT(closeDialog()));

    return newPage;
}

void KExportWizard::showPage(KExportWizardPage *page)
{
    m_history.setCurrentWidget(page);
}

void KExportWizard::showPage(int index)
{
    m_history.setCurrentIndex(index);
}

void KExportWizard::cancel()
{       
    close();
}

void KExportWizard::back()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(m_history.currentWidget());
    QString tag = current->getTag();

    if (current)
        current->aboutToBackPage();

    if (tag.compare("IMAGES")==0)
        m_history.setCurrentIndex(m_history.currentIndex()-2);
    else
        m_history.setCurrentIndex(m_history.currentIndex()-1);
    
    if (tag.compare("SCENE")==0)
        m_backButton->setEnabled(false);

    m_nextButton->setText("Next >");
    m_nextButton->setEnabled(true);

    if ((tag.compare("EXPORT")==0) && (!m_nextButton->isEnabled())) 
        m_nextButton->setEnabled(true);
}

void KExportWizard::next()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(m_history.currentWidget());
    QString tag = current->getTag();

    if (current)
        current->aboutToNextPage();

    //m_history.setCurrentIndex(m_history.currentIndex()+1);

    if (tag.compare("PLUGIN")==0) {
        m_backButton->setEnabled(true);
        m_history.setCurrentIndex(m_history.currentIndex()+1);
    }

    if (tag.compare("EXPORT")==0) {
        emit saveFile();
    }

    if (tag.compare("IMAGES")==0) {
        emit exportArray();
    }

    if (tag.compare("SCENE")==0)  {
        emit setFileName();
        if (format.compare(".jpg") == 0 || format.compare(".png") == 0)
            m_history.setCurrentIndex(m_history.currentIndex()+2);
        else
            m_history.setCurrentIndex(m_history.currentIndex()+1);
    } 

    pageCompleted();
}

void KExportWizard::pageCompleted()
{
    KExportWizardPage *current = qobject_cast<KExportWizardPage *>(m_history.currentWidget());
    QString tag = current->getTag();

    if (tag.compare("EXPORT")!=0 && tag.compare("IMAGES")!=0) {
        m_nextButton->setEnabled(current->isComplete());
    } else {
        m_nextButton->setText(tr("Save"));
        m_nextButton->setEnabled(true);
    }

    if (m_history.currentIndex() == 1)
        emit updateScenes();
}

void KExportWizard::disableButton() 
{
    if (m_nextButton->isEnabled())
        m_nextButton->setEnabled(false);
}

void KExportWizard::closeDialog()
{
    close();
}

void KExportWizard::setFormat(int code, const QString &extension)
{
    format = extension;
}

KExportWizardPage::KExportWizardPage(const QString &title, QWidget *parent) : KVHBox(parent)
{
    KVHBox *theTitle = new KVHBox(this, Qt::Vertical);
    new QLabel(title, theTitle);
    new KSeparator(theTitle);
    boxLayout()->setAlignment(theTitle, Qt::AlignTop);

    m_container = new QFrame(this);

    m_layout = new QGridLayout(m_container);

    m_image = new QLabel;
    m_layout->addWidget(m_image, 0, 0, Qt::AlignLeft);
    m_image->hide();

    new KSeparator(this);

    hide();
}

void KExportWizardPage::setPixmap(const QPixmap &px)
{
    m_image->setPixmap(px);
    m_image->show();
}

void KExportWizardPage::setWidget(QWidget *w)
{
    m_layout->addWidget(w, 0, 1);
}

void KExportWizardPage::setTag(const QString &label)
{
    tag = label;
}

const QString KExportWizardPage::getTag()
{
    return tag;
}

KExportWizardPage::~KExportWizardPage() {};
