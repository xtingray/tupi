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

#include "kformvalidator.h"
#include "tdebug.h"

#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QtDebug>

KFormValidator::KFormValidator(QWidget *form) : m_parent(form)
{
}

KFormValidator::~KFormValidator()
{
}

void KFormValidator::setForm(QWidget *form)
{
    m_parent = form;
}

bool KFormValidator::validatesNumerically(bool real)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 if (real)
                     line->setValidator(new QDoubleValidator(line));
                 else 
                     line->setValidator(new QIntValidator(line));

                 ok = true;
             }
    }
    
    return ok;
}

bool KFormValidator::validatesRange(int i, int e)
{
    bool ok = false;
    
    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 line->setValidator(new QIntValidator(i, e, line));
                 ok = true;
             } else if (QSpinBox *spin = qobject_cast<QSpinBox *>(child)) {
                        spin->setMaximum(e);
                        spin->setMinimum(i);
                        ok = true;
             }
    }
    
    return ok;
}

bool KFormValidator::validatesRegExp(const QString &regexp)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 line->setValidator(new QRegExpValidator(QRegExp(regexp), line));
                 ok = true;
             }
    }
    
    return ok;
}

bool KFormValidator::validatesNumericallyOf(bool real, const QString &name)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (child->objectName() == name) {
                 if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                     ok = true;
                     if (real)
                         line->setValidator(new QDoubleValidator(line));
                     else
                         line->setValidator(new QIntValidator(line));
                 }
             }
    }

    return ok;
}

bool KFormValidator::validatesRangeOf(int i, int e, const QString &name)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (child->objectName() == name) {
                 if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                     ok = true;
                     line->setValidator(new QIntValidator(i, e, line));
                 } else if (QSpinBox *spin = qobject_cast<QSpinBox *>(child)) {
                            spin->setMaximum(e);
                            spin->setMinimum(i);
                            ok = true;
                 }
             }
    }

    return ok;
}

bool KFormValidator::validatesRegExpOf(const QString &regexp, const QString &name)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (child->objectName() == name) {
                 if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                     line->setValidator(new QRegExpValidator(QRegExp(regexp), line));
                     ok = true;
                 }
             }
    }
    
    return ok;
}

bool KFormValidator::validatesLength(int max)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 line->setMaxLength(max);
                 ok = true;
             }
    }

    return ok;
}

bool KFormValidator::validatesLengthOf(int max, const QString &name)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (child->objectName() == name) {
                 if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                     line->setMaxLength(max);
                     ok = true;
                 }

                 break;
             }
    }

    return ok;
}

bool KFormValidator::validatesMask(const QString &mask)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 line->setInputMask(mask);
                 ok = true;
             }
    }

    return ok;
}

bool KFormValidator::validatesMaskOf(const QString &mask, const QString &name)
{
    bool ok = false;

    foreach (QObject *child, m_parent->children()) {
             if (child->objectName() == name) {
                 if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                     line->setInputMask(mask);
                     ok = true;
                 }

                 break;
             }
    }

    return ok;
}

void KFormValidator::validatesMaskOf(const QString &mask, QLineEdit *line)
{
    line->setInputMask(mask);
    m_childs << line;
}

void KFormValidator::validatesLengthOf(int max, QLineEdit *line)
{
    line->setMaxLength(max);
    m_childs << line;
}

void KFormValidator::validatesNumericallyOf(bool real, QLineEdit *line)
{
    if (real)
        line->setValidator(new QDoubleValidator(line));
    else
        line->setValidator(new QIntValidator(line));

    m_childs << line;
}

void KFormValidator::validatesRangeOf(int i, int e, QLineEdit *line)
{
    line->setValidator(new QIntValidator(i, e, line));
    m_childs << line;
}

void KFormValidator::validatesRegExpOf(const QString &regexp, QLineEdit *line)
{
    line->setValidator(new QRegExpValidator(QRegExp(regexp), line));
    m_childs << line;
}


bool KFormValidator::validate()
{
    /* SQA: Check if this code is really required
    foreach (QLineEdit *child, m_childs) {
             if (!validate(child)) {
                 tFatal() << "KFormValidator::validate() - field is wrong";
                 return false;
             } else {
                 tFatal() << "KFormValidator::validate() - field is ok";
             }
    }
    */

    foreach (QObject *child, m_parent->children()) {
             if (QLineEdit *line = qobject_cast<QLineEdit *>(child)) {
                 if (!validate(line))
                     return false;
             }
    }
    
    return true;
}

bool KFormValidator::validate(QLineEdit *line)
{
    if (line->echoMode() == QLineEdit::Password)
        return true;

    if (line->text().isEmpty()) {
        QPalette pal = line->palette();
        pal.setBrush(QPalette::Base, QColor(255, 140, 138));
        line->setPalette(pal);
        return false;
    } 

    return true;

    /* SQA: Check why this code crashes all the time
    int pos = 0;
    QPalette pal = line->palette();
    QString word = QString(line->text());
    QValidator::State state = line->validator()->validate(word, pos);

    switch (state) {
            case QValidator::Acceptable:
            {
                 // pal.setBrush(QPalette::Base, Qt::green);
            }
            break;
            case QValidator::Intermediate:
            {
                 // pal.setBrush(QPalette::Base, Qt::yellow);
                 pal.setBrush(QPalette::Base, QColor(255, 225, 138));
                 ok = false;
            }
            break;
            case QValidator::Invalid:
            {
                 // pal.setBrush(QPalette::Base, Qt::red);
                 pal.setBrush(QPalette::Base, QColor(255, 140, 138));
                 ok = false;
            }
            break;
    }
    line->setPalette(pal);

    if (line->validator() == 0)
        ok = false;
    
    return ok;
    */
}
