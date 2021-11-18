#!/bin/sh
dir=$(pwd)
args="-l -I$dir/../src/core/graphicsEngine/shaders/shaders"

declare -a ignore=(
    "WGEColormapping-fragment.glsl"
    "WGEColormapping-vertex.glsl"
)

shopt -s globstar
set -e
for file in ../src/**/*.glsl;
do

    for ign in "${ignore[@]}";
    do
        if [[ $file = *${ign} ]];
        then
            echo "skip $file"
            echo " "
            continue 2
        fi
    done

    if [[ $file == *-vertex.glsl ]];
    then
        glslangvalidator "$args" -S vert "$file"
    fi
    if [[ $file == *-fragment.glsl ]];
    then
        glslangvalidator "$args" -S frag "$file"
    fi
    if [[ $file == *-geometry.glsl ]];
    then
        glslangvalidator "$args" -S geom "$file"
    fi

    echo ""
done
