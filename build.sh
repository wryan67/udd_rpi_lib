#!/bin/bash
#:#####################################################:#
#:#  build.sh - source code compile & install tool    #:#
#:#  Written by Wade Ryan                             #:#
#:#  1/5/2020                                         #:#
#:#                                                   #:# 
#:#  Build commands (lifecycle):                      #:# 
#:#                                                   #:# 
#:#  clean    - remove all intermediate files         #:# 
#:#  compile  - build all sources & demo executable   #:# 
#:#  package  - create libraries                      #:# 
#:#  install  - install libraries to system folders   #:# 
#:#                                                   #:# 
#:#  * install will use sudo to access                #:# 
#:#    the system folders                             #:# 
#:#                                                   #:# 
#:#                                                   #:# 
#:#####################################################:#
set -a


BASE="."
SRC=$BASE/src
BIN=$BASE/bin
LIB=$BASE/lib
OBJ=$BASE/obj
EXE=$BIN/demo
MAIN=$BASE/example/main.cpp

HEADER_NAME="udd.h"
LIBNAME=libwiringPiUDDrpi
VERSION=`cat $BASE/version 2>/dev/null`
[ "$VERSION" = "" ] && VERSION=0.1

DESTDIR=/usr
PREFIX=/local
TMP=/tmp/build.$$

C="gcc"
CC="g++"
CFLAGS="-c -O2 -std=c11 -Wall"
CCFLAGS="-c -O2 -std=gnu++11 -Wall -Dversion=$VERSION"
LDFLAGS="-pthread -lwiringPi -lm -luuid  -Wl,--no-undefined -Wl,-z,now"
INCLUDES=`find $SRC -type d | awk '{printf("-I%s ",$0);}'`


#:###################:#
#:#    Die          #:#
#:###################:#
die() { 
  exit 2
}

#:###################:#
#:#    Clean        #:#
#:###################:#
clean() { 
  if [ "$1" = "objects" ];then
    rm -rf $OBJ || die
  else 
    rm -rf $BIN $OBJ $LIB || die
  fi
}

#:###################:#
#:#    Compile      #:#
#:###################:#
compile() {
  LAST=$(ls -1tr "$OBJECT" "$SOURCE" "$HEADER" 2>/dev/null | tail -1)

  if [ "$LAST" = "$OBJECT" ];then
    return
  fi

  RELINK=1
  echo compiling "$SOURCE"
  eval \${$1} \${$2} $DYNAMIC $INCLUDES "$SOURCE" -o "$OBJECT"
  RET=$?

  if [ $RET != 0 ];then
    die
  fi
}

#:#####################:#
#:#  Examine Objects  #:#
#:#####################:#
examineObjects() {
  for SOURCE in `find $SRC -type f`
  do
    DIRNAME=`dirname ${SOURCE}`
    FILNAME=`basename ${SOURCE}`
    SRCNAME="${FILNAME%.*}"
    EXT="${FILNAME##*.}"
    OBJECT="$OBJ/$DIRNAME/$SRCNAME" 
    HEADER="$DIRNAME/$SRCNAME.h"

    mkdir -p $OBJ/$DIRNAME

    if [ $EXT = "c" ];then
      compile C CFLAGS
    elif [ $EXT = "cpp" ];then
      compile CC CCFLAGS
    fi
  done
}


#:###################:#
#:#  build          #:#
#:###################:#
build() {

  mkdir -p $BIN
  mkdir -p $OBJ

  examineObjects

  LAST=`ls -1tr "$EXE" "$MAIN" 2>&1 | tail -1`
  [ "$LAST" != "$EXE" ] && RELINK=1
  

  if [ $RELINK = 1 ];then
    echo linking "$EXE"
    OBJECTS=$(find $OBJ -type f)
    echo $CC $LDFLAGS $INCLUDES $OBJECTS $MAIN -o "$EXE" 
         $CC $LDFLAGS $INCLUDES $OBJECTS $MAIN -o "$EXE"  || die
  else 
    echo "all objects are up to date"
  fi
}

#:###################:#
#:#  package        #:#
#:###################:#
package() {
  mkdir -p $LIB 

  echo Buliding source for dynamic library for sake of size and compatibility
  DYNAMIC="-fPIC"
  clean objects
  build

  echo Packagiang $LIBNAME.so
  OBJECTS=$(find $OBJ -type f)
  ${CC} -shared -Wl,-soname,$LIBNAME.so -o $LIB/$LIBNAME.so.$VERSION ${OBJECTS}  || die


  echo Buliding source for static library for sake of runtime speed
  DYNAMIC=""
  clean objects
  build

  echo Packagiang $LIBNAME.a
  OBJECTS=$(find $OBJ -type f)
  ar rcs $LIB/$LIBNAME.a $OBJECTS || die
  ranlib $LIB/$LIBNAME.a  || die
}

#:###################:#
#:#  install        #:#
#:###################:#
install() {
  echo "[Install Headers]"

  mkdir -p   ${DESTDIR}${PREFIX}/include || die
  chmod a+rx ${DESTDIR}${PREFIX}/include || die

  echo "#pragma once" > ${DESTDIR}${PREFIX}/include/$HEADER_NAME
  echo  "pragma once" > $TMP.headers.dat

  find $SRC -name "*.h" | while read FILENAME; do basename $FILENAME; done | awk '{printf("include.*%s\n",$0)}' >> $TMP.headers.dat
  
  cat $(cat $HEADER_NAME) | egrep -avf $TMP.headers.dat >> ${DESTDIR}${PREFIX}/include/$HEADER_NAME

  chmod 0644 ${DESTDIR}${PREFIX}/include/$HEADER_NAME
  rm -f $TMP.headers.dat

  echo "[Install Dynamic Lib]"
  mkdir -p   ${DESTDIR}${PREFIX}/lib || die
  chmod a+rx ${DESTDIR}${PREFIX}/lib || die

  STATIC=$LIBNAME.a
  DYNAMIC=$LIBNAME.so.$VERSION

  cp $LIB/$STATIC   ${DESTDIR}${PREFIX}/lib/$STATIC 
  cp $LIB/$DYNAMIC  ${DESTDIR}${PREFIX}/lib/$DYNAMIC 
  ln -sf            ${DESTDIR}${PREFIX}/lib/$DYNAMIC  ${DESTDIR}/lib/$LIBNAME.so
  ldconfig
}

#:###################:#
#:#  un-install     #:#
#:###################:#
remove() {
  echo "[Un-install]"
  rm -f ${DESTDIR}${PREFIX}/include/$HEADER_NAME
  rm -f ${DESTDIR}${PREFIX}/lib/$LIBNAME.*
  ldconfig
}


#:###################:#
#:#    Main         #:#
#:###################:#

CLEAN=0
BUILD=0
INSTALL=0
PACKAGE=0
REMOVE=0
RELINK=0

if [ "$*" = "" ];then
  BUILD=1
fi

for COMMAND in $* 
do
  typeset -l $COMMAND
  case $COMMAND in
    clean)     CLEAN=1
               ;;
    build)     BUILD=1
               ;;
    package)   PACKAGE=1
               ;;
    install)   PACKAGE=1
               INSTALL=1
               ;;
    remove)    REMOVE=1
               ;;
    *)         BUILD=1
               ;;
  esac
done

[ $CLEAN = 1 ]   && clean
[ $BUILD = 1 ]   && build
[ $PACKAGE = 1 ] && package
[ $INSTALL = 1 ] && install
[ $REMOVE = 1 ]  && remove

exit 0
