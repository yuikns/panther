# The MIT License (MIT)
#
# Copyright (c) 2015 Yu Jing <yu@argcv.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
SET(snappy_build "${CMAKE_CURRENT_BINARY_DIR}/snappy")

# FIND_LIBRARY( SNAPPY_LIBRARY_INFO  NAMES  snappy PATHS "${PROJECT_BINARY_DIR}/lib" )
INCLUDE(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT_VAL)


EXTERNALPROJECT_ADD(
    snappy_proj
    GIT_REPOSITORY https://github.com/google/snappy.git
    #GIT_TAG v1.18
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./autogen.sh && ./configure
    SOURCE_DIR ${snappy_build}
    # BINARY_DIR ${snappy_build}
    BUILD_COMMAND make -j${PROCESSOR_COUNT_VAL}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
#    INSTALL_COMMAND  sh -c "rsync -a ${snappy_build}/include/ ${PROJECT_BINARY_DIR}/include/"
#    INSTALL_COMMAND  sh -c "rsync -a ${snappy_build}/libsnappy.* ${PROJECT_BINARY_DIR}/lib/ && rsync -a ${snappy_build}/include/ ${PROJECT_BINARY_DIR}/include/"
)

SET(SNAPPY_INCLUDE_DIR ${snappy_build}/include)
SET(SNAPPY_LIB_DIR ${snappy_build})

ADD_LIBRARY(snappy SHARED IMPORTED)
#SET_PROPERTY(TARGET snappy PROPERTY IMPORTED_LOCATION ${snappy_build}/${CMAKE_SHARED_LIBRARY_PREFIX}snappy${CMAKE_SHARED_LIBRARY_SUFFIX})
SET_PROPERTY(TARGET snappy PROPERTY IMPORTED_LOCATION ${snappy_build}/.libs/${CMAKE_SHARED_LIBRARY_PREFIX}snappy${CMAKE_SHARED_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB SHARD " ${snappy_build}/${CMAKE_SHARED_LIBRARY_PREFIX}snappy${CMAKE_SHARED_LIBRARY_SUFFIX})

ADD_LIBRARY(snappy_static STATIC IMPORTED)
#SET_PROPERTY(TARGET snappy_static PROPERTY IMPORTED_LOCATION ${snappy_build}/${CMAKE_STATIC_LIBRARY_PREFIX}snappy${CMAKE_STATIC_LIBRARY_SUFFIX})
SET_PROPERTY(TARGET snappy_static PROPERTY IMPORTED_LOCATION ${snappy_build}/.libs/${CMAKE_STATIC_LIBRARY_PREFIX}snappy${CMAKE_STATIC_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB STATIC " ${snappy_build}/${CMAKE_STATIC_LIBRARY_PREFIX}snappy${CMAKE_STATIC_LIBRARY_SUFFIX})

ADD_DEPENDENCIES(snappy snappy_proj)

ADD_DEPENDENCIES(snappy_static snappy_proj)

SET(SNAPPY_LIBRARY snappy)
SET(SNAPPY_LIBRARY_STATIC snappy_static)


