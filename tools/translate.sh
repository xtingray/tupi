#!/bin/sh
# Author: krawek [at] toonka.com
# Version: 0.1
# This help to generate the ts file

#if [ ! "$0" == "translate.sh" ]
#then
#	echo "Use: sh translate.sh [output.ts]"
#	exit 0
#fi

KTDIR="`dirname $0`/.."
KTTS=/tmp/tupi_untranslated.ts
TRPRO=trans.pro

if [ $# -gt 0 ]
then
	KTTS=$1
fi

echo "Your translate file will be generated in " $KTTS
echo "KTDIR: " $KTDIR
echo "TRPRO: " $TRPRO

echo "PREFIX =" > $TRPRO
echo "ROOT =" >> $TRPRO
echo "" >> $TRPRO

HEADERS=$(find $KTDIR -name "*.h" -printf "%p ")
SOURCES=$(find $KTDIR -name "*.cpp" -printf "%p ")

echo "HEADERS = $HEADERS" >> $TRPRO
echo "SOURCES = $SOURCES" >> $TRPRO

echo "TRANSLATIONS = $KTTS" >> $TRPRO

lupdate -noobsolete $TRPRO
rm -fR $TRPRO

LING=`which linguist`

if [ -x $LING -a $LING ]
then
	linguist $KTTS &
else
	echo "* Please install linguist"
fi

cd $KTDIR

echo "File generated!"

