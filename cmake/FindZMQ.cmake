find_package(PkgConfig REQUIRED)

pkg_check_modules(ZMQ libzmq>=4.0.4 REQUIRED)

find_library(ZMQ_LIBRARY "${ZMQ_LIBRARIES}")
if(NOT ZMQ_LIBRARY)
		message(FATAL_ERROR "libzmq not found!")
endif()

# I can't find a way to create a shared imported library target and set its
# properties correctly so they will be propogated after linking from outside + export
# for now, just have find_library + pkg_check_modules define the CMAKE variables
# for the library and link them manually from outside.

#add_library(libzmq SHARED IMPORTED)
#
#set_target_properties(libzmq PROPERTIES
#	IMPORTED_LOCATION "${ZMQ_LIBRARY}"
#  INTERFACE_INCLUDE_DIRECTORIES "${ZMQ_INCLUDE_DIRS}"
#)

