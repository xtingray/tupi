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

#include "tapplication.h"
#include "tdebug.h"
#include "tactionmanager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>
#include <QLocale>

#include <QApplication>
#include <QMap>

TApplication::TApplication(int & argc, char ** argv) : QApplication(argc, argv)
{
    #ifdef K_DEBUG
           TINIT;
    #endif 

    QApplication::setEffectEnabled(Qt::UI_AnimateMenu, true);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, true);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu, true);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, true);

    parseArgs(argc, argv);

    m_actionManager = new TActionManager(this);
}

TApplication::~TApplication()
{
    TCONFIG->sync();

    #ifdef K_DEBUG
           TEND;
    #endif
}

void TApplication::applyTheme(const QString &file)
{
    m_themeManager.applyTheme(file);
}

void TApplication::applyTheme(const KThemeDocument &kd)
{
    m_themeManager.applyTheme(kd);
}

void TApplication::applyColors(ColorSchema cs)
{
    QPalette pal = QApplication::palette();

    switch (cs) {
            case DarkBlue:
               {
                  const QColor bg(32, 32, 82);
                  const QColor bgAlt(57, 64, 98);

                  pal.setColor(QPalette::Text, Qt::white);
                  pal.setColor(QPalette::Base, bg);
                  pal.setColor(QPalette::Foreground, 0xd7d7ef);
                  pal.setColor(QPalette::Background, bgAlt);

                  pal.setColor(QPalette::Button, bgAlt);
                  pal.setColor(QPalette::ButtonText, 0xd7d7ef);

                  pal.setColor(QPalette::Highlight, Qt::white);
                  pal.setColor(QPalette::HighlightedText, bg);
                  int h, s, v;
                  bgAlt.getHsv(&h, &s, &v);
                  pal.setColor(QPalette::Midlight, QColor(h, s/3, (int)(v * 1.2)));
               }
            break;
    }

    applyPalette(pal);
}

void TApplication::applyPalette(const QPalette &pal)
{
    setPalette(pal);

    QWidgetList list = allWidgets ();
    for (int i = 0; i < list.count(); i++) {
         QObject *o = list[i];
         if (o)
             static_cast<QWidget*>(o)->setPalette(pal);
    }
}

void TApplication::changeFont(const QFont &font)
{
    QApplication::setFont(font, "QWidget");
}

TConfig *TApplication::config(const QString &group)
{
    TConfig *config = TConfig::instance();
    config->beginGroup(group);

    return config;
}

void TApplication::parseArgs(int &argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
         QString opt = QString(argv[i]).simplified();
         if (opt.startsWith("--")) {
             QString arg = "";
             if (argv[i+1] && ! QString(argv[i+1]).startsWith("-"))
                 arg = QString(argv[i+1]).simplified();
             m_parseArgs.insert(opt.remove(0,2), arg);
         } else if (opt.startsWith("-")) {
                    QString arg = "";
                    if (argv[i+1] && ! QString(argv[i+1]).startsWith("-"))
                        arg = QString(argv[i+1]).simplified();

                    m_parseArgs.insert(opt.remove(0,1), arg);
         }
    }
}

bool TApplication::isArg(const QString &arg)
{
    return m_parseArgs.keys().contains(arg);
}

QString TApplication::getParam(const QString &arg)
{
    if (! m_parseArgs.contains(arg))
        return "";

    return m_parseArgs[arg];
}

bool TApplication::firstRun() 
{
    return false;
}

bool TApplication::insertGlobalAction(QAction *action, const QString& id)
{
    if (m_actionManager->insert(action, id, "global")) {
        action->setShortcutContext(Qt::ApplicationShortcut);
        return true;
    }

    return false;
}

void TApplication::removeGlobalAction(QAction *action)
{
    m_actionManager->remove(action, "global");
}

QAction *TApplication::findGlobalAction(const QString &id)
{
    return m_actionManager->find(id, "global");
}
