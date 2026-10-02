# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn")
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn")
endif()
file(MAKE_DIRECTORY
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/sensor_pnp_npn"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/tmp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/src/sensor_pnp_npn-stamp"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/src"
  "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/src/sensor_pnp_npn-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/src/sensor_pnp_npn-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marek/dev/hio/chester-sdk/chester/samples/sensor_pnp_npn/build/_sysbuild/sysbuild/images/sensor_pnp_npn-prefix/src/sensor_pnp_npn-stamp${cfgdir}") # cfgdir has leading slash
endif()
