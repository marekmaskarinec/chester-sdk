# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/dfu_external_flash"
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/src/dfu_external_flash-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/src/dfu_external_flash-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/src/dfu_external_flash-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/dfu_external_flash/build/_sysbuild/sysbuild/images/dfu_external_flash-prefix/src/dfu_external_flash-stamp${cfgdir}") # cfgdir has leading slash
endif()
