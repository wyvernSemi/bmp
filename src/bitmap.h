//=============================================================
// bitmap.h                                  Date: 2003/04/14 
//                                                               
// Copyright (c) 2003-2024 Simon Southwell
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
//=============================================================

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "general.h"

// General bitmap definitions
#define FORMATHDRSIZE        0x0E
#define INFOHDRSIZE          0x28
#define HDRSIZE              (FORMATHDRSIZE+INFOHDRSIZE)

// TransformBmp monochrome unary flags
#define MONOALL              0
#define MONORED              1
#define MONOGREEN            2
#define MONOBLUE             4

#define TBMP_ERR_BADPARAM    1
#define TBMP_ERR_CONVERROR   2

// GetBitmap Error codes
#define GBMP_ERR_MEM         1
#define GBMP_ERR_EOF         2
#define GBMP_ERR_NOTBMP      3
#define GBMP_ERR_BADPLANES   4
#define GBMP_ERR_BADPIXELS   5
#define GBMP_ERR_BADCOMPRESS 6

// ConvertBmpTo24bit error codes
#define CBMP_ERR_MEM         1
#define CBMP_ERR_CONVERROR   2

#if __BYTE_ORDER == __LITTLE_ENDIAN

// No endian conversion needed for WIN32
#define SWPEND32(_x) (_x)
#define SWPEND16(_x) (_x)
#define HDRENDIAN

#else

// Endian conversion macros for big endian machines
#define SWPEND32(_x) ((((_x) & 0xff)<<24) | (((_x) & 0xff00)<<8) | (((_x) & 0xff0000)>>8) | (((_x) & 0xff000000)>>24))
#define SWPEND16(_x) ((((_x) & 0xff)<<8) | (((_x) & 0xff00)>>8))

// Endian conversion for header
#define HDRENDIAN(_hdr) { \
    (_hdr)->f.bfSize           = SWPEND32((_hdr)->f.bfSize);          \
    (_hdr)->f.bfOffBits        = SWPEND32((_hdr)->f.bfOffBits);       \
    (_hdr)->i.biSize           = SWPEND32((_hdr)->i.biSize);          \
    (_hdr)->i.biWidth          = SWPEND32((_hdr)->i.biWidth);         \
    (_hdr)->i.biHeight         = SWPEND32((_hdr)->i.biHeight);        \
    (_hdr)->i.biPlanes         = SWPEND16((_hdr)->i.biPlanes);        \
    (_hdr)->i.biBitCount       = SWPEND16((_hdr)->i.biBitCount);      \
    (_hdr)->i.biCompression    = SWPEND32((_hdr)->i.biCompression);   \
    (_hdr)->i.biSizeImage      = SWPEND32((_hdr)->i.biSizeImage);     \
    (_hdr)->i.biXPxlsPerMeter  = SWPEND32((_hdr)->i.biXPxlsPerMeter); \
    (_hdr)->i.biYPxlsPerMeter  = SWPEND32((_hdr)->i.biYPxlsPerMeter); \
    (_hdr)->i.biClrUsed        = SWPEND32((_hdr)->i.biClrUsed);       \
    (_hdr)->i.biClrImportant   = SWPEND32((_hdr)->i.biClrImportant);  \
 }
#endif

// Set packing of structures to 1 byte boundaries so that headers
// are aligned correctly
#if __BYTE_ORDER == __LITTLE_ENDIAN
#pragma pack (push, 1)                      
#else
#pragma pack (1)                      
#endif

// Bitmap format header structure
typedef struct {
    char     bfType[2];
    uint32_t bfSize;
    char     bfRes[4];
    uint32_t bfOffBits;

} bmfh_t, *pbmfh_t;

// Bitmap information table structure
typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPxlsPerMeter;
    uint32_t biYPxlsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} bmih_t, *pbmih_t;

// Combined format header/information table header
typedef struct {
    bmfh_t f;
    bmih_t i;
} bmhdr_t, *pbmhdr_t;

// Bitmap RGB Quad structure
typedef struct {
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
    uint8_t rgbReserved;
} rgbquad_t, *prgbquad_t;

// Revert to default packing
#if __BYTE_ORDER == __LITTLE_ENDIAN
#pragma pack (pop)                      
#else
#pragma pack ()                      
#endif

// Control structure for TransformBmp()
typedef struct {
    uint32_t clip;                      // Clip the bitmap
    uint32_t reverse;                   // Reverse colours when non-zero
    uint32_t brightness;                // Percentage brighteness---100% is normal, 0 is disable
    uint32_t contrast;                  // Percentage contrast---not yet implemented
    uint32_t grey;                      // Grey scale when non-zero
    uint32_t flipv;                     // Flip about vertical axis when non-zero
    uint32_t fliph;                     // Flip about horizontal axis when non-zero
    uint32_t mono;                      // Unary colour enable flags (bits 0 = Red, 1 = Green, 2 = Blue.
                                        //     All 0 disables monochromatic extraction
} trans_t, *ptrans_t;

typedef struct {
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;
} rect_t, *prect_t;

// Exported functions
extern int      GetBitmap         (FILE *, pbmhdr_t *, prgbquad_t *, unsigned char **, perrmsg_t);
extern uint32_t ConvertBmpTo24bit (unsigned char **, const pbmhdr_t, const prgbquad_t, const unsigned char *, perrmsg_t);
extern int      TransformBmp      (unsigned char *, const ptrans_t, perrmsg_t);
extern uint32_t ClipBitmap        (char*, const prect_t, uint32_t *);

#endif
