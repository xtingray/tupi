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

#ifndef TEXPORTWIZARD_H
#define TEXPORTWIZARD_H

#include "tvhbox.h"
#include "tglobal.h"
#include "toolview.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>

class TExportWizardPage;

/**
 * @author David Cuadrado
*/

class T_GUI_EXPORT TExportWizard : public QDialog
{
    Q_OBJECT

    public:
        TExportWizard(QWidget *parent = 0);
        ~TExportWizard();
        TExportWizardPage *addPage(TExportWizardPage *page);
        void showPage(int index);
        void showPage(TExportWizardPage *page);

    private slots:
        void cancel();
        void back();
        void next();
        void pageCompleted();
        void disableButton();
        void closeDialog();
        void setFormat(int code, const QString &extension);

    signals:
        void cancelled();
        void updateScenes();
        void exportAnimation();
        void exportAnimatedImage();
        void saveVideoToServer();
        void exportImagesArray();
        void setFileName();
        void isDone();

    private:
        struct Private;
        Private *const k;
};

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

class TExportWizardPage : public TVHBox
{
    Q_OBJECT

    public:
        TExportWizardPage(const QString &title, QWidget *parent = 0);
        virtual ~TExportWizardPage();
        virtual bool isComplete() const = 0;
        virtual void reset() = 0;

        void setPixmap(const QPixmap &pixmap);
        void setWidget(QWidget *widget);
        void setTag(const QString &label);
        const QString getTag();

    public slots:
        virtual void aboutToNextPage() {}
        virtual void aboutToBackPage() {}

    signals:
        void completed();
        void emptyField();
        void formatSelected(int format, const QString &extension);

    private:
        struct Private;
        Private *const k;
};

#endif
