if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

# Force the compiler to produce ANSI-colored output when using ninja build
# https://github.com/ninja-build/ninja/wiki/FAQ
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

if (NOT REVISION)
    execute_process(
        COMMAND "git" "rev-parse" "--short" "HEAD"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE 
        OUTPUT_VARIABLE REVISION
    )
endif()

if (NOT BUILD_DATE)
    execute_process(COMMAND "date" "+%F %T" OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE BUILD_DATE)
endif()

message(STATUS "prplmesh Version: ${prplmesh_VERSION}")
message(STATUS "prplmesh Build Date: ${BUILD_DATE}")
message(STATUS "prplmesh Revision: ${REVISION}")
