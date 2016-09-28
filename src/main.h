//=============================================================
// main.h                                    Date: 2003/04/14 
//                                                               
// Copyright (c) 2003 Simon Southwell
//                                                               
// This file is part of bmp.
//
// bmp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// bmp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with bmp. If not, see <http://www.gnu.org/licenses/>.
//
// $Id: main.h,v 1.3 2010-07-06 13:51:11 simon Exp $
// $Source: /home/simon/CVS/src/bitmaps/src/main.h,v $
//
//=============================================================

#ifndef _MAIN_H_
#define _MAIN_H_

#include "general.h"
#include "bitmap.h"

#define ERRBUFSIZE    1024
#define DEFAULTIFNAME "test.bmp"

#define USAGE \
fprintf(stderr, "\nUsage: bmp [-dhrgVH] [-b <val>] [-c <val>] [-m <colour>]\n"        \
             "           [-C <rect quad>] [-i <file>] [-o <file>]\n\n"                \
             "    -h Display this message\n"                                          \
             "    -d Increase debug output level (default no debug output)\n"         \
             "    -b Change image brightness by specified percent (100%% = normal)\n" \
             "    -c Change image contrast by specified percent (50%% = normal)\n"    \
             "    -g Change image to grey scale\n"                                    \
             "    -r Reverse image colours\n"                                         \
             "    -V Flip image about vertical axis\n"                                \
             "    -H Flip image about horizontal axis\n"                              \
             "    -m Extract monochromatic colour image:\n"                           \
             "         R[ed]\n"                                                       \
             "         G[reen]\n"                                                     \
             "         B[lue]\n"                                                      \
             "         Y[ellow]\n"                                                    \
             "         C[yan]\n"                                                      \
             "         M[agenta]\n"                                                   \
             "    -C Clip image to rectangle\n"                                       \
             "    -i Input filename (default %s)\n"                                   \
             "    -o Output filename (default no output)\n"                           \
             "\n", DEFAULTIFNAME)

#define DISPLAYTABLES(_bmp) {                                                                     \
        HDRENDIAN(_bmp);                                                                          \
        /* The Format table */                                                                    \
        fprintf(stderr, "Type               = %c%c\n",     _bmp->f.bfType[0], _bmp->f.bfType[1]); \
        fprintf(stderr, "File Size          = 0x%08x\n",   _bmp->f.bfSize);                       \
        fprintf(stderr, "Offset             = 0x%08x\n\n", _bmp->f.bfOffBits);                    \
        /* The information table */                                                               \
        fprintf(stderr, "Size               = 0x%08x\n",   _bmp->i.biSize);                       \
        fprintf(stderr, "Width              = 0x%08x\n",   _bmp->i.biWidth);                      \
        fprintf(stderr, "Height             = 0x%08x\n",   _bmp->i.biHeight);                     \
        fprintf(stderr, "Planes             = 0x%04x\n",   _bmp->i.biPlanes);                     \
        fprintf(stderr, "Bits per Pixel     = 0x%04x\n",   _bmp->i.biBitCount);                   \
        fprintf(stderr, "Compression        = 0x%08x\n",   _bmp->i.biCompression);                \
        fprintf(stderr, "Image Size         = 0x%08x\n",   _bmp->i.biSizeImage);                  \
        fprintf(stderr, "X Pixels per Meter = 0x%08x\n",   _bmp->i.biXPxlsPerMeter);              \
        fprintf(stderr, "Y Pixles per Meter = 0x%08x\n",   _bmp->i.biYPxlsPerMeter);              \
        fprintf(stderr, "Colour Used        = 0x%08x\n",   _bmp->i.biClrUsed);                    \
        fprintf(stderr, "Colour Important   = 0x%08x\n",   _bmp->i.biClrImportant);               \
        HDRENDIAN(_bmp);                                                                          \
}

// Imported objects
extern char * optarg;

#endif
