# all related files
# AUX_SOURCE_DIRECTORY(. DIR_TARGET)
# ADD_LIBRARY (relfiles ${DIR_TARGET})


# main exec
# SET(SRC_LIST argcvse.cc)
ADD_EXECUTABLE(panther main.cc)

ADD_DEPENDENCIES(panther rdsextr_static)
#ADD_DEPENDENCIES(panther inihpp_static)
#ADD_DEPENDENCIES(panther argcv_util_static)
#ADD_DEPENDENCIES(panther argcv_util_dict_static)

#IF(OPTIONS_USE_EXTERNAL_CRFPP)
#	ADD_DEPENDENCIES(panther crfpp_proj)
#ENDIF(OPTIONS_USE_EXTERNAL_CRFPP)

#IF(OPTIONS_USE_EXTERNAL_LEVELDB)
#ADD_DEPENDENCIES(panther leveldb_proj)
#ENDIF(OPTIONS_USE_EXTERNAL_LEVELDB)

#ADD_DEPENDENCIES(panther argcv_util_oid_static)

#  executable target links
# TARGET_LINK_LIBRARIES(abcs relfiles)
#TARGET_LINK_LIBRARIES(panther inihpp_static)
#TARGET_LINK_LIBRARIES(panther leveldb)
TARGET_LINK_LIBRARIES(panther pthread) # for leveldb
#TARGET_LINK_LIBRARIES(panther crfpp)
#TARGET_LINK_LIBRARIES(panther argcv_util_static)
#TARGET_LINK_LIBRARIES(panther argcv_util_dict_static)
#TARGET_LINK_LIBRARIES(panther argcv_util_oid_static)
TARGET_LINK_LIBRARIES(panther rdsextr_static)
TARGET_LINK_LIBRARIES(panther leveldb_static)


#  executable properties
SET_TARGET_PROPERTIES(panther PROPERTIES OUTPUT_NAME "panther")

ADD_EXECUTABLE(panther_ctrl main_ctrl.cc)
ADD_DEPENDENCIES(panther_ctrl rdsextr_static)
TARGET_LINK_LIBRARIES(panther_ctrl pthread) # for leveldb
TARGET_LINK_LIBRARIES(panther_ctrl rdsextr_static)
TARGET_LINK_LIBRARIES(panther_ctrl leveldb_static)



