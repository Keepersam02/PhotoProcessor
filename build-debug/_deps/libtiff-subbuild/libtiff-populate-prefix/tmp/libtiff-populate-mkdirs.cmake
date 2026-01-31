# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-src"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-build"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/tmp"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/src/libtiff-populate-stamp"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/src"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/src/libtiff-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/src/libtiff-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/libtiff-subbuild/libtiff-populate-prefix/src/libtiff-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
