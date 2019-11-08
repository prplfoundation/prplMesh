find_package(PkgConfig REQUIRED)

pkg_check_modules(ZMQ libzmq>=4.0.4 REQUIRED)

find_library(ZMQ_LIBRARY "${ZMQ_LIBRARIES}")
if(NOT ZMQ_LIBRARY)
		message(FATAL_ERROR "libzmq not found!")
endif()

add_library(zmq::zmq UNKNOWN IMPORTED)

# Include directory
set_target_properties(zmq::zmq PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ZMQ_INCLUDE_DIRS}"
)

# Library
set_target_properties(zmq::zmq PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ZMQ_LIBRARY}"
)

