//=============================================================
// bitmap.c                                  Date: 2003/04/14 
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
//
// Contains four library functions for bitmap manipulation:
//
//   GetBitmap()         : reads a bitmap file into internal structures
//   ConvertBmpTo24bit() : Convert 2, 4 or 8 to 24 bit bitmap
//   TransformBmp()      : Performs varoius 24 bit bitmap transformations
//   ClipBitmap()        : Clips bitmap to a defined input rectangle
//
//=============================================================

#include "bitmap.h"

//=================================================================
// GetBitmap()
//
// Reads in a bitmap file separating the header, RGB quad table
// (for non 24 bit bitmaps) and the data bytes. Pointers to the
// sections are returned in the supplied pointers 'bmp', 'r' and
// 'data'. The allocated memory for the sections is guaranteed to 
// be contiguous.
//
//=================================================================

int GetBitmap(FILE *fp, pbmhdr_t *bmp, prgbquad_t *r, unsigned char **data, perrmsg_t e)
{
    static const char *funcname = "GetBitmap()";

    unsigned char *buf, *tmp_buf;
    int byte;
    uint32_t i;

    *r    = NULL;
    *bmp  = NULL;
    *data = NULL;

    // Get enough space for a header
    if ((buf = malloc(HDRSIZE)) == NULL) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - unable to allocate memory.\n", funcname);
            e->errnum = GBMP_ERR_MEM;
        }
        return BADSTATUS;
    }

    // Read in header bytes
    for (i = 0; i < HDRSIZE; i++) {
        if ((byte = getc(fp)) == EOF) {
            if (e != NULL) {
                snprintf(e->errbuf, e->errsize, "***Error: %s - unexpected end of file reading header.\n", funcname);
                e->errnum = GBMP_ERR_EOF;
            }
            free(buf);
            return BADSTATUS;
        }
        buf[i] = (unsigned char)(byte & BYTEMASK);
    }

    // Cast to header structure
    *bmp = (pbmhdr_t) buf;

    // Header endian conversion for big endian machines. 
    HDRENDIAN(*bmp);

    // Reallocate the buffer so that the whole file will fit
    tmp_buf = buf;
    buf = realloc(tmp_buf, (*bmp)->f.bfSize);
    if (buf == NULL) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - unable to allocate memory.\n", funcname);
            e->errnum = GBMP_ERR_MEM;
        }
        free(tmp_buf);
        return BADSTATUS;
    }
    *bmp = (pbmhdr_t) buf;

    // Get rest of file---information table, RGB Quad table and data
    for (i = HDRSIZE; i < (*bmp)->f.bfSize; i++) {
        if ((byte = getc(fp)) == EOF) {
            if (e != NULL) {
                 snprintf(e->errbuf, e->errsize, "***Error: %s - unexpected end of file.\n", funcname);
                 e->errnum = GBMP_ERR_EOF;
            }
            free(buf);
            return BADSTATUS;
        }
        buf[i] = (unsigned char)(byte & BYTEMASK);
    }

    // If not a 24 bit bitmap, point to the colour table
    if ((*bmp)->i.biBitCount != 24) 
        // Cast the colour table to an RGB Quad structure array
        *r = (prgbquad_t) &buf[HDRSIZE];

    // Point to data
    *data = &buf[(*bmp)->f.bfOffBits];

    // Check it's a bitmap
    if ((*bmp)->f.bfType[0] != 'B' || (*bmp)->f.bfType[1] != 'M') {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - not a bitmap file.\n", funcname);
            e->errnum = GBMP_ERR_NOTBMP;
        }
        free(buf);
        return BADSTATUS;
    }

    // Check there's only one plane
    if ((*bmp)->i.biPlanes != 1) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - unsupported number of planes (%d).\n", 
                          funcname, (*bmp)->i.biPlanes);
            e->errnum = GBMP_ERR_BADPLANES;
        }
        free(buf);
        return BADSTATUS;
    }

    // Check valid bits per pixel
    if ((*bmp)->i.biBitCount != 1 && (*bmp)->i.biBitCount != 4 && 
        (*bmp)->i.biBitCount != 8 && (*bmp)->i.biBitCount != 24) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - invalid bits per pixel (%d).\n", 
                          funcname, (*bmp)->i.biBitCount);
            e->errnum = GBMP_ERR_BADPIXELS;
        }
        free(buf);
        return BADSTATUS;
    }

    // Check there's no compression
    if ((*bmp)->i.biCompression != 0) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - unsupported compressed format (%d).\n", 
                          funcname, (*bmp)->i.biCompression);
            e->errnum = GBMP_ERR_BADCOMPRESS;
        }
        free(buf);
        return BADSTATUS;
    }

    // Header endian put back before exit
    HDRENDIAN(*bmp);

    return GOODSTATUS;
}

