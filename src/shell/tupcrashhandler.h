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

#ifndef TUPCRASHHANDLER_H
#define TUPCRASHHANDLER_H

#ifdef K_DEBUG

#include "tapplicationproperties.h"
#include "tdebug.h"

extern "C"
{
#include <sys/types.h> //pid_t
#include <sys/wait.h>  //waitpid
#include <unistd.h>    //write, getpid
#include <stdio.h>
}

#include <QString>
#include <QMap>
#include <QColor>
#include <QPair>
#include <QImage>
#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QTemporaryFile>
#include <QProcess>
#include <QTranslator>
#include <QDesktopWidget>
#include <csignal>
#include <cstdio>

class TupCrashHandler;

class TupCrashHandler
{
    protected:
        TupCrashHandler();

    public:
        ~TupCrashHandler();
        void setConfig(const QString &filePath);
        void setTrapper(void (*trapper)(int));
        static TupCrashHandler *instance();
        static void init();

    public:
        void setProgram(const QString &prog);
        QString program() const;
        void setImagePath(const QString &imagePath);
        QString imagePath() const;

        QString title() const;
        QString message() const;
        QColor messageColor() const;
        QString closeButtonLabel() const;
        QString launchButtonLabel() const;
        QString defaultText() const;
        QString defaultImage() const;
        QString signalText(int signal);
        QString signalImage(int signal);
        bool containsSignalEntry(int signal);

    private:
        struct CrashHandlerConfig {
               QString title;
               QString message;
               QColor  messageColor;
               QString closeButton;
               QString launchButton;
               QString defaultText;
               QString defaultImage;
               QMap<int, QPair<QString, QString> > signalEntry;
        } m_config;

        static TupCrashHandler *m_instance; // Singleton
        QString m_program;
        QString m_imagePath;
        // bool m_verbose;
};

#define CHANDLER TupCrashHandler::instance()

#endif
#endif
