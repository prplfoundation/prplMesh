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
