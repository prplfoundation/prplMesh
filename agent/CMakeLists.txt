# Default Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread -std=c++11 -Wno-unused-parameter -Wno-missing-field-initializers -Wno-address-of-packed-member -fpermissive")

# Add "strip" flags
if (CMAKE_BUILD_TYPE STREQUAL "Release")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -Wformat -Wformat-security -Wl,-S -fPIE -fPIC")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

# Linker Options
# - Setting the "rpath-link" linker variable to help the linker resolve secondery dependecies
#   (Libraries that are not directly referenced by the executable, but a linked library)
# - Setting "rpath" linker to add the cmake installed library folder to the linker search path
#   (Removes the need to set LD_LIBRARY_PATH manually when installing to a none standard location)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=${CMAKE_INSTALL_FULL_LIBDIR},-rpath-link=${PLATFORM_STAGING_DIR}/usr/lib")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
endif()

add_definitions(-DBEEROCKS_VERSION="${prplmesh_VERSION}")
add_definitions(-DBEEROCKS_BUILD_DATE="${BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${REVISION}")

add_definitions(-DBEEROCKS_BIN_PATH="${INSTALL_PATH}/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="${INSTALL_PATH}/config/")

# FIXME - better to use target_get_property for SOVERSION which is set in the bwl CMakeLists.txt, but for some reason it is not propogated correctly
add_definitions(-DBWL_MODULE_NAME="libbwl.so.${prplmesh_VERSION_MAJOR}")
add_definitions(-DBPL_MODULE_NAME="libbpl.so.${prplmesh_VERSION_MAJOR}")

add_subdirectory("src/beerocks")
add_subdirectory("config")
