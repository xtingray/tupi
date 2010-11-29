#!/bin/bash

# Author: David Cuadrado <krawek (at) gmail (dot) com>
# Description: This script shows an state report about 
#              a local copy of the Tupi source code
# Use: % cd /path/to/tupi/code/ 
#      % ./tools/checkSvn.sh 

ADDED=""
MODIFIED=""
UNKNOWN=""

function showConflicts
{
	CONFLICTS=`svn st | grep ^C | awk '{ print $2 }'`
	echo '##############################'
	echo 'Conflicted files'
	echo $CONFLICTS | xargs -n1 echo -e "\t"
	
}

function showCombined
{
	COMBINED=`svn st | grep ^G | awk '{ print $2 }'`
	echo '##############################'
	echo 'Combined files'
	echo $COMBINED | xargs -n1 echo -e "\t"
}

function showAdded()
{
	ADDED=`svn st | grep ^A | awk '{ print $2 }'`
	echo '##############################'
	echo 'Added files'
	echo $ADDED | xargs -n1 echo -e "\t"
}

function showModified()
{
	MODIFIED=`svn st | grep ^M | awk '{ print "   " $2 }'`
	echo '##############################'
	echo 'Modified files'
	echo $MODIFIED | xargs -n1 echo -e "\t"
}

function showUnknown()
{
	UNKNOWN=`svn st | grep ^? | grep -vE 'moc_|Makefile|\.ui|\.moc|qmake_|\.a|\.so|\.pcs|\.obj' | awk '{print $2}'`
	echo '##############################'
	echo 'Unknown files'
	echo $UNKNOWN | xargs -n1 echo -e "\t"
}

echo "Checking for files in $(pwd)"

showConflicts
showCombined
showAdded
showModified
showUnknown

