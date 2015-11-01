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

#include "tconfig.h"

class TConfig::Private
{
    public:
        QDomDocument document;
        QString path;

        bool firstTime;
        bool isOk;
        QDir configDirectory;

        QHash<QString, QDomElement> groups;
        QDomElement currentGroup;
        QString lastGroup;
};

TConfig* TConfig::m_instance = 0;

TConfig::TConfig() : QObject(), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TConfig()]";
        #else
            TINIT;
        #endif
    #endif

    QString base = QDir::homePath() + QDir::separator();

	/*
    #ifdef Q_OS_WIN32
            k->configDirectory.setPath(base + QCoreApplication::applicationName());
    #else
            k->configDirectory.setPath(base + "." + QCoreApplication::applicationName());
    #endif
	*/
	
	k->configDirectory.setPath(base + "." + QCoreApplication::applicationName());

    if (!k->configDirectory.exists()) {
        k->firstTime = true;
        #ifdef K_DEBUG
            QString msg = "TConfig::TConfig() - Config file doesn't exist. Creating path: " + k->configDirectory.path();
            #ifdef Q_OS_WIN32
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

        if (!k->configDirectory.mkdir(k->configDirectory.path())) {
            #ifdef K_DEBUG
                QString msg = "TConfig::TConfig() - Fatal Error: Can't create path -> " + k->configDirectory.path();
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    } else {
        k->firstTime = false;
    }

    k->path = k->configDirectory.path() + QDir::separator() + QCoreApplication::applicationName().toLower() + ".cfg";
    init();
}

TConfig::~TConfig()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TConfig()]";
        #else
            TEND;
        #endif
    #endif

    if (m_instance) 
        delete m_instance;
}

TConfig *TConfig::instance()
{
    if (! m_instance)
        m_instance = new TConfig;

    return m_instance;
}

void TConfig::init()
{
    QFile config(k->path);
    k->isOk = false;

    if (config.exists()) {
        QString errorMsg = "";
        int errorLine = 0;
        int errorColumn = 0;

        k->isOk = k->document.setContent(&config, &errorMsg, &errorLine, &errorColumn);

        if (!k->isOk) {
            #ifdef K_DEBUG
                QString msg1 = "TConfig::init() - Fatal Error: Configuration file is corrupted - Line: " + QString::number(errorLine) + " - Column: " + QString::number(errorColumn);
                QString msg2 = "TConfig::init() - Message: " + errorMsg;
                #ifdef Q_OS_WIN32
                    qDebug() << msg1;
                    qDebug() << msg2;
                #else
                    tError() << msg1;
                    tError() << msg2;
                #endif
            #endif
        }

        config.close();
   }

   if (!k->isOk) {
       QDomProcessingInstruction header = k->document.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
       k->document.appendChild(header);

       QDomElement root = k->document.createElement("Config");
       k->document.appendChild(root);
   }
}

bool TConfig::firstTime()
{
    return k->firstTime;
}

bool TConfig::isOk()
{
    return k->isOk;
}

QDomDocument TConfig::document()
{
    return k->document;
}

void TConfig::sync()
{
    QFile f(k->path);

    if (f.open(QIODevice::WriteOnly)) {
        QTextStream st(&f);
        st << k->document.toString() << endl;
        k->isOk = true;
        f.close();
    } else {
        k->isOk = false;
    }

    init();
}

void TConfig::beginGroup(const QString & prefix)
{
    // QString stripped = Qt::escape(prefix);
    QString stripped = QString(prefix).toHtmlEscaped();

    stripped.replace(' ', "_");
    stripped.replace('\n', "");

    k->lastGroup = k->currentGroup.tagName();

    if (k->groups.contains(stripped)) {
        k->currentGroup = k->groups[stripped];
    } else {
        k->currentGroup = find(k->document.documentElement(), stripped);

        if (k->currentGroup.isNull()) {
            k->currentGroup = k->document.createElement(stripped);
            k->document.documentElement().appendChild(k->currentGroup);
        }
    }
}

void TConfig::endGroup()
{
    if (!k->lastGroup.isEmpty())
        beginGroup(k->lastGroup);
}

void TConfig::setValue(const QString & key, const QVariant & value)
{
    QDomElement element = find(k->currentGroup, key);

    if (!element.isNull()) {
        if (value.canConvert(QVariant::StringList)) {
            QStringList list = value.toStringList();
            element.setAttribute("value", list.join(";"));
        } else {
            element.setAttribute("value", value.toString());
        }
    } else {
        element = k->document.createElement(key);

        if (value.canConvert(QVariant::StringList)) {
            QStringList list = value.toStringList();
            element.setAttribute("value", list.join(";"));
        } else {
            element.setAttribute("value", value.toString());
        }

        k->currentGroup.appendChild(element);
    }
}

QVariant TConfig::value(const QString & key, const QVariant & defaultValue) const
{
   QDomElement element = find(k->currentGroup, key); // Current group or root?
	
   if (element.isNull())
       return defaultValue;

   QVariant v = element.attribute("value");

   if (v.toString() == "false") {
       return false;
   } else if (v.toString() == "true") {
              return true;
   }

   return v;
}

QDomElement TConfig::find(const QDomElement &element, const QString &key) const 
{
   QDomElement recent;
   QDomNode n = element.firstChild();

   while (!n.isNull()) {
          QDomElement e = n.toElement();
          if (!e.isNull()) {
              if (e.tagName() == key) {
                  recent = e;
                  break;
              }
          }
          n = n.nextSibling();
   }

   return recent;
}

QString TConfig::currentGroup()
{
    return k->lastGroup;
}
