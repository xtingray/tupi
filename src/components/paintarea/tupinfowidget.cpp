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

#include "tupinfowidget.h"
#include "tdebug.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "timagebutton.h"
#include "tpushbutton.h"
#include "tupwebhunter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QHeaderView>

struct TupInfoWidget::Private
{
    QVBoxLayout *innerLayout;
    QString currentCurrency;
    QList<QString> currencyList;
    QTableWidget *table;
};

TupInfoWidget::TupInfoWidget(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->currencyList << "ARS";
    k->currencyList << "AUD";
    k->currencyList << "BRL";
    k->currencyList << "CAD";
    k->currencyList << "CNY";
    k->currencyList << "COP";
    k->currencyList << "EUR";
    k->currencyList << "MXN";
    k->currencyList << "NZD";
    k->currencyList << "NIO";
    k->currencyList << "NOK";
    k->currencyList << "PAB";
    k->currencyList << "PEN";
    k->currencyList << "PKR";
    k->currencyList << "SEK";
    k->currencyList << "TWD";
    k->currencyList << "USD";
    k->currencyList << "UYU";

    k->currentCurrency = k->currencyList.at(k->currencyList.indexOf("USD"));

    QBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(2);

    k->innerLayout = new QVBoxLayout;

    setUIContext();

    TImageButton *closeButton = new TImageButton(QPixmap(THEME_DIR + "icons/close_big.png"), 60, this, true);
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(closePanel()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    k->innerLayout->addWidget(new TSeparator());
    k->innerLayout->addWidget(buttonBox);

    layout->addLayout(k->innerLayout);
}

TupInfoWidget::~TupInfoWidget()
{
}

void TupInfoWidget::setUIContext()
{
    k->table = new QTableWidget(k->currencyList.count() - 1, 2);
    k->table->setSelectionMode(QAbstractItemView::SingleSelection);
    k->table->horizontalHeader()->hide();
    k->table->verticalHeader()->hide();

    k->table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    k->table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    k->table->setMaximumWidth(250);
    k->table->setMaximumHeight((k->currencyList.count() - 1)*30);
    k->table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    k->table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    QBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(5);

    QLabel *titleLabel = new QLabel(tr("Currency Converter"));
    titleLabel->setFont(QFont("Arial", 12, QFont::Bold, false));
    titleLabel->setAlignment(Qt::AlignHCenter);

    QLabel *currencyLabel = new QLabel(tr("Currency"));
    QComboBox *currency = new QComboBox();

    for (int i=0; i<k->currencyList.count(); i++)
         currency->addItem(tr("%1").arg(k->currencyList.at(i)));

    connect(currency, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentCurrency(int)));
    currency->setCurrentIndex(k->currencyList.indexOf("USD"));

    currencyLabel->setBuddy(currency);

    QHBoxLayout *currencyLayout = new QHBoxLayout;
    currencyLayout->addWidget(currencyLabel);
    currencyLayout->addWidget(currency);

    QLabel *sourceLabel = new QLabel(tr("Source"));
    QLineEdit *source = new QLineEdit("http://www.webservicex.net");

    updateMoneyTable();

    QHBoxLayout *sourceLayout = new QHBoxLayout;
    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(source);

    QLabel *checkerLabel = new QLabel(tr("Update data every"));
    QComboBox *minutesCombo = new QComboBox();

    minutesCombo->addItem(tr("1") + " " + tr("minute"));
    for (int i=5; i< 20; i+=5)
         minutesCombo->addItem(tr("%1").arg(i) + " " + tr("minutes"));

    QHBoxLayout *checkerLayout = new QHBoxLayout;
    checkerLayout->addWidget(checkerLabel);
    checkerLayout->addWidget(minutesCombo);

    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(currencyLayout);
    mainLayout->addLayout(sourceLayout);
    mainLayout->addWidget(k->table);
    mainLayout->addLayout(checkerLayout);

    k->innerLayout->addLayout(mainLayout);

    getDataFromNet();
}

void TupInfoWidget::getDataFromNet()
{
    for (int i=0; i<k->currencyList.count(); i++) {
         if (k->currencyList.at(i).compare(k->currentCurrency) != 0)
             getCurrencyConversionFromNet(k->currentCurrency, k->currencyList.at(i));
    }
}

void TupInfoWidget::getCurrencyConversionFromNet(const QString &money1, const QString &money2)
{
    QList<QString> params;
    params << money1;
    params << money2;
    QString url = "http://www.webservicex.net//currencyconvertor.asmx/ConversionRate?FromCurrency=1&ToCurrency=2";

    TupWebHunter *hunter = new TupWebHunter(TupWebHunter::Currency, url, params);
    hunter->start();
    connect(hunter, SIGNAL(dataReady(const QString &)), this, SLOT(updateObjectInformation(const QString &)));
}

void TupInfoWidget::updateObjectInformation(const QString &data)
{
    QStringList parts = data.split(":"); 
    QString currency = parts.at(0);
    QString value = parts.at(1);

    for (int i=0; i < k->table->rowCount(); i++) {
         QTableWidgetItem *item = k->table->item(i, 0);
         QString label = item->text();
         if (label.compare(currency) == 0) {
             QTableWidgetItem *label = new QTableWidgetItem("  " + tr("%1").arg(value));
             k->table->setItem(i, 1, label);
         }
    }
}

void TupInfoWidget::setCurrentCurrency(int index)
{
    k->currentCurrency = k->currencyList.at(index);
    updateMoneyTable();
}

void TupInfoWidget::updateMoneyTable()
{
    int j = 0;
    for (int i=0; i<k->currencyList.count(); i++) {
         if (k->currencyList.at(i).compare(k->currentCurrency) != 0) {
             QTableWidgetItem *label = new QTableWidgetItem(tr("%1").arg(k->currencyList.at(i)));
             label->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
             QTableWidgetItem *empty = new QTableWidgetItem(tr(""));
             empty->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

             k->table->setItem(j, 0, label);
             k->table->setItem(j, 1, empty);
             j++;
         }
    }

    getDataFromNet();
}
