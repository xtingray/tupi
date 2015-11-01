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

#ifndef TUPEXPORTMODULE_H
#define TUPEXPORTMODULE_H

#include "tglobal.h"
#include "tupexportwidget.h"
#include "tupexportinterface.h"
#include "tupexportwizard.h"
#include "tupproject.h"
#include "txyspinbox.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

class TUPI_EXPORT TupExportModule : public TupExportWizardPage
{
    Q_OBJECT

    public:
        TupExportModule(TupProject *project, TupExportWidget::OutputFormat output, QString title, const TupExportWidget *widget);
        ~TupExportModule();

        bool isComplete() const;
        void reset();
        void aboutToFinish();

    public slots:
        void exportIt();

    private slots:
        void updateState(const QString &text);
        void chooseFile();
        void chooseDirectory();
        void updateNameField();
        void enableTransparency(bool flag);

    private:
        QList<TupScene *> scenesToExport() const;

    public slots:
        void setScenesIndexes(const QList<int> &indexes);
        void setCurrentExporter(TupExportInterface *currentExporter);
        void setCurrentFormat(int currentFormat, const QString &extension);

    signals:
        void saveFile();
        void exportArray();
        void isDone();

    private:
        QList<int> m_indexes;
        TupExportInterface *m_currentExporter;
        TupExportInterface::Format m_currentFormat;
        TupExportWidget::OutputFormat output;

        TupProject *m_project;
        QLineEdit *m_filePath;
        QLineEdit *m_prefix;
        QSpinBox *m_fps;
        TXYSpinBox *m_size;
        QString filename;
        QString path;
        QString extension;
        QCheckBox *bgTransparency;
        bool transparency;
};

#endif