//=================================================================
// ConvertBmpTo24bit()
//
// Convert lower order bitmap to a 24 bit bitmap, and place in
// allocated memory. Bitmap header pointed to by 'bmp',
// with quad table referenced by 'r' and data bytes by 'data'.
// On return, *newbmp set to point to image data, and return value
// set to image size.
//
//=================================================================

uint32_t ConvertBmpTo24bit(unsigned char **newbmp, const pbmhdr_t bmp, const prgbquad_t r, const unsigned char *data, 
                           perrmsg_t e)
{
    static const char *funcname = "ConvertBmpTo24bit()";

    // Local variables
    pbmhdr_t new_header;                                // Pointer to output header
    uint32_t i_rowlen, i_padrowlen, i_partialbits;      // Input bitmap parameters
    uint32_t i_pixelsperbyte, i_pixelsinbyte;           // Pixel counts
    uint32_t o_imgsize, o_rowlen, o_padrowlen;          // Output bitmap parameters
    unsigned char *p;                                   // Pointer to output buffer data area
    const unsigned char *this_row;                      // Pointer to input row data
    uint32_t i, j, k, idx, oidx;                        // Indexing

    // Header endian conversion on big endian machine
    HDRENDIAN(bmp);

    // If bitmap already 24 bits return an error (byte count of 0)
    if (bmp->i.biBitCount > BYTEWIDTH) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - attempt to convert a bitmap already in 24 bit format.\n", funcname);
            e->errnum = CBMP_ERR_CONVERROR;
        }
        return 0;
    }

    // Number of pixels in each (whole) byte. Either 1, 2 or 8.
    i_pixelsperbyte = BYTEWIDTH/bmp->i.biBitCount;

    // Number pixels in last byte. Zero indicates no partial byte.
    i_partialbits = bmp->i.biWidth % i_pixelsperbyte;                           

    // Calculate input row length in bytes, and as padded to 32 bits
    i_rowlen = bmp->i.biWidth/i_pixelsperbyte + (i_partialbits ? 1 : 0);
    i_padrowlen = 4 * ((i_rowlen+3)/4);

    // Calculate output row length in bytes, and as padded to 32 bits
    o_rowlen    = (bmp->i.biWidth * 3);
    o_padrowlen = 4 * ((o_rowlen+3)/4);

    // Size of 24 bit image in bytes (not including header)
    o_imgsize = o_padrowlen * bmp->i.biHeight;

    // Allocate some memory for the new 24 bit bitmap
    if ((*newbmp = malloc(o_imgsize + HDRSIZE)) == NULL) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - unable to allocate memory.\n", funcname);
            e->errnum = CBMP_ERR_MEM;
        }
        return 0;
    }

    // Cast start of allocated memory to a header structure
    new_header = (pbmhdr_t) *newbmp;

    // Copy the old bitmap header to the new one (most fields common)
    *new_header = *bmp;

    // Update format table for 24 bits
    new_header->f.bfSize    = o_imgsize + HDRSIZE;
    new_header->f.bfOffBits = HDRSIZE;

    // Update information table for 24 bits
    new_header->i.biBitCount  = 24;
    new_header->i.biSizeImage = o_imgsize;

    // Don't assume original bitmap had the colour fields at 0
    new_header->i.biClrUsed      = 0;                   
    new_header->i.biClrImportant = 0;

    // Endian conversion (if required)
    HDRENDIAN(new_header);

    // Point to data area of allocated memory
    p = (*newbmp) + HDRSIZE;

    // Clear output buffer index
    oidx = 0;

    // Convert data, row at a time
    for (i = 0; i < bmp->i.biHeight; i++) {

        // Point to start of current row
        this_row = &data[i * i_padrowlen];

        // For each input byte in the row
        for (j = 0; j < i_rowlen; j++) {

            // Calculate number of pixels in this byte. 
            // Only partial count if at last byte and i_partialbits not zero
            i_pixelsinbyte = ((j == (i_rowlen-1)) && i_partialbits) ? i_partialbits : i_pixelsperbyte;

            // For each pixel, calculate 24 bit triplet
            for (k = 0; k < i_pixelsinbyte; k++) {
                // Calculate RGB quad index using appropriate bits from byte (MSB first)
                idx = (this_row[j] >> ((i_pixelsperbyte-1-k) * bmp->i.biBitCount)) & ((1 << bmp->i.biBitCount) - 1);
                p[oidx++] = r[idx].Blue;
                p[oidx++] = r[idx].Green;
                p[oidx++] = r[idx].Red;
            }
        }

        // Pad new row to 32 bit boundary
        for (j = 0; j < (o_padrowlen-o_rowlen); j++)
            p[oidx++] = 0x00;
    }

    // Header endian put back before exit
    HDRENDIAN(bmp);

    return o_imgsize + HDRSIZE;
}

