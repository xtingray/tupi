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

#ifndef KWIZARD_H
#define KWIZARD_H

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>

#include "kvhbox.h"
#include "tglobal.h"

class TWizardPage;

/**
 * @author David Cuadrado
*/

class K_GUI_EXPORT TWizard : public QDialog
{
    Q_OBJECT

    public:
        TWizard(QWidget *parent = 0);
        ~TWizard();
        TWizardPage *addPage(TWizardPage *page);
        void showPage(int index);
        void showPage(TWizardPage *page);

    private slots:
        void back();
        void next();
        void pageCompleted();
        void finish();

    private:
        QStackedWidget m_history;
        QPushButton *m_cancelButton;
        QPushButton *m_backButton;
        QPushButton *m_nextButton;
        QPushButton *m_finishButton;
        QHBoxLayout *m_buttonLayout;
        QVBoxLayout *m_mainLayout;
};

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

class TWizardPage : public KVHBox
{
    Q_OBJECT

    public:
        TWizardPage(const QString &title, QWidget *parent = 0);
        virtual ~TWizardPage();

        virtual bool isComplete() const = 0;
        virtual void reset() = 0;

        void setPixmap(const QPixmap &px);
        void setWidget(QWidget *w);

    public slots:
        virtual void aboutToNextPage() {}
        virtual void aboutToBackPage() {}
        virtual void aboutToFinish() {};

    private:
        QFrame *m_container;
        QGridLayout *m_layout;
        QLabel *m_image;

    signals:
        void completed();
};

#endif
