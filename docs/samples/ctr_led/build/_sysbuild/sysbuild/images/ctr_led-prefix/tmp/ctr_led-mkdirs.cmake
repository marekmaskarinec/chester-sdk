# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/ctr_led"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/src/ctr_led-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/src/ctr_led-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/src/ctr_led-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ctr_led/build/_sysbuild/sysbuild/images/ctr_led-prefix/src/ctr_led-stamp${cfgdir}") # cfgdir has leading slash
endif()
