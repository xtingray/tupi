#!/bin/bash

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
#   the Free Software Foundation; either version 2 of the License, or     #
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

# Description: This bash script gets the source code of
# Tupi from its GIT repository and builds the
# Debian/Ubuntu packages
#
# Dependencies:
#
# * You need to have a valid OpenPGP key installed
# in your account. More info at:
# https://help.ubuntu.com/community/GnuPrivacyGuardHowto
#
# * You need to install all the Tupi dependencies in
# your system. More info at:
# http://www.maefloresta.com/portal/howCompileTupi

VERSION="5"
SOURCE="/home/username"

echo ""
echo "Setting a cool environment in a temp directory..."
echo ""
cd $SOURCE/tmp

if [ -d "debian" ]; then
	rm -rf debian	
fi 

mkdir debian 
cd debian

echo "Getting the source code from Gitorious or a local place..."
echo ""
cp -r $SOURCE/tupi/sources/tupi .
cd tupi
make distclean
\rm -rf .git
cd ..
echo ""
echo "Making the tarballs required..."
echo ""
mv tupi tupi-0.1
tar cfz tupi_0.1.orig.tar.gz tupi-0.1

echo ""
echo "Building Tupi package..."
echo ""
cd tupi-0.1
./configure
debuild

echo ""
echo "Building Tupi source package..."
echo ""

rm -rf debian/tupi
make distclean
./configure
debuild -S

cd ..
ls -al

echo ""
echo "Debian packaging process: successful! :)"
echo ""
echo "Debian/Ubuntu package of Tupi available from: packages/tupi"
md5sum tupi_0.1-0ubuntu${VERSION}_i386.deb
