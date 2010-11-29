/* Copyright (C) 2001-2005 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */

/* this header file contains some platform dependent stuff */

#ifndef PLATFORM_H
#define PLATFORM_H

/* in Windows, set all file i/o to binary */
#ifdef __MINGW32__
#include <fcntl.h>
unsigned int _CRT_fmode = _O_BINARY;
#endif

#endif /* PLATFORM_H */
