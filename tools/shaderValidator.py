import os
import re
import subprocess

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

tmp_name = "tmp.glsl"
tmp_file = None
globalShaders = "../src/core/graphicsEngine/shaders/shaders/"
colorPre = "#define ColormapPreTransform mat4( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 )"
includePattern = re.compile( "^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*" )
versionPattern = re.compile( "^[ ]*#[ ]*version[ ]+([123456789][0123456789][0123456789]).*$" )
mainPattern = re.compile( "^[ ]*void[ ]+main[ ]*\([ ]*\).*" )

def processShaderRecursive( file, folder, level ):
    global tmp_file

    fnLocal = os.path.join( folder, file )
    fnGlobal = os.path.join( globalShaders, file )

    if os.path.exists( fnLocal ):
        path = fnLocal
    elif os.path.exists( fnGlobal ):
        path = fnGlobal
    else:
        return


    currfile = open( path, "r" )
    if level > 32:
        return

    count = 0
    tmp_file.write( "#line 1\n" )
    for line in currfile:
        count += 1

        m = includePattern.search( line )
        if m:
            tmp_file.write( "#line 1\n")
            processShaderRecursive( m.group(1), folder, level + 1 )
            tmp_file.write( f"#line {count}\n")
        else:
            tmp_file.write( line )
    currfile.close()

def processVersion():
    tmp = open( tmp_name, "r" )
    cleaned = ""
    version = 120
    hasMain = False

    for line in tmp:
        m = versionPattern.search( line )
        if m:
            versionNum = int( m.group( 1 ) )
            version = max( version, versionNum )
            cleaned += "// " + line
        else:
            cleaned += line
        
        if mainPattern.match( line ):
            hasMain = True
    
    cleaned = f"#version {version} \n{colorPre} \n#line 1 \n{cleaned}"
    tmp.close()
    tmp = open( tmp_name, "w" )
    tmp.write( cleaned )
    tmp.close()
    return hasMain

def processShader( file, folder ):
    global tmp_file

    path = os.path.join( folder, file )

    print( f"{bcolors.OKBLUE}Found shader at {path}" )
    stype = "none"
    if file.endswith( "-vertex.glsl"):
        stype = "vert"
    elif file.endswith( "-fragment.glsl"):
        stype = "frag"
    elif file.endswith( "-geometry.glsl"):
        stype = "geom"
    else:
        print( f"{bcolors.ENDC}Skip shader: no stage\n" )
        return
    
    tmp_file = open( tmp_name, "w" )
    processShaderRecursive( file, folder, 1 )
    tmp_file.close()
    
    if not processVersion():
        print( f"{bcolors.ENDC}Skip shader: no main\n" )
        return

    result = subprocess.run( [ "glslangvalidator", "-S", stype, tmp_name ], stdout=subprocess.PIPE )
    res = result.stdout.decode( "utf-8" )
    if res != "":
        res = res.split("\n", 1)[1].strip()
        print( f"{bcolors.FAIL}{res}" )
    if result.returncode == 0:
        print( f"{bcolors.OKGREEN}Ok." )
    print(" ")

def main():
    rootdir = "../src"

    for subdir, dirs, files in os.walk( rootdir ):
        for file in files:
            if file.endswith( ".glsl" ):
                processShader( file, subdir )
    os.remove( tmp_name )

if __name__ == "__main__":
    main()
