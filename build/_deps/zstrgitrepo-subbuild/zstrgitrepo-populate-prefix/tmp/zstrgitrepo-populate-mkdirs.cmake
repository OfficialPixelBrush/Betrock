# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-src"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-build"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/tmp"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/src/zstrgitrepo-populate-stamp"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/src"
  "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/src/zstrgitrepo-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/src/zstrgitrepo-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/torben/Documents/GitHub/Betrock/build/_deps/zstrgitrepo-subbuild/zstrgitrepo-populate-prefix/src/zstrgitrepo-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
