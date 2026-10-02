# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/therm")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/therm")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/therm"
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/src/therm-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/src/therm-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/src/therm-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/therm/build/_sysbuild/sysbuild/images/therm-prefix/src/therm-stamp${cfgdir}") # cfgdir has leading slash
endif()
