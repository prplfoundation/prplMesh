find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC json-c REQUIRED)

find_library(JSONC_LIBRARY "${JSONC_LIBRARIES}")
if(NOT JSONC_LIBRARY)
	message(FATAL_ERROR "libjson-c not found!")
endif()

# I can't find a way to create a shared imported library target and set its
# properties correctly so they will be propogated after linking from outside + export
# for now, just have find_library + pkg_check_modules define the CMAKE variables
# for the library and link them manually from outside.

#add_library(libjsonc SHARED IMPORTED)
#
#set_target_properties(libjsonc PROPERTIES
#	IMPORTED_LOCATION "${JSONC_LIBRARY}"
#	INTERFACE_INCLUDE_DIRECTORIES "${JSONC_INCLUDE_DIRS}"
#)

