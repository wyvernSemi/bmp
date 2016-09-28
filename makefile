#=============================================================
# 
# Copyright (c) 2003 Simon Southwell
# 
# Date: 14th April 2003
# 
# This file is part of bmp.
#
# bmp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# bmp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with bmp. If not, see <http://www.gnu.org/licenses/>.
#
# $Id: makefile,v 1.4 2016-09-27 10:29:03 simon Exp $
# $Source: /home/simon/CVS/src/bitmaps/makefile,v $
# 
#=============================================================

OSTYPE:=$(shell uname -o)

#
# Compile output
#
TARGET  = bmp
OBJECTS = bitmap.o
LIBOBJ  = libbitmap.a
ifeq (${OSTYPE}, Cygwin)
  SHAREDOBJ = libbitmap.dll
else
  SHAREDOBJ = libbitmap.so
endif

#
# Directories
#
SRCDIR = ./src
OBJDIR = ./objs

OSTYPE:=$(shell uname -o)

#
# GNU compiler flags
#
CC = gcc
LD = ld

LDOPTS = -L . -lbitmap
COPTS  = -Ofast -I . -I${SRCDIR} -I${HOME}/src/include

ifneq (${OSTYPE}, Cygwin)
  COPTS += -fPIC
endif



# Default target

all : ${TARGET} ${SHAREDOBJ} ${LIBOBJ}

#####################
# Dependencies
#####################

${OBJDIR}/bitmap.o : ${SRCDIR}/bitmap.c ${SRCDIR}/bitmap.h
${OBJDIR}/main.o   : ${SRCDIR}/main.c ${SRCDIR}/main.h ${SRCDIR}/bitmap.h

#####################
# Compilation rules
#####################

#
# Compile executable
#
${TARGET} : ${LIBOBJ} ${OBJDIR}/main.o
	$(CC) ${OBJDIR}/main.o -o ${TARGET} ${LDOPTS}

#
# Create shared object library
#
${SHAREDOBJ} : ${OBJDIR}/bitmap.o
	$(CC) -shared ${OBJDIR}/bitmap.o -o $@

#
# Archive the position independant object file
#
${LIBOBJ}: ${OBJDIR}/bitmap.o 
	ar -r $@ ${OBJDIR}/bitmap.o

#
# Generic relocatable object rule
#
${OBJDIR}/%.o : ${SRCDIR}/%.c
	$(CC) $(COPTS) -c $< -o $@ 

#
# Tidy up
#
clean: 
	@/bin/rm -f ${TARGET} *.so *.dll *.a ${OBJDIR}/*.o

