//=============================================================
// main.c                                    Date: 2003/04/14 
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
// $Id: main.c,v 1.3 2010-07-06 13:51:11 simon Exp $
// $Source: /home/simon/CVS/src/bitmaps/src/main.c,v $
//
//=============================================================
//
// Main command line entry point for bitmap program.
//
// Display bitmap header information, and convert low order
// bitmaps to 24 bits if an output file specified.
//
//=============================================================

#include "main.h"

int main(int argc, char **argv)
{
    trans_t control;
    int option, debug = 0, convert = FALSE, grey = FALSE;
    uint32_t i, imgsize;
    unsigned char *data, *newdata, reverse = 0x00, dim = 100;
    long tmp;
    rect_t rect;

    char *ifname = DEFAULTIFNAME, *ofname = NULL;
    FILE *ifp, *ofp;
    errmsg_t err;

    // Bitmap structure pointers
    pbmhdr_t bmp, nbmp;                 // Header
    prgbquad_t r;                       // RGB Quad table

    // Default the transformation controls
    control.clip       = FALSE;
    control.reverse    = FALSE;
    control.brightness = 0;
    control.contrast   = 0;
    control.grey       = FALSE;
    control.flipv      = FALSE;
    control.fliph      = FALSE;
    control.mono       = MONOALL;

    rect.top    = 100;
    rect.bottom = 0;
    rect.left   = 0;
    rect.right  = 100;

    // Process command line options
    while ((option = getopt(argc, argv, "c:m:HVgb:rhdi:o:C:")) != EOF) {
        switch (option) {
        case 'C':
            control.clip = TRUE;
            i = 0;
            while (optarg[i] == ' ' || optarg[i] == '\t')
                i++;
            rect.left = strtol(&optarg[i], NULL, 0);
            while (optarg[i] != ' ' && optarg[i] != '\t')
                i++;
            while (optarg[i] == ' ' || optarg[i] == '\t')
                i++;
            rect.right = strtol(&optarg[i], NULL, 0);
            while (optarg[i] != ' ' && optarg[i] != '\t')
                i++;
            while (optarg[i] == ' ' || optarg[i] == '\t')
                i++;
            rect.bottom = strtol(&optarg[i], NULL, 0);
            while (optarg[i] != ' ' && optarg[i] != '\t')
                i++;
            while (optarg[i] == ' ' || optarg[i] == '\t')
                i++;
            rect.top = strtol(&optarg[i], NULL, 0);
            break;
        case 'b':
            tmp = strtol(optarg, NULL, 0);
            if (tmp <= 0) {
                fprintf(stderr, "***Error: bad 'brightness' specification (brightness > 0).\n");
                return BADSTATUS;
            }
            control.brightness = (uint32_t) tmp;
            break;
        case 'c':
            tmp = strtol(optarg, NULL, 0);
            if (tmp < 0 || tmp > 100) {
                fprintf(stderr, "***Error: bad 'contrast' specification.\n");
                return BADSTATUS;
            }
            control.contrast = (uint32_t) tmp;
            break;
        case 'g':
            control.grey = TRUE;
            break;
        case 'r':
            control.reverse = TRUE;
            break;
        case 'V':
            control.flipv = TRUE;
            break;
        case 'm':
            switch (optarg[0]) {
            case 'R': case 'r':
                control.mono = MONORED;
                break;
            case 'G': case 'g':
                control.mono = MONOGREEN;
                break;
            case 'B': case 'b':
                control.mono = MONOBLUE;
                break;
            case 'Y': case 'y':
                control.mono = MONORED | MONOGREEN;
                break;
            case 'C': case 'c':
                control.mono = MONOBLUE | MONOGREEN;
                break;
            case 'M': case 'm':
                control.mono = MONORED | MONOBLUE;
                break;
            default:
                fprintf(stderr, "***Error: bad monochrome colour specification\n");
                return BADSTATUS;
            }
            break;
        case 'H':
            control.fliph = TRUE;
            break;
        case 'i':
            ifname = optarg;
            break;
        case 'o':
            ofname = optarg;
            convert = TRUE;
            break;
        case 'd':
            debug++;
            break;
        case 'h':
        default:
            USAGE;
            return (option == 'h') ? GOODSTATUS : BADSTATUS;
        }
    }

    // Assign some space for returned error messages
    err.errsize = ERRBUFSIZE;
    err.errnum = 0;
    if ((err.errbuf = malloc(err.errsize)) == NULL) {
        fprintf(stderr, "***Error: unable to allocate memory.\n");
        return BADSTATUS;
    }

    // Open input file for reading
    if ((ifp = fopen(ifname, "rb")) == NULL) {
        fprintf(stderr, "***Error: unable to open input file for reading.\n");
        return BADSTATUS;
    }

    // Read in bitmap file, setting pointers to the headers and data
    if (GetBitmap(ifp, &bmp, &r, &data, &err) == BADSTATUS) {
        fprintf(stderr, "%s", err.errbuf);
        return BADSTATUS;
    }

    // Print formatted tables if requested
    if (debug) 
        DISPLAYTABLES(bmp);


    // If not a 24 bit bitmap, display the colour table and convert to 24 bit if specified
    if (r != NULL) {
        // Print out the table
        if (debug) 
            for (i = 0; i < (1U << SWPEND16(bmp->i.biBitCount)); i++) 
                fprintf(stdout, "%03d : Red = 0x%02x Green = 0x%02x Blue = 0x%02x\n", 
                    i, (int)r[i].Red, (int)r[i].Green, (int)r[i].Blue);
    }

    // By default, new data is the input bitmap
    newdata = (unsigned char *)bmp;
    imgsize = SWPEND32(bmp->f.bfSize);

    // If conversion enabled, convert to 24 bits
    if (convert && r != NULL) {
        if ((imgsize = ConvertBmpTo24bit(&newdata, bmp, r, data, &err)) == 0) {
            // Error in conversion. Print error message and return bad status.
            fprintf(stdout, "%s", err.errbuf);
            return BADSTATUS;
        }
    } 

    // If an output file specified, do any transforms required and dump to file
    if (ofname != NULL) {
        nbmp = (pbmhdr_t) newdata;

        // Transform the data as specified
        if (TransformBmp (newdata, &control, &err) == BADSTATUS) {
            fprintf(stdout, "%s", err.errbuf);
            return BADSTATUS;
        }

        if (control.clip == TRUE) {
            if (ClipBitmap(newdata, &rect, &imgsize) == BADSTATUS) {
                fprintf(stderr, "***Error: ClipBitmap encountered a problem.\n");
                return BADSTATUS;
            }
        }

        // Open file for writing
        if ((ofp = fopen(ofname, "wb")) == NULL) {
            fprintf(stderr, "***Error: unable to open output file.\n");
            return BADSTATUS;
        }

        // Output image
        for (i = 0; i < imgsize; i++) 
            putc(newdata[i], ofp);

        fclose(ofp);
    }

    return GOODSTATUS;
}
