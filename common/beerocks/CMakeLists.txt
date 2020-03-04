add_subdirectory(bcl)
add_subdirectory(bwl)
add_subdirectory(tlvf)
add_subdirectory(btl)
add_subdirectory(scripts)

# BCL depends on BTLVF, so make sure it's built first
add_dependencies(bcl btlvf)
add_dependencies(bwl bcl)
add_dependencies(btl bcl)
