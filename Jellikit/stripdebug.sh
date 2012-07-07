#!/bin/sh
# simple script to rid an app of its DWARF2 debugging cruft without
# damaging its precious resources

APPNAME=Jellikit

DIRNAME=$(dirname $0)
COPY_RESFILE=$DIRNAME/Resource-copy.rsrc
EXECUTABLE=$DIRNAME/$APPNAME

xres -o $COPY_RESFILE $EXECUTABLE
strip $EXECUTABLE
xres -o $EXECUTABLE $COPY_RESFILE
mimeset -f $EXECUTABLE