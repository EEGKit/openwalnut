### Uncomment as needed
#$deppath = "C:\ow_workspace\ow_dependencies\"
$deppath = "C:\ow_workspace\"
#$deppath = "C:\ow_workspace\" # Fill in your on path here

function setVariables 
{
    $boostpath  = $deppath + "boost"
    $nifti_include  = $deppath + "niftilib\include"
    $nifti_lib  = $deppath + "niftilib\lib"
    $osg = $deppath + "openscenegraph"
    $zlib_include = $deppath + "zlib\include"
    $zlib_lib = $deppath + "zlib\lib"

    Write-Output "Setting environment variables for OpenWalnut dependencies"
    setx BOOST_ROOT $boostpath
    setx NIFTILIB_INCLUDE_DIR $nifti_include
    setx NIFTILIB_LIBRARY_DIR $nifti_lib
    setx OSG_ROOT $osg
    setx ZLIB_INCLUDE_DIR $zlib_include
    setx ZLIB_LIBRARY_DIR $zlib_lib
}

function removeVariables
{
    Write-Output "Removing environment variables for OpenWalnut dependencies"
    REG delete "HKCU\Environment" /F /V "BOOST_ROOT"
    REG delete "HKCU\Environment" /F /V "NIFTILIB_INCLUDE_DIR"
    REG delete "HKCU\Environment" /F /V "NIFTILIB_LIBRARY_DIR"
    REG delete "HKCU\Environment" /F /V "OSG_ROOT"
    REG delete "HKCU\Environment" /F /V "ZLIB_INCLUDE_DIR"
    REG delete "HKCU\Environment" /F /V "ZLIB_LINRARY_DIR"
}

setVariables
#removeVariables