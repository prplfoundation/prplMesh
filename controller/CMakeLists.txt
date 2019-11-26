set(PROJECT CONTROLLER)

# Default values
set(BEEROCKS_WLAN1_IFACE "wlan0")
set(BEEROCKS_WLAN2_IFACE "wlan2")

# Platform specific flags
if (TARGET_PLATFORM STREQUAL "ugw")
    add_definitions(-DBEEROCKS_UGW)
elseif(TARGET_PLATFORM STREQUAL "rdkb")
    add_definitions(-DBEEROCKS_RDKB)
    set(CMAKE_SKIP_RPATH TRUE)
elseif (TARGET_PLATFORM STREQUAL "openwrt")
    set(BEEROCKS_WLAN2_IFACE "wlan1")
endif()

# Update output directories for easy deployment
set(CMAKE_MULTIAP_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}/bin)

message(STATUS "CMAKE_FIND_ROOT_PATH - ${CMAKE_FIND_ROOT_PATH}")
message(STATUS "CMAKE_INSTALL_PREFIX - ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_MODULE_PATH - ${CMAKE_MODULE_PATH}")
message(STATUS "CMAKE_PREFIX_PATH - ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_MULTIAP_OUTPUT_DIRECTORY - ${CMAKE_MULTIAP_OUTPUT_DIRECTORY}")

# Add "strip" flags

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment  -Wno-unused-parameter -Wno-shift-negative-value -Wno-missing-field-initializers -fPIC")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Wl,-S -fPIE")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

# Release flags
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
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

include(multiap-helpers)
add_definitions(-DBEEROCKS_VERSION="${${PROJECT}_VERSION_STRING}")
add_definitions(-DBEEROCKS_BUILD_DATE="${${PROJECT}_BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${${PROJECT}_REVISION}")

add_definitions(-DBEEROCKS_BIN_PATH="${INSTALL_PATH}/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="${INSTALL_PATH}/config/")

add_subdirectory("src/beerocks")
add_subdirectory("config")
