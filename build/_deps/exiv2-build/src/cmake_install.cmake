# Install script for directory: /home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src

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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libexiv2.so.0.28.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libexiv2.so.28"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "$ORIGIN/../lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/lib/libexiv2.so.0.28.7"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/lib/libexiv2.so.28"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libexiv2.so.0.28.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libexiv2.so.28"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "::::::::::::::"
           NEW_RPATH "$ORIGIN/../lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/lib/libexiv2.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exiv2" TYPE FILE FILES
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/basicio.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/bmffimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/bmpimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/config.h"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/convert.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/cr2image.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/crwimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/datasets.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/easyaccess.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/epsimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/error.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/exif.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/exiv2.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/futils.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/gifimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/image.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/image_types.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/iptc.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/jp2image.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/jpgimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/metadatum.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/mrwimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/orfimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/pgfimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/photoshop.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/preview.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/properties.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/psdimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/rafimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/rw2image.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/slice.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/tags.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/tgaimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/tiffimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/types.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/value.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/version.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/webpimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/xmp_exiv2.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/xmpsidecar.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/pngimage.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/asfvideo.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/matroskavideo.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/quicktimevideo.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/src/../include/exiv2/riffvideo.hpp"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/exv_conf.h"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/exiv2lib_export.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Targets.cmake"
         "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Targets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build/src/exiv2ConfigVersion.cmake"
    "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build/src/exiv2Config.cmake"
    )
endif()