//=============================================================
// TransformBmp()
//
// Performs various transformations on a 24 bit bitmap. Bitmap
// pointer passed in, as 'bitmap', with transformation controlled
// by the 'control' structure. A normal return passes back
// GOODSTATUS, else BADSTATUS is returned with an error message
// sent to buffer pointed to in 'e' (if not NULL).
//
// The transforms currently supports are:
//     Reverse colours (negative)
//     Adjust brightness
//     Adjust contrast
//     Convert to grey scale 
//     Flip about vertical axis
//     Flip about horizontal axis
//     Extract a colour component 
//        (red, green, blue, yellow, cyan or magenta)
//
//=============================================================

int TransformBmp (unsigned char *bitmap, const ptrans_t control, perrmsg_t e)
{
    char *funcname = "TransformBmp()";

    // Local variable declarations
    unsigned char *data, *row, *irow, tmp;              // Pointers to pixel data
    pbmhdr_t hdr;                                       // Pointer to bitmap header
    rgbquad_t value;                                    // Temporary RGB store
    uint32_t width, height, rowlen, padrowlen;          // Bitmap size parameters
    uint32_t val;                                       // General purpose store
    uint32_t i, j;                                      // Indexes

    // Point to bitmap data and header sections
    data = bitmap + HDRSIZE;
    hdr  = (pbmhdr_t) bitmap;

    // Header endian conversion for big endian machines. 
    HDRENDIAN(hdr);

    // Check the bitmap
    if (hdr->i.biBitCount != 24) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - attempt to transform bitmap that's not 24 bit.\n", funcname);
            e->errnum = TBMP_ERR_CONVERROR;
        }
        return BADSTATUS;
    }

    // Check control parameters
    if (control->brightness < 0) {
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - bad brightness control parameter (%d).\n", funcname, control->brightness);
            e->errnum = TBMP_ERR_BADPARAM;
        }
        return BADSTATUS;
    }

    if (control->mono < 0 || control->mono >= 0x7) { 
        if (e != NULL) {
            snprintf(e->errbuf, e->errsize, "***Error: %s - bad monochrome control parameter (%d).\n", funcname, control->mono);
            e->errnum = TBMP_ERR_BADPARAM;
        }
        return BADSTATUS;
    }

    // Calculate bitmap size parameters
    width     = hdr->i.biWidth;
    height    = hdr->i.biHeight;
    rowlen    = width * 3;
    padrowlen = 4 * ((rowlen+3)/4);

    // Undo any endian conversion
    HDRENDIAN(hdr);

    // Process row at a time...
    for (i = 0; i < height; i++) {
        row = &data[i * padrowlen];

        // Flip horizontally if requested
        if (i < height/2 && control->fliph) {
            irow = &data[(height-1-i) * padrowlen];
            for (j = 0; j < rowlen; j++) {
                tmp = row[j];
                row[j] = irow[j];
                irow[j] = tmp;
            }
        }

        // For each pixel in the row
        for (j = 0; j < rowlen; j += 3) {

            // Flip vertically if requested
            if (j < rowlen/2 && control->flipv) {
                value.Blue  = row[j];
                value.Green = row[j+1];
                value.Red   = row[j+2];

                row[j]   = row[rowlen-1-j-2];
                row[j+1] = row[rowlen-1-j-1];
                row[j+2] = row[rowlen-1-j-0];

                row[rowlen-1-j-2] = value.Blue;
                row[rowlen-1-j-1] = value.Green;
                row[rowlen-1-j] = value.Red;
            }

            // Extract colour values for pixel
            value.Blue  = row[j];
            value.Green = row[j+1];
            value.Red   = row[j+2];

            // Reverse video---simply invert all the bits
            if (control->reverse) {
                value.Blue  ^= 0xff;
                value.Green ^= 0xff;
                value.Red   ^= 0xff;
            } 
            
            // Scale each colour value by a constant (%), clipping at maximum
            if (control->brightness) {
                val = ((uint32_t)value.Blue  * control->brightness)/100;
                value.Blue  = (uint8_t)((val > 0xff) ? 0xff : val & 0xff);

                val = ((uint32_t)value.Green * control->brightness)/100;
                value.Green = (uint8_t)((val > 0xff) ? 0xff : val & 0xff);

                val = ((uint32_t)value.Red   * control->brightness)/100;
                value.Red   = (uint8_t)((val > 0xff) ? 0xff : val & 0xff);
            } 
            
            // Extract the monochrome colours
            if (control->mono) {

                // Zero all unspecified colours
                value.Blue  = (control->mono & MONOBLUE)  ? value.Blue  : 0;
                value.Green = (control->mono & MONOGREEN) ? value.Green : 0;
                value.Red   = (control->mono & MONORED)   ? value.Red   : 0;

                // If not RGB monochromatic...
                if (control->mono & (control->mono - 1)) {

                    // Average the two components (one is already zeroed)
                    val = (value.Blue + value.Green + value.Red) / 2;

                    // Set the two enabled colours to be the averaged value
                    value.Blue  = (uint8_t)((control->mono & MONOBLUE)  ? val : 0);
                    value.Green = (uint8_t)((control->mono & MONOGREEN) ? val : 0);
                    value.Red   = (uint8_t)((control->mono & MONORED)   ? val : 0);
                } 
            } 

            // Grey---values are uniformly set to average of all three
            if (control->grey) {

                val = (value.Blue + value.Green + value.Red);

                // If monochrome specified, reduce averaging divisor
                // appropriate to number of colours remaining (normalise)
                val /= (!control->mono ? 3 : (control->mono & (control->mono - 1)) ? 2 : 1) ;

                value.Blue  = (uint8_t)val;
                value.Green = (uint8_t)val;
                value.Red   = (uint8_t)val;
            } 

            // Write back transformed values
            row[j]   = value.Blue;
            row[j+1] = value.Green;
            row[j+2] = value.Red;

        }
    }

    return GOODSTATUS;
}

