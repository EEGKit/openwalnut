#!/bin/bash

#---------------------------------------------------------------------------
#
# Project: OpenWalnut ( http://www.openwalnut.org )
#
# Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
# For more information see http://www.openwalnut.org/copying
#
# This file is part of OpenWalnut.
#
# OpenWalnut is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# OpenWalnut is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
#
#---------------------------------------------------------------------------

BUILD_DIR=AppImageBuild

Build()
{
    echo "* Creating build folder"
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR

    echo "* Configure build system"
    cmake ../../../src -DCMAKE_INSTALL_PREFIX=/usr

    echo "* build application"
    make -j$(nproc)

    rm -rf AppDir/*

    echo "* install resources to AppDir"
    make install DESTDIR=AppDir
    
    cd ..
}

Package()
{
    cd $BUILD_DIR
    
    echo "* Creating Desktop Entry"
    rm -f openwalnut.desktop
    echo "[Desktop Entry]"                                      >> ./openwalnut.desktop
    echo "Type=Application"                                     >> ./openwalnut.desktop
    echo "Name=OpenWalnut"                                      >> ./openwalnut.desktop
    echo "Exec=openwalnut"                                      >> ./openwalnut.desktop
    echo "Icon=openwalnut"                                      >> ./openwalnut.desktop
    echo "Terminal=false"                                       >> ./openwalnut.desktop
    echo "Categories=Education;"                                >> ./openwalnut.desktop
    echo "Keywords=visualization;medical data;neuroscience;"    >> ./openwalnut.desktop
    
    echo "* Copy AppRun"
    cp ../AppRun ./AppDir/AppRun
    
    echo "* Downloading appimagetool"
    wget https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage
    chmod +x appimagetool-x86_64.AppImage

    echo "* Packaging"
    export LD_LIBRARY_PATH="$(pwd)/AppDir/usr/lib/${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
    ARCH=x86_64 linuxdeploy \
                --appdir AppDir \
                -d openwalnut.desktop \
                -i ../../../artwork/Icons/openwalnut_128x128.png \
                --plugin qt
    ARCH=x86_64 ./appimagetool-x86_64.AppImage --appimage-extract-and-run ./AppDir
    cd ..
}

BuildAll()
{
    Build
    Package
}

CleanUp()
{
    rm -rf $BUILD_DIR/*
}

UsageExit()
{
        echo "Builds an AppImage of OpenWalnut"
        echo ""
        echo "Usage: $0 {clean|build}"
        echo "  COMMANDS:"
        echo "    clean: removes created build sub-directories"
        echo "    build: does the building and packaging."
        echo "    package: only packages"
        echo ""
        exit 2
}

# Handle user command
case "$1" in
    clean)
        CleanUp
        ;;
    build)
        BuildAll || exit $?
        ;;
    package)
        Package || exit $?
        ;;
    *)
        UsageExit
        ;;
esac