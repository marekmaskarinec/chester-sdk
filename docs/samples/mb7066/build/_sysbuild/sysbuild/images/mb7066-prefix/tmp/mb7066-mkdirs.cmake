# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/mb7066"
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/src/mb7066-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/src/mb7066-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/src/mb7066-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/mb7066/build/_sysbuild/sysbuild/images/mb7066-prefix/src/mb7066-stamp${cfgdir}") # cfgdir has leading slash
endif()
