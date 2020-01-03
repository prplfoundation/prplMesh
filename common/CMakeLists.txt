message("${BoldWhite}Preparing ${BoldGreen}common${BoldWhite} for the ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} platform${ColourReset}")

# Default values
set(BEEROCKS_PLATFORM_INIT "false")

# Default Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pthread -Wno-comment -Wno-unused-parameter -Wno-missing-field-initializers -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

# Compiler specific flags
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-address-of-packed-member")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector-strong")
else()
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fstack-protector")
endif()

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2 -Wformat -Wformat-security -Wl,-S -fPIE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")

message(STATUS "Prefix=${CMAKE_INSTALL_PREFIX}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now -pie")
endif()

message(STATUS "Build Type=${CMAKE_BUILD_TYPE}")

# Modules
add_subdirectory(beerocks)
