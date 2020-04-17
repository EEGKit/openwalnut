#!/bin/sh
cd /c/ow_workspace/OpenWalnut/build/
export NIFTILIB_INCLUDE_DIR=/c/ow_workspace/nifticlib-2.0.0/build/include
export NIFTILIB_LIBRARY_DIR=/c/ow_workspace/nifticlib-2.0.0/build/bin
export LIBLAS_INCLUDE_DIR=/c/ow_workspace/libLAS-1.8.1/build/include/
export LIBLAS_LIBRARY_DIR=/c/ow_workspace/libLAS-1.8.1/build
cmake -G"Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=/c/ow_workspace/OpenWalnut_Packaged -DOW_FIX_BOOST_PO_ARG=TRUE -DOW_FIX_EEP_WINDOWS_ERROR=TRUE -DOW_FIX_WINDOWS_RANDOM_ERROR=TRUE ../src
make -j10 ;
cp /c/ow_workspace/nifticlib-2.0.0/build/bin/*.dll bin
cp /c/ow_workspace/libLAS-1.8.0/build/libliblas* bin/
exec $SHELL