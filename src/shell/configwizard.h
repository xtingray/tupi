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

#ifndef CONFIGWIZARD_H
#define CONFIGWIZARD_H

#include <QString>

#include "kwizard.h"

class CWFirstPage;
class CWSecondPage;

class QLineEdit;

/**
 * @if english
 * This class helps to config Tupi!
 * @endif
 * @author David Cuadrado
 */
class ConfigWizard : public KWizard
{
    Q_OBJECT
    public:
        /**
         * Crea el dialogo configuracion.
         */
        ConfigWizard();
        /**
         * destuctor
         */
        ~ConfigWizard();

        /**
         * Pone los directorios por defecto en el dialogo.
         * @param home 
         * @param repos 
         */
        void setInitialData(const QString &home, const QString &repos);

        /**
         * @return la ruta del directorio home
         */
        QString home() const;

        /**
         * @return la ruta del directorio repository
         */
        QString cache() const;

     private:
        CWFirstPage *m_firstPage;
        CWSecondPage *m_secondPage;

        QString kthome;
        QString ktrepos;
};

class CWFirstPage : public KWizardPage
{
    Q_OBJECT
    public:
        CWFirstPage(QWidget *parent = 0);
        ~CWFirstPage();

        bool isComplete() const { return true; };
        void reset() { };
};

class CWSecondPage : public KWizardPage
{
    Q_OBJECT
    public:
        CWSecondPage(QWidget *parent = 0);
        ~CWSecondPage();
        void setData(const QString &home, const QString &repos);
        QString home() const;
        QString repository() const;

        bool isComplete() const;
        void reset();

    private slots:
        void verify(const QString &);

    private:
        QLineEdit *m_kthome;
        QLineEdit *m_ktrepos;
};

#endif

