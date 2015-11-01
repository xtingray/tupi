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

#ifndef TUPEXPORTWIZARD_H
#define TUPEXPORTWIZARD_H

#include "tglobal.h"
#include "tvhbox.h"
#include "toolview.h"
#include "tseparator.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QBitmap>

class TupExportWizardPage;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupExportWizard : public QDialog
{
    Q_OBJECT

    public:
        TupExportWizard(QWidget *parent = 0);
        ~TupExportWizard();
        TupExportWizardPage *addPage(TupExportWizardPage *page);
        void showPage(int index);
        void showPage(TupExportWizardPage *page);

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
        void setAnimationFileName();
        void setAnimatedImageFileName();
        void setImagesArrayFileName();

        void isDone();

    private:
        struct Private;
        Private *const k;
};

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

class TUPI_EXPORT TupExportWizardPage : public TVHBox
{
    Q_OBJECT

    public:
        TupExportWizardPage(const QString &title, QWidget *parent = 0);
        virtual ~TupExportWizardPage();
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
        // void formatSelected(int format, const QString &extension);

    private:
        struct Private;
        Private *const k;
};

#endif
