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

DOC_UPLOAD="https://seafile.rlp.net/ajax/u/d/1452fc748c4448638847/upload/?r=18ca3568-f333-400a-afeb-5895eff3de62"
APP_UPLOAD="https://seafile.rlp.net/ajax/u/d/1452fc748c4448638847/upload/?r=18ca3568-f333-400a-afeb-5895eff3de62"
UPLOAD_REGEX="\\{\"url\": \"([^\"]*)\"\\}"
GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
GIT_COMMIT=$(git rev-parse --short HEAD)
DATE=$(date '+%Y-%m-%d_%H-%M')

DOC_NAME="doc_${GIT_BRANCH}_${DATE}_${GIT_COMMIT}.tar.gz"
APP_NAME="OpenWalnut_${GIT_BRANCH}_${DATE}_${GIT_COMMIT}.AppImage"

uploadDoc() {
    echo "TAR DOC"
    tar -czvf $DOC_NAME ../../doc/developer/html

    echo "UPLOAD DOC"
    getDocUpload=$(curl "$DOC_UPLOAD" -H "Accept: application/json" -H "X-Requested-With: XMLHttpRequest")
    link=""
    if [[ $getDocUpload =~ $UPLOAD_REGEX ]]
    then
        link="${BASH_REMATCH[1]}"
    fi

    docUploadDone=$(curl "$link" -F file=@./$DOC_NAME -F parent_dir="/")
    echo "$docUploadDone"

    echo "REMOVE DOC"
    rm -f $DOC_NAME
}

uploadAppImage() {
    echo "COPY APPIMAGE"
    cp ../AppImage/AppImageBuild/OpenWalnut*.AppImage ./$APP_NAME

    echo "UPLOAD APPIMAGE"
    getAppUpload=$(curl "$APP_UPLOAD" -H "Accept: application/json" -H "X-Requested-With: XMLHttpRequest")
    link=""
    if [[ $getAppUpload =~ $UPLOAD_REGEX ]]
    then
        link="${BASH_REMATCH[1]}"
    fi

    appUploadDone=$(curl "$link" -F file=@./$APP_NAME -F parent_dir="/")
    echo "$appUploadDone"

    echo "REMOVE APPIMAGE"
    rm -f $APP_NAME
}

if [[ "$GIT_BRANCH" == "master" ]]
then
    uploadDoc
else
    echo "DOC OMMITED: not master branch"
fi

uploadAppImage