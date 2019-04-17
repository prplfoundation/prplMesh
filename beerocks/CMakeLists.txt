add_subdirectory(bcl)
add_subdirectory(bwl)
add_subdirectory(tlvf)
add_subdirectory(btl)
add_subdirectory(scripts)

# BWL depends on BCL, so make sure it's built first
add_dependencies(bwl bcl)
add_dependencies(btlvf bcl)
add_dependencies(btl btlvf)