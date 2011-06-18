#
# The ADD_EXAMPLE macro (defined in examples/CMakeLists.txt) ensures that
# the example is correctly built for the requested connector. It is equivalent
# to the following two lines:
#   ADD_EXECUTABLE(hello.wt hello.C)
#   TARGET_LINK_LIBRARIES(hello.wt ${EXAMPLES_CONNECTOR})
# except when the ISAPI (for Microsoft IIS) connector is used, where it will
# build a DLL with the proper symbols exported.
#
##WT_ADD_EXAMPLE(hello.wt hello.C)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_BUILD_TYPE Debug)

find_package(FLEX)
FLEX_TARGET(repl repl.l ${CMAKE_CURRENT_BINARY_DIR}/repl.cc)



ADD_EXECUTABLE(repl ${FLEX_repl_OUTPUTS})

