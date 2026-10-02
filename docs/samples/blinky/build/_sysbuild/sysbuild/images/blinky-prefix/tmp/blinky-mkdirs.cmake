# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/blinky")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/blinky")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/blinky"
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/src/blinky-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/src/blinky-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/src/blinky-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/blinky/build/_sysbuild/sysbuild/images/blinky-prefix/src/blinky-stamp${cfgdir}") # cfgdir has leading slash
endif()
