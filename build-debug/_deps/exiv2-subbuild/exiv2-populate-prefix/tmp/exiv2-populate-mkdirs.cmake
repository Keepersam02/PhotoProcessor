# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-src"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-build"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/tmp"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/src/exiv2-populate-stamp"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/src"
  "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/src/exiv2-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/src/exiv2-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/build-debug/_deps/exiv2-subbuild/exiv2-populate-prefix/src/exiv2-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
