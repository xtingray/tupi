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

#ifndef KEXPORTWIZARD_H
#define KEXPORTWIZARD_H

#include "kvhbox.h"
#include "tglobal.h"
#include "ktoolview.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>

class KExportWizardPage;

/**
 * @author David Cuadrado
*/

class K_GUI_EXPORT KExportWizard : public QDialog
{
    Q_OBJECT

    public:
        KExportWizard(QWidget *parent = 0);
        ~KExportWizard();
        KExportWizardPage *addPage(KExportWizardPage *page);
        void showPage(int index);
        void showPage(KExportWizardPage *page);

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
        void saveFile();
        void saveVideoToServer();
        void exportArray();
        void setFileName();
        void isDone();

    private:
        struct Private;
        Private *const k;
};

#include <QFrame>
#include <QGridLayout>
#include <QLabel>

class KExportWizardPage : public KVHBox
{
    Q_OBJECT

    public:
        KExportWizardPage(const QString &title, QWidget *parent = 0);
        virtual ~KExportWizardPage();
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
