Note: Just for now, the Tupi project is only available for Unix systems. 
We hope to port it to many other platforms in the future. Meanwhile, the 
best way to try it is from any GNU/Linux distro (and FreeBSD or Unix-alike 
systems).

## Dependencies

The compilation process of any software usually requires a set of specific
tools previously installed in your system. Those additional components are 
called "dependencies".

In this case, before you start to compile Tupi, you must ensure to get 
installed the next list of dependencies in your computer:

* build-essential: All the C/C++ free compilers, libraries and utilities
  required to create a binary from any Unix-alike C/C++ source code (gcc,
  g++, cpp, make, etc)
* Qt: The full development toolkit used to create Tupi
* ruby: A script language used to do the configuration process previous 
  to the compilation
* libavcodec-dev: A subcomponent of the project libav, to deal with 
  video manipulation
* libavformat-dev: A subcomponent of the project libav, to deal with 
  video manipulation
* zlib-dev: A file-compression library
* quazip: A very handy API to deal with zip files
* qtmobility-dev: A useful Qt library to manage camera devices

For Ubuntu users (version 10.10) with some experience using the console, 
these are the commands they should run to install Tupi dependencies:

Open a terminal (from Aplications -> Accesories) and type:

    % sudo apt-get install build-essential    [Press enter]
    % sudo apt-get install ruby    [Press enter]
    % sudo apt-get install libavcodec-dev libavformat-dev    [Press enter]
    % sudo apt-get install libqt4-gui qt4-qmake qt4-dev-tools    [Press enter]
    % sudo apt-get install zlib1g-dev    [Press enter]
    % sudo apt-get install libquazip0-dev    [Press enter]
    % sudo apt-get install qtmobility-dev    [Press enter]

Note: If you are not using Ubuntu, check for the equivalent packages for your
Linux distro

## Get the source code and compile it

Get the Tupi source code from our Download section
(http://www.maefloresta.com/portal/downloads). Then, 
run these commands:

    % tar xvfz tupi-XXX.tar.gz    [Press enter]
    % cd tupi-XXX    [Press enter]
    % ./configure --prefix=/usr/local/tupi    [Press enter]
    % make    [Press enter]
    % sudo make install    [Press enter]

Note: Only for advanced users, the configure script provides many options 
useful to set specific installation paths. Try "./configure --help" to get 
more information about it.

## Animation time!

Execute:
    /usr/local/tupi/bin/tupi &    [Press enter]

Or look for the launcher from Applications -> Graphics -> Tupi

And enjoy Tupi!

Note: Whether you find any error in this how-to or you want to share your own
installation notes with us, feel free to send us your documents.

Additional info: http://www.maefloresta.com

