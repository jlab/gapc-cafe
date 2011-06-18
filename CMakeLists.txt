

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

# Release
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost 1.38 COMPONENTS system REQUIRED)

find_package(FLEX REQUIRED)
FLEX_TARGET(hl hl.l ${CMAKE_CURRENT_BINARY_DIR}/hl.cc)

LINK_DIRECTORIES(/mnt/gms/local/wt-3.1.9/lib)

SET(EXAMPLES_CONNECTOR wthttp)

#ADD_CUSTOM_COMMAND(
#  OUTPUT pages/outer.html_ 
#  COMMAND web/repl -t menu=pages/menu.html_ pages/page.html_ > pages/outer.html_
#  DEPENDS pages/menu.html_ pages/page.html_
#)

ADD_EXECUTABLE(gap.wt main.cc gap_app.cc form.cc tools.cc highlight.cc ${FLEX_hl_OUTPUTS} )

ADD_LIBRARY(process STATIC IMPORTED)
SET_PROPERTY(TARGET process PROPERTY IMPORTED_LOCATION $ENV{HOME}/program/process/libprocess.a)


TARGET_LINK_LIBRARIES(gap.wt ${EXAMPLES_CONNECTOR} process)

#
# If you have Wt installed somehwere, you should use the
# installed Wt header files for your own Wt projects.
# e.g. INCLUDE_DIRECTORIES(/usr/local/include)
# instead of the following:
#
INCLUDE_DIRECTORIES(/mnt/gms/local/wt-3.1.9/include $ENV{HOME}/program/process)



