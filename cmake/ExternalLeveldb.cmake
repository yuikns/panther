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
SET(leveldb_build "${CMAKE_CURRENT_BINARY_DIR}/leveldb")

# FIND_LIBRARY( LEVELDB_LIBRARY_INFO  NAMES  leveldb PATHS "${PROJECT_BINARY_DIR}/lib" )
INCLUDE(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT_VAL)


EXTERNALPROJECT_ADD(
    leveldb_proj
    GIT_REPOSITORY https://github.com/google/leveldb.git
    #GIT_TAG v1.18
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    SOURCE_DIR ${leveldb_build}
    # BINARY_DIR ${leveldb_build}
    BUILD_COMMAND make -j${PROCESSOR_COUNT_VAL}
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
#    INSTALL_COMMAND  sh -c "rsync -a ${leveldb_build}/include/ ${PROJECT_BINARY_DIR}/include/"
#    INSTALL_COMMAND  sh -c "rsync -a ${leveldb_build}/libleveldb.* ${PROJECT_BINARY_DIR}/lib/ && rsync -a ${leveldb_build}/include/ ${PROJECT_BINARY_DIR}/include/"
)

SET(LEVELDB_INCLUDE_DIR ${leveldb_build}/include)
SET(LEVELDB_LIB_DIR ${leveldb_build})

ADD_LIBRARY(leveldb SHARED IMPORTED)
#SET_PROPERTY(TARGET leveldb PROPERTY IMPORTED_LOCATION ${leveldb_build}/${CMAKE_SHARED_LIBRARY_PREFIX}leveldb${CMAKE_SHARED_LIBRARY_SUFFIX})
SET_PROPERTY(TARGET leveldb PROPERTY IMPORTED_LOCATION ${leveldb_build}/out-shared/${CMAKE_SHARED_LIBRARY_PREFIX}leveldb${CMAKE_SHARED_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB SHARD " ${leveldb_build}/${CMAKE_SHARED_LIBRARY_PREFIX}leveldb${CMAKE_SHARED_LIBRARY_SUFFIX})

ADD_LIBRARY(leveldb_static STATIC IMPORTED)
#SET_PROPERTY(TARGET leveldb_static PROPERTY IMPORTED_LOCATION ${leveldb_build}/${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})
SET_PROPERTY(TARGET leveldb_static PROPERTY IMPORTED_LOCATION ${leveldb_build}/out-static/${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB STATIC " ${leveldb_build}/${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})

ADD_DEPENDENCIES(leveldb leveldb_proj)

ADD_DEPENDENCIES(leveldb_static leveldb_proj)

INCLUDE(ExternalSnappy)

ADD_DEPENDENCIES(leveldb snappy)
ADD_DEPENDENCIES(leveldb_static snappy_static)


SET(LEVELDB_LIBRARY leveldb snappy)
SET(LEVELDB_LIBRARY_STATIC leveldb_static snappy_static)

#TARGET_LINK_LIBRARIES(leveldb snappy)
#TARGET_LINK_LIBRARIES(leveldb_static snappy_static)


