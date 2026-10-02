# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/sleep_chester_x2"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/src/sleep_chester_x2-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/src/sleep_chester_x2-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/src/sleep_chester_x2-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sleep_chester_x2/build/_sysbuild/sysbuild/images/sleep_chester_x2-prefix/src/sleep_chester_x2-stamp${cfgdir}") # cfgdir has leading slash
endif()
