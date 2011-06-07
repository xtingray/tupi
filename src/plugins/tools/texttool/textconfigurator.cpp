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

#include "textconfigurator.h"
#include "kfontchooser.h"
#include "kspellhighlighter.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QFontDatabase>
#include <QCheckBox>

TextConfigurator::TextConfigurator(QWidget *parent) : QWidget(parent)
{
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(layout);

    m_fontChooser = new KFontChooser;
    layout->addWidget(m_fontChooser);

    m_text = new QTextEdit(this);
    layout->addWidget(m_text);

    m_isHtml = new QCheckBox(tr("Html"));
    layout->addWidget(m_isHtml);

    layout->addStretch(1);

    connect(m_fontChooser, SIGNAL(fontChanged()), this, SLOT(changeFont()));
    changeFont();

    new KSpellHighlighter(m_text->document());
}

TextConfigurator::~TextConfigurator()
{
}

QString TextConfigurator::text() const
{
    return m_text->toPlainText();
}

QFont TextConfigurator::textFont() const
{
    return font;
}

void TextConfigurator::changeFont()
{
    font = m_fontChooser->currentFont();

    // kFatal() << "TextConfigurator::changeFont() - Font Family: " << font.family();
    // kFatal() << "TextConfigurator::changeFont() - Font Style: " << font.style(); 
    // kFatal() << "TextConfigurator::changeFont() - Font Size: " << font.pointSize();

    m_text->setFont(font);

    adjustSize();
}

void TextConfigurator::setDocument(QTextDocument *doc)
{
    m_text->setDocument(doc);
}

bool TextConfigurator::isHtml() const
{
    return m_isHtml->isChecked();
}
