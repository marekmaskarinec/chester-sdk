# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/opt3001"
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/src/opt3001-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/src/opt3001-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/src/opt3001-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/opt3001/build/_sysbuild/sysbuild/images/opt3001-prefix/src/opt3001-stamp${cfgdir}") # cfgdir has leading slash
endif()
