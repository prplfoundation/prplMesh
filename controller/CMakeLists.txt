add_definitions(-DBEEROCKS_VERSION="${prplmesh_VERSION}")
add_definitions(-DBEEROCKS_BUILD_DATE="${BUILD_DATE}")
add_definitions(-DBEEROCKS_REVISION="${PRPLMESH_REVISION}")

add_definitions(-DBEEROCKS_BIN_PATH="${INSTALL_PATH}/bin/")
add_definitions(-DBEEROCKS_CONF_PATH="${INSTALL_PATH}/config/")

add_subdirectory("src/beerocks")
add_subdirectory("config")
