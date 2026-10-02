# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/ssd1306"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/src/ssd1306-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/src/ssd1306-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/src/ssd1306-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/ssd1306/build/_sysbuild/sysbuild/images/ssd1306-prefix/src/ssd1306-stamp${cfgdir}") # cfgdir has leading slash
endif()
