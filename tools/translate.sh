#!/bin/sh

###########################################################################
#   Project TUPI: Magia 2D                                                #
#   Project Contact: info@maefloresta.com                                 #
#   Project Website: http://www.maefloresta.com                           #
#   Project Leader: Gustav Gonzalez <info@maefloresta.com>                #
#                                                                         #
#   Developers:                                                           #
#   2010:                                                                 #
#    Gustavo Gonzalez / xtingray                                          #
#                                                                         #
#   KTooN's versions:                                                     #
#                                                                         #
#   2006:                                                                 #
#    David Cuadrado                                                       #
#    Jorge Cuadrado                                                       #
#   2003:                                                                 #
#    Fernado Roldan                                                       #
#    Simena Dinas                                                         #
#                                                                         #
#   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       #
#   License:                                                              #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 3 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>. #
###########################################################################

if [ ! "$0" == "translate.sh" ] 
  then
     echo "Use: sh translate.sh [output.ts]"
     exit 0
fi

TUPI_DIR="`dirname $0`/.."
TS_FILE=/tmp/tupi_untranslated.ts
PRO_FILE=trans.pro

if [ $# -gt 0 ]
then
     TS_FILE=$1
fi

echo "The .ts file will be generated in " $TS_FILE
echo "TUPI_DIR: " $TUPI_DIR
echo "PRO_FILE: " $TRPRO

echo "PREFIX =" > $PRO_FILE
echo "ROOT =" >> $PRO_FILE
echo "" >> $PRO_FILE

HEADERS=$(find $TUPI_DIR -name "*.h" -printf "%p ")
SOURCES=$(find $TUPI_DIR -name "*.cpp" -printf "%p ")

echo "HEADERS = $HEADERS" >> $PRO_FILE
echo "SOURCES = $SOURCES" >> $PRO_FILE

echo "TRANSLATIONS = $TS_FILE" >> $PRO_FILE

lupdate -noobsolete $PRO_FILE
rm -fR $PRO_FILE

LING=`which linguist`

if [ -x $LING -a $LING ]
then
	linguist $TS_FILE &
else
	echo "* Please install linguist"
fi

cd $TUPI_DIR

echo "File generated!"

