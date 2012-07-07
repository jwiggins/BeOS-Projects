#!/bin/sh
#
# Use this script to change the language that Jellikit uses in its user interface.
# If you translate this to another language, I would be very happy to include
# it with the main Jellikit distribution. Send translated versions to john@ubermensch.net
#
# Oh yeah, If you know how I could do this with a for loop instead of unrolled I'd like to
# see what you can offer. Bash is obviously not my specialty :)
#

DIRNAME=$(dirname $0)
RESFILE="$DIRNAME/JellikitStrings.rsrc"
COPY_RESFILE="$DIRNAME/Resource-copy.rsrc"
XRES="xres -o $RESFILE"

EXECUTABLE="$DIRNAME/Jellikit"
ELLIPSIS='…'

# make a copy of old non-string resources
xres -o $COPY_RESFILE -d CSTR $EXECUTABLE

#erase all the executable's resources (all the type ignores are a bit kludgy)
xres -d CSTR -d RAWT -d MSGG -d MIMS -d APPV -d APPF -d ICON -d MICN -o $EXECUTABLE $EXECUTABLE

# dump a wad of strings into new resource file
$XRES -a CSTR:0 -s "OK"
$XRES -a CSTR:1 -s "Cancel" $RESFILE
$XRES -a CSTR:2 -s "File" $RESFILE
$XRES -a CSTR:3 -s "New File" $RESFILE
$XRES -a CSTR:4 -s "Open File$ELLIPSIS" $RESFILE
$XRES -a CSTR:5 -s "Close" $RESFILE
$XRES -a CSTR:6 -s "Save" $RESFILE
$XRES -a CSTR:7 -s "Save As$ELLIPSIS" $RESFILE
$XRES -a CSTR:8 -s "About" $RESFILE
$XRES -a CSTR:9 -s "Quit" $RESFILE
$XRES -a CSTR:10 -s "Edit" $RESFILE
$XRES -a CSTR:11 -s "Add Attribute$ELLIPSIS" $RESFILE
$XRES -a CSTR:12 -s "Add Predefined Attribute" $RESFILE
$XRES -a CSTR:13 -s "Remove Attribute" $RESFILE
$XRES -a CSTR:14 -s "Attribute Name"  $RESFILE
$XRES -a CSTR:15 -s "Untitled" $RESFILE
$XRES -a CSTR:16 -s "Type" $RESFILE
$XRES -a CSTR:17 -s "Predefined Attribute"  $RESFILE
$XRES -a CSTR:18 -s "Name"  $RESFILE
$XRES -a CSTR:19 -s "string" $RESFILE
$XRES -a CSTR:20 -s "int64" $RESFILE
$XRES -a CSTR:21 -s "int32" $RESFILE
$XRES -a CSTR:22 -s "double" $RESFILE
$XRES -a CSTR:23 -s "float" $RESFILE
$XRES -a CSTR:24 -s "rect" $RESFILE
$XRES -a CSTR:25 -s "bool" $RESFILE
$XRES -a CSTR:26 -s "raw" $RESFILE
$XRES -a CSTR:27 -s "mime string"  $RESFILE
$XRES -a CSTR:28 -s "other"  $RESFILE
$XRES -a CSTR:29 -s "Version"  $RESFILE
$XRES -a CSTR:30 -s "Save changes to '%s'?"  $RESFILE
$XRES -a CSTR:31 -s "Don't Save"  $RESFILE
$XRES -a CSTR:32 -s "An attribute with the same name already exists for this file. Would you like to replace it with the one you are moving?" $RESFILE
$XRES -a CSTR:33 -s "The %s manager failed to initialize correctly. That pretty much makes this program non-functional. I will quit now."  $RESFILE


# dump string resource file and old resources into executable file
xres -o $EXECUTABLE $RESFILE $COPY_RESFILE

# clean up
rm $COPY_RESFILE $RESFILE
