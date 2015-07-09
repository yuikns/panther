SET(leveldb_build "${CMAKE_CURRENT_BINARY_DIR}/leveldb")

# FIND_LIBRARY( LEVELDB_LIBRARY_INFO  NAMES  leveldb PATHS "${PROJECT_BINARY_DIR}/lib" )
INCLUDE(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT_VAL)


EXTERNALPROJECT_ADD(
    leveldb_proj
    GIT_REPOSITORY https://github.com/yuikns/leveldb.git
    GIT_TAG v1.18
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
SET_PROPERTY(TARGET leveldb PROPERTY IMPORTED_LOCATION ${leveldb_build}/${CMAKE_SHARED_LIBRARY_PREFIX}leveldb${CMAKE_SHARED_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB SHARD " ${leveldb_build}/${CMAKE_SHARED_LIBRARY_PREFIX}leveldb${CMAKE_SHARED_LIBRARY_SUFFIX})

ADD_LIBRARY(leveldb_static STATIC IMPORTED)
SET_PROPERTY(TARGET leveldb_static PROPERTY IMPORTED_LOCATION ${leveldb_build}/${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})

MESSAGE(STATUS "ARGCV LEVEL_DB STATIC " ${leveldb_build}/${CMAKE_STATIC_LIBRARY_PREFIX}leveldb${CMAKE_STATIC_LIBRARY_SUFFIX})

ADD_DEPENDENCIES(leveldb leveldb_proj)

ADD_DEPENDENCIES(leveldb_static leveldb_proj)

SET(LEVELDB_LIBRARY ${leveldb})
SET(LEVELDB_LIBRARY_STATIC ${leveldb_static})


