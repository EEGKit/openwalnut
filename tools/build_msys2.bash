#!/bin/sh
# This file sets the needed exports for cmake.
# This script can for example be executed from Visual Studio Code (VSC) directly.
# Thus it can improve the integratred worklfow with OpenWlanut, MSYS2 and VSC
# See e.g. https://bsvgit.informatik.uni-leipzig.de/openwalnut/openwalnut/wikis/visualstudiocodebuildmsys2
cd /c/ow_workspace/OpenWalnut/build/
export NIFTILIB_INCLUDE_DIR=/c/ow_workspace/nifticlib-2.0.0/build/include
export NIFTILIB_LIBRARY_DIR=/c/ow_workspace/nifticlib-2.0.0/build/bin
export LIBLAS_INCLUDE_DIR=/c/ow_workspace/libLAS-1.8.1/build/include/
export LIBLAS_LIBRARY_DIR=/c/ow_workspace/libLAS-1.8.1/build
export OPENVR_DIR=/c/ow_workspace/openvr
cmake -G"Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=/c/ow_workspace/OpenWalnut_Packaged -DOW_GUI_QT5_WebKit=ON -DOW_FIX_BOOST_PO_ARG=TRUE -DOW_FIX_EEP_WINDOWS_ERROR=TRUE -DOW_FIX_WINDOWS_RANDOM_ERROR=TRUE ../src
make -j10 ;
cp /c/ow_workspace/nifticlib-2.0.0/build/bin/*.dll bin
cp /c/ow_workspace/libLAS-1.8.0/build/libliblas* bin/
cp /c/ow_workspace/openvr/bin/win64/*.dll bin
exec $SHELL
