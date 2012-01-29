#!/bin/sh
# Author: krawek [at] toonka.com
# Version: 0.1
# This help to generate the ts file

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