//=================================================================
// ClipBitmap()
//
// Takes bitmap image passed in with 'bmp' and clips (trims) down
// to a sub-rectangular region defined in 'boundary'. The image
// is updated in place, and the header modified for the new image
// parameters. The final images size value is returned in 
// 'imgsize'. The functions will return BADSTATUS if the specified
// clipping region does not lie fully within the input image area.
// Otherwise, GOODSTATUS is returned.
//
//=================================================================

uint32_t ClipBitmap(char* bmp, const prect_t boundary, uint32_t *imgsize)
{
    uint32_t newwidth, newheight;
    uint32_t i, j, idx;
    pbmhdr_t bm;
    char *data, *this_row;
    uint32_t i_padrowlen;

    bm = (pbmhdr_t) bmp;
    data = bmp + HDRSIZE;

    HDRENDIAN(bm);

    // Clip top and bottom if outside image
    if (boundary->right > bm->i.biWidth)
        boundary->right = bm->i.biWidth;
    if (boundary->top > bm->i.biHeight)
        boundary->right = bm->i.biHeight;

    // Check that the rectangle is valid
    if (boundary->right <= boundary->left ||
        boundary->top   <= boundary->bottom)
        return BADSTATUS;

    // New dimensions
    newwidth  = boundary->right - boundary->left;
    newheight = boundary->top - boundary->bottom;

    // Input bitmaps padded row length
    i_padrowlen = 4 * ((3*bm->i.biWidth+3)/4);

    // Calculate new sizes
    bm->i.biSizeImage = (4 * ((3*newwidth+3)/4)) * newheight;
    bm->f.bfSize = bm->i.biSizeImage + bm->f.bfOffBits;

    // Copy relevant data to bottom of data buffer
    idx = 0;
    for (i = boundary->bottom; i < boundary->top; i++) {
        this_row = &data[i * i_padrowlen];
        for (j = boundary->left; j < boundary->right; j++) {
            data[idx++] = this_row[3*j];
            data[idx++] = this_row[3*j+1];
            data[idx++] = this_row[3*j+2];
        }
        // Pad to 32 bit boundary
        while(idx % 4) {
            data[idx++] = 0;
        }
    }

    // Update height and widths in header
    bm->i.biWidth  = newwidth;
    bm->i.biHeight = newheight;

    // return new size
    *imgsize = bm->i.biSizeImage + HDRSIZE;

    HDRENDIAN(bm);

    return GOODSTATUS;
}
