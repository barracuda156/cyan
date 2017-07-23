#!/bin/sh
CWD=`pwd`

if [ ! -f "$CWD/cyan.pro" ]; then
  echo "Can't find cyan.pro"
fi

MXE=/opt/mxe

MINGW=${MXE}/usr/i686-w64-mingw32.static
QT=${MINGW}/qt5
QMAKE=${QT}/bin/qmake

export PATH=${MXE}/usr/bin:$PATH
export PKG_CONFIG_PATH=${MINGW}/lib/pkgconfig

rm -f *.exe || true
rm -rf build || true
${QMAKE} cyan.pro CONFIG+=release CONFIG+=mxe || exit 1
make || exit 1
strip -s build/cyan.exe
rm -rf Makefile* || true
rm -rf cyan_plugin_import.cpp  cyan_resource.rc debug release .qmake.stash || true
mv build/cyan.exe Cyan32.exe
rm -rf build || true

MINGW=${MXE}/usr/x86_64-w64-mingw32.static
QT=${MINGW}/qt5
QMAKE=${QT}/bin/qmake

export PATH=${MXE}/usr/bin:$PATH
export PKG_CONFIG_PATH=${MINGW}/lib/pkgconfig

rm -rf build || true
${QMAKE} cyan.pro CONFIG+=release CONFIG+=mxe || exit 1
make || exit 1
strip -s build/cyan.exe
rm -rf Makefile* || true
rm -rf cyan_plugin_import.cpp  cyan_resource.rc debug release .qmake.stash || true
mv build/cyan.exe Cyan.exe
rm -rf build || true

