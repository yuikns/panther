# all related files
# AUX_SOURCE_DIRECTORY(. DIR_TARGET)
# ADD_LIBRARY (relfiles ${DIR_TARGET})


# main exec
# SET(SRC_LIST argcvse.cc)
ADD_EXECUTABLE(RdSExtr main.cc)

ADD_DEPENDENCIES(RdSExtr rdsextr_static)
#ADD_DEPENDENCIES(RdSExtr inihpp_static)
#ADD_DEPENDENCIES(RdSExtr argcv_util_static)
#ADD_DEPENDENCIES(RdSExtr argcv_util_dict_static)

#IF(OPTIONS_USE_EXTERNAL_CRFPP)
#	ADD_DEPENDENCIES(RdSExtr crfpp_proj)
#ENDIF(OPTIONS_USE_EXTERNAL_CRFPP)

#IF(OPTIONS_USE_EXTERNAL_LEVELDB)
#ADD_DEPENDENCIES(RdSExtr leveldb_proj)
#ENDIF(OPTIONS_USE_EXTERNAL_LEVELDB)

#ADD_DEPENDENCIES(RdSExtr argcv_util_oid_static)

#  executable target links
# TARGET_LINK_LIBRARIES(abcs relfiles)
#TARGET_LINK_LIBRARIES(RdSExtr inihpp_static)
#TARGET_LINK_LIBRARIES(RdSExtr leveldb)
TARGET_LINK_LIBRARIES(RdSExtr pthread) # for leveldb
#TARGET_LINK_LIBRARIES(RdSExtr crfpp)
#TARGET_LINK_LIBRARIES(RdSExtr argcv_util_static)
#TARGET_LINK_LIBRARIES(RdSExtr argcv_util_dict_static)
#TARGET_LINK_LIBRARIES(RdSExtr argcv_util_oid_static)
TARGET_LINK_LIBRARIES(RdSExtr rdsextr_static)
TARGET_LINK_LIBRARIES(RdSExtr leveldb_static)


#  executable properties
SET_TARGET_PROPERTIES(RdSExtr PROPERTIES OUTPUT_NAME "RdSExtr")
