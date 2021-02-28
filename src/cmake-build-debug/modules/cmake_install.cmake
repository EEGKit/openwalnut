# Install script for directory: /home/top2021/SIVERT/sivert-vis-project/src/modules

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryPlane" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryPlane/libarbitraryPlane.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/arbitraryPlane/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/arbitraryPlane/shaders/WMArbitraryPlane-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/arbitraryPlane/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/arbitraryPlane/shaders/WMArbitraryPlane-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryROIs" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/arbitraryROIs/libarbitraryROIs.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/colormapper/libcolormapper.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/colormapper/libcolormapper.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/colormapper" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/colormapper/libcolormapper.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/colormapper/libcolormapper.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/colormapper/libcolormapper.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/colormapper/libcolormapper.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/colormapper/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/colormapper/shaders/WMColormapper-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/colormapper/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/colormapper/shaders/WMColormapper-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/coordinateSystem" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/coordinateSystem/libcoordinateSystem.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/directVolumeRendering" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/directVolumeRendering/libdirectVolumeRendering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/directVolumeRendering/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/directVolumeRendering/shaders/WMDirectVolumeRendering-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/directVolumeRendering/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/directVolumeRendering/shaders/WMDirectVolumeRendering-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/directVolumeRendering/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/directVolumeRendering/shaders/WMDirectVolumeRendering-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMap/libdistanceMap.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMap/libdistanceMap.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMap" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/distanceMap/libdistanceMap.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/distanceMap/libdistanceMap.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMap/libdistanceMap.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMap/libdistanceMap.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMapIsosurface" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/distanceMapIsosurface/libdistanceMapIsosurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberDisplay" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberDisplay/libfiberDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-EndCap-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-EndCap-geometry.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-EndCap-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/shaders/WMFiberDisplay-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberDisplay" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberDisplay/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberResampling/libfiberResampling.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberResampling/libfiberResampling.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberResampling" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberResampling/libfiberResampling.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberResampling/libfiberResampling.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberResampling/libfiberResampling.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberResampling/libfiberResampling.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gridRenderer/libgridRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gridRenderer/libgridRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gridRenderer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/gridRenderer/libgridRenderer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/gridRenderer/libgridRenderer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gridRenderer/libgridRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gridRenderer/libgridRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceLIC" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceLIC/libimageSpaceLIC.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Advection-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Advection-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-ClipBlend-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-ClipBlend-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Edge-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Edge-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Transformation-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Transformation-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/shaders/WMImageSpaceLIC-Transformation-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceLIC" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceLIC/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurface/libisosurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurface/libisosurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurface" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/isosurface/libisosurface.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/isosurface/libisosurface.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurface/libisosurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurface/libisosurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/isosurface/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/isosurface/shaders/WMIsosurface-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/isosurface/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/isosurface/shaders/WMIsosurface-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurfaceRaytracer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/isosurfaceRaytracer/libisosurfaceRaytracer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/isosurfaceRaytracer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/isosurfaceRaytracer/shaders/WMIsosurfaceRaytracer-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/isosurfaceRaytracer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/isosurfaceRaytracer/shaders/WMIsosurfaceRaytracer-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/isosurfaceRaytracer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/isosurfaceRaytracer/shaders/WMIsosurfaceRaytracer-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/navigationSlices/libnavigationSlices.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/navigationSlices/libnavigationSlices.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/navigationSlices" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/navigationSlices/libnavigationSlices.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/navigationSlices/libnavigationSlices.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/navigationSlices/libnavigationSlices.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/navigationSlices/libnavigationSlices.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/navigationSlices/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/navigationSlices/shaders/WMNavigationSlices-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/navigationSlices/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/navigationSlices/shaders/WMNavigationSlices-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/navigationSlices" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/navigationSlices/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/projectionsAsContext" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/projectionsAsContext/libprojectionsAsContext.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/projectionsAsContext/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/projectionsAsContext/shaders/WMProjectionsAsContext-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/projectionsAsContext/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/projectionsAsContext/shaders/WMProjectionsAsContext-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/projectionsAsContext" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/projectionsAsContext/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarOperator/libscalarOperator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarOperator/libscalarOperator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarOperator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/scalarOperator/libscalarOperator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/scalarOperator/libscalarOperator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarOperator/libscalarOperator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarOperator/libscalarOperator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/superquadricGlyphs" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/superquadricGlyphs/libsuperquadricGlyphs.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/superquadricGlyphs/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/superquadricGlyphs/shaders/WMSuperquadricGlyphs-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/superquadricGlyphs/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/superquadricGlyphs/shaders/WMSuperquadricGlyphs-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/superquadricGlyphs/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/superquadricGlyphs/shaders/WMSuperquadricGlyphs-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/triangleMeshRenderer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/triangleMeshRenderer/libtriangleMeshRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/triangleMeshRenderer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/triangleMeshRenderer/shaders/WMTriangleMeshRenderer-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/triangleMeshRenderer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/triangleMeshRenderer/shaders/WMTriangleMeshRenderer-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorOperator/libvectorOperator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorOperator/libvectorOperator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorOperator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorOperator/libvectorOperator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorOperator/libvectorOperator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorOperator/libvectorOperator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorOperator/libvectorOperator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorPlot/libvectorPlot.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorPlot/libvectorPlot.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorPlot" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorPlot/libvectorPlot.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorPlot/libvectorPlot.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorPlot/libvectorPlot.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorPlot/libvectorPlot.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/voxelizer/libvoxelizer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/voxelizer/libvoxelizer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/voxelizer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/voxelizer/libvoxelizer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/voxelizer/libvoxelizer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/voxelizer/libvoxelizer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/voxelizer/libvoxelizer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/voxelizer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/voxelizer/shaders/WMVoxelizer-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/voxelizer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/voxelizer/shaders/WMVoxelizer-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunction1D" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunction1D/libtransferFunction1D.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/transferFunction1D" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/transferFunction1D/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/eegView/libeegView.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/eegView/libeegView.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/eegView" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/eegView/libeegView.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/eegView/libeegView.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/eegView/libeegView.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/eegView/libeegView.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readDipoles/libreadDipoles.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readDipoles/libreadDipoles.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readDipoles" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readDipoles/libreadDipoles.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readDipoles/libreadDipoles.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readDipoles/libreadDipoles.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readDipoles/libreadDipoles.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gaussFiltering" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/gaussFiltering/libgaussFiltering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramEqualization" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramEqualization/libhistogramEqualization.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/spatialDerivative" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/spatialDerivative/libspatialDerivative.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTranslator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTranslator/libfiberTranslator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readAmiraMesh" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readAmiraMesh/libreadAmiraMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readMesh/libreadMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readMesh/libreadMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readMesh" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readMesh/libreadMesh.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readMesh/libreadMesh.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readMesh/libreadMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readMesh/libreadMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readRawData/libreadRawData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readRawData/libreadRawData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readRawData" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readRawData/libreadRawData.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readRawData/libreadRawData.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readRawData/libreadRawData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readRawData/libreadRawData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readSimpleTextLineData" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readSimpleTextLineData/libreadSimpleTextLineData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/readSimpleTextLineData" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/readSimpleTextLineData/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVCL/libreadVCL.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVCL/libreadVCL.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVCL" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readVCL/libreadVCL.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readVCL/libreadVCL.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVCL/libreadVCL.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVCL/libreadVCL.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/readVCL" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/readVCL/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVIM/libreadVIM.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVIM/libreadVIM.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVIM" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readVIM/libreadVIM.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/readVIM/libreadVIM.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVIM/libreadVIM.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/readVIM/libreadVIM.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/readVIM" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/readVIM/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeAmiraMesh" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeAmiraMesh/libwriteAmiraMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/writeAmiraMesh" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/writeAmiraMesh/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeDendrogram" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeDendrogram/libwriteDendrogram.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeMesh/libwriteMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeMesh/libwriteMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeMesh" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeMesh/libwriteMesh.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeMesh/libwriteMesh.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeMesh/libwriteMesh.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeMesh/libwriteMesh.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeRawData/libwriteRawData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeRawData/libwriteRawData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeRawData" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeRawData/libwriteRawData.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeRawData/libwriteRawData.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeRawData/libwriteRawData.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeRawData/libwriteRawData.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTracts/libwriteTracts.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTracts/libwriteTracts.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTracts" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeTracts/libwriteTracts.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeTracts/libwriteTracts.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTracts/libwriteTracts.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTracts/libwriteTracts.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTransferFunction" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/writeTransferFunction/libwriteTransferFunction.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/writeTransferFunction" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/writeTransferFunction/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/anisotropicFiltering" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/anisotropicFiltering/libanisotropicFiltering.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/applyMask/libapplyMask.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/applyMask/libapplyMask.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/applyMask" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/applyMask/libapplyMask.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/applyMask/libapplyMask.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/applyMask/libapplyMask.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/applyMask/libapplyMask.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateGFA/libcalculateGFA.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateGFA/libcalculateGFA.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateGFA" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateGFA/libcalculateGFA.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateGFA/libcalculateGFA.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateGFA/libcalculateGFA.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateGFA/libcalculateGFA.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateHistogram" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateHistogram/libcalculateHistogram.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/calculateHistogram" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/calculateHistogram/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateTensors/libcalculateTensors.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateTensors/libcalculateTensors.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateTensors" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateTensors/libcalculateTensors.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/calculateTensors/libcalculateTensors.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateTensors/libcalculateTensors.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/calculateTensors/libcalculateTensors.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/calculateTensors" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/calculateTensors/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplay" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplay/libclusterDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplayVoxels" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/clusterDisplayVoxels/libclusterDisplayVoxels.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataCreator/libdataCreator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataCreator/libdataCreator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataCreator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/dataCreator/libdataCreator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/dataCreator/libdataCreator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataCreator/libdataCreator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataCreator/libdataCreator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetManipulator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetManipulator/libdatasetManipulator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetProfile/libdatasetProfile.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetProfile/libdatasetProfile.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetProfile" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/datasetProfile/libdatasetProfile.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/datasetProfile/libdatasetProfile.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetProfile/libdatasetProfile.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/datasetProfile/libdatasetProfile.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataTypeConversion" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/dataTypeConversion/libdataTypeConversion.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/deterministicFTMori" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/deterministicFTMori/libdeterministicFTMori.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/diffTensorScalars" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/diffTensorScalars/libdiffTensorScalars.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/diffTensorScalars" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/diffTensorScalars/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/effectiveConnectivityCluster" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/effectiveConnectivityCluster/libeffectiveConnectivityCluster.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterIndex" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterIndex/libfiberFilterIndex.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberFilterIndex" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberFilterIndex/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterROI" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberFilterROI/libfiberFilterROI.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fiberFilterROI" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fiberFilterROI/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberParameterColoring" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberParameterColoring/libfiberParameterColoring.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberSelection/libfiberSelection.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberSelection/libfiberSelection.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberSelection" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberSelection/libfiberSelection.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberSelection/libfiberSelection.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberSelection/libfiberSelection.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberSelection/libfiberSelection.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fibersToPoints" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fibersToPoints/libfibersToPoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/fibersToPoints" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/fibersToPoints/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTransform/libfiberTransform.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTransform/libfiberTransform.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTransform" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberTransform/libfiberTransform.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/fiberTransform/libfiberTransform.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTransform/libfiberTransform.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/fiberTransform/libfiberTransform.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/functionalMRIViewer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/functionalMRIViewer/libfunctionalMRIViewer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramView/libhistogramView.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramView/libhistogramView.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramView" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/histogramView/libhistogramView.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/histogramView/libhistogramView.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramView/libhistogramView.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/histogramView/libhistogramView.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageExtractor/libimageExtractor.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageExtractor/libimageExtractor.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageExtractor" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageExtractor/libimageExtractor.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageExtractor/libimageExtractor.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageExtractor/libimageExtractor.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageExtractor/libimageExtractor.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceTensorLIC" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/imageSpaceTensorLIC/libimageSpaceTensorLIC.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Advection-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Advection-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-ClipBlend-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-ClipBlend-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Edge-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Edge-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Postprocessing-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Postprocessing-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Transformation-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Transformation-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/shaders/WMImageSpaceTensorLIC-Transformation-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/imageSpaceTensorLIC" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/imageSpaceTensorLIC/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergeComponentsToVector" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergeComponentsToVector/libmergeComponentsToVector.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/mergeComponentsToVector" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/mergeComponentsToVector/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergePoints/libmergePoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergePoints/libmergePoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergePoints" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/mergePoints/libmergePoints.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/mergePoints/libmergePoints.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergePoints/libmergePoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/mergePoints/libmergePoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/mergePoints" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/mergePoints/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/meshToPoints/libmeshToPoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/meshToPoints/libmeshToPoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/meshToPoints" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/meshToPoints/libmeshToPoints.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/meshToPoints/libmeshToPoints.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/meshToPoints/libmeshToPoints.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/meshToPoints/libmeshToPoints.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/meshToPoints" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/meshToPoints/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/paintTexture/libpaintTexture.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/paintTexture/libpaintTexture.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/paintTexture" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/paintTexture/libpaintTexture.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/paintTexture/libpaintTexture.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/paintTexture/libpaintTexture.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/paintTexture/libpaintTexture.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVR/libpickingDVR.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVR/libpickingDVR.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVR" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pickingDVR/libpickingDVR.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pickingDVR/libpickingDVR.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVR/libpickingDVR.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVR/libpickingDVR.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pickingDVR" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pickingDVR/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVREvaluation" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pickingDVREvaluation/libpickingDVREvaluation.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pickingDVREvaluation" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pickingDVREvaluation/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pointRenderer/libpointRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pointRenderer/libpointRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pointRenderer" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pointRenderer/libpointRenderer.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/pointRenderer/libpointRenderer.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pointRenderer/libpointRenderer.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/pointRenderer/libpointRenderer.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pointRenderer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pointRenderer/shaders/WMPointRenderer-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pointRenderer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pointRenderer/shaders/WMPointRenderer-geometry.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pointRenderer/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pointRenderer/shaders/WMPointRenderer-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/pointRenderer" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/pointRenderer/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/probTractDisplay" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/probTractDisplay/libprobTractDisplay.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/sampleOnFibers" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/sampleOnFibers/libsampleOnFibers.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/sampleOnFibers" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/sampleOnFibers/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarSegmentation" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/scalarSegmentation/libscalarSegmentation.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/splineSurface/libsplineSurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/splineSurface/libsplineSurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/splineSurface" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/splineSurface/libsplineSurface.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/splineSurface/libsplineSurface.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/splineSurface/libsplineSurface.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/splineSurface/libsplineSurface.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/splineSurface/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/splineSurface/shaders/WMSplineSurface-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/splineSurface/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/splineSurface/shaders/WMSplineSurface-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceIllustrator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceIllustrator/libsurfaceIllustrator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/surfaceIllustrator/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/surfaceIllustrator/shaders/WMSurfaceIllustrator-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/surfaceIllustrator/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/surfaceIllustrator/shaders/WMSurfaceIllustrator-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceParameterAnimator" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/surfaceParameterAnimator/libsurfaceParameterAnimator.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/surfaceParameterAnimator/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/surfaceParameterAnimator/shaders/WMSurfaceParameterAnimator-Beams-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/surfaceParameterAnimator/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/surfaceParameterAnimator/shaders/WMSurfaceParameterAnimator-Beams-varyings.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/surfaceParameterAnimator/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/surfaceParameterAnimator/shaders/WMSurfaceParameterAnimator-Beams-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunctionColorBar" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/transferFunctionColorBar/libtransferFunctionColorBar.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/transferFunctionColorBar/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/transferFunctionColorBar/shaders/WMTransferFunctionColorBar-fragment.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/transferFunctionColorBar/shaders" TYPE FILE FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/transferFunctionColorBar/shaders/WMTransferFunctionColorBar-vertex.glsl")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorAlign/libvectorAlign.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorAlign/libvectorAlign.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorAlign" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorAlign/libvectorAlign.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorAlign/libvectorAlign.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorAlign/libvectorAlign.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorAlign/libvectorAlign.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorNormalize" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorNormalize/libvectorNormalize.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/vectorNormalize" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/vectorNormalize/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorScale/libvectorScale.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorScale/libvectorScale.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorScale" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorScale/libvectorScale.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/vectorScale/libvectorScale.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorScale/libvectorScale.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/vectorScale/libvectorScale.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openwalnut/modules/vectorScale" TYPE DIRECTORY FILES "/home/top2021/SIVERT/sivert-vis-project/src/modules/vectorScale/resources/" REGEX "/bin\\/[^/]*$" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xMODULESx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/webglSupport/libwebglSupport.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/webglSupport/libwebglSupport.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/openwalnut/webglSupport" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/webglSupport/libwebglSupport.so.1.5.0"
    "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib/openwalnut/webglSupport/libwebglSupport.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/webglSupport/libwebglSupport.so.1.5.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/openwalnut/webglSupport/libwebglSupport.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/data/cmake_install.cmake")
  include("/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/readLAS/cmake_install.cmake")
  include("/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/hierarchicalClustering/cmake_install.cmake")
  include("/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/openIGTLink/cmake_install.cmake")
  include("/home/top2021/SIVERT/sivert-vis-project/src/cmake-build-debug/modules/template/cmake_install.cmake")

endif()

