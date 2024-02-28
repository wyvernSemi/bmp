/*****************************************************************
 * general.h                                    Date: 2003/10/04 *
 *                                                               *
 * Copyright (c) 2003-2024 Simon Southwell. All rights reserved. *
 *                                                               *
 * General purpose definitions and conversions between windows   *
 * and un*x. It is not complete, and will be added too as and    *
 * when new requirements are needed. Only generic entries are    *
 * added. I.e. things like MAXBUFSIZE should not creep in as     *
 * these necessarily are different between applications.         *
 *                                                               *
 *****************************************************************/

#ifndef _GENERAL_H_
#define _GENERAL_H_

#ifdef WIN32

#define __BYTE_ORDER __LITTLE_ENDIAN

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdint.h>
#include <endian.h>
#endif

// Only define if not already (windows.h defines this)
#ifndef TRUE
#define TRUE            (1==1)
#define FALSE           (0==1)
#endif

// Useful general definitions
#define GOODSTATUS      0
#define BADSTATUS       1

#define NODEBUG         0
#define DEBUGLVL1       1
#define DEBUGLVL2       2
#define DEBUGLVL3       3

#define BYTEWIDTH       8
#define BYTESHIFT       BYTEWIDTH
#define BYTEMASK        ((1 << BYTEWIDTH) - 1)
#define NIBSPERBYTE     2

#define NEWLINE         0x0a
#define CARRIAGERTN     0x0d

// Conversions between operating systems
#ifdef WIN32

#define strcasecmp      _stricmp
#define strncasecmp     _strnicmp
#define snprintf        _snprintf

#define dlopen(_X, _Y)  LoadLibrary(_X)
#define dlsym(_X, _Y)   GetLoadAddress(_X, _Y)
#define dlclose(_X)     FreeLibrary(_X)

//typedef HINSTANCE       handle_t;

typedef char            int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short  uint16_t;
typedef long            int32_t;
typedef unsigned long   uint32_t;
typedef INT64           int64_t;
typedef UINT64          uint64_t;

#define getopt Getopt

extern int getopt(int, char **, char *);

#else 

#define stricmp         strcasecmp
#define strnicmp        strncasecmp

#include <dlfcn.h>
//#include <link.h>

#define LoadLibrary(_X)         dlopen(_X, RTLD_GLOBAL | RTLD_NOW)
#define GetLoadAddress(_X, _Y)  dlsym(_X, _Y)
#define FreeLibrary(_X)         dlclose(_X)

// Conversion from window types to un*x
typedef double          DOUBLE;
typedef float           FLOAT;
typedef unsigned short  USHORT;
typedef short           SHORT;
typedef int             INT;
typedef unsigned int    UINT;
typedef long long       __int64;
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef unsigned char   BOOL;

#define stricmp         strcasecmp
#define strnicmp        strncasecmp

#endif

// Error message repository type
typedef struct {
    char *errbuf;               // Pointer to message buffer
    unsigned int errsize;       // Size of message buffer
    unsigned int errnum;	// Error code
} errmsg_t, *perrmsg_t;

extern int getopt(int, char **, char *);

#endif
